
#ifndef QUACK_DISPLAY_H_
#define QUACK_DISPLAY_H_

#include "quack_config.h"
#include "quack_utils.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#ifdef DISPLAY_ENABLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#else

#include <cstdio>

#define OLED_RESET 4
#define SSD1306_WHITE 1

class Adafruit_SSD1306 {
public:
    Adafruit_SSD1306(const u32 screenWidth, const u32 screenHeight,
                     void* i2cWire, const u8 resetPin) {
        printf("[DISPLAY] Initializing with RESET pin on %d.\n", resetPin);
    }

    void display() {
        printf("[DISPLAY] Displaying.\n");
    }

    void write(u8 character) {
        printf("[DISPLAY] Writing character on screen: %d.\n", character);
    }

    void clearDisplay() {
        printf("[DISPLAY] Clearing display.\n");
    }

    void setTextSize(const u32 size) {
        printf("[DISPLAY] Setting text scale to %d.\n", size);
    }

    void setTextColor(const u32 color) {
        printf("[DISPLAY] Setting text color to %d.\n", color);
    }

    void setCursor(const u32 x, const u32 y) {
        printf("[DISPLAY] Setting cursor to (%d, %d).\n", x, y);
    }

    void cp437(const bool cp) {
        printf("[DISPLAY] Setting cp to %s.\n", (cp)? "true" : "false");
    }
};

class _Wire {};

static _Wire Wire;

#endif

class QuackDisplay {

private:
    Adafruit_SSD1306 display;

public:
    QuackDisplay() : display{DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET} {
        
        display.setTextSize(1);
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0, 0);
        display.cp437(true);
    }

    void write(const u8* const str, const u16 len) {
        display.clearDisplay();

        for(u16 i = 0; i < len; i++) {
            display.write(str[i]);
        }

        display.display();
    }
};

#endif