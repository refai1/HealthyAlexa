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

#include "OLED_types.h"
#include "OLED_info.h"
#include "mbed.h"
#include "Hexi_OLED_SSD1351.h"
#include "OpenSans_Font.h"
   
const init_cmd_t seq[] = {
    OLED_CMD_SET_CMD_LOCK,  CMD_BYTE,
    OLED_UNLOCK,            DATA_BYTE,
    OLED_CMD_SET_CMD_LOCK,  CMD_BYTE,
    OLED_ACC_TO_CMD_YES,    DATA_BYTE,
    OLED_CMD_DISPLAYOFF,    CMD_BYTE,
    OLED_CMD_SET_OSC_FREQ_AND_CLOCKDIV, CMD_BYTE,
    0xF1,                   DATA_BYTE,
    OLED_CMD_SET_MUX_RATIO, CMD_BYTE,
    0x5F,                   DATA_BYTE,
    OLED_CMD_SET_REMAP,     CMD_BYTE,
    OLED_REMAP_SETTINGS,    DATA_BYTE,
    OLED_CMD_SET_COLUMN,    CMD_BYTE,
    0x00,                   DATA_BYTE,
    0x5F,                   DATA_BYTE,
    OLED_CMD_SET_ROW,       CMD_BYTE,
    0x00,                   DATA_BYTE,
    0x5F,                   DATA_BYTE,
    OLED_CMD_STARTLINE,     CMD_BYTE,
    0x80,                   DATA_BYTE,
    OLED_CMD_DISPLAYOFFSET, CMD_BYTE,
    0x60,                   DATA_BYTE,
    OLED_CMD_PRECHARGE,     CMD_BYTE,
    0x32,                   CMD_BYTE,
    OLED_CMD_VCOMH,         CMD_BYTE,
    0x05,                   CMD_BYTE,
    OLED_CMD_NORMALDISPLAY, CMD_BYTE,
    OLED_CMD_CONTRASTABC,   CMD_BYTE,
    0x8A,                   DATA_BYTE,
    0x51,                   DATA_BYTE,
    0x8A,                   DATA_BYTE,
    OLED_CMD_CONTRASTMASTER, CMD_BYTE,
    0xCF,                   DATA_BYTE,
    OLED_CMD_SETVSL,        CMD_BYTE,
    0xA0,                   DATA_BYTE,
    0xB5,                   DATA_BYTE,
    0x55,                   DATA_BYTE,
    OLED_CMD_PRECHARGE2,    CMD_BYTE,
    0x01,                   DATA_BYTE,
    OLED_CMD_DISPLAYON,     CMD_BYTE
    };


    
SSD1351::SSD1351(PinName mosiPin,PinName sclkPin,PinName pwrPin, PinName csPin,PinName rstPin, PinName dcPin): spi(mosiPin,NC,sclkPin) , power(pwrPin), cs(csPin),rst(rstPin),dc(dcPin) 
{

    spi.frequency(8000000);

    
    dc =0;
    PowerOFF();
    wait_ms(1);
    rst = 0 ; 
    wait_ms(1);
    rst = 1 ;
    wait_ms(1);
    PowerON();
    
    currentChar_width  = 0,
    currentChar_height = 0;
    colorMask = COLOR_WHITE;
    
    oled_text_properties.alignParam = OLED_TEXT_ALIGN_CENTER;
    oled_text_properties.background = NULL;
    oled_text_properties.font       = OpenSans_10x15_Regular;
    oled_text_properties.fontColor  = COLOR_WHITE;
    SetTextProperties(&oled_text_properties);
        
    oled_dynamic_area.areaBuffer = NULL;
    oled_dynamic_area.height = 0;
    oled_dynamic_area.width = 0;
    oled_dynamic_area.xCrd = 0;
    oled_dynamic_area.yCrd = 0;
   
   
    for (int i=0;i<39;i++) 
    {
       SendCmd(seq[i].cmd, seq[i].type);
    }



} 


SSD1351::~SSD1351(void)
{
  //TO_DO
  //Run Free and zero pointers. 
  
}



 
void SSD1351::SendCmd(uint32_t cmd,
                   uint8_t isFirst)
{
  
  
  
  uint8_t
     txSize = 1,
     txBuf[4];

  memcpy((void*)txBuf, (void*)&cmd, txSize );

  if (isFirst )
  {
    dc = 0;   
  }
  else
  {
    dc = 1;
  }
  
    cs = 0;
    spi.write(*txBuf);
    cs  = 1; 
   
}



void SSD1351::SendData ( const uint8_t* dataToSend, 
                           uint32_t dataSize)
                            
{
    

  uint16_t* arrayPtr = (uint16_t*)dataToSend;

  for( uint32_t i = 0; i < dataSize/2; i++ )
  {
      arrayPtr[i] &= colorMask;
  }
  

  SendCmd( OLED_CMD_WRITERAM, CMD_BYTE );
  
  /* sending data -> set DC pin */
  dc = 1;
  cs = 0 ;

  const uint8_t*
  /* traversing pointer */ 
  bufPtr = dataToSend;


  for ( uint32_t i = 0; i < dataSize; i++)
  {
      spi.write(*bufPtr);
      bufPtr += 1;
  }
  
  cs = 1;
 
}


