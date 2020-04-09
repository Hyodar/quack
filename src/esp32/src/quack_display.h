
#ifndef QUACK_DISPLAY_H_
#define QUACK_DISPLAY_H_

#include <quack_config.h>

#ifdef DISPLAY_ENABLED

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <Arduino.h>
#undef u16

#include <quack_utils.h>

#else

// little hack here because of u16 typedef inside USBAPI.h
#define u16 __u16
#include <Arduino.h>
#undef u16

#include <quack_utils.h>

#define SSD1306_WHITE 1
#define SSD1306_SWITCHCAPVCC 1 // ?

class Adafruit_SSD1306 {
public:
    Adafruit_SSD1306(const u32 screenWidth, const u32 screenHeight,
                     void* i2cWire, const u8 resetPin) {
        // no-op                         
    }

    bool begin(int a, int b) {
        // no-op        
        return true;
    }

    void display() {
        // no-op        
    }

    void write(u8 character) {
        // no-op            
    }

    void clearDisplay() {
        // no-op        
    }

    void setTextSize(const u32 size) {
        // no-op            
    }

    void setTextColor(const u32 color) {
        // no-op            
    }

    void setCursor(const u32 x, const u32 y) {
        // no-op            
    }

    void cp437(const bool cp) {
        // no-op        
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