
#ifndef QUACK_DISPLAY_H_
#define QUACK_DISPLAY_H_

#include "quack_config.h"

#define DISPLAY_WIDTH 128
#define DISPLAY_HEIGHT 32

#ifdef DISPLAY_ENABLED

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>

#else

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16

#include "quack_utils.h"

#define OLED_RESET 4
#define SSD1306_WHITE 1
#define SSD1306_SWITCHCAPVCC 1 // ?

class Adafruit_SSD1306 {
public:
    Adafruit_SSD1306(const u32 screenWidth, const u32 screenHeight,
                     void* i2cWire, const u8 resetPin) {
        DEBUGGING_PRINTF("[DISPLAY] Initializing with RESET pin on %d.\n", resetPin);
    }

    bool begin(int a, int b) {
        DEBUGGING_PRINTF("[DISPLAY] Beginning.\n");
        return true;
    }

    void display() {
        DEBUGGING_PRINTF("[DISPLAY] Displaying.\n");
    }

    void write(u8 character) {
        DEBUGGING_PRINTF("[DISPLAY] Writing character on screen: %c.\n", character);
    }

    void clearDisplay() {
        DEBUGGING_PRINTF("[DISPLAY] Clearing display.\n");
    }

    void setTextSize(const u32 size) {
        DEBUGGING_PRINTF("[DISPLAY] Setting text scale to %d.\n", size);
    }

    void setTextColor(const u32 color) {
        DEBUGGING_PRINTF("[DISPLAY] Setting text color to %d.\n", color);
    }

    void setCursor(const u32 x, const u32 y) {
        DEBUGGING_PRINTF("[DISPLAY] Setting cursor to (%d, %d).\n", x, y);
    }

    void cp437(const bool cp) {
        DEBUGGING_PRINTF("[DISPLAY] Setting cp to %s.\n", (cp)? "true" : "false");
    }
};

class _Wire {};

static _Wire Wire;

#endif

class QuackDisplay {

private:
    Adafruit_SSD1306 display;

public:
    QuackDisplay();

    void begin();

    void write(const u8* const str, const u16 len);
};

#endif