oled_status_t SSD1351::DrawBox (
                            int8_t xCrd,
                            int8_t yCrd,
                            uint8_t width,
                            uint8_t height,
                            uint16_t color
                          )
{
  
  oled_status_t status;  
  oled_dynamic_area_t boxArea;

  boxArea.xCrd   = xCrd;
  boxArea.yCrd   = yCrd;
  boxArea.width  = width;
  boxArea.height = height;

  uint32_t
    boxSize = width*height;

  SetDynamicArea( &boxArea );

  /* helper pointer */
  uint8_t*
    boxBuf = (uint8_t*)oled_dynamic_area.areaBuffer;

  if ( NULL == boxBuf )
  {
  
    return OLED_STATUS_ERROR;
  }

  /* check the bounds */ 
  if AreCoordsNotValid( xCrd, yCrd, width, height )
  {
    status = OLED_STATUS_INIT_ERROR;
  }

  else
  {
    /* fill the buffer with color */

    for ( uint16_t i = 0; i < boxSize; i++ )
    {
      boxBuf[ 2*i ]     = color >> 8;
      boxBuf[ 2*i + 1 ] = color;
    }

    /* set the locations */

    /* adjust for the offset */
    OLED_AdjustColumnOffset(xCrd);
    OLED_AdjustRowOffset(yCrd);

    SendCmd( OLED_CMD_SET_COLUMN, CMD_BYTE);   
    SendCmd( xCrd, DATA_BYTE );  
    SendCmd( xCrd + (width-1), DATA_BYTE );
    SendCmd( OLED_CMD_SET_ROW, CMD_BYTE );
    SendCmd( yCrd, DATA_BYTE );
    SendCmd( yCrd + (height-1), DATA_BYTE );
 
    /* fill the GRAM */
    SendData( (uint8_t*)boxBuf, boxSize*OLED_BYTES_PER_PIXEL );
    DestroyDynamicArea();
   
  }

  return status;
}

/**
 * fill the entire screen
 * @param  color color to fill with
 * @return status flag
 */
void SSD1351::FillScreen( uint16_t color )
{
  /* fill the screen buffer with color */
  for ( uint16_t i = 0; i < ( OLED_SCREEN_WIDTH * OLED_SCREEN_HEIGHT ); i++ )
  {
    screenBuf[ 2*i ]     = color >> 8;
    screenBuf[ 2*i + 1 ] = color;
  }

  /* set the locations */
  SetBorders( 0, 0, OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT );

  /* fill GRAM */
  SendData( (uint8_t*)screenBuf, OLED_SCREEN_WIDTH * OLED_SCREEN_HEIGHT * OLED_BYTES_PER_PIXEL  );

}



oled_status_t SSD1351::DrawPixel (
                               int8_t xCrd,
                               int8_t yCrd,
                              uint16_t color
                            )
{
  /* check the bounds */
  if AreCoordsNotValid( xCrd, yCrd, 1, 1 )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  else
  {
    /* set directions */
    SetBorders( xCrd, yCrd, OLED_SCREEN_WIDTH, OLED_SCREEN_HEIGHT);

    uint16_t
      /* swap bytes */
      dot = color;

    OLED_SwapMe(dot);

    /* fill the GRAM */
    SendData( (uint8_t*)&dot, 2 );
    
    return OLED_STATUS_SUCCESS;
  }
}


oled_status_t SSD1351::DrawScreen (
                                   const uint8_t* image,
                                          int8_t xCrd,
                                          int8_t yCrd,
                                          uint8_t width,
                                          uint8_t height,
                                oled_transition_t transition
                              )
{
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  if AreCoordsNotValid( xCrd, yCrd, width, height )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  switch ( transition )
  {
    case OLED_TRANSITION_NONE:        {
                                        /* set the locations */
                                        SetBorders( xCrd, yCrd, width, height);

                                        /* fill the GRAM */
                                        SendData( (const uint8_t*)image, width * height * OLED_BYTES_PER_PIXEL );                                        
                                        break;
                                      }

    case OLED_TRANSITION_TOP_DOWN:    {
                                        TopDown( image, xCrd, yCrd, width, height );
                                        break;
                                      }

    case OLED_TRANSITION_DOWN_TOP:    {
                                        DownTop( image, xCrd, yCrd, width, height );
                                        break;
                                      }

    case OLED_TRANSITION_LEFT_RIGHT:  {
                                        LeftRight( image, xCrd, yCrd, width, height );
                                        break;
                                      }

    case OLED_TRANSITION_RIGHT_LEFT:  {
                                        RightLeft( image, xCrd, yCrd, width, height );
                                        break;
                                      }

    default: {}
  }

  return status;
}


