/*****************************************************************************
* | File      	:   EPD_2in66bses.cpp
* | Author      :   Kimi based on Waveshare team's sample code
* | Function    :   2.66inch e-paper wbr from SES VUSION 2.6 BWR GL420
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2022-09-13
* | Info        :
* | Tested epd  :   BE2266ES0550ET7AMY01125
* |                 TC026SC1C3-S5(AE2266ES0550EZ7B22009TY)
* -----------------------------------------------------------------------------
# Permission is hereby granted, free of charge, to any person obtaining a copy
# of this software and associated documnetation files (the "Software"), to deal
# in the Software without restriction, including without limitation the rights
# to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
# copies of the Software, and to permit persons to  whom the Software is
# furished to do so, subject to the following conditions:
#
# The above copyright notice and this permission notice shall be included in
# all copies or substantial portions of the Software.
#
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS OR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
# THE SOFTWARE.
#
******************************************************************************/
#include "EPD_2in66bses.h"
#include "Debug.h"

static uint8_t part_flag = 0;

//full screen update LUT
const unsigned char lut_20_vcomDC[] =
{
  0x00, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x60, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x00, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x00, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};

const unsigned char lut_21_ww[] =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_22_bw[] =
{
  0x40, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x40, 0x14, 0x00, 0x00, 0x00, 0x01,
  0xA0, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_23_wb[] =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_24_bb[] =
{
  0x80, 0x08, 0x00, 0x00, 0x00, 0x02,
  0x90, 0x28, 0x28, 0x00, 0x00, 0x01,
  0x80, 0x14, 0x00, 0x00, 0x00, 0x01,
  0x50, 0x12, 0x12, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

//partial screen update LUT
//#define Tx19 0x19 // original value is 25 (phase length)
#define Tx19 0x20   // new value for test is 32 (phase length)
const unsigned char lut_20_vcomDC_partial[] =
{
  0x00, Tx19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00,
};

const unsigned char lut_21_ww_partial[] =
{
  0x00, Tx19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_22_bw_partial[] =
{
  0x80, Tx19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_23_wb_partial[] =
{
  0x40, Tx19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

const unsigned char lut_24_bb_partial[] =
{
  0x00, Tx19, 0x01, 0x00, 0x00, 0x01,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
  0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
};

/******************************************************************************
function :	Software reset
parameter:
******************************************************************************/
static void EPD_2IN66BSES_Reset(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(2);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(200);
}

/******************************************************************************
function :	send command
parameter:
     Reg : Command register
******************************************************************************/
static void EPD_2IN66BSES_SendCommand(UBYTE Reg)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Reg);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

/******************************************************************************
function :	send data
parameter:
    Data : Write data
******************************************************************************/
static void EPD_2IN66BSES_SendData(UBYTE Data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_Digital_Write(EPD_CS_PIN, 0);
    DEV_SPI_WriteByte(Data);
    DEV_Digital_Write(EPD_CS_PIN, 1);
}

static void EPD_2IN66_SetLUT(const unsigned char *lut, int len) {
    for(int i = 0; i < len; i++) {
        EPD_2IN66BSES_SendData(lut[i]);
    }
}

static void EPD_2IN66B_SetLUA(const uint8_t *data, uint16_t n)
{
    UWORD count;
    for (count = 0; count < n; count++)
    {
        EPD_2IN66BSES_SendData (*(data + count));
    }
}

/******************************************************************************
function :	Wait until the busy_pin goes LOW
parameter:
******************************************************************************/
static void EPD_2IN66BSES_ReadBusy(void)
{
    Debug("SES266 e-Paper busy\r\n");
    DEV_Delay_ms(50);
    while(DEV_Digital_Read(EPD_BUSY_PIN) == 0) {      //LOW: idle, HIGH: busy
        DEV_Delay_ms(10);
    }
    DEV_Delay_ms(50);
    Debug("SES266 e-Paper busy release\r\n");
}

/******************************************************************************
function :	Turn On Display
parameter:
******************************************************************************/
static void EPD_2IN66BSES_TurnOnDisplay(void)
{
    EPD_2IN66BSES_SendCommand(0x12);
    EPD_2IN66BSES_ReadBusy();
}
/******************************************************************************
function :	Initialize the e-Paper register
parameter:
******************************************************************************/
void EPD_2IN66BSES_Init(void)
{
    EPD_2IN66BSES_Reset();

    EPD_2IN66BSES_SendCommand(0x06); // BOOSTER_SOFT_START
    EPD_2IN66BSES_SendData(0x17);
    EPD_2IN66BSES_SendData(0x17);
    EPD_2IN66BSES_SendData(0x17);
	
    EPD_2IN66BSES_SendCommand(0x04); // POWER_ON
    Debug("SES266 POWER ON CHK Busy\r\n");
    EPD_2IN66BSES_ReadBusy();
	
    EPD_2IN66BSES_SendCommand(0x00); // PANEL_SETTING
    EPD_2IN66BSES_SendData(0xCF);
	
    EPD_2IN66BSES_SendCommand(0x50); // VCOM_AND_DATA_INTERVAL_SETTING
    EPD_2IN66BSES_SendData(0xB7);

    EPD_2IN66BSES_SendCommand(0x61); // RESOLUTION_SETTING
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_WIDTH); // width: 152
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT >> 8); // height: 292
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT & 0xFF);
	EPD_2IN66BSES_SendData(0x98);
	EPD_2IN66BSES_SendData(0x01);
	EPD_2IN66BSES_SendData(0x28);

    EPD_2IN66BSES_SendCommand(0x82);
    EPD_2IN66BSES_SendData(0x0A);
}


void EPD_2IN66BSES_InitBLK(void)
{
    EPD_2IN66BSES_Reset();

  EPD_2IN66BSES_SendCommand(0x01);     //POWER SETTING
  EPD_2IN66BSES_SendData (0x03);
  EPD_2IN66BSES_SendData (0x00);
  EPD_2IN66BSES_SendData (0x2b);
  EPD_2IN66BSES_SendData (0x2b);
  EPD_2IN66BSES_SendData (0x03);

    EPD_2IN66BSES_SendCommand(0x06); // BOOSTER_SOFT_START
    EPD_2IN66BSES_SendData(0x17);
    EPD_2IN66BSES_SendData(0x17);
    EPD_2IN66BSES_SendData(0x17);
	
    EPD_2IN66BSES_SendCommand(0x04); // POWER_ON
    Debug("SES266 POWER ON CHK Busy\r\n");
    EPD_2IN66BSES_ReadBusy();
	
    EPD_2IN66BSES_SendCommand(0x00); // PANEL_SETTING
    EPD_2IN66BSES_SendData(0xBF);	
    EPD_2IN66BSES_SendData(0x0d);
    EPD_2IN66BSES_SendCommand(0x30); // VCOM_AND_DATA_INTERVAL_SETTING
    EPD_2IN66BSES_SendData(0x3a);

    EPD_2IN66BSES_SendCommand(0x61); // RESOLUTION_SETTING
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_WIDTH); // width: 152
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT >> 8); // height: 292
    //EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT & 0xFF);
	EPD_2IN66BSES_SendData(0x98);
	EPD_2IN66BSES_SendData(0x01);
	EPD_2IN66BSES_SendData(0x28);

    EPD_2IN66BSES_SendCommand(0x82);
   EPD_2IN66BSES_SendData(0x08);
   EPD_2IN66BSES_SendCommand(0X50); //VCOM AND DATA INTERVAL SETTING
  EPD_2IN66BSES_SendData(0x97);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
  unsigned int count;
  {
    EPD_2IN66BSES_SendCommand(0x20);              //vcom
    for (count = 0; count < 44; count++)
    {
      EPD_2IN66BSES_SendData(lut_20_vcomDC[count]);
    }

    EPD_2IN66BSES_SendCommand(0x21);              //ww --
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_21_ww[count]);
    }

    EPD_2IN66BSES_SendCommand(0x22);              //bw r
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_22_bw[count]);
    }

    EPD_2IN66BSES_SendCommand(0x23);              //wb w
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_23_wb[count]);
    }

    EPD_2IN66BSES_SendCommand(0x24);              //bb b
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_24_bb[count]);
    }
  }
}

  /* 
   void EPD_2IN66BSES_Init(void)
{
    part_flag = 0;

    EPD_2IN66BSES_Reset();
    EPD_2IN66BSES_ReadBusy();

    EPD_2IN66BSES_SendCommand(0x01); // POWER SETTING
    EPD_2IN66BSES_SendData(0x03);
    EPD_2IN66BSES_SendData(0x00);
    EPD_2IN66BSES_SendData(0x2b);
    EPD_2IN66BSES_SendData(0x2b);
    EPD_2IN66BSES_SendData(0x03);
    EPD_2IN66BSES_SendCommand(0x06); // boost soft start
    EPD_2IN66BSES_SendData(0x17);    // A
    EPD_2IN66BSES_SendData(0x17);    // B
    EPD_2IN66BSES_SendData(0x17);    // C

    EPD_2IN66BSES_SendCommand(0x00); // panel setting
    EPD_2IN66BSES_SendData(0xCF);    // LUT from REG 128x296
    EPD_2IN66BSES_SendData(0x0d);    // VCOM to 0V fast

    EPD_2IN66BSES_SendCommand(0x30); // PLL setting
    EPD_2IN66BSES_SendData(0x3a);    // 3a 100HZ   29 150Hz 39 200HZ 31 171HZ

    EPD_2IN66BSES_SendCommand(0x61); // resolution setting
    EPD_2IN66BSES_SendData(EPD_2IN66BSES_WIDTH);
    EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT / 256);
    EPD_2IN66BSES_SendData(EPD_2IN66BSES_HEIGHT % 256);
    EPD_2IN66BSES_SendCommand(0x82); // vcom_DC setting
    // EPD_2IN66_SendData (0x00);   // -0.1
    EPD_2IN66BSES_SendData(0x08);    // -0.1 + 8 * -0.05 = -0.5V from demo
                                  //    EPD_2IN66B_SendData (0x12);   // -0.1 + 18 * -0.05 = -1.0V from OTP, slightly better
                                  //!    EPD_2IN66B_SendData (0x1c); // -0.1 + 28 * -0.05 = -1.5V test, better
                                  //    EPD_2IN66_SendData (0x26);   // -0.1 + 38 * -0.05 = -2.0V test, same
                                  // EPD_2IN66_SendData (0x30);   // -0.1 + 48 * -0.05 = -2.5V test, darker
    EPD_2IN66BSES_SendCommand(0x50); // VCOM AND DATA INTERVAL SETTING
    EPD_2IN66BSES_SendData(0xB7);    // WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7

    EPD_2IN66BSES_SendCommand(0x20);
    EPD_2IN66B_SetLUA(lut_20_vcom0_full, sizeof(lut_20_vcom0_full));
    EPD_2IN66BSES_SendCommand(0x21);
    EPD_2IN66B_SetLUA(lut_21_ww_full, sizeof(lut_21_ww_full));
    EPD_2IN66BSES_SendCommand(0x22);
    EPD_2IN66B_SetLUA(lut_22_bw_full, sizeof(lut_22_bw_full));
    EPD_2IN66BSES_SendCommand(0x23);
    EPD_2IN66B_SetLUA(lut_23_wb_full, sizeof(lut_23_wb_full));
    EPD_2IN66BSES_SendCommand(0x24);
    EPD_2IN66B_SetLUA(lut_24_bb_full, sizeof(lut_24_bb_full));
    EPD_2IN66BSES_SendCommand(0x04); // POWER ON
    EPD_2IN66BSES_ReadBusy();
}
 */
