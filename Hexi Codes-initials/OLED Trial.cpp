#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "string.h"

int main() {
    char text[20];  /* Text Buffer */ 
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
    strcpy((char *) text,"TEXT EXAMPLE");
    oled.Label((uint8_t *)text,7,0);
        
    /* Change font color to blue */ 
    textProperties.fontColor   = COLOR_BLUE;
    oled.SetTextProperties(&textProperties);

    /* Display text at (x=5,y=40) */ 
    strcpy(text,"Timer(s):");
    oled.Label((uint8_t *)text,5,40);
        
    /* Set text properties to white and right aligned for the dynamic text */
    textProperties.fontColor = COLOR_WHITE;
    textProperties.alignParam = OLED_TEXT_ALIGN_RIGHT;
    oled.SetTextProperties(&textProperties);    

    
    time.start(); /* start timer */

    while (true) {
        
        /* Format the time reading */
        sprintf(text,"%.2f",time.read());
        
        /* Display time reading in 35px by 15px textbox at(x=55, y=40) */
        oled.TextBox((uint8_t *)text,55,40,35,15); //Increase textbox for more digits
             
        Thread::wait(1000);
    }
}


    
    