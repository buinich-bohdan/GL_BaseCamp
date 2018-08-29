
#include "lcd.h"
#include <string.h>
#include "i2cmaster.h"
#include "myFont.h"

void setup_i2c()
{
    // Init sequence for 128x64 OLED module
    i2c_write(SSD1306_DISPLAYOFF); // Display Off

    i2c_write(SSD1306_SETLOWCOLUMN | SSD1306_NO_OFFSET); // low col = 0
    i2c_write(SSD1306_SETHIGHCOLUMN | SSD1306_NO_OFFSET); // hi col = 0
    i2c_write(SSD1306_SETSTARTLINE | SSD1306_NO_OFFSET); // line #0

    i2c_write(SSD1306_SETCONTRAST); // Set Contrast 0x81
    i2c_write(SSD1306_FLIPS_DISPLAY); // flips display
    i2c_write(SSD1306_SEGREMAP); // Segremap - 0xA1
    i2c_write(SSD1306_COMSCANDEC); // COMSCAN DEC 0xC8 C0
    i2c_write(SSD1306_NORMALDISPLAY); // Normal Display 0xA6 (Invert A7)

    i2c_write(SSD1306_DISPLAYALLON_RESUME); // DISPLAY ALL ON RESUME - 0xA4
    i2c_write(SSD1306_SETMULTIPLEX); // Set Multiplex 0xA8
    i2c_write(SSD1306_DUTY_CYCLE_1_64); // 1/64 Duty Cycle

    i2c_write(SSD1306_SETDISPLAYOFFSET); // Set Display Offset 0xD3
    i2c_write(SSD1306_NO_OFFSET); // no offset

    i2c_write(SSD1306_SETDISPLAYCLOCKDIV); // Set Display Clk Div 0xD5
    i2c_write(SSD1306_RESISTOR_RATIO); // Recommneded resistor ratio 0x80

    i2c_write(SSD1306_SETPRECHARGE); // Set Precharge 0xd9
    i2c_write(0xF1);

    i2c_write(SSD1306_SETCOMPINS); // Set COM Pins0xDA
    i2c_write(0x12);

    i2c_write(SSD1306_SETVCOMDETECT); // Set VCOM Detect - 0xDB
    i2c_write(0x40);

    i2c_write(SSD1306_MEMORYMODE); // Set Memory Addressing Mode
    i2c_write(SSD1306_SETLOWCOLUMN); // 0x00 - Horizontal

    i2c_write(SSD1306_SETSTARTLINE | SSD1306_NO_OFFSET); // Set start line at line 0 - 0x40

    i2c_write(SSD1306_CHARGEPUMP); // Charge Pump -0x8D
    i2c_write(0x14);

    i2c_write(SSD1306_DISPLAYALLON_RESUME); //--turn on all pixels - A5. Regular mode A4
    i2c_write(SSD1306_DISPLAYON); //--turn on oled panel - AF
}

void drawPixel(int16_t x, int16_t y){
    if (x >= 0 && x < SSD1306_WIDTH && y >= 0 && y < SSD1306_HEIGHT) {
    buffer[x+(y/8)*128] |= (1 << (y & 7));}
}

void clearBuffer(uint8_t *buff){
    memset(buff, 0, ((128 * 64)/8));
}

void drawCircle(int16_t x0, int16_t y0, int16_t r)
{
    int16_t f = 1 - r;
    int16_t ddF_x = 1;
    int16_t ddF_y = -2 * r;
    int16_t x = 0;
    int16_t y = r;

    drawPixel(x0, y0+r);
    drawPixel(x0, y0-r);
    drawPixel(x0+r, y0);
    drawPixel(x0-r, y0);

    while (x<y) {
        if (f >= 0) {
            y--;
            ddF_y += 2;
            f += ddF_y;
        }
        x++;
        ddF_x += 2;
        f += ddF_x;

        drawPixel(x0 + x, y0 + y);
        drawPixel(x0 - x, y0 + y);
        drawPixel(x0 + x, y0 - y);
        drawPixel(x0 - x, y0 - y);
        drawPixel(x0 + y, y0 + x);
        drawPixel(x0 - y, y0 + x);
        drawPixel(x0 + y, y0 - x);
        drawPixel(x0 - y, y0 - x);
    }
}


void drawBuffer(uint8_t column_address, uint8_t page_address, uint8_t *buff){
    i2c_start(DevSSD1306+I2C_WRITE);  
    i2c_write(0x21);        // column address
    i2c_write(0);           // column start address (0 = reset)
    i2c_write(127);         // column end addres (127 = reset)

    i2c_write(0x22);        // page address
    i2c_write(0);           // page start address (0 = reset);
    i2c_write(7);           // page end address

    uint8_t twbrbackup = TWBR;
    TWBR = 12;

    for (uint16_t i=0; i<((128*64)/8); i++){
        unsigned char ret = i2c_start(DevSSD1306+I2C_WRITE);   // set device address and write mode
    
            i2c_write(0x40);                        // set display RAM display start line register
            for (uint8_t x=0; x<16; x++){
                i2c_write(buff[i]);
                i++;
            }
            i--;
            i2c_stop();
            TWBR = twbrbackup;
    //    }
    }
}

void lcd_draw_char(unsigned char column, unsigned char page, unsigned char letter, uint8_t *buff){
    uint8_t ascii_offset = 32;
    for(uint8_t i=0; i<5; i++){
        buff[i+((page*128)+column)] = Ascii_1[letter-ascii_offset][i];
    }
}
void lcd_draw_string(uint8_t column, uint8_t page, char *string, uint8_t *buff){
    uint8_t i = 0;
    while(string[0] != 0){
        lcd_draw_char(column+(5*i), page, (string[0]), buff);
        string++;
        i++;
    }
}