oled_status_t SSD1351::SetFont(
                            const uint8_t* newFont,
                                  uint16_t newColor
                          )
{
  /* save the new values in intern variables */

  selectedFont           = newFont;
  selectedFont_firstChar = newFont[2] | ( (uint16_t)newFont[3] << 8 );
  selectedFont_lastChar  = newFont[4] | ( (uint16_t)newFont[5] << 8 );
  selectedFont_height    = newFont[6];
  selectedFont_color     = newColor;

  OLED_SwapMe( selectedFont_color );

  return OLED_STATUS_SUCCESS;
}


void SSD1351::SetDynamicArea(oled_dynamic_area_t *dynamic_area)
{

  if( NULL == oled_dynamic_area.areaBuffer )
  {
      oled_dynamic_area.areaBuffer = (oled_pixel_t)AllocateDynamicArea( dynamic_area->width * dynamic_area->height );
  }
  else if(
          ( dynamic_area->height != oled_dynamic_area.height ) ||
          ( dynamic_area->width != oled_dynamic_area.width )
         )
  {
    DestroyDynamicArea();
    oled_dynamic_area.areaBuffer = (oled_pixel_t)AllocateDynamicArea( dynamic_area->width * dynamic_area->height );
  }

  oled_dynamic_area.xCrd   = dynamic_area->xCrd;
  oled_dynamic_area.yCrd   = dynamic_area->yCrd;
  oled_dynamic_area.width  = dynamic_area->width;
  oled_dynamic_area.height = dynamic_area->height;
}


void SSD1351::DestroyDynamicArea()
{
    if ( NULL != oled_dynamic_area.areaBuffer )
    {
        DestroyDynamicArea( oled_dynamic_area.areaBuffer );
        oled_dynamic_area.areaBuffer = NULL;
    }
}


void SSD1351::SetTextProperties(oled_text_properties_t *textProperties)
{
  oled_text_properties.font       = textProperties->font;
  oled_text_properties.fontColor  = textProperties->fontColor;
  oled_text_properties.alignParam = textProperties->alignParam;
  oled_text_properties.background = textProperties->background;

  SetFont( oled_text_properties.font, oled_text_properties.fontColor );
}

void SSD1351::GetTextProperties(oled_text_properties_t *textProperties)
{
  textProperties->font        = oled_text_properties.font;      
  textProperties->fontColor   = oled_text_properties.fontColor;  
  textProperties->alignParam  = oled_text_properties.alignParam;
  textProperties->background  = oled_text_properties.background;
}

uint8_t SSD1351::GetTextWidth(const uint8_t* text)
{
    uint8_t chrCnt = 0;
    uint8_t text_width  = 0;

    while ( 0 != text[chrCnt] )
    {
        text_width += *( selectedFont + 8 + (uint16_t)( ( text[chrCnt++] - selectedFont_firstChar ) << 2 ) );
        /* make 1px space between chars */
        text_width++;
    }
    /* remove the final space */
    text_width--;

    return text_width;
}



uint8_t SSD1351::CharCount(uint8_t width, const uint8_t* font, const uint8_t* text, uint8_t length)
{
    uint8_t chrCnt = 0;
    uint8_t text_width  = 0;
    uint16_t firstChar;

    firstChar = font[2] | ( (uint16_t)font[3] << 8 );

    while ( chrCnt < length )
    {
        text_width += *( font + 8 + (uint16_t)( ( text[chrCnt++] - firstChar ) << 2 ) );
        if(text_width > width)
        {
            chrCnt--;
            break;
        }
        /* make 1px space between chars */
        text_width++;
    }

    return chrCnt;
}




