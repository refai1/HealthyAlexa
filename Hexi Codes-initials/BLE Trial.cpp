#include "mbed.h"
#include "Hexi_KW40Z.h"
#include "Hexi_OLED_SSD1351.h"
#include "OLED_types.h"
#include "OpenSans_Font.h"
#include "string.h"

#define LED_ON      0
#define LED_OFF     1

void UpdateSensorData(void);
void StartHaptic(void);
void StopHaptic(void const *n);
void txTask(void);

DigitalOut redLed(LED1,1);
DigitalOut greenLed(LED2,1);
DigitalOut blueLed(LED3,1);
DigitalOut haptic(PTB9);

/* Define timer for haptic feedback */
RtosTimer hapticTimer(StopHaptic, osTimerOnce);

/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 
KW40Z kw40z_device(PTE24, PTE25);

/* Instantiate the SSD1351 OLED Driver */ 
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); /* (MOSI,SCLK,POWER,CS,RST,DC) */

/*Create a Thread to handle sending BLE Sensor Data */ 
Thread txThread;

 /* Text Buffer */ 
char text[20]; 

uint8_t battery = 100;
uint8_t light = 0;
uint16_t humidity = 4500;
uint16_t temperature = 2000;
uint16_t pressure = 9000;
uint16_t x = 0;
uint16_t y = 5000;
uint16_t z = 10000;

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
}

/***********************End of Call Back Functions*****************************/

/********************************Main******************************************/

int main()
{    
    /* Register callbacks to application functions */
    kw40z_device.attach_buttonLeft(&ButtonLeft);
    kw40z_device.attach_buttonRight(&ButtonRight);
    kw40z_device.attach_passkey(&PassKey);

    /* Turn on the backlight of the OLED Display */
    oled.DimScreenON();
    
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
    
    while (true) 
    {
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
                
        /*The following is sending dummy data over BLE. Replace with real data*/
    
        /*Send Battery Level for 20% */ 
        kw40z_device.SendBatteryLevel(battery);
               
        /*Send Ambient Light Level at 50% */ 
        kw40z_device.SendAmbientLight(light);
        
        /*Send Humidity at 90% */
        kw40z_device.SendHumidity(humidity);
        
        /*Send Temperature at 25 degrees Celsius */
        kw40z_device.SendTemperature(temperature);

        /*Send Pressure at 100kPA */ 
        kw40z_device.SendPressure(pressure);
        
        /*Send Mag,Accel,Gyro Data. */
        kw40z_device.SendGyro(x,y,z);
        kw40z_device.SendAccel(z,x,y);
        kw40z_device.SendMag(y,z,x);

        Thread::wait(1000);                 
    }
}

void UpdateSensorData(void)
{    
    battery -= 5;
    if(battery < 5) battery = 100;
    
    light += 20;
    if(light > 100) light = 0;
    
    humidity += 500;
    if(humidity > 8000) humidity = 2000;
    
    temperature -= 200;
    if(temperature < 200) temperature = 4200;
    
    pressure += 300;
    if(pressure > 10300) pressure = 7500;
    
    x += 1400;
    y -= 2300;
    z += 1700;
}

void StartHaptic(void)  {
    hapticTimer.start(50);
    haptic = 1;
}

void StopHaptic(void const *n) {
    haptic = 0;
    hapticTimer.stop();
}

