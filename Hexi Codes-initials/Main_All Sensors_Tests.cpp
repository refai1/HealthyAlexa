#include "mbed.h"
//For Accelerometer
#include "FXOS8700.h"

//For OLED 
#include "Hexi_OLED_SSD1351.h"
#include "string.h"



//Gyro Inits
DigitalOut led1(LED_GREEN);
// Initialize Serial port
Serial pc(USBTX, USBRX);

// Pin connections & address for Hexiwear
FXOS8700 accel(PTC11, PTC10);


//BLE Initialization
/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 
//KW40Z kw40z_device(PTE24, PTE25);
/*Create a Thread to handle sending BLE Sensor Data */ 
//Thread txThread;  

// main() runs in its own thread in the OS
// (note the calls to Thread::wait below for delays)
int main() {

// Configure Accelerometer FXOS8700, Magnetometer FXOS8700
    accel.accel_config();

    float accel_data[3]; float accel_rms=0.0;

    printf("Begin Data Acquisition from FXOS8700CQ sensor....\r\n\r\n");
    wait(0.5);

    
//OLED Text Code   
    char text[100];  /* Text Buffer */ 
    Timer time;     /* Instantiate Time */

    /* Instantiate the SSD1351 OLED Driver */ 
    SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); /* (MOSI,SCLK,POWER,CS,RST,DC) */
    
    /* Get OLED Class Default Text Properties */
    oled_text_properties_t textProperties = {0};
    oled.GetTextProperties(&textProperties);    

    /* Turn on the backlight of the OLED Display */
    oled.DimScreenON();
    
    /* Fills the screen with solid black */         
    oled.FillScreen(COLOR_BLACK);

    /* Display Text at (x=7,y=0) */
    strcpy((char *) text,"Sensor Readings");
    oled.Label((uint8_t *)text,5,0);
        
    /* Change font color to blue */ 
    textProperties.fontColor   = COLOR_BLUE;
    oled.SetTextProperties(&textProperties);

    /* Display text at (x=5,y=40) */ 
    //strcpy(text,"Timer(s):");
    //oled.Label((uint8_t *)text,5,40);
        
    /* Set text properties to white and right aligned for the dynamic text */
    textProperties.fontColor = COLOR_WHITE;
    textProperties.alignParam = OLED_TEXT_ALIGN_RIGHT;
    oled.SetTextProperties(&textProperties);    
    
    time.start(); /* start timer */
    
////// Looping     
    while (1) {
        
        // Accelerometer      
        led1 = !led1;
        
        //Data aquisition
        accel.acquire_accel_data_g(accel_data);
        accel_rms = sqrt(((accel_data[0]*accel_data[0])+(accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))/3);
        wait(1);
        
        /* Display Accelerometer x-axis reading in 35px by 15px textbox at(x=55, y=40) */
        sprintf(text,"Accelerometer X-Axis");
        oled.TextBox((uint8_t *)text,5,20,70,30);
        sprintf(text,"%4.2f",accel_data[0]);
        oled.TextBox((uint8_t *)text,35,20,35,15); //Increase textbox for more digits
        
        /* Display Accelerometer y-axis reading in 35px by 15px textbox at(x=55, y=40) */
        sprintf(text,"Accelerometer Y-Axis");
        oled.TextBox((uint8_t *)text,5,20,35,15);
        sprintf(text,"%4.2f",accel_data[1]);
        oled.TextBox((uint8_t *)text,35,35,35,15); //Increase textbox for more digits
        wait(1);
        
        /* Display Accelerometer Z-axis reading in 35px by 15px textbox at(x=55, y=40) */
        sprintf(text,"Accelerometer Z-Axis");
        oled.TextBox((uint8_t *)text,5,20,35,15);
        sprintf(text,"%4.2f",accel_data[2]);
        oled.TextBox((uint8_t *)text,35,50,35,15); //Increase textbox for more digits
        wait(1);
        
        /* Display Accelerometer RMS reading in 35px by 15px textbox at(x=55, y=40) */
        sprintf(text,"Accelerometer RMS");
        oled.TextBox((uint8_t *)text,5,20,35,15);
        sprintf(text,"%4.2f",accel_rms);
        oled.TextBox((uint8_t *)text,35,65,35,15); //Increase textbox for more digits   
        
        Thread::wait(500);
    }
}




