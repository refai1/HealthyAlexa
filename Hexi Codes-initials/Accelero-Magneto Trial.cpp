#include "mbed.h"
#include "FXOS8700.h"

//  Check out the full featured example application for interfacing to the 
//  Accelerometer/Magnetometer device at the following URL
//  https://developer.mbed.org/users/trm/code/fxos8700cq_example/

DigitalOut led1(LED_GREEN);

// Initialize Serial port
Serial pc(USBTX, USBRX);

// Pin connections & address for Hexiwear
FXOS8700 accel(PTC11, PTC10);
FXOS8700 mag(PTC11, PTC10);

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {
    
    // Configure Accelerometer FXOS8700, Magnetometer FXOS8700
    accel.accel_config();
    mag.mag_config();

    float accel_data[3]; float accel_rms=0.0;
    float mag_data[3];   float mag_rms=0.0;

    printf("Begin Data Acquisition from FXOS8700CQ sensor....\r\n\r\n");
    wait(0.5);
    
    while (1) {
        led1 = !led1;
        // Example data printing
      
      accel.acquire_accel_data_g(accel_data);
      accel_rms = sqrt(((accel_data[0]*accel_data[0])+(accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))/3);
      printf("Accelerometer \tX-Axis %4.2f \tY-Axis %4.2f \tZ-Axis %4.2f \tRMS %4.2f\n\r",accel_data[0],accel_data[1],accel_data[2],accel_rms);
      wait(0.01);
      
      mag.acquire_mag_data_uT(mag_data);
      mag_rms = sqrt(((mag_data[0]*mag_data[0])+(mag_data[1]*mag_data[1])+(mag_data[2]*mag_data[2]))/3);
      printf("Magnetometer \tX-Axis %4.2f \tY-Axis %4.2f \tZ-Axis %4.2f \tRMS %4.2f\n\n\r",mag_data[0],mag_data[1],mag_data[2],mag_rms);
      wait(0.01);
        
        Thread::wait(500);
    }
}