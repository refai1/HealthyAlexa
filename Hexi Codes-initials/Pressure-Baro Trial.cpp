#include "mbed.h"
#include "MPL3115A2.h"

/*  Check out the full featured example application for interfacing to the 
 *  Barometer/Altimeter/Thermometer device at the following URL
 *  https://developer.mbed.org/users/clemente/code/WIGO_MPL3115A2/
*/
 
#define MPL3115A2_I2C_ADDRESS (0x60<<1)

DigitalOut led1(LED_GREEN);

// Initialize Serial port
Serial pc(USBTX, USBRX);

// Pin connections for Hexiwear
MPL3115A2 MPL3115A2( PTC11, PTC10, MPL3115A2_I2C_ADDRESS);
/* pos [0] = altimeter or pressure value */
/* pos [1] = temperature value */

float sensor_data[2];

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
    
   
    // Set over sampling value (see MPL3115A2.h for details)
    MPL3115A2.Oversample_Ratio(OVERSAMPLE_RATIO_64);
    // Configure the sensor as Barometer.
    MPL3115A2.Barometric_Mode();
    
    printf("\rBegin Data Acquisition from MPL3115A2 sensor....\r\n\r\n");
    wait(0.5);
    
    while(1) {
        MPL3115A2.getAllData(&sensor_data[0]);
        printf("\tPressure: %f\tTemperature: %f\r\n", sensor_data[0], sensor_data[1]);    
        led1 = !led1;
        Thread::wait(500);
    }
}

