/**
HTU21D / HPP828E031 driver for mbed.
- Includes RTOS hooks if RTOS is detected during compile.
Author: Kevin Braun
**/

#include "htu21d.h"

double theTempIs = 0.0;
double theHumIs = 0.0;

#if not defined HTU21Di2cLOWLEVEL
char htuBuffer[8];
#endif

//--------------------------------------------------------------------------------------------------------------------------------------//
//Contstructor

htu21d::htu21d(PinName sda, PinName scl) : _i2c(sda, scl) {
    _i2c.frequency(400000);
}

htu21d::htu21d(PinName sda, PinName scl, int i2cFrequency) : _i2c(sda, scl) {
    _i2c.frequency(i2cFrequency);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Destructor

htu21d::~htu21d() {
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Perform a soft reset on the HTU21D. REturn of 1 = ok, 0 = timeout.

int htu21d::softReset() {
    int htu21 = 0;
#if defined HTU21Di2cLOWLEVEL
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21DRESET);            //soft reset, must wait 15mS
        _i2c.stop();
        wait_ms(16);                        //must wait a least 15mS for reset to finish
        htu21d::getSNReg();                 //go load up the s/n registers
    }
    return(htu21);
#else
    htuBuffer[0] = HTU21DRESET;
    htu21 = _i2c.write(HTU21Di2cWRITE, htuBuffer, 1, false);
    wait_ms(16);
    htu21d::getSNReg();
    return(!(htu21));
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Get the HTU21D user register. Returns 8 bit register.

uint8_t htu21d::getUserReg() {
#if defined HTU21Di2cLOWLEVEL
    int htu21 = 0;
    uint8_t htu21data = 0;
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21DREADUSER);
        _i2c.start();
        htu21 = _i2c.write(HTU21Di2cREAD);
        htu21data = _i2c.read(0);
        _i2c.stop();
    }
    return(htu21data);
#else
    htuBuffer[0] = HTU21DREADUSER;
    _i2c.write(HTU21Di2cWRITE, htuBuffer, 1, true);
    if(!(_i2c.read(HTU21Di2cREAD, htuBuffer, 1, false))) {
        return(htuBuffer[0]);
    } else {
        return(0);
    }
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Turn ON the heater the HTU21D user register. 

int htu21d::heaterOn() {
    uint8_t htu21data = htu21d::getUserReg();
    htu21data |= HTU21DHEATER;
#if defined HTU21Di2cLOWLEVEL
    int htu21 = 0;
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21DWRITEUSER);
        htu21 = _i2c.write(htu21data);
        _i2c.stop();
    }
    return(htu21);
#else
    htuBuffer[0] = HTU21DWRITEUSER;
    htuBuffer[1] = htu21data;
    return(_i2c.write(HTU21Di2cWRITE, htuBuffer, 2, false));
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Turn OFF the heater the HTU21D user register. 

int htu21d::heaterOff() {
    uint8_t htu21data = htu21d::getUserReg();
    htu21data &= ~HTU21DHEATER;
#if defined HTU21Di2cLOWLEVEL
    int htu21 = 0;
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21DWRITEUSER);
        htu21 = _i2c.write(htu21data);
        _i2c.stop();
    }
    return(htu21);
#else
    htuBuffer[0] = HTU21DWRITEUSER;
    htuBuffer[1] = htu21data;
    return(_i2c.write(HTU21Di2cWRITE, htuBuffer, 2, false));
#endif

}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Get the status of the heater the HTU21D user register. 0 = off, 4 = on.

uint8_t htu21d::getHeater() {
    uint8_t htu21data = htu21d::getUserReg();
    htu21data &= HTU21DHEATER;
    return(htu21data);
}
    
//--------------------------------------------------------------------------------------------------------------------------------------//
//generic routine to get temp or humidity from HTU21D. 
//Returns 14 bits of data (anded 0xFFFC) or 0000 if i2c timeout occurs.
//After a read temp or humidity command, HTU21D responds with NACKs until data is ready.
//NOTE: Use non-hold commands

