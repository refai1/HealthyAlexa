/**
HTU21D / HPP828E031 driver for mbed.
- Includes RTOS hooks if RTOS is detected during compile.
Author: Kevin Braun
 **/

#ifndef HTU21D_H
#define HTU21D_H

#include "mbed.h"

#if(defined(TARGET_KL25Z) )//|| defined(TARGET_K64F))

    #define HTU21Di2cLOWLEVEL   1           //if the use of low-level I2C routines is needed
    #warning "HTU21D using low level I2C routines"
    
#endif

//Defines for HTU21D
#define HTU21Di2cWRITE      0x80
#define HTU21Di2cREAD       0x81

#define HTU21DWRITEUSER     0xE6
#define HTU21DREADUSER      0xE7
#define HTU21DtempNOHOLD    0xF3
#define HTU21DhumNOHOLD     0xF5
#define HTU21DRESET         0xFE

#define HTU21SNAC1          0xFC
#define HTU21SNAC2          0xC9
#define HTU21SNB1           0xFA
#define HTU21SNB2           0x0F

#define HTU21DHEATER        0x04


    /**
    * measurement specialties / Honeywell HTU21D digital humidity and temperature sensor.
    * - Web site: http://www.meas-spec.com  
    * - Part Number: HPP828E031
    * - HTU21D = +-3% rh error at 55%
    * - HTU20D = +-5% rh error at 55%
    * - Main code generated from datasheet dated October 2013
    * - Serial number code generated from App Note "HTU2X Serial Number Reading", dated Februrary 2014
    * - No checksum checking is performed in this code
    *
    * @code
    * //Tested on FRDM-K64F
    *
    * #include "mbed.h"
    * #include "htu21d.h"
    *
    * #define SDA     PTE25 
    * #define SCL     PTE24
    *
    * Serial pc(USBTX, USBRX);                  //local terminal
    * htu21d htu(SDA, SCL);                     //Temp Hum || sda, scl
    *
    * float H21Temp = 0.0;                      //Temperture from HTU21D
    * float H21Hum = 0.0;                       //Humidity from HTU21D
    * float H21Dew = 0.0;                       //Dew Point from HTU21D
    *
    * //Note: If RTOS is used, Mutex for I2C must be initialized
    * #ifdef RTOS_H
    * Mutex MutexI2cWait;
    * #endif
    *
    * int main() {
    *     pc.baud(230400);                        //local terminal baud
    *     pc.printf("\r\n\r\nK64F_HTU21D basic operation\r\n"); 
    * 
    *     //initialize the HTU21D
    *     int htu21 = htu.softReset();
    *     if(htu21 == 0) {
    *         pc.printf(" - HTU21D broken...\r\n");
    *     } else {
    *         uint8_t HTU21DuserReg = htu.getUserReg();
    *         pc.printf("HTU21D UserReg: 0x%02x   SN: 0x%04x %08x %04x\r\n", 
    *                   HTU21DuserReg, htu.HTU21sn.HTU21D_sna, htu.HTU21sn.HTU21D_snb, htu.HTU21sn.HTU21D_snc);
    *     }
    * 
    *     while(true) {
    *         //get humidity, temperature and dew point from HTU21D
    *         if(htu21 == 1) {    //if HTU21D didn't initialize, don't access HTU21D anymore
    *             H21Hum = htu.getHum();
    *             if((double)H21Hum == 255.0) pc.printf("\r\n*** HTU21D Hum error!!\r\n");
    *             H21Temp = htu.getTemp();
    *             if((double)H21Temp == 255.0) pc.printf("\r\n*** HTU21D Temp error!!\r\n");
    *             H21Dew = htu.getDewPtFast();
    *         }
    *         pc.printf("Temp: %7.2f C %7.2f F   Hum: %4.1f %%   DewPt: %7.2f C\r\n", H21Temp, H21Hum, H21Dew);
    *         wait(1.0);
    *     }
    * }
    * @endcode
    **/
