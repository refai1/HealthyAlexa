//The compiled code with all characteristics and sensors

#include "mbed.h"
//For Gyro
#include "FXAS21002.h"
//For Accelerometer
#include "FXOS8700.h"
//For Thermometer
//#include "MPL3115A2.h"
#include "HTU21D.h"
//For Heart Rate
#include "MAX30101.h"
//For BLE
#include "Hexi_KW40Z.h"
//For OLED
#include "Hexi_OLED_SSD1351.h"
#include "OLED_types.h"
#include "OpenSans_Font.h"
#include "string.h"
#include "images.h"


//Define Characteristics
#define LED_ON      0
#define LED_OFF     1

void UpdateSensorData(void);
void StartHaptic(void);
void StopHaptic(void const *n);
void txTask(void);

//DigitalOut redLed(LED1,1);
//DigitalOut greenLed(LED2,1);
DigitalOut blueLed(LED3,1);
DigitalOut haptic(PTB9);

/* Define timer for haptic feedback */
RtosTimer hapticTimer(StopHaptic, osTimerOnce);



//HH
// Initialize Serial port
Serial pc(USBTX, USBRX);

// Pin connections for accelerometer 
FXOS8700 accel(PTC11, PTC10);

// Pin connections for thermometer
HTU21D temphumid(PTB1,PTB0); // HTU21D Sensor
DigitalOut powerEN (PTB12); // Power Enable HTU21D Sensor

// Pin connections for heart rate 
MAX30101 heart(PTB1, PTB0);

// Pin connections for Gyroscope
FXAS21002 gyro(PTC11,PTC10); //Note HH: same pin fix

/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 
KW40Z kw40z_device(PTE24, PTE25);

/* Instantiate the SSD1351 OLED Driver */ 
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); /* (MOSI,SCLK,POWER,CS,RST,DC) */

/*Create a Thread to handle sending BLE Sensor Data */ 
Thread txThread;

 /* Text Buffer */ 
char text[20]; 

/*Parameters to be changed based on function calls in the main*/
uint16_t humidity = 0;
uint16_t temperature = 0;
uint16_t x = 0;
uint16_t y = 0;
uint16_t z = 0;
//for temp
float sensor_data[2];
//for gyro
float gyro_data[3];  // Storage for the data from the sensor
float gyro_rms=0.0; // RMS value from the sensor
float gx, gy, gz; // Integer value from the sensor to be displayed
const uint8_t *image1; // Pointer for the image1 to be displayed
char text1[20]; // Text Buffer for dynamic value displayed
char text2[20]; // Text Buffer for dynamic value displayed
char text3[20]; // Text Buffer for dynamic value displayed
//for accelerometer
float accel_data[3]; 
float accel_rms=0.0;
/****************************Call Back Functions*******************************/
void ButtonRight(void)
{
    StartHaptic();
    kw40z_device.ToggleAdvertisementMode();
}

void ButtonLeft(void)
{
    StartHaptic();
    kw40z_device.ToggleAdvertisementMode();
}

void PassKey(void)
{
    StartHaptic();
    strcpy((char *) text,"PAIR CODE");
    oled.TextBox((uint8_t *)text,0,25,95,18);
  
    /* Display Bond Pass Key in a 95px by 18px textbox at x=0,y=40 */
    sprintf(text,"%d", kw40z_device.GetPassKey());
    oled.TextBox((uint8_t *)text,0,40,95,18);
    pc.printf("pass key\n");
}

/***********************End of Call Back Functions*****************************/

/********************************Main******************************************/