uint16_t htu21d::getData(uint8_t reg) {
    int htu21cnt = 0;           //number of NACKs before ACK or timeout 
#if defined HTU21Di2cLOWLEVEL
    uint16_t htu21data = 0;     //returned data
    int htu21 = 0;              //ACK flag
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);
    _i2c.write(reg);            //read temp, no hold
    _i2c.stop();
    if(htu21 == 0) return 0;    //HTU21T not responding
    do {
        htu21cnt++;
        _i2c.start();
        htu21 = _i2c.write(HTU21Di2cREAD);
        if(htu21 == 1) {
            htu21data = _i2c.read(1) << 8;
            htu21data |= _i2c.read(0) & 0xFC;
            _i2c.stop();
        }
        wait_us(1000);
    } while((htu21cnt < 100) && (htu21 == 0));  //htu21cnt takes 55 to get temp, 16 for humidity (at 1mS loops)
        
    if(htu21 == 0) return 0;    //HTU21D ACK response timed out
    return(htu21data);          //return 14 bit value
#else
    htuBuffer[0] = reg;
    _i2c.write(HTU21Di2cWRITE, htuBuffer, 1, false);
    do {
        htu21cnt++;
        if(!(_i2c.read(HTU21Di2cREAD, htuBuffer, 2, false))) {
            return((htuBuffer[0] << 8) | htuBuffer[1]);
        }
        wait_us(1000);
    } while(htu21cnt < 100);
    return 0;
#endif
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//get temperature from HTU21D in degrees C. Returns with 255.0 if HTU21D had timed out.

