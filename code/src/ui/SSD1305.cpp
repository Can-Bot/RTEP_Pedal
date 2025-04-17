/*****************************************************************************
* | File      	:   ssd1305.c
* | Author      :   Waveshare team
* | Function    :   2.23inch OLED HAT Drive function
* | Info        :
*----------------
* |	This version:   V1.0
* | Date        :   2023-12-20
* | Info        :
* -----------------------------------------------------------------------------
#
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
#include "SSD1305.h"
#include "DEV_Config.h"
#include <stdint.h>
unsigned char buffer[WIDTH * HEIGHT / 8];

void command(uint8_t cmd){
   #if USE_SPI
	OLED_DC_0;
    // OLED_CS_0;
    DEV_SPI_WriteByte(cmd);
    // OLED_CS_1;
#elif USE_IIC
    I2C_Write_Byte(cmd,IIC_CMD);
#endif
}

void data(uint8_t Data)
{
	#if USE_SPI
		OLED_DC_1;
		DEV_SPI_WriteByte(Data);
	#elif USE_IIC
		I2C_Write_Byte(Data,IIC_RAM);
	#endif
}
void SSD1305_begin()
{
	
	OLED_RST_1;
    DEV_Delay_ms(100);
    OLED_RST_0;
    DEV_Delay_ms(100);
    OLED_RST_1;
    DEV_Delay_ms(100);

    command(0xAE);//--turn off oled panel
	command(0x04);//--Set Lower Column Start Address for Page Addressing Mode	
	command(0x10);//--Set Higher Column Start Address for Page Addressing Mode
	command(0x40);//---Set Display Start Line
	command(0x81);//---Set Contrast Control for BANK0
	command(0x80);//--Contrast control register is set
	command(0xA1);//--Set Segment Re-map
	command(0xA6);//--Set Normal/Inverse Display
	command(0xA8);//--Set Multiplex Ratio
	command(0x1F);
	command(0xC8);//--Set COM Output Scan Direction
	command(0xD3);//--Set Display Offset
	command(0x00);
	command(0xD5);//--Set Display Clock Divide Ratio/ Oscillator Frequency
	command(0xF0);
	command(0xD8);//--Set Area Color Mode ON/OFF & Low Power Display Mode
	command(0x05);
	command(0xD9);//--Set pre-charge period
	command(0xC2);
	command(0xDA);//--Set COM Pins Hardware Configuration
	command(0x12);
	command(0xDB);//--Set VCOMH Deselect Level
	command(0x3C);//--Set VCOM Deselect Level
	command(0xAF);//--Normal Brightness Display ON
}
void SSD1305_clear()
{
	size_t i;
	for(i = 0;i<sizeof(buffer);i++)
	{
		buffer[i] = 0;
	}
}
void SSD1305_pixel(int x, int y, char color)
{
    if (x >= WIDTH || y >= HEIGHT) return; 
    if (color)
        buffer[x + (y / 8) * WIDTH] |= (1 << (y % 8)); 
    else
        buffer[x + (y / 8) * WIDTH] &= ~(1 << (y % 8));
}

void SSD1305_char(unsigned char x,unsigned char y,char acsii,char size,char mode)
{
	unsigned char i,j,y0=y;
	char temp;
	unsigned char ch = acsii - ' ';
	for(i = 0;i<size;i++)
	{
		if(size == 12)
		{
			if(mode)temp=Font1206[ch][i];
			else temp = ~Font1206[ch][i];
		}
		else 
		{			
			if(mode)temp=Font1608[ch][i];
			else temp = ~Font1608[ch][i];
		}
		for(j =0;j<8;j++)
		{
			if(temp & 0x80) SSD1305_pixel(x,y,1);
			else SSD1305_pixel(x,y,0);
			temp <<=1;
			y++;
			if((y-y0)==size)
			{
				y = y0;
				x ++;
				break;
			}
		}
	}
}

void SSD1305_string(unsigned char x,unsigned char y, const char *pString,
					unsigned char Size,unsigned char Mode)
{
    while (*pString != '\0') {       
        if (x > (WIDTH - Size / 2)) {
			x = 0;
			y += Size;
			if (y > (HEIGHT - Size)) {
				y = x = 0;
			}
		}
		
        SSD1305_char(x, y, *pString, Size, Mode);
        x += Size / 2;
        pString ++;
    }
}

void SSD1305_bitmap(unsigned char x, unsigned char y, const unsigned char *pBmp,
	unsigned char chWidth, unsigned char chHeight, char mode)
{
	unsigned char i, j, byteWidth = (chWidth + 7) / 8;
	for (j = 0; j < chHeight; j++) {
		for (i = 0; i < chWidth; i++) {
		// Check if the pixel in the bitmap is set
		int bit = (*(pBmp + j * byteWidth + i / 8) & (128 >> (i & 7))) ? 1 : 0;
			if (mode == 1) {
				// Set the pixel (layer it)
				SSD1305_pixel(x + i, y + j, bit);
			} else if (mode == 0) {
				// Clear the pixel
				SSD1305_pixel(x + i, y + j, !bit);
			}
		}	
	}
}

void SSD1305_display()
{
    UWORD page, column;
    for (page = 0; page < (HEIGHT / 8); page++) {  
        /* set page address */
        command(0xB0 + page);
        /* set low column address */
        command(0x04); 
        /* set high column address */
        command(0x10); 
        /* write data */
		for(column=0; column< WIDTH; column++) {
            data(buffer[page * WIDTH + column]);
        }
    }
}

