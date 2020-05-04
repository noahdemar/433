/* 
 * File:   ssd1306.c
 * Author: Noah DeMar
 *
 * Created on April 27, 2020, 9:25 PM
 */

// based on adafruit and sparkfun libraries
#include <string.h> // for memset
#include <xc.h> // for the core timer delay
#include "ssd1306.h"

unsigned char ssd1306_write = 0b01111000; // i2c address
unsigned char ssd1306_read = 0b01111001;  // i2c address
unsigned char ssd1306_buffer[512];  // pixel array

void ssd1306_setup() {
    _CP0_SET_COUNT(0);
    while (_CP0_GET_COUNT() < 48000000 / 2 / 50) {
    }
    ssd1306_command(SSD1306_DISPLAYOFF);
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(0x80);
    ssd1306_command(SSD1306_SETMULTIPLEX);
    ssd1306_command(0x1F); // height-1 = 31
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x0);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x02);
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0x8F);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);
    ssd1306_clear();
    ssd1306_update();
}

// send command
void ssd1306_command(unsigned char c) {
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x00);
    i2c_master_send(c);
    i2c_master_stop();
}

//  pixel update
void ssd1306_update() {
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(0xFF);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128 - 1); // Width

    unsigned short count = 512; // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char * ptr = ssd1306_buffer; // pixel buff address
    i2c_master_start();
    i2c_master_send(ssd1306_write);
    i2c_master_send(0x40);
    while (count--) {
        i2c_master_send(*ptr++);
    }
    i2c_master_stop();
}

// set a pixel val
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color) {
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 32)) {
        return;
    }

    if (color == 1) {
        ssd1306_buffer[x + (y / 8)*128] |= (1 << (y & 7));
    } else {
        ssd1306_buffer[x + (y / 8)*128] &= ~(1 << (y & 7));
    }
}

// zero every pixel value
void ssd1306_clear() {
    memset(ssd1306_buffer, 0, 512); // set all pixel bits to 0
}

