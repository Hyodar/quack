
#include "quack_display.h"

QuackDisplay::QuackDisplay() : display{DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET} {
    // no-op
}

void
QuackDisplay::begin() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
        Serial.println("SSD1306 allocation failed");
        for(;;); // Don't proceed, loop forever
    }

    display.setTextSize(1);
    display.setTextColor(SSD1306_WHITE);
    display.setCursor(0, 0);
    display.cp437(true);
}

void
QuackDisplay::write(const u8* const str, const u16 len) {
    display.clearDisplay();

    for(u16 i = 0; i < len; i++) {
        display.write(str[i]);
    }

    display.display();
}