oled_status_t SSD1351::AddText( const uint8_t* text,int8_t xCrd, int8_t yCrd )
{
    uint16_t
        chrCnt = 0;
        oled_pixel_t
        chrBuf = NULL;

    uint8_t
        currentChar_x = 0,
        currentChar_y = 0;

    uint8_t
        text_height = 0,
        text_width  = 0;

    text_width = GetTextWidth(text);

    /*
     * set default values, if necessary
     */

    text_height = selectedFont_height;
    oled_dynamic_area_t textArea;

    textArea.width  = text_width;
    textArea.height = text_height;
    textArea.xCrd = xCrd;
    textArea.yCrd = yCrd; 
    SetDynamicArea(&textArea);

    currentChar_y = ( oled_dynamic_area.height - text_height ) >> 1;

    switch ( oled_text_properties.alignParam & OLED_TEXT_HALIGN_MASK )
    {
        case OLED_TEXT_ALIGN_LEFT:
        {
            currentChar_x = 0;
            break;
        }

        case OLED_TEXT_ALIGN_RIGHT:
        {
            currentChar_x = ( oled_dynamic_area.width - text_width );
            break;
        }

        case OLED_TEXT_ALIGN_CENTER:
        {
            currentChar_x += ( oled_dynamic_area.width - text_width ) >> 1 ;
            break;
        }

        case OLED_TEXT_ALIGN_NONE:
        {
            break;
        }

        default: {}
    }

    if ( CreateTextBackground() != OLED_STATUS_SUCCESS )
    {
        return OLED_STATUS_ERROR;
    }

    /*
    * write the characters into designated space, one by one
    */

    chrCnt = 0;
    while ( 0 != text[chrCnt] )
    {
        WriteCharToBuf( text[chrCnt++], &chrBuf );

        if ( NULL == chrBuf )
        {
          return OLED_STATUS_INIT_ERROR;
        }

        else
        {
          if  (
                        ( ( currentChar_x + currentChar_width )  > oled_dynamic_area.width )
                    ||  ( ( currentChar_y + currentChar_height ) > oled_dynamic_area.height )
              )
          {
            DestroyDynamicArea( chrBuf );
            chrBuf = NULL;
            return OLED_STATUS_ERROR;
          }

          /* copy data */
          oled_pixel_t
              copyAddr = oled_dynamic_area.areaBuffer + ( currentChar_y * oled_dynamic_area.width + currentChar_x );
              
          AddCharToTextArea( chrBuf, currentChar_width, currentChar_height, copyAddr, oled_dynamic_area.width );

          currentChar_x += ( currentChar_width+1 );
          currentChar_y += 0;

          DestroyDynamicArea( chrBuf );
          chrBuf = NULL;
        }
    }

    UpdateBuffer(
                                        oled_dynamic_area.xCrd,
                                        oled_dynamic_area.yCrd,
                                        oled_dynamic_area.width,
                                        oled_dynamic_area.height,
                                        (const uint8_t*)oled_dynamic_area.areaBuffer
                                    );

    return OLED_STATUS_SUCCESS;
}


oled_status_t SSD1351::AddText( const uint8_t* text)
{
    uint16_t
        chrCnt = 0;
        oled_pixel_t
        chrBuf = NULL;

    uint8_t
        currentChar_x = 0,
        currentChar_y = 0;

    uint8_t
        text_height = 0,
        text_width  = 0;

    text_width = GetTextWidth(text);

    /**
     * set default values, if necessary
     */

    text_height = selectedFont_height;
  
    /* Disable this for now.Generated Fontwidth supposedly a lot wider than it really is*/
    #if 0
   if  (( oled_dynamic_area.width  < text_width )||( oled_dynamic_area.height < text_height ))
   {
        oled_dynamic_area_t textArea;
        textArea.width  = text_width;
        textArea.height = text_height;
        SetDynamicArea(&textArea);
   }
   #endif 
  
    currentChar_y = ( oled_dynamic_area.height - text_height ) >> 1;

    switch ( oled_text_properties.alignParam & OLED_TEXT_HALIGN_MASK )
    {
        case OLED_TEXT_ALIGN_LEFT:
        {
            currentChar_x = 0;
            break;
        }

        case OLED_TEXT_ALIGN_RIGHT:
        {
            currentChar_x = ( oled_dynamic_area.width - text_width );
            break;
        }

        case OLED_TEXT_ALIGN_CENTER:
        {
            currentChar_x += ( oled_dynamic_area.width - text_width ) >> 1 ;
            break;
        }

        case OLED_TEXT_ALIGN_NONE:
        {
            break;
        }

        default: {}
    }

    if ( CreateTextBackground() != OLED_STATUS_SUCCESS )
    {
        return OLED_STATUS_ERROR;
    }

    /**
    * write the characters into designated space, one by one
    */

    chrCnt = 0;
    while ( 0 != text[chrCnt] )
    {
        WriteCharToBuf( text[chrCnt++], &chrBuf );

        if ( NULL == chrBuf )
        {
          return OLED_STATUS_INIT_ERROR;
        }

        else
        {
          if  (
                        ( ( currentChar_x + currentChar_width )  > oled_dynamic_area.width )
                    ||  ( ( currentChar_y + currentChar_height ) > oled_dynamic_area.height )
              )
          {
            DestroyDynamicArea( chrBuf );
            chrBuf = NULL;
            return OLED_STATUS_ERROR;
          }

          /* copy data */ 
          oled_pixel_t
              copyAddr = oled_dynamic_area.areaBuffer + ( currentChar_y * oled_dynamic_area.width + currentChar_x );
              
          AddCharToTextArea( chrBuf, currentChar_width, currentChar_height, copyAddr, oled_dynamic_area.width );

          currentChar_x += ( currentChar_width+1 );
          currentChar_y += 0;

          DestroyDynamicArea( chrBuf );
          chrBuf = NULL;
        }
    }

    UpdateBuffer(
                                        oled_dynamic_area.xCrd,
                                        oled_dynamic_area.yCrd,
                                        oled_dynamic_area.width,
                                        oled_dynamic_area.height,
                                        (const uint8_t*)oled_dynamic_area.areaBuffer
                                    );

    return OLED_STATUS_SUCCESS;
}



