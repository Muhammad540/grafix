#ifndef GRAFIX_H
#define GRAFIX_H

#include <Arduino.h>
#include <Wire.h>

#define SCREEN_WIDTH 128
#define SCREEN_HEIGHT 64
#define I2C_ADDRESS 0x3C
#define BUFFER_LENGTH (SCREEN_HEIGHT * SCREEN_WIDTH / 8)

class Grafix {
    public:
        Grafix();
        /** setup the i2c and oled startup sequence */
        void begin();
        /** Push the Buffer to the screen */
        void display();
        /** clear the buffer */
        void clear();
        /** 0-255 brightness levels */
        void setBrightness(uint8_t level);
        /** graphic display */
        void drawPixel(int16_t x, int16_t y, uint8_t color = 1);

        /** simple shape primitives */
        void drawLine(int16_t x0, int16_t y0, int16_t x1, int16_t y1, uint8_t color);
        void drawRect(int16_t x , int16_t y, int16_t w, int16_t h);
        void fillRect(int16_t x , int16_t y, int16_t w, int16_t h);

        /** Text engine */
        void setCursor(int16_t x, int16_t y);
        void print(const char* str);
        void print(int number);

    private:
        uint8_t buffer[BUFFER_LENGTH];
        int16_t cursorX, cursorY;

        /** I2C comm */
        void sendCommand(uint8_t command);
        void sendData(uint8_t data);

        void drawChar(int16_t x, int16_t y, unsigned char c);
}
#endif // GRAFIX_H