class htu21d {

public:
    /**
     * Constructor
     * - Fixed at I2C address 0x80
     * - I2C speed set to 400000
     *
     * @param PinName sda and scl, mbed I2C interface pins
     */
    htu21d(PinName sda, PinName scl);
    /**
     * Constructor
     * - Fixed at I2C address 0x80
     * - I2C speed set by user
     *
     * @param PinName sda and scl, mbed I2C interface pins 
     * @param int I2C frequency
     */
    htu21d(PinName sda, PinName scl, int i2cFrequency);
    /**
     * Destructor
     *
     * @param --none--
     */
    ~htu21d();
    /**
     * Reset the HTU21D chip
     * - Waits 15mS before exiting, allowing the chip reset to finish
     * - Executes getSNReg() which loads up HTU21D serial number structure
     * 
     * @param --none-- NOTE: run softReset() once at initialization time
     *
     * @return success / failure of HTU21D i2c access. 1 = ok, 0 = error
    */
    int softReset();
    /**
     * Get HTU21D user register
     * 
     * @param --none--
     *
     * @return 8 bit user register value
    */
    uint8_t getUserReg();
    /**
     * Turn ON the heater on the HTU21D
     * 
     * @param --none--
     *
     * @return success / failure of HTU21D i2c access. 1 = ok, 0 = error
    */
    int heaterOn();
    /**
     * Turn OFF the heater on the HTU21D
     * 
     * @param --none--
     *
     * @return success / failure of HTU21D i2c access. 1 = ok, 0 = error
    */
    int heaterOff();
    /**
     * Get heater on/off status of the HTU21D
     * 
     * @param --none--
     *
     * @return 0x04 = on, 0 = off
    */
    uint8_t getHeater();
    /**
     * Get HTU21D Temperature
     * 
     * @param --none--
     *
     * @return float of Temperature in degrees C.  255.0 if error
    */
    float getTemp();
    /**
     * Get HTU21D Humidity
     * 
     * @param --none--
     *
     * @return float of Humidity in percentage.  255.0 if error
    */
    float getHum();
    /**
     * Calculate the Dew Point
     * 
     * @param --none-- NOTE: You MUST run getTemp() and getHum() first!!
     *
     * @return float of Dew Point
    */
    float getDewPt();
    /**
     * Calculate the Dew Point fast
     * - 5x faster than getDewPt()
     * - slightly less accurate than getDewPt()
     * 
     * @param --none-- NOTE: You MUST run getTemp() and getHum() first!!
     *
     * @return float of Dew Point
    */
    float getDewPtFast();
    /**
     * Structure to access HTU21D's serial number 
     * - HTU21D_sna is the hi  16 bit word of the s/n, always is 0x4854
     * - HTU21D_snb is the mid 32 bit word of the s/n, 0x00--------
     * - HTU21D_snc is the low 16 bit word of the s/n, 0x32--
     * - The complete 64 bit s/n value is: 0x48 54 00 -- -- -- 32 --
     * - The numbers shown are fixed fields
     * - The '-' numbers are variable
     * - For reference, the CRC values for the s/n are included
    */    
    struct HTU21snStruct {
        uint16_t HTU21D_sna;            /**< Highest order 16 bit word of SN
                                            - Value always = 0x4854
                                            */
        uint32_t HTU21D_snb;            /**< Middle order 32 bit word of SN
                                            - Value = 0x00--------
                                            - Highest byte always = 0x00
                                            - Lower 3 bytes are variable
                                            */
        uint16_t HTU21D_snc;            /**< Lowest order 16 bit word of SN
                                            - Value = 0x32--
                                            - Highest byte always = 0x32
                                            - Lowest byte is variable
                                            */
        uint8_t HTU21D_crca;            /**< Single byte checksum from HTU21D_sna
                                            */  
        uint32_t HTU21D_crcb;           /**< Four byte checksum from HTU21D_snb
                                            */
        uint8_t HTU21D_crcc;            /**< Single byte checksum from HTU21D_snc
                                            */
        HTU21snStruct() {
            HTU21D_sna = 0;
            HTU21D_snb = 0;
            HTU21D_snc = 0;
            HTU21D_crca = 0;
            HTU21D_crcb = 0;
            HTU21D_crcc = 0;
        }
    } HTU21sn;

private:
    I2C _i2c;
    /**
     * I2C access for getting raw Temperature and Humidity data
     * 
     * @param 8 bit HTU21D register to get data from. Must use non-blocking regs
     *
     * @return 16 bit raw i2c data, ANDed to 14 bits 0xFFFC. 0000 if error
    */
    uint16_t getData(uint8_t reg);
    /**
     * Get the HTU21D's serial number. 
     * - Number returned is 0x4854 00-- ---- 32--
     * - The numbers shown are fixed fields
     * - The '-' numbers are variable
     * 
     * @param --none--
     *
     * @return --none--
    */
#if not defined HTU21Di2cLOWLEVEL
char htuBuffer[8];
#endif
    void getSNReg();
    double theTempIs;
    double theHumIs;
};

#endif