/******************************************************************************
function :	Sends the image buffer in RAM to e-Paper and displays
parameter:
******************************************************************************/
void EPD_2IN66BSES_Display(UBYTE *ImageBlack, UBYTE*ImageRed)
{
    UWORD Width, Height;
    Width = (EPD_2IN66BSES_WIDTH % 8 == 0)? (EPD_2IN66BSES_WIDTH / 8 ): (EPD_2IN66BSES_WIDTH / 8 + 1);
    Height = EPD_2IN66BSES_HEIGHT;

    EPD_2IN66BSES_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(ImageBlack[i + j * Width]);
        }
    }
	EPD_2IN66BSES_SendCommand(0x92);
	
    EPD_2IN66BSES_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(~ImageRed[i + j * Width]);
        }
    }
    EPD_2IN66BSES_SendCommand(0x92);

    EPD_2IN66BSES_TurnOnDisplay();
}

/******************************************************************************
function :	Clear screen
parameter:
******************************************************************************/
void EPD_2IN66BSES_Clear(void)
{
    UWORD Width, Height;
    Width = (EPD_2IN66BSES_WIDTH % 8 == 0)? (EPD_2IN66BSES_WIDTH / 8 ): (EPD_2IN66BSES_WIDTH / 8 + 1);
    Height = EPD_2IN66BSES_HEIGHT;

    EPD_2IN66BSES_SendCommand(0x10);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(0xff);
        }
    }
	EPD_2IN66BSES_SendCommand(0x13);
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(0xff);
        }
    }
    EPD_2IN66BSES_TurnOnDisplay();
}