float htu21d::getTemp() {
    uint16_t getData = htu21d::getData(HTU21DtempNOHOLD);
    if (getData == 0) return(255.0);                        //return with error
    double tempData = (double)getData / 65536.0;
    theTempIs = -46.85 + (175.72 * tempData);
    return(theTempIs);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//get humidity from HTU21D in percentage. Returns with 255.0 if HTU21D had timed out.

float htu21d::getHum() {
    uint16_t getData = htu21d::getData(HTU21DhumNOHOLD);
    if (getData == 0) return(255.0);                        //return with error
    double tempData = (double)getData / 65536.0;
    theHumIs = -6.0 + (125.0 * tempData);
    return(theHumIs);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Calculate the Dew Point from getTemp and getHum. User must first execute both getTemp and getHum for an accurate result.
//Calculations come from DHT library
/*  Copyright (C) Wim De Roeve
 *                based on DHT22 sensor library by HO WING KIT
 *                Arduino DHT11 library
*/

float htu21d::getDewPt() {
    // dewPoint function NOAA
    // reference: http://wahiduddin.net/calc/density_algorithms.htm    
    double A0= 373.15/(273.15 + (double)theTempIs);
    double SUM = -7.90298 * (A0-1);
    SUM += 5.02808 * log10(A0);
    SUM += -1.3816e-7 * (pow(10, (11.344*(1-1/A0)))-1) ;
    SUM += 8.1328e-3 * (pow(10,(-3.49149*(A0-1)))-1) ;
    SUM += log10(1013.246);
    double VP = pow(10, SUM-3) * theHumIs;
    double T = log(VP/0.61078);   // temp var
    return (241.88 * T) / (17.558-T);
}

float htu21d::getDewPtFast() {
    // delta max = 0.6544 wrt dewPoint()
    // 5x faster than dewPoint()
    // reference: http://en.wikipedia.org/wiki/Dew_point
    double h21DtzA = 17.271;
    double h21DtzB = 237.7;
    double h21DtzC = (h21DtzA * theTempIs) / (h21DtzB + theTempIs) + log(theHumIs/100);
    double h21DtzD = (h21DtzB * h21DtzC) / (h21DtzA - h21DtzC);
    return (h21DtzD);
}

//--------------------------------------------------------------------------------------------------------------------------------------//
//Get the HTU21D serial number registers. Returns 64 bit register.
//should return 0x4854 00xx xxxx 32xx

void htu21d::getSNReg() {
    //get 16 bit SNC register, 8 bit SNC-CRC, 16 bit SNA register, 8 bit SNA-CRC
#if defined HTU21Di2cLOWLEVEL
    int htu21 = 0;
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21SNAC1);
        _i2c.write(HTU21SNAC2);
        _i2c.start();
        htu21 = _i2c.write(HTU21Di2cREAD);
        HTU21sn.HTU21D_snc = _i2c.read(1) << 8;
        HTU21sn.HTU21D_snc |= _i2c.read(1);
        HTU21sn.HTU21D_crcc = _i2c.read(1);
        HTU21sn.HTU21D_sna = _i2c.read(1) << 8;
        HTU21sn.HTU21D_sna |= _i2c.read(1);
        HTU21sn.HTU21D_crca = _i2c.read(0);
        _i2c.stop();
    } else {
        HTU21sn.HTU21D_snc = HTU21sn.HTU21D_crcc = HTU21sn.HTU21D_sna = HTU21sn.HTU21D_crca = 0;
    }
#else
    htuBuffer[0] = HTU21SNAC1;
    htuBuffer[1] = HTU21SNAC2;
    _i2c.write(HTU21Di2cWRITE, htuBuffer, 2, true);
    if(!(_i2c.read(HTU21Di2cREAD, htuBuffer, 6, false))) {
        HTU21sn.HTU21D_snc = (htuBuffer[0] << 8) | htuBuffer[1];
        HTU21sn.HTU21D_crcc = htuBuffer[2];
        HTU21sn.HTU21D_sna = (htuBuffer[3] << 8) | htuBuffer[4];
        HTU21sn.HTU21D_crca = htuBuffer[5];
    } else {
        HTU21sn.HTU21D_snc = HTU21sn.HTU21D_crcc = HTU21sn.HTU21D_sna = HTU21sn.HTU21D_crca = 0;
    }
#endif
    
    //get 32 bit SNB register, 32 bit SNB-CRC - regs are intermixed
#if defined HTU21Di2cLOWLEVEL
    htu21 = 0;
    _i2c.start();
    htu21 = _i2c.write(HTU21Di2cWRITE);     //i2c, 1 = ack
    if(htu21 == 1) {
        _i2c.write(HTU21SNB1);
        _i2c.write(HTU21SNB2);
        _i2c.start();
        htu21 = _i2c.write(HTU21Di2cREAD);
        HTU21sn.HTU21D_snb = _i2c.read(1) << 24;
        HTU21sn.HTU21D_crcb = _i2c.read(1) << 24;
        HTU21sn.HTU21D_snb |= _i2c.read(1) << 16;
        HTU21sn.HTU21D_crcb |= _i2c.read(1) << 16;
        HTU21sn.HTU21D_snb |= _i2c.read(1) << 8;
        HTU21sn.HTU21D_crcb |= _i2c.read(1) << 8;
        HTU21sn.HTU21D_snb |= _i2c.read(1);
        HTU21sn.HTU21D_crcb |= _i2c.read(0);
        _i2c.stop();
    } else {
        HTU21sn.HTU21D_snb = HTU21sn.HTU21D_crcb = 0;
    }
#else
    htuBuffer[0] = HTU21SNB1;
    htuBuffer[1] = HTU21SNB2;
    _i2c.write(HTU21Di2cWRITE, htuBuffer, 2, true);
    if(!(_i2c.read(HTU21Di2cREAD, htuBuffer, 8, false))) {
        HTU21sn.HTU21D_snb = (htuBuffer[0] << 24) | (htuBuffer[2] << 16) | (htuBuffer[4] << 8) | htuBuffer[6];
        HTU21sn.HTU21D_crcb = (htuBuffer[1] << 24) | (htuBuffer[3] << 16) | (htuBuffer[5] << 8) | htuBuffer[7];
    } else {
        HTU21sn.HTU21D_snb = HTU21sn.HTU21D_crcb = 0;
    }
#endif
}

