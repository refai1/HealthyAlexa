/*Author: Hannaneh Hojaiji
** Sensors used: Gyro, accel, humid, temp, heartrate, calorie
**EE202A
*/ 
//The compiled code with all characteristics and sensors
#include "mbed.h"
#include "mbed_events.h"
//For Gyro
#include "FXAS21002.h"
//For Accelerometer
#include "FXOS8700.h"
//For Thermometer
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
#define FIFO_DATA_MAX 288
//Define Characteristics
void UpdateSensorData(void);
void StartHaptic(void);
void StopHaptic(void const *n);
void txTask(void);

// Pin connections for heart rate 
DigitalOut pwr1v8(PTA29);
DigitalOut pwr3v3b(PTC13);
DigitalOut pwr15v(PTB12);
I2C i2c0(PTB1, PTB0);
InterruptIn maximInterrupt(PTB18);
// Initialize Serial port
Serial pc(USBTX, USBRX);

// Pin connections for accelerometer 
FXOS8700 accel(PTC11, PTC10);

// Pin connections for Gyroscope
FXAS21002 gyro(PTC11,PTC10); 


// Pin connections for thermometer
HTU21D temphumid(PTB1,PTB0); // HTU21D Sensor
DigitalOut powerEN (PTB12); // Power Enable HTU21D Sensor


/* Instantiate the SSD1351 OLED Driver */ 
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); /* (MOSI,SCLK,POWER,CS,RST,DC) */

/* Instantiate the Hexi KW40Z Driver (UART TX, UART RX) */ 
KW40Z kw40z_device(PTE24, PTE25);

DigitalOut haptic(PTB9);
EventQueue evqueue(32 * EVENTS_EVENT_SIZE);
Thread t;
/* Define timer for haptic feedback */
RtosTimer hapticTimer(StopHaptic, osTimerOnce);




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

//for accelerometer
float accel_data[3]; 
float accel_rms=0.0;
//for gyro
float gyro_data[3];  // Storage for the data from the sensor
float gyro_rms=0.0; // RMS value from the sensor
float gx, gy, gz; // Integer value from the sensor to be displayed
const uint8_t *image1; // Pointer for the image1 to be displayed
char text1[20]; // Text Buffer for dynamic value displayed
char text2[20]; // Text Buffer for dynamic value displayed
char text3[20]; // Text Buffer for dynamic value displayed

//Heartrate
/*Create a Thread to handle sending BLE Sensor Data */ 
MAX30101 hr(i2c0);
int realHeartRate;
float calorie;
int mask_ppg = 0;
uint32_t count = 0;
uint32_t num;
uint8_t testsignal = 60;
/****************************Call Back Functions*******************************/
void StartHaptic(void)  {
    hapticTimer.start(50);
    haptic = 1;
}

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

void StopHaptic(void const *n) {
    haptic = 0;  
    hapticTimer.stop();
}


/* txTask() transmits the sensor data */
void txTask(void){
   
   while (true) 
   {  
        UpdateSensorData();
        
        /*Notify Hexiwear App that it is running Sensor Tag mode*/
        kw40z_device.SendSetApplicationMode(GUI_CURRENT_APP_SENSOR_TAG);
        //send heartrate
        kw40z_device.SendHeartRate(testsignal);        
                
        /*The following is sending sensor data over BLE*/
        
        /*Send Humidity*/
        pc.printf("H: %d\n", humidity); 
        
        /*Send Temperature*/
        pc.printf("T: %d \n", temperature); 
        
        /*Send Mag,Accel,Gyro Data. */
        pc.printf("ax: %f, ay: %f, az: %f\n", accel_data[0], accel_data[1], accel_data[2]);         
        pc.printf("gx: %f, gy: %f, gz: %f\n", gyro_data[0], gyro_data[1], gyro_data[2]); 
        
        kw40z_device.SendHumidity(humidity);
        kw40z_device.SendTemperature(temperature);
        kw40z_device.SendAccel(x,y,z);
        kw40z_device.SendGyro(gx,gy,gz);
        

        Thread::wait(1000);                 
    }
}

void UpdateSensorData(void)
{    
    testsignal+=1;
    //Accelerometer Data aquisition
    accel.acquire_accel_data_g(accel_data);
    //accel_rms = sqrt(((accel_data[0]*accel_data[0])+(accel_data[1]*accel_data[1])+(accel_data[2]*accel_data[2]))/3);
    //x = accel_data[0];
    //y = accel_data[1];
    //z = accel_data[2];
    
    wait(0.005);
    ////Gyro Data aquisition
    gyro.acquire_gyro_data_dps(gyro_data);
    //gyro_rms = sqrt(((gyro_data[0]*gyro_data[0])+(gyro_data[1]*gyro_data[1])+(gyro_data[2]*gyro_data[2]))/3);
    //gx = gyro_data[0];
    //gy = gyro_data[1];
    //gz = gyro_data[2];  
    
    //Temperature & Humidity Data aquisition  
    temperature = temphumid.sample_ctemp();//C
    humidity = temphumid.sample_humid();;
}

