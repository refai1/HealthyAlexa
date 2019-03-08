/** OLED Display Driver for Hexiwear 
 *  This file contains OLED driver functionality for drawing images and text
 *
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 *
 * Redistributions of source code must retain the above copyright notice, this list
 * of conditions and the following disclaimer.
 *
 * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 *
 * Neither the name of NXP, nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 *
 * visit: http://www.mikroe.com and http://www.nxp.com
 *
 * get support at: http://www.mikroe.com/forum and https://community.nxp.com
 *
 * Project HEXIWEAR, 2015
 */

#ifndef HG_HEXI_OLED_SSD1351
#define HG_HEXI_OLED_SSD1351

#include "mbed.h"
#include "OLED_types.h" 
#include "OLED_info.h"


/** OLED Display Driver for Hexiwear
*/
class SSD1351{

public:

 /**
  * Create a Hexiwear OLED Driver connected to the specified pins
  *
  * @param mosiPin SPI Master Out, Slave In Pin
  * @param sclkPin SPI CLock Pin
  * @param pwrPin  OLED Power Pin
  * @param csPin   OLED Chip Select Pin
  * @param rstPin  OLED Reset Pin
  * @param dcPin   OLED DC Pin
  *
  * @note Default TextProperties
  *
  *       .font       = OpenSans_10x15_Regular,    
  *       .fontColor  = COLOR_WHITE,
  *       .alignParam = OLED_TEXT_ALIGN_CENTER, 
  *       .background = NULL
  *
  */
  SSD1351(PinName mosiPin,PinName sclkPin,PinName pwrPin, PinName csPin,PinName rstPin, PinName dcPin);

  /**
  * Destroy the Hexiwear instance
  */   
  ~SSD1351(); 

  /**
   * Send the command to OLED
   * @param  cmd     OLED command from the datasheet
   * @param  isFirst designate if this is the first byte in the command
   */
  void SendCmd(uint32_t cmd,
                     uint8_t isFirst);


  /**
   * Send data to OLED
   * @param  dataToSend data to send to OLED
   * @param  dataSize   data-size
   */
  void SendData ( const uint8_t* dataToSend, 
                             uint32_t dataSize);

  /**
   * draw box on OLED
   * @param  xCrd   x-coordinate for box's uper left corner
   * @param  yCrd   y-coordinate for box's uper left corner
   * @param  width  box's width
   * @param  height box's height
   * @param  color  color of the box
   * @return        status flag
   */
  oled_status_t DrawBox (
                              int8_t xCrd,
                              int8_t yCrd,
                              uint8_t width,
                              uint8_t height,
                              uint16_t color
                            );

  /**
   * Fill the entire screen with specified color
   * @param  color color to fill with
   */
  void FillScreen( uint16_t color );



  /**
   * Draw a single pixel
   * @param  xCrd  pixel's x coordinate
   * @param  yCrd  pixel's y coordinate
   * @param  color pixel's color
   * @return       status flag
   */
  oled_status_t DrawPixel (
                                 int8_t xCrd,
                                 int8_t yCrd,
                                uint16_t color
                              );


  /**
   * Draw the whole screen
   * @param  image      image to draw
   * @param  xCrd       image x-coordinate
   * @param  yCrd       image y-coordinate
   * @param  width      image width
   * @param  height     image height
   * @param  transition transition style for the new image arrival
   * @return            status flag
   */
  oled_status_t DrawScreen (
                                     const uint8_t* image,
                                            int8_t xCrd,
                                            int8_t yCrd,
                                            uint8_t width,
                                            uint8_t height,
                                  oled_transition_t transition
                                );


  /**
   * Set the font to use
   * @param newFont  desired font
   * @param newColor desired color
   * @return         status flag
   */
  oled_status_t SetFont(
                              const uint8_t* newFont,
                                    uint16_t newColor
                            );


  /**
   * Set OLED dynamic area
   * @param dynamic_area data-structure with desired values
   */
  void SetDynamicArea(oled_dynamic_area_t *dynamic_area);

  /**
   * Destroy current OLED dynamic area
   */
  void DestroyDynamicArea();


  /**
   * Set OLED class text properties from parameter 
   * @param textProperties data-structure with desired properties
   */
  void SetTextProperties(oled_text_properties_t *textProperties);

  /**
   * Copy OLED class text properties to parameter
   * @param textProperties destination data-structure
   */
  void GetTextProperties(oled_text_properties_t *textProperties);

  /**
   * Return the width in [px] required for the given string to be displayed
   * @param  text desired string
   * @return      required text width in [px]
   */
  uint8_t GetTextWidth(const uint8_t* text);


  /**
   * Count the characters
   * @param  width  text width
   * @param  font   text font
   * @param  text   given text string
   * @param  length text length
   * @return        character count
   */
  uint8_t CharCount(uint8_t width, const uint8_t* font, const uint8_t* text, uint8_t length);

  /**
   * Add text to the main screen buffer at position x,y. 
   * @param text    text to add
   * @param xCrd    x-coordinate for the given text
   * @param yCrd    y-coordinate for the given text
   * @return        status flag
   */
  oled_status_t AddText( const uint8_t* text,int8_t xCrd, int8_t yCrd );
    /**
   * Add text to the main screen buffer. Used with SetDynamicArea() Function.
   * @param  text text to add
   * @return      status flag
   */
  oled_status_t AddText( const uint8_t* text);
 
