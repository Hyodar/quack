
#ifndef QUACK_DISPLAY_H_
#define QUACK_DISPLAY_H_

#include "quack_config.h"
#include "quack_utils.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#ifdef DISPLAY_ENABLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#else

#include <cstdio>

#define OLED_RESET 4

class QuackDisplay {
private:
    
    Adafruit_SSD1306(u32 screenWidth, u32 screenHeight, void* i2cWire, u8 resetPin) {}

    void display() { printf("[DISPLAY] Displaying.\n"); }
};

#endif

#endif