void EPD_2IN66BSES_ClearBlk(void)
{
    UWORD Width, Height;
    Width = (EPD_2IN66BSES_WIDTH % 8 == 0)? (EPD_2IN66BSES_WIDTH / 8 ): (EPD_2IN66BSES_WIDTH / 8 + 1);
    Height = EPD_2IN66BSES_HEIGHT;

    EPD_2IN66BSES_SendCommand(0x10); //13?
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(0xff);
        }
    }

    EPD_2IN66BSES_SendCommand(0x13); //13?
    for (UWORD j = 0; j < Height; j++) {
        for (UWORD i = 0; i < Width; i++) {
            EPD_2IN66BSES_SendData(0xff);
        }
    }

    EPD_2IN66BSES_TurnOnDisplay();
}
/******************************************************************************
function :	Enter sleep mode
parameter:
******************************************************************************/
void EPD_2IN66BSES_Sleep(void)
{
    EPD_2IN66BSES_SendCommand(0x02);
    EPD_2IN66BSES_ReadBusy();
    EPD_2IN66BSES_SendCommand(0x07); // DEEP_SLEEP
    EPD_2IN66BSES_SendData(0xA5); // check code
}


void EPD_2IN66B_Img(const unsigned char *black_image)
{
    // Отправка черно-белых данных (команда 0x13)
    EPD_2IN66BSES_SendCommand(0x13);
    for (int i = 0; i < (EPD_2IN66BSES_WIDTH / 8) * EPD_2IN66BSES_HEIGHT; i++) {
        EPD_2IN66BSES_SendData(black_image[i]);
    }
    
    // Отправка красных данных (команда 0x14)
   // EPD_2IN66B_SendCommand(0x14);
   // for (int i = 0; i < (EPD_2IN66B_WIDTH / 8) * EPD_2IN66B_HEIGHT; i++) {
   //     EPD_2IN66B_SendData(red_image[i]);
   // }
    
    // Обновление экрана
    EPD_2IN66BSES_TurnOnDisplay();
}