  /**
   * Write text on OLED at position set in Dynamic Area Field. Used with SetDynamicArea() Function.
   * @param text desired text
   * 
   */
  oled_status_t DrawText ( const uint8_t* text);

  /**
   * Return the dimensions of image 
   * @param width  given image's width
   * @param height given image's height
   * @param image  desired image
   */
  void GetImageDimensions(uint8_t *width, uint8_t *height, const uint8_t* image);

  /**
   * Add image to the main screen buffer.Used with SetDynamicArea() Function.
   * @param  image      desired image
   * @return            status flag
   */
  oled_status_t AddImage ( const uint8_t* image );

    /**
   * Add image to the main screen buffer at position x,y 
   * @param  image      desired image
   * @param  xCrd       image x-coordinate
   * @param  yCrd       image y-coordinate
   * @return            status flag
   */
  oled_status_t AddImage ( const uint8_t* image, int8_t xCrd, int8_t yCrd );

  /**
   * Send image to OLED GRAM.Used with SetDynamicArea() Function for positioning image. 
   * @param  image      desired image
   * @return            status flag
   */
  oled_status_t DrawImage ( const uint8_t* image );
    /**
   * Send image to OLED GRAM at position x,y.
   * @param  image      desired image
   * @param  xCrd       image x-coordinate
   * @param  yCrd       image y-coordinate
   * @return            status flag
   */
  oled_status_t DrawImage ( const uint8_t* image, int8_t xCrd, int8_t yCrd );

  /**
   * Dim OLED screen on
   */
  void DimScreenON();

  /**
   * Return OLED back to full contrast
   */
  void DimScreenOFF();

  /**
   * Swap image's bytes per pixel to obtain the correct color format
   * @param imgDst  desired image
   * @param imgSrc  original image
   * @param imgSize image's size
   */
  void Swap(
                      oled_pixel_t imgDst,
                    const uint8_t* imgSrc,
                          uint16_t imgSize
                  );
    

  /**
   * Turn on Power for OLED Display
   */              
  void PowerON();

  /**
   * Turn off Power for OLED Display
   */ 
  void PowerOFF(); 

  /**
   * update the main screen buffer
   * with the given image

   * @param  xCrd       image x-coordinate
   * @param  yCrd       image y-coordinate
   * @param  width      image width
   * @param  height     image height
   * @param  image      image for buffer
   */
  void UpdateBuffer (
                                int8_t xCrd,
                                int8_t yCrd,
                                uint8_t width,
                                uint8_t height,
                                const uint8_t* image
                              );


    /**
   * Write text on OLED at position x,y. Recommended for Static Text. 
   * @param text    desired text
   * @param xCrd    x-coordinate for the given text
   * @param yCrd    y-coordinate for the given text
   */

  oled_status_t Label(const uint8_t* text,
                            int8_t xCrd, 
                            int8_t yCrd );

  /**
  * Create a text box of width,height at position x,y. Recommended for Dynamic Text.
  * Text is aligned in textbox accordingly to the align parameter set by SetTextProperties(). 
  * @param text    desired text
  * @param xCrd    x-coordinate for the textbox
  * @param yCrd    y-coordinate for the textbox
  * @param width   width of the textbox
  * @param height  height of the textbox 
  */
  
  oled_status_t TextBox(const uint8_t* text,
                              int8_t xCrd, 
                              int8_t yCrd,
                              uint8_t width,
                              uint8_t height);


private:

  SPI spi;
  DigitalOut power;
  DigitalOut cs;
  DigitalOut rst; 
  DigitalOut dc;

 
  const uint8_t* selectedFont;

  uint8_t
    currentChar_width,
    currentChar_height,
    screenBuf[OLED_GRAM_SIZE];

  uint16_t
    selectedFont_color,
    selectedFont_firstChar, /* first character in the font table */
    selectedFont_lastChar,  /* last character in the font table */
    selectedFont_height,
    colorMask;

  oled_dynamic_area_t oled_dynamic_area;
  oled_text_properties_t oled_text_properties;

  
  /* Internal Functions */
  void Transpose( oled_pixel_t transImage, const oled_pixel_t image, uint8_t width, uint8_t height );
  oled_status_t TopDown   ( const uint8_t* image, int8_t xCrd, int8_t yCrd, uint8_t width, uint8_t height );
  oled_status_t DownTop   ( const uint8_t* image, int8_t xCrd, int8_t yCrd, uint8_t width, uint8_t height );
  oled_status_t LeftRight ( const uint8_t* image, int8_t xCrd, int8_t yCrd, uint8_t width, uint8_t height );
  oled_status_t RightLeft ( const uint8_t* image, int8_t xCrd, int8_t yCrd, uint8_t width, uint8_t height );
  void SetBorders( int8_t xCrd, int8_t yCrd, uint8_t width, uint8_t height );
  oled_status_t CreateTextBackground();
  void WriteCharToBuf( uint16_t charToWrite, oled_pixel_t* chrBuf );
  oled_status_t AddCharToTextArea( oled_pixel_t chrPtr, uint8_t chrWidth, uint8_t chrHeight, oled_pixel_t copyAddr, uint8_t imgWidth );
  void* AllocateDynamicArea( uint32_t area );
  oled_status_t DestroyDynamicArea(void * ptr);

};

#endif           
