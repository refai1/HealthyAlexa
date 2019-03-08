/** OLED Types 
 *  This file contains OLED-related data structures.
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
 
 
/**
 * OLED-related data structures
 * Project HEXIWEAR, 2015
 */

#ifndef HG_OLED_TYPES
#define HG_OLED_TYPES

#include <stdint.h>

typedef enum
{
  OLED_TRANSITION_NONE,
  OLED_TRANSITION_TOP_DOWN,
  OLED_TRANSITION_DOWN_TOP,
  OLED_TRANSITION_LEFT_RIGHT,
  OLED_TRANSITION_RIGHT_LEFT

} oled_transition_t;

typedef enum
{
  OLED_STATUS_SUCCESS,        // success
  OLED_STATUS_ERROR,          // fail
  OLED_STATUS_PROTOCOL_ERROR, // SPI failure
  OLED_STATUS_INIT_ERROR,     // initialization error
  OLED_STATUS_DEINIT_ERROR    // deinitialization error

} oled_status_t;


#if 0 
typedef struct
{
  /**
   * SPI relevant information
   */
  genericSpiHandle_t protocol;

} handleOLED_t;
#endif


typedef uint16_t* oled_pixel_t;

typedef struct
{
  uint32_t DCpin;
  uint32_t CSpin;
  uint32_t RSTpin;
// uint32_t RWpin;
  uint32_t ENpin;

} settingsOLED_t;

typedef enum
{
  OLED_TEXT_ALIGN_NONE = 0,

  OLED_TEXT_ALIGN_LEFT   = 0x1,
  OLED_TEXT_ALIGN_RIGHT  = 0x2,
  OLED_TEXT_ALIGN_CENTER = 0x3,

  OLED_TEXT_VALIGN_TOP    = 0x10,
  OLED_TEXT_VALIGN_BOTTOM = 0x20,
  OLED_TEXT_VALIGN_CENTER = 0x30

} oled_text_align_t;

typedef struct
{
         int8_t xCrd;
         int8_t yCrd;
         uint8_t width;
         uint8_t height;
    oled_pixel_t areaBuffer;

} oled_dynamic_area_t;

typedef struct
{
  const uint8_t* font;
  uint16_t fontColor;
  oled_text_align_t alignParam;
  const uint8_t* background;

} oled_text_properties_t;


/** color chart */
typedef enum
{
  COLOR_BLACK    = 0x0000,
  COLOR_BLUE_1   = 0x06FF,
  COLOR_BLUE     = 0x001F,
  COLOR_RED      = 0xF800,
  COLOR_GREEN    = 0x07E0,
  COLOR_CYAN     = 0x07FF,
  COLOR_MAGENTA  = 0xF81F,
  COLOR_YELLOW   = 0xFFE0,
  COLOR_GRAY     = 0x528A,
  COLOR_WHITE    = 0xFFFF

} Color_t;     

typedef struct _init_cmd_tag
{
    uint32_t cmd;
    uint8_t  type;
} init_cmd_t;

#endif