oled_status_t SSD1351::DrawText( const uint8_t* text)
{

  if ( NULL == text )
  {
    return OLED_STATUS_ERROR;
  }

  AddText(text);
  /* set the locations */
  SetBorders( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height );
  /* fill the GRAM */
  SendData( (const uint8_t*)oled_dynamic_area.areaBuffer, oled_dynamic_area.width * oled_dynamic_area.height * OLED_BYTES_PER_PIXEL );

  //free( currentTextAreaImage );
  return OLED_STATUS_SUCCESS;
}


void SSD1351::GetImageDimensions(uint8_t *width, uint8_t *height, const uint8_t* image)
{
  *height = image[2] + (image[3] << 8);
  *width  = image[4] + (image[5] << 8);
}


oled_status_t SSD1351::AddImage ( const uint8_t* image )
{
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  /* check the bounds */
  if AreCoordsNotValid( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height )
  {
    status = OLED_STATUS_INIT_ERROR;
  }

  else
  {
    Swap( (oled_pixel_t)oled_dynamic_area.areaBuffer, BMP_SkipHeader(image), oled_dynamic_area.width*oled_dynamic_area.height );

    /* update the main screen buffer */
    UpdateBuffer( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height, (const uint8_t *)oled_dynamic_area.areaBuffer );
  }

  return status;
}


oled_status_t SSD1351::AddImage ( const uint8_t* image, int8_t xCrd, int8_t yCrd )
{
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  oled_dynamic_area_t  image_dynamicArea;

  image_dynamicArea.xCrd = xCrd;
  image_dynamicArea.yCrd = yCrd;

  GetImageDimensions(&image_dynamicArea.width, &image_dynamicArea.height, image);
  SetDynamicArea(&image_dynamicArea);

  /* check the bounds */
  if AreCoordsNotValid( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height )
  {
    status = OLED_STATUS_INIT_ERROR;
  }

  else
  {
    Swap( (oled_pixel_t)oled_dynamic_area.areaBuffer, BMP_SkipHeader(image), oled_dynamic_area.width*oled_dynamic_area.height );

    /* update the main screen buffer */
    UpdateBuffer( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height, (const uint8_t *)oled_dynamic_area.areaBuffer );
  }

  return status;
}



oled_status_t SSD1351::DrawImage ( const uint8_t* image )
{
  
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  status = AddImage( image );

  /* set the locations */
  SetBorders( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height );

  /* fill the GRAM */
  SendData( (const uint8_t*)oled_dynamic_area.areaBuffer, oled_dynamic_area.width * oled_dynamic_area.height * OLED_BYTES_PER_PIXEL );


  return status;
}

oled_status_t SSD1351::DrawImage ( const uint8_t* image, int8_t xCrd, int8_t yCrd  )
{
  
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  status = AddImage( image,xCrd,yCrd);

  /* set the locations */
  SetBorders( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height );

  /* fill the GRAM */
  SendData( (const uint8_t*)oled_dynamic_area.areaBuffer, oled_dynamic_area.width * oled_dynamic_area.height * OLED_BYTES_PER_PIXEL );


  return status;
}



void SSD1351::DimScreenON()
{
    for ( int i = 0; i < 16; i++ )
    {
        SendCmd( OLED_CMD_CONTRASTMASTER, CMD_BYTE );
        SendCmd( 0xC0 | (0xF-i), DATA_BYTE );
        wait_ms(20);
        // OSA_TimeDelay( 20 );
    }
}


void SSD1351::DimScreenOFF()
{
  SendCmd( OLED_CMD_CONTRASTMASTER, CMD_BYTE );
  SendCmd( 0xC0 | 0xF, DATA_BYTE );
}



void SSD1351::Swap(
                    oled_pixel_t imgDst,
                  const uint8_t* imgSrc,
                        uint16_t imgSize
                )
{
  for ( int var = 0; var < imgSize; var++ )
  {
    *imgDst = *imgSrc << 8;
    imgSrc++;
    *imgDst |= *imgSrc;
    imgDst++;
    imgSrc++;
  }
}


void SSD1351::PowerON()
{
  power = 1;
}

void SSD1351::PowerOFF()
{
  power = 0;
}



 /* Formerly Known as GuiDriver_UpdateScreen */
void SSD1351::UpdateBuffer (
                                int8_t xCrd,
                                int8_t yCrd,
                                uint8_t width,
                                uint8_t height,
                                const uint8_t* image
                              )
{
  /* copy data */
  oled_pixel_t
    copyAddr = (oled_pixel_t)screenBuf + ( yCrd*OLED_SCREEN_WIDTH + xCrd );

  for ( uint8_t i = 0; i < height; i++ )
  {
    memcpy( (void*)copyAddr, (void*)image, width*OLED_BYTES_PER_PIXEL );
    copyAddr += OLED_SCREEN_WIDTH;
    image    += width*OLED_BYTES_PER_PIXEL;
  }
} 


