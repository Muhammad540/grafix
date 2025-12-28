#include "grafix.h"

// stndrd hex codes from the datasheet (link: https://cdn-shop.adafruit.com/datasheets/SSD1306.pdf)
#define SSD1306_MEMORYMODE          0x20
#define SSD1306_COLUMNADDR          0x21
#define SSD1306_PAGEADDR            0x22
#define SSD1306_SETCONTRAST         0x81
#define SSD1306_CHARGEPUMP          0x8D
#define SSD1306_SEGREMAP            0xA1
#define SSD1306_DISPLAYALLON_RESUME 0xA4
#define SSD1306_NORMALDISPLAY       0xA6
#define SSD1306_INVERTDISPLAY       0xA7
#define SSD1306_SETMULTIPLEX        0xA8
#define SSD1306_DISPLAYOFF          0xAE
#define SSD1306_DISPLAYON           0xAF
#define SSD1306_COMSCANDEC          0xC8
#define SSD1306_SETDISPLAYOFFSET    0xD3
#define SSD1306_SETCOMPINS          0xDA
#define SSD1306_SETVCOMDETECT       0xDB
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5
#define SSD1306_SETPRECHARGE        0xD9
#define SSD1306_SETSTARTLINE        0x40
#define COMMAND_MODE 0x00
#define DATA_MODE 0x40
#define COMM_SPEED 400000

Grafix::Grafix() {
    cursorX = 0;
    cursorY = 0;
}

void Grafix::sendCommand(uint8_t command){
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(COMMAND_MODE); 
    Wire.write(command);
    Wire.endTransmission();
}

void Grafix::sendData(uint8_t data){
    Wire.beginTransmission(I2C_ADDRESS);
    Wire.write(DATA_MODE);
    Wire.write(data);
    Wire.endTransmission();
}

void Grafix::begin() {
    Wire.begin();
    Wire.setClock(COMM_SPEED);

    // initialize sequence based on the datasheet
    sendCommand(SSD1306_DISPLAYOFF);
    sendCommand(SSD1306_SETDISPLAYCLOCKDIV); sendCommand(0x80); 
    sendCommand(SSD1306_SETMULTIPLEX);       sendCommand(0x3F); 
    sendCommand(SSD1306_SETDISPLAYOFFSET);   sendCommand(0x0);  
    sendCommand(SSD1306_SETSTARTLINE | 0x0);                    
    sendCommand(SSD1306_CHARGEPUMP);         sendCommand(0x14); 
    sendCommand(SSD1306_MEMORYMODE);         sendCommand(0x00); 
    sendCommand(SSD1306_SEGREMAP);                              
    sendCommand(SSD1306_COMSCANDEC);                            
    sendCommand(SSD1306_SETCOMPINS);         sendCommand(0x12);
    sendCommand(SSD1306_SETCONTRAST);        sendCommand(0xCF); 
    sendCommand(SSD1306_SETPRECHARGE);       sendCommand(0xF1);
    sendCommand(SSD1306_SETVCOMDETECT);      sendCommand(0x40);
    sendCommand(SSD1306_DISPLAYALLON_RESUME);
    sendCommand(SSD1306_NORMALDISPLAY);
    sendCommand(SSD1306_DISPLAYON);

    clear();
    display();
}

void Grafix::clear(){
    memset(buffer, 0, sizeof(buffer));
    cursorX = 0;
    cursorY = 0;
}

/**
 * screen is divided into 8 pages 
 * Each page has 8 rows and 128 columns
 * A single bytes that we write represents a veritcal column of 8 pixels in a page
 */
void Grafix::drawPixel(int16_t x, int16_t y, uint8_t color){
    if ((x < 0) || (x >= SCREEN_WIDTH) || (y < 0) || (y >= SCREEN_HEIGHT))
        return;

    // x is the column
    // y is the row, y / 8 = page num
    uint16_t index = x + (y / 8) * SCREEN_WIDTH;

    if (color) {
        // setting
        buffer[index] |= (1 << (y & 7));
    } else { 
        // clearing
        buffer[index] &= ~(1<< (y & 7));
    }
}

void Grafix::display() {
    sendCommand(SSD1306_COLUMNADDR);
    sendCommand(0);
    sendCommand(SCREEN_WIDTH - 1);

    sendCommand(SSD1306_PAGEADDR);
    sendCommand(0);
    sendCommand(7);

    for (uint16_t i = 0; i < sizeof(buffer); i++) {
        if (i % 16 == 0) {
            if (i > 0) Wire.endTransmission();
            Wire.beginTransmission(I2C_ADDRESS);
            // control byte: this informs the screen that they next incoming bytes are data 
            Wire.write(0x40);
        }
        Wire.write(buffer[i]);
    }
    Wire.endTransmission();
}

void Grafix::drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color) {
    int16_t steep = abs(y1 - y0) > abs(x1 - x0);
    if (steep) {
        int16_t temp;
        temp = x0; x0 = y0; y0 = temp;
        temp = x1; x1 = y1; y1 = temp;
    }

    if (x0 > x1) {
        int16_t temp;
        temp = x0; x0 = x1; x1 = temp;
        temp = y0; y0 = y1; y1 = temp;
    }

    int16_t dx, dy;
    dx = x1 - x0;
    dy = abs(y1 - y0);

    int16_t err = dx / 2;
    int16_t ystep;

    if (y0 < y1) {
        ystep = 1;
    } else {
        ystep = -1;
    }

    for (; x0 <= x1; x0++) {
        if (steep) {
            drawPixel(y0, x0, color);
        } else {
            drawPixel(x0, y0, color);
        }
        err -= dy;
        if (err < 0) {
            y0 += ystep;
            err += dx;
        }
    }
}