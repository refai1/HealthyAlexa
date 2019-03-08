 /* Copyright (c) 2015 NXP Semiconductors. MIT License
*
* Permission is hereby granted, free of charge, to any person obtaining a copy of this software
* and associated documentation files (the "Software"), to deal in the Software without
* restriction, including without limitation the rights to use, copy, modify, merge, publish,
* distribute, sublicense, and/or sell copies of the Software, and to permit persons to whom the
* Software is furnished to do so, subject to the following conditions:
*
* The above copyright notice and this permission notice shall be included in all copies or
* substantial portions of the Software.
*
* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING
* BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
* NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM,
* DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
* OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.
*/

#ifndef FXAS21002_H
#define FXAS21002_H
#include "mbed.h"

#define FXAS21002_I2C_ADDRESS 0x40

#define FXAS21002_STATUS 0x00
#define FXAS21002_WHO_AM_I 0x0C
#define FXAS21002_CTRL_REG0 0x0D
#define FXAS21002_CTRL_REG1 0x13
#define FXAS21002_WHO_AM_I_VALUE 0xD1

class FXAS21002
{
    public:
    
    FXAS21002(PinName sda, PinName scl);
      
    void gyro_config(void);
        
    void acquire_gyro_data_dps(float * du);
    
    private:
    I2C gyroi2c;
    
};

#endif