oled_status_t SSD1351::Label ( const uint8_t* text,int8_t xCrd, int8_t yCrd )
{

  if ( NULL == text )
  {
    return OLED_STATUS_ERROR;
  }

  AddText(text,xCrd,yCrd);

  /* set the locations */
  SetBorders( oled_dynamic_area.xCrd, oled_dynamic_area.yCrd, oled_dynamic_area.width, oled_dynamic_area.height );

  /* fill the GRAM */
  SendData( (const uint8_t*)oled_dynamic_area.areaBuffer, oled_dynamic_area.width * oled_dynamic_area.height * OLED_BYTES_PER_PIXEL );

  //free( currentTextAreaImage );
  return OLED_STATUS_SUCCESS;
}


oled_status_t SSD1351::TextBox(const uint8_t* text, int8_t xCrd, int8_t yCrd,uint8_t width,uint8_t height)
{

  if ( NULL == text )
  {
    return OLED_STATUS_ERROR;
  }

  oled_dynamic_area_t textArea;
  textArea.width  = width;
  textArea.height = height;
  textArea.xCrd = xCrd;
  textArea.yCrd = yCrd; 

  SetDynamicArea(&textArea);
  DrawText(text);

  return OLED_STATUS_SUCCESS;

}


/* Internal Functions */

/**
 * [transpose description]
 * @param transImage  Transposed Image    
 * @param image       Source Image  
 * @param width       Width to Transpose
 * @param height      Height to Transpose 
 */
void SSD1351::Transpose(
                             oled_pixel_t transImage,
                       const oled_pixel_t image,
                                  uint8_t width,
                                  uint8_t height
                     )
{
  for ( uint8_t i = 0; i < height; i++ )
  {
    for ( uint8_t j = 0; j < width ; j++ )
    {
      transImage[ j*height + i ] = image[ i*width + j ];
    }
  }
}



/**
 * TopDown Transition Effect for Image
 * @param  image  image to be transitioned
 * @param  xCrd   x coordinate of image
 * @param  yCrd   y coordinate of image
 * @param  width  width of image
 * @param  height height of image
 * @return        status flag
 */
oled_status_t SSD1351::TopDown(
                                const uint8_t* image,
                                       int8_t xCrd,
                                       int8_t yCrd,
                                       uint8_t width,
                                       uint8_t height
                            )
{
  uint16_t
    transStep = OLED_TRANSITION_STEP;

  uint16_t
    partImgSize = width*transStep;

  oled_status_t
    status = OLED_STATUS_SUCCESS;

  uint8_t*
    partImgPtr = (uint8_t*)image + ( height - transStep ) * ( width * OLED_BYTES_PER_PIXEL );

  /**
   * set locations
   */

  while (1)
  {
    SetBorders( xCrd, yCrd, width, height );

    if ( partImgSize > width*height )
    {
        SendData( (const uint8_t*)image, width*height*OLED_BYTES_PER_PIXEL );
        break;
    }
    else
    {
        SendData( (const uint8_t*)partImgPtr, partImgSize * OLED_BYTES_PER_PIXEL );
    }


    /**
     * update variables
     */

    partImgPtr  -= ( width * transStep ) * OLED_BYTES_PER_PIXEL;
    partImgSize += ( width * transStep );
    transStep++;
  }

  return status;
}

/**
 * DownTop Transition Effect for Image
 * @param  image  image to be transitioned
 * @param  xCrd   x coordinate of image
 * @param  yCrd   y coordinate of image
 * @param  width  width of image
 * @param  height height of image
 * @return        status flag
 */

oled_status_t SSD1351::DownTop(
                                const uint8_t* image,
                                      int8_t xCrd,
                                      int8_t yCrd,
                                      uint8_t width,
                                      uint8_t height
                            )
{
  uint16_t
    transStep = OLED_TRANSITION_STEP;

  uint16_t
    partImgSize = width*transStep;

  oled_status_t
    status = OLED_STATUS_SUCCESS;

  uint8_t*
    partImgPtr = (uint8_t*)image;

  uint8_t
    yCrd_moving = ( yCrd + height ) - 1;

  /**
   * set locations
   */

  while (1)
  {
    if  (
             ( partImgSize > OLED_SCREEN_SIZE )
          ||        ( yCrd_moving < yCrd )
        )
    {
      /* draw full image */
      SetBorders( xCrd, yCrd, width, height );
      SendData( (const uint8_t*)image, width * height * OLED_BYTES_PER_PIXEL );
      break;
    }

    else
    {
      SetBorders( xCrd, yCrd_moving, width, ( yCrd + height ) - yCrd_moving );
      SendData( (const uint8_t*)partImgPtr, partImgSize * OLED_BYTES_PER_PIXEL );
    }

    /**
     * update variables
     */

    yCrd_moving -= transStep;
    partImgSize += ( width * transStep );
    transStep++;
  }

  return status;
}


/**
 * LeftRight Transition Effect for Image
 * @param  image  image to be transitioned
 * @param  xCrd   x coordinate of image
 * @param  yCrd   y coordinate of image
 * @param  width  width of image
 * @param  height height of image
 * @return        status flag
 */