int main()
{    
    powerEN = 0;
    /* Register callbacks to application functions */
    kw40z_device.attach_buttonLeft(&ButtonLeft);
    kw40z_device.attach_buttonRight(&ButtonRight);
    kw40z_device.attach_passkey(&PassKey);


     pc.printf("keys passed\n");
    // CStart Heart Rate Measurement
    heart.enable();
    
    // Configure Accelerometer FXOS8700
    //accel.accel_config();
    
    // Configure Gyroscope FXAS21002    
    gyro.gyro_config();
    
    /* Setting pointer location of the 96 by 96 pixel bitmap */
    //image1  = Gyro;
    
    
    /* Turn on the backlight of the OLED Display */
    //oled.DimScreenON();
    
    /* Fills the screen with solid black */         
    oled.FillScreen(COLOR_BLACK);

    /* Get OLED Class Default Text Properties */
    oled_text_properties_t textProperties = {0};
    oled.GetTextProperties(&textProperties);    
        
    /* Change font color to Blue */ 
    textProperties.fontColor   = COLOR_BLUE;
    oled.SetTextProperties(&textProperties);
    
    /* Display Bluetooth Label at x=17,y=65 */ 
    strcpy((char *) text,"BLUETOOTH");
    oled.Label((uint8_t *)text,17,65);
    
    /* Change font color to white */ 
    textProperties.fontColor   = COLOR_WHITE;
    textProperties.alignParam = OLED_TEXT_ALIGN_CENTER;
    oled.SetTextProperties(&textProperties);
    
    /* Display Label at x=22,y=80 */ 
    strcpy((char *) text,"Tap Below");
    oled.Label((uint8_t *)text,22,80);
         
    uint8_t prevLinkState = 0; 
    uint8_t currLinkState = 0;
     
    txThread.start(txTask); /*Start transmitting Sensor Tag Data */
    pc.printf("tx task is happenning\n");
    image1  = TempHumid;
    
    /* Fill 96px by 96px Screen with 96px by 96px NXP Image starting at x=0,y=0 */
    oled.DrawImage(image1,0,0); 
    
    while (true) 
    {
        pc.printf("Bolean for led: %d\n", kw40z_device.GetAdvertisementMode());
        blueLed = !kw40z_device.GetAdvertisementMode(); /*Indicate BLE Advertisment Mode*/   
        Thread::wait(50);
    }
}

/******************************End of Main*************************************/


/* txTask() transmits the sensor data */
void txTask(void){
   
   while (true) 
   {
        
        
        UpdateSensorData();
        
        /*Notify Hexiwear App that it is running Sensor Tag mode*/
        kw40z_device.SendSetApplicationMode(GUI_CURRENT_APP_SENSOR_TAG);
                
        /*The following is sending sensor data over BLE*/
        
        /*Send Humidity*/
        kw40z_device.SendHumidity(humidity);
        pc.printf("H: %d\n", humidity); 
        
        /*Send Temperature*/
        kw40z_device.SendTemperature(temperature);
        pc.printf("T: %d \n", temperature); 
        
        /*Send Mag,Accel,Gyro Data. */
        kw40z_device.SendGyro(gx,gy,gz);
        pc.printf("gx: %f, gy: %f, gz: %f\n", gx, gy, gz); 
        
        //kw40z_device.SendAccel(x,y,z);
        //pc.printf("ax: %f, ay: %f, az: %f\n", x, y, z); 
        
        //pc.printf("Heartrate %f\r\n", heart.getTemp());

        Thread::wait(1000);                 
    }
}

void UpdateSensorData(void)
{    
    //Accelerometer Data aquisition
    //accel.acquire_accel_data_g(accel_data);
    //accel_rms = sqrt(((accel_data[0]*accel_data[0])+(accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))/3);
    //x = accel_data[0];
    //y = accel_data[1];
    //z = accel_data[2];
    
    ////Gyro Data aquisition
    gyro.acquire_gyro_data_dps(gyro_data);
    //gyro_rms = sqrt(((gyro_data[0]*gyro_data[0])+(gyro_data[1]*gyro_data[1])+(gyro_data[2]*gyro_data[2]))/3);
    gx = gyro_data[0];
    gy = gyro_data[1];
    gz = gyro_data[2];  
    
    //Temperature & Humidity Data aquisition  
    temperature = temphumid.sample_ctemp();//C
    humidity = temphumid.sample_humid();;
    

}

void StartHaptic(void)  {
    hapticTimer.start(50);
    haptic = 1;
}

void StopHaptic(void const *n) {
    haptic = 0;
    hapticTimer.stop();
}