void EPD_2IN66B_ImgBR(const unsigned char *black_image, const unsigned char *red_image)
{
    // Отправка черно-белых данных (команда 0x13)
    EPD_2IN66BSES_SendCommand(0x10);
    for (int i = 0; i < (EPD_2IN66BSES_WIDTH / 8) * EPD_2IN66BSES_HEIGHT; i++) {
        EPD_2IN66BSES_SendData(black_image[i]);
    }
    
    // Отправка красных данных (команда 0x14)
    EPD_2IN66BSES_SendCommand(0x13);
    for (int i = 0; i < (EPD_2IN66BSES_WIDTH / 8) * EPD_2IN66BSES_HEIGHT; i++) {
        EPD_2IN66BSES_SendData(red_image[i]);
    }
    
    // Обновление экрана
    EPD_2IN66BSES_TurnOnDisplay();
}

void EPD_2IN66BSES_Init_PartialUpdate(void)
{
  EPD_2IN66BSES_SendCommand(0x82);     //vcom_DC setting
  EPD_2IN66BSES_SendData (0x08);
  EPD_2IN66BSES_SendCommand(0X50);
  //_writeData(0x47);
  //_writeData(0x97);    //WBmode:VBDF 17|D7 VBDW 97 VBDB 57   WBRmode:VBDF F7 VBDW 77 VBDB 37  VBDR B7
  EPD_2IN66BSES_SendData(0x17);
  unsigned int count;
  {
    EPD_2IN66BSES_SendCommand(0x20);              //vcom
    for (count = 0; count < 44; count++)
    {
      EPD_2IN66BSES_SendData(lut_20_vcomDC_partial[count]);
    }

    EPD_2IN66BSES_SendCommand(0x21);              //ww --
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_21_ww_partial[count]);
    }

    EPD_2IN66BSES_SendCommand(0x22);              //bw r
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_22_bw_partial[count]);
    }

    EPD_2IN66BSES_SendCommand(0x23);              //wb w
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_23_wb_partial[count]);
    }

    EPD_2IN66BSES_SendCommand(0x24);              //bb b
    for (count = 0; count < 42; count++)
    {
      EPD_2IN66BSES_SendData(lut_24_bb_partial[count]);
    }
  }
}