oled_status_t SSD1351::LeftRight(
                                 const uint8_t* image,
                                        int8_t xCrd,
                                        int8_t yCrd,
                                        uint8_t width,
                                        uint8_t height
                              )
{
  oled_status_t
    status = OLED_STATUS_SUCCESS;

  oled_dynamic_area_t
      transImageArea =
      {
          .xCrd = 0,
          .yCrd = 0,

          .width = 96,
          .height= 96
      };

  SetDynamicArea( &transImageArea );

  /* helper pointer */
  oled_pixel_t
    transImage = (oled_pixel_t)oled_dynamic_area.areaBuffer;

  if ( NULL == transImage )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  Transpose( transImage,(oled_pixel_t)image, width, height );

  SendCmd( OLED_CMD_SET_REMAP, CMD_BYTE );
  SendCmd( OLED_REMAP_SETTINGS | REMAP_VERTICAL_INCREMENT, DATA_BYTE );
 
  uint16_t
    transStep = OLED_TRANSITION_STEP;

  uint16_t
    partImgSize = height*transStep;

  uint8_t*
    partImgPtr = (uint8_t*)transImage + ( width - transStep ) * ( height * OLED_BYTES_PER_PIXEL );

  /**
   * set locations
   */

  while (1)
  {
    SetBorders( xCrd, yCrd, width, height );

    if ( partImgSize > width*height )
    {
      SendData((const uint8_t*)transImage, width * height * OLED_BYTES_PER_PIXEL );
      break;
    }
    else
    {
      SendData( (const uint8_t*)partImgPtr, partImgSize * OLED_BYTES_PER_PIXEL );
    }


    partImgPtr  -= ( transStep * height ) * OLED_BYTES_PER_PIXEL;
    partImgSize += ( transStep * height );
    transStep++;
    
  }
   
    SendCmd( OLED_CMD_SET_REMAP, CMD_BYTE );
    SendCmd( OLED_REMAP_SETTINGS, DATA_BYTE );
    DestroyDynamicArea();
    return status;
}


/**
 * RightLeft Transition Effect for Image
 * @param  image  image to be transitioned
 * @param  xCrd   x coordinate of image
 * @param  yCrd   y coordinate of image
 * @param  width  width of image
 * @param  height height of image
 * @return        status flag
 */
oled_status_t SSD1351::RightLeft(
                                 const uint8_t* image,
                                        int8_t xCrd,
                                        int8_t yCrd,
                                        uint8_t width,
                                        uint8_t height
                              )
{
  oled_dynamic_area_t
      transImageArea =
      {
          .xCrd = 0,
          .yCrd = 0,

          .width = 96,
          .height= 96
      };

  SetDynamicArea( &transImageArea );

  /* helper pointer */
  oled_pixel_t
    transImage = oled_dynamic_area.areaBuffer;

  if ( NULL == transImage )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  Transpose(transImage, (oled_pixel_t)image, width, height );

  SendCmd( OLED_CMD_SET_REMAP, CMD_BYTE );
  SendCmd( OLED_REMAP_SETTINGS | REMAP_VERTICAL_INCREMENT, DATA_BYTE );


  uint16_t
    transStep = OLED_TRANSITION_STEP;

  uint16_t
    partImgSize = height * transStep;

  uint8_t*
    partImgPtr = (uint8_t*)transImage;

  uint8_t
    xCrd_moving = ( xCrd + width ) - 1;

  /** set locations */

  while (1)
  {
    if  (( partImgSize > width*height )|| ( xCrd_moving < xCrd ))       
    {
       SetBorders( xCrd, yCrd, width, height );
       SendData( (const uint8_t*)transImage, height * width * OLED_BYTES_PER_PIXEL );
       break;
    }
    else
    {
       SetBorders( xCrd_moving, yCrd, ( xCrd + width ) - xCrd_moving, height );
       SendData( (const uint8_t*)partImgPtr, partImgSize * OLED_BYTES_PER_PIXEL );
    }

    /** update variables*/

    xCrd_moving -= transStep;
    partImgSize += ( height * transStep );
    transStep++;
  }

  SendCmd( OLED_CMD_SET_REMAP, CMD_BYTE );
  SendCmd( OLED_REMAP_SETTINGS, DATA_BYTE );

  DestroyDynamicArea();

  return OLED_STATUS_SUCCESS;
}


/**
 * [setDirection description]
 * @param  self [description]
 * @param  xCrd [description]
 * @param  yCrd [description]
 * @return      [description]
 */
void SSD1351::SetBorders(
                        int8_t xCrd,
                        int8_t yCrd,
                        uint8_t width,
                        uint8_t height
                      )
{
  
  /* adjust for the offset*/
  OLED_AdjustColumnOffset(xCrd);
  OLED_AdjustRowOffset(yCrd);

  SendCmd( OLED_CMD_SET_COLUMN, CMD_BYTE );
  SendCmd( xCrd, DATA_BYTE );
  SendCmd( xCrd + (width-1), DATA_BYTE );
  SendCmd( OLED_CMD_SET_ROW, CMD_BYTE );
  SendCmd( yCrd, DATA_BYTE );
  SendCmd( yCrd + (height-1), DATA_BYTE );
  
}