void SSD1305_string_5x7(unsigned char x, unsigned char y, const char *pString, unsigned char Mode)
{
    unsigned char x_pos = x;
    
    // Process each character in the string
    while (*pString != '\0') {
        // Calculate character index (ASCII value - 32 for space)
        unsigned char ch = *pString - ' ';
        
        // Skip if out of bounds of the 95-character array
        if (ch >= 95) {
            x_pos += 6;  // 5px width + 1px spacing
            pString++;
            continue;
        }
        
        // Draw character column by column
        for (unsigned char col = 0; col < 5; col++) {
            unsigned char byte = Font5x7[ch][col];
            
            // Skip empty columns for efficiency
            if (byte == 0 && Mode == 1) continue;
            
		// Draw each pixel in the column
		for (unsigned char row = 0; row < 7; row++) {
			if (byte & (1 << row)) {
				SSD1305_pixel(x_pos + col, y + row, Mode);
			}
			 // Clear pixels if we're in inverse mode
			else if (Mode == 0) { 
				SSD1305_pixel(x_pos + col, y + row, 1); 
			}
		}
        }
        
        // Move to next character position with 1 pixel spacing
        x_pos += 6;
        pString++;
    }
}


void SSD1305_string_4x7(unsigned char x, unsigned char y, const char *pString, unsigned char Mode)
{
    unsigned char x_pos = x;
    
    // Process each character in the string
    while (*pString != '\0') {
        unsigned char ch = *pString - ' ';
        
        // Skip if out of bounds of the 95-character array
        if (ch >= 95) {
            x_pos += 5;
            pString++;
            continue;
        }

		// Special case for space character (index 0)
		if (ch == 0) {
			// Just skip 2 columns for space instead of 5
			x_pos += 2;
			pString++;
			continue;
		}
        
        // Draw character column by column
        for (unsigned char col = 0; col < 4; col++) {
            unsigned char byte = Font4x7[ch][col];
            
            // Skip empty columns for efficiency
            if (byte == 0 && Mode == 1) continue;
            
            // Draw each pixel in the column
            for (unsigned char row = 0; row < 7; row++) {
                if (byte & (1 << row)) {
                    SSD1305_pixel(x_pos + col, y + row, Mode);
                }
				// Only clear pixels if in inverse mode
                else if (Mode == 0) {  
                    SSD1305_pixel(x_pos + col, y + row, 1);
                }
            }
        }
        // Move to next character position with 1 pixel spacing, 4px width + 1px spacing
        x_pos += 5;
        pString++;
    }
}