#include "mbed.h"
#include "HTU21D.h"
#include "Hexi_OLED_SSD1351.h"
#include "images.h"
#include "string.h"

// Pin connections
DigitalOut led1(LED_GREEN); // RGB LED
Serial pc(USBTX, USBRX); // Serial interface
HTU21D temphumid(PTB1,PTB0); // HTU21D Sensor
DigitalOut powerEN (PTB12); // Power Enable HTU21D Sensor
SSD1351 oled(PTB22,PTB21,PTC13,PTB20,PTE6, PTD15); // SSD1351 OLED Driver (MOSI,SCLK,POWER,CS,RST,DC)

// Variables
int sample_ftemp;
int sample_ctemp;
int sample_ktemp;
int sample_humid;
const uint8_t *image1; // Pointer for the image to be displayed
char text[20]; // Text Buffer for dynamic value displayed

int main() {
    powerEN = 0;
    
    /* Setting pointer location of the 96 by 96 pixel bitmap */
    image1  = TempHumid;

    /* Turn on the backlight of the OLED Display */
    // oled.DimScreenON();
    
    /* Fill 96px by 96px Screen with 96px by 96px NXP Image starting at x=0,y=0 */
    oled.DrawImage(image1,0,0);  
    
    while(true) {

        sample_ftemp = temphumid.sample_ftemp();
        printf("Temperature: %d F\n\r", sample_ftemp);

        sample_ctemp = temphumid.sample_ctemp();
        printf("Temperature: %d C\n\r", sample_ctemp);

        sample_ktemp = temphumid.sample_ktemp();
        printf("Temperature: %d K\n\r", sample_ktemp);

        sample_humid = temphumid.sample_humid();
        printf("Humidity: %d %%\n\r", sample_humid);
        printf("\n\r");
        
        /* Get OLED Class Default Text Properties */
        oled_text_properties_t textProperties = {0};
        oled.GetTextProperties(&textProperties);

        /* Set text properties to white and right aligned for the dynamic text */
        textProperties.fontColor = COLOR_RED;
        textProperties.alignParam = OLED_TEXT_ALIGN_RIGHT;
        oled.SetTextProperties(&textProperties);  
      
        /* Display Legends */
        strcpy((char *) text,"Temp.");
        oled.Label((uint8_t *)text,5,67);      
      
        /* Format the value */
        sprintf(text,"%i",sample_ctemp);
        /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
        oled.TextBox((uint8_t *)text,57,67,20,15); //Increase textbox for more digits
        
        /* Display Units */
        strcpy((char *) text,"dC");
        oled.Label((uint8_t *)text,82,67);     
      
        /* Set text properties to white and right aligned for the dynamic text */ 
        textProperties.fontColor = COLOR_BLUE;
        textProperties.alignParam = OLED_TEXT_ALIGN_RIGHT;
        oled.SetTextProperties(&textProperties);  
      
        /* Display Legends */
        strcpy((char *) text,"Humidity");
        oled.Label((uint8_t *)text,5,81);       
      
        /* Format the value */
        sprintf(text,"%i",sample_humid);
        /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
        oled.TextBox((uint8_t *)text,57,81,20,15); //Increase textbox for more digits
        
        /* Display Units */
        strcpy((char *) text,"%");
        oled.Label((uint8_t *)text,82,81); 
        
        led1 = !led1;
        Thread::wait(500);
    }
}