/**
 * create the buffer for a partial image
 * @param  imgBuf [description]
 * @param  width  [description]
 * @param  height [description]
 * @return        [description]
 */
oled_status_t SSD1351::CreateTextBackground()
{
  uint8_t
    xCrd   = oled_dynamic_area.xCrd,
    yCrd   = oled_dynamic_area.yCrd,
    width  = oled_dynamic_area.width,
    height = oled_dynamic_area.height;

  oled_pixel_t
    imgBuf = oled_dynamic_area.areaBuffer,
    copyAddr;

  const uint8_t*
    background = oled_text_properties.background;

  /* copy data */

  if  (
            ( NULL == imgBuf )
        ||  ( ( xCrd + width )  > OLED_SCREEN_WIDTH )
        ||  ( ( yCrd + height ) > OLED_SCREEN_HEIGHT )
      )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  if ( NULL == background )
  {
    for ( uint8_t i = 0; i < height; i++ )
    {
      memset( (void*)imgBuf, 0, width*OLED_BYTES_PER_PIXEL );
      imgBuf   += width;
    }
  }

  else
  {
    copyAddr = (oled_pixel_t)( BMP_SkipHeader( background ) ) + ( yCrd*OLED_SCREEN_WIDTH + xCrd );
    for ( uint8_t i = 0; i < height; i++ )
    {
      Swap( (oled_pixel_t)imgBuf, (const uint8_t*)copyAddr, width );
      imgBuf   += width;
      copyAddr += OLED_SCREEN_WIDTH;
    }
  }

  return OLED_STATUS_SUCCESS;
}


/**
 * Write the character to Buffer
 * @param charToWrite character to be written
 * @param chrBuf      given pointer for buffer for the character
 */
void SSD1351::WriteCharToBuf(
                            uint16_t charToWrite,
                            oled_pixel_t* chrBuf
                          )
{
  uint8_t
    foo = 0,
    mask;

  const uint8_t*
    pChTable = selectedFont + 8 + (uint16_t)( ( charToWrite - selectedFont_firstChar ) << 2 );

  currentChar_width  = *pChTable,
  currentChar_height = selectedFont_height;

  uint32_t
    offset =      (uint32_t)pChTable[1]
              | ( (uint32_t)pChTable[2] << 8 )
              | ( (uint32_t)pChTable[3] << 16 );

  const uint8_t*
    pChBitMap = selectedFont + offset;

  /* allocate space for char image */
  *chrBuf = (oled_pixel_t)AllocateDynamicArea( currentChar_height * currentChar_width );

  if ( NULL == *chrBuf )
  {
    return;
  }

  for ( uint8_t yCnt = 0; yCnt < currentChar_height; yCnt++ )
  {
    mask = 0;

    for ( uint8_t xCnt = 0; xCnt < currentChar_width; xCnt++ )
    {
      if ( 0 == mask )
      {
        mask = 1;
        foo  = *pChBitMap++;
      }

      if ( 0 != ( foo & mask ) )
      {
        *( *chrBuf + yCnt*currentChar_width + xCnt ) = selectedFont_color;
      }

       else
       {
           *( *chrBuf + yCnt*currentChar_width + xCnt ) = 0;
       }

      mask <<= 1;
    }
  }
}


/**
 * Add subimage/character to the active image buffer
 * @param  xOffset offset for the x-coordinate
 * @param  yOffset offset for the y-coordinate
 * @param  width   desired width
 * @param  height  desired height
 * @return         status flag
 */
oled_status_t SSD1351::AddCharToTextArea(
                                        oled_pixel_t chrPtr,
                                             uint8_t chrWidth,
                                             uint8_t chrHeight,
                                        oled_pixel_t copyAddr,
                                             uint8_t imgWidth
                                      )
{
  if ( NULL == copyAddr )
  {
    return OLED_STATUS_INIT_ERROR;
  }

  for ( uint8_t i = 0; i < chrHeight; i++ )
  {
    for ( uint8_t j = 0; j < chrWidth; j++ )
    {
      if ( 0 != chrPtr[j] )
      {
          copyAddr[j] = chrPtr[j];
      }
    }
    copyAddr += imgWidth;
    chrPtr   += chrWidth;
  }
  return OLED_STATUS_SUCCESS;
}


/**
 * Allocate memory for the desired image/character
 * @param area desired area dimensions
 */
void* SSD1351::AllocateDynamicArea( uint32_t area )
{
  void*
      ptr = malloc( area * OLED_BYTES_PER_PIXEL );

  if ( NULL == ptr )
  {
      return NULL;
  }

  return ptr;
}


/**
 * Deallocate current area
 * @param area pointer to current area
 */
oled_status_t SSD1351::DestroyDynamicArea( void* ptr )
{
  if ( NULL == ptr )
  {
      return OLED_STATUS_INIT_ERROR;
  }

   free(ptr); 

   return OLED_STATUS_SUCCESS; 
}
