#ifndef _LCD_H
#define _LCD_H  
#include  <stdint.h>



#define DevSSD1306  0x78      // device address of SSD1306 OLED, uses 8-bit address (0x3c)!!! 

#define SSD1306_DUTY_CYCLE_1_64 0x3F
#define SSD1306_FLIPS_DISPLAY 0xCF
#define SSD1306_SETCONTRAST 0x81
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_DISPLAYALLON 0xA5
#define SSD1306_NORMALDISPLAY 0xA6
#define SSD1306_INVERTDISPLAY 0xA7
#define SSD1306_DISPLAYOFF 0xAE
#define SSD1306_DISPLAYON 0xAF
#define SSD1306_SETDISPLAYOFFSET 0xD3
#define SSD1306_SETCOMPINS 0xDA
#define SSD1306_SETVCOMDETECT 0xDB
#define SSD1306_SETDISPLAYCLOCKDIV 0xD5
#define SSD1306_SETPRECHARGE 0xD9
#define SSD1306_SETMULTIPLEX 0xA8
#define SSD1306_SETLOWCOLUMN 0x00
#define SSD1306_SETHIGHCOLUMN 0x10
#define SSD1306_SETSTARTLINE 0x40
#define SSD1306_MEMORYMODE 0x20
#define SSD1306_COLUMNADDR 0x21
#define SSD1306_PAGEADDR   0x22
#define SSD1306_COMSCANINC 0xC0
#define SSD1306_COMSCANDEC 0xC8
#define SSD1306_SEGREMAP 0xA1
#define SSD1306_CHARGEPUMP 0x8D
#define SSD1306_SWITCHCAPVCC 0x2
#define SSD1306_NOP 0xE3
#define SSD1306_NO_OFFSET 0x0
#define SSD1306_RESISTOR_RATIO 0x80


#define SSD1306_WIDTH 128
#define SSD1306_HEIGHT 64


uint8_t buffer[(128 * 64) / 8];

void drawPixel(int16_t x, int16_t y);
void clearBuffer(uint8_t *buff);
void drawCircle(int16_t x0, int16_t y0, int16_t r);
void drawBuffer(uint8_t column_address, uint8_t page_address, uint8_t *buff);
void lcd_draw_char(unsigned char column, unsigned char page, unsigned char letter, uint8_t *buff);
void lcd_draw_string(uint8_t column, uint8_t page, char *string, uint8_t *buff);
void setup_i2c();

/**@}*/
#endif