uint16_t EPD_2IN66BSES_setPartialRamArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
{
  x &= 0xFFF8; // byte boundary
  xe = (xe - 1) | 0x0007; // byte boundary - 1
  EPD_2IN66BSES_SendCommand(0x90); // partial window
  //IO.writeDataTransaction(x / 256);
  EPD_2IN66BSES_SendData(x % 256);
  //IO.writeDataTransaction(xe / 256);
  EPD_2IN66BSES_SendData(xe % 256);
  EPD_2IN66BSES_SendData(y / 256);
  EPD_2IN66BSES_SendData(y % 256);
  EPD_2IN66BSES_SendData(ye / 256);
  EPD_2IN66BSES_SendData(ye % 256);
  EPD_2IN66BSES_SendData(0x01); // don't see any difference
  //IO.writeDataTransaction(0x00); // don't see any difference
  return (7 + xe - x) / 8; // number of bytes to transfer per line
}

void eraseDisplayPartial()
{

    EPD_2IN66BSES_Init_PartialUpdate();
    for (uint16_t twice = 0; twice < 2; twice++)
    { // leave both controller buffers equal
      EPD_2IN66BSES_SendCommand(0x91); // partial in
      EPD_2IN66BSES_setPartialRamArea(0, 0, EPD_2IN66BSES_WIDTH - 1, EPD_2IN66BSES_HEIGHT - 1);
      EPD_2IN66BSES_SendCommand(0x13);
      for (uint32_t i = 0; i < EPD_2IN66BSES_BUFFER_SIZE; i++)
      {
        EPD_2IN66BSES_SendData(0xFF); // white is 0xFF on device
      }
      EPD_2IN66BSES_SendCommand(0x12); //display refresh
      //_waitWhileBusy("eraseDisplay");
      EPD_2IN66BSES_SendCommand(0x92); // partial out

    } // leave both controller buffers equal
  }

  void EPD_2IN66BSES_EraseArea(uint16_t x, uint16_t y, uint16_t xe, uint16_t ye)
  {
       
    // 1. Инициализация режима частичного обновления
    EPD_2IN66BSES_Init_PartialUpdate();
    
    // 2. Определяем область для стирания (квадрат в центре)
    
    // 3. Устанавливаем область частичного обновления
    EPD_2IN66BSES_setPartialRamArea(x, y, 
                                    xe - 1, 
                                    ye - 1);
    
    // 4. Стираем область (заполняем белым)
    EPD_2IN66BSES_SendCommand(0x91); // partial in
    EPD_2IN66BSES_SendCommand(0x13); // запись данных
    
    // Вычисляем количество байт для стираемой области
    int bytes_per_line = (xe + 7) / 8;
    for (int y = 0; y < ye; y++) {
        for (int x = 0; x < bytes_per_line; x++) {
            EPD_2IN66BSES_SendData(0xFF); // белый цвет
        }
    }
    
    // 5. Обновление дисплея
    EPD_2IN66BSES_SendCommand(0x12); // display refresh
    DEV_Delay_ms(2000);
    EPD_2IN66BSES_SendCommand(0x92); // partial out
  }


   void drawPartialBitmap(const uint8_t* bitmap_3C, uint32_t size_3C)
   {
    EPD_2IN66BSES_SendCommand(0x91); // partial in
    EPD_2IN66BSES_setPartialRamArea(0, 0, EPD_2IN66BSES_WIDTH - 1, EPD_2IN66BSES_HEIGHT - 1);
    for (uint32_t i = 0; i < EPD_2IN66BSES_BUFFER_SIZE; i++)
    {
      uint8_t black_data = (i < size_3C) ? pgm_read_byte(&bitmap_3C[i]) : 0x00;
    //  _send8pixel(black_data);

      for (uint8_t j = 0; j < 8; j++)
        {
          uint8_t t = black_data & 0x80 ? 0x00 : 0x03;
          t <<= 4;
          black_data <<= 1;
          j++;
          t |= black_data & 0x80 ? 0x00 : 0x03;
          black_data <<= 1;
          EPD_2IN66BSES_SendData(t);
        }


    }
    EPD_2IN66BSES_SendCommand(0x12);      //display refresh
    EPD_2IN66BSES_ReadBusy();
    EPD_2IN66BSES_SendCommand(0x92); // partial out

  }


 void drawPartialBitmap(const uint8_t* bitmap, uint32_t size, uint16_t x, uint16_t y, uint16_t width, uint16_t height)
{
    // 1. Инициализация режима частичного обновления
    EPD_2IN66BSES_Init_PartialUpdate();
    
    // 2. Выравниваем x по байтам (как в setPartialRamArea)
    x &= 0xFFF8;
    
    // 3. Устанавливаем область частичного обновления
    // Используем ту же логику, что и в EraseArea
    EPD_2IN66BSES_setPartialRamArea(x, y, x + width - 1, y + height - 1);
    
    // 4. Рисуем изображение
    EPD_2IN66BSES_SendCommand(0x91); // partial in
    EPD_2IN66BSES_SendCommand(0x13); // запись данных
    
    // Вычисляем количество байт на строку (как в EraseArea)
    int bytes_per_line = (width + 7) / 8;
    
    // Отправляем данные изображения
    for (int row = 0; row < height; row++) {
        for (int col = 0; col < bytes_per_line; col++) {
            int idx = row * bytes_per_line + col;
            uint8_t data = (idx < size) ? bitmap[idx] : 0xFF; // 0xFF = белый фон
            EPD_2IN66BSES_SendData(data);
        }
    }
    
    // 5. Обновление дисплея (как в EraseArea)
    EPD_2IN66BSES_SendCommand(0x12); // display refresh
    DEV_Delay_ms(2000);
    EPD_2IN66BSES_SendCommand(0x92); // partial out
}