void interruptHandlerQueued() {
    
    MAX30101::InterruptBitField_u interruptStatus;
    hr.getInterruptStatus(interruptStatus);
//    printf("Interrupt Status: 0x%02x\r\n", interruptStatus.all);
    
    if (interruptStatus.bits.pwr_rdy == 0x1) {
//        printf("Powered on\r\n");
        
        // Soft reset
        MAX30101::ModeConfiguration_u modeConf;
        modeConf.all = 0;
        modeConf.bits.reset = 1;
        hr.setModeConfiguration(modeConf);
        wait(0.01);
        
        // Configure FIFO
        MAX30101::FIFO_Configuration_u fifoConf;
        hr.getFIFOConfiguration(fifoConf);
//        pc.printf("FIFO Configuration: 0x%02x\r\n", fifoConf.all);
          
        // Set LED power
        hr.setLEDPulseAmplitude(MAX30101::LED1_PA, 0x0C);
        hr.setLEDPulseAmplitude(MAX30101::ProxModeLED_PA, 0x19);
//        pc.printf("LED set\r\n");
        
        MAX30101::SpO2Configuration_u spo2Conf;
        hr.getSpO2Configuration(spo2Conf);
        spo2Conf.bits.led_pw = MAX30101::PW_1;
        spo2Conf.bits.spo2_sr = MAX30101::SR_100_Hz;
        hr.setSpO2Configuration(spo2Conf);
        hr.getSpO2Configuration(spo2Conf);
//        pc.printf("SpO2 Configuration: 0x%02x\r\n", spo2Conf.all);
        
        // Proximity settings
        hr.setProxIntThreshold(0x14);
        
        // Enable HR mode
        modeConf.all = 0;
        modeConf.bits.mode = MAX30101::HeartRateMode;
        hr.setModeConfiguration(modeConf);
//        printf("Mode set\r\n");
    }
    
    if (interruptStatus.bits.prox_int == 0x1) {
//        printf("Proximity Triggered, entered HR Mode.");
    }
    
    if (interruptStatus.bits.ppg_rdy == 0x1) {
//        printf("PPG Ready.\r\n");
        mask_ppg = 1;
    }
    
    if (interruptStatus.bits.a_full == 0x1) {
//        printf("FIFO Almost Full.\r\n");
        uint8_t data[FIFO_DATA_MAX];
        uint16_t readBytes = 0;
       
        
        hr.readFIFO(MAX30101::OneLedChannel, data, readBytes);
        //printf("data length: %u \r\n",readBytes);
        //printf("data length: %u \r\n",data);
        for (uint16_t i = 0; i < readBytes; i += 3) {
            uint8_t sample[4] = {0};
            sample[0] = data[i + 2];
            sample[1] = data[i + 1];
            sample[2] = data[i];
            
            num = *(uint32_t *) sample;
            if (num < 310000){
                realHeartRate = 0;
                printf("keep closer to your hand \r\n");
                pc.printf("keep closer to your hand \r\n");
            }
            else {
                
                //realHeartRate = 65;
                realHeartRate = (num - 310000)/100;
                if (realHeartRate >45){
                    printf("%d\r\n", realHeartRate);
                    pc.printf("%d\r\n", realHeartRate);
                }
            }
            //printf("%u\r\n", num);
            
            
        }
    }
    
    interruptStatus.all = 0xFF;
    if (mask_ppg == 1) {
        interruptStatus.bits.ppg_rdy = 0;
    }
    hr.enableInterrupts(interruptStatus);
}

void interruptHandler() {
    evqueue.call(interruptHandlerQueued);
}


/***********************End of Call Back Functions*****************************/

/********************************Main******************************************/

int main()
{    
    t.start(callback(&evqueue, &EventQueue::dispatch_forever));
    /* Register callbacks to application functions */
    kw40z_device.attach_buttonLeft(&ButtonLeft);
    kw40z_device.attach_buttonRight(&ButtonRight);
    //kw40z_device.attach_passkey(&PassKey);
    
    pwr1v8 = 1;
    pwr3v3b = 1;
    pwr15v = 0;
    
    maximInterrupt.fall(interruptHandler);
    maximInterrupt.enable_irq();
    
    MAX30101::InterruptBitField_u interruptStatus;
    interruptStatus.all = 0xFF;
    hr.enableInterrupts(interruptStatus);


     pc.printf("keys passed\n");
    // CStart Heart Rate Measurement
    //heart.enable();
    
    // Configure Accelerometer FXOS8700
    accel.accel_config();
    
    wait(0.005);
    
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
         
     
    //txThread.start(txTask); /*Start transmitting Sensor Tag Data */
    pc.printf("tx task is happenning\n");
    image1  = TempHumid;
    
    /* Fill 96px by 96px Screen with 96px by 96px NXP Image starting at x=0,y=0 */
    oled.DrawImage(image1,0,0); 
    
    txThread.start(txTask);
    while (true) 
    {
        //pc.printf("Bolean for led: %d\n", kw40z_device.GetAdvertisementMode());
        //blueLed = !kw40z_device.GetAdvertisementMode(); /*Indicate BLE Advertisment Mode*/   
        /* Format the time reading */
       // sprintf(text,"%d",realHeartRate);
        pc.printf("Heartrate: %d\n",realHeartRate); 
        
        /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
        //oled.TextBox((uint8_t *)text,55,15,35,15); //Increase textbox for more digits
        if (realHeartRate > 45){
            calorie = (-55.1 + (0.6309*realHeartRate)+(0.1988*75)+(0.2017*25))*60/4.184 ;
            //sprintf(text,"%0.2f",calorie);
             pc.printf("calorie: %0.2f\n",calorie); 
            
            /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
            //oled.TextBox((uint8_t *)text,55,55,35,15); //Increase textbox for more digits
        }
        else{
            sprintf(text,"wait HR");
            pc.printf("wait HR\n"); 
            
            /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
            //oled.TextBox((uint8_t *)text,55,55,35,15);
        }
             
        Thread::wait(1000);
    }    
    //return 0;
}

/******************************End of Main*************************************/



