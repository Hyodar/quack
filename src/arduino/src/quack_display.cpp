
#include "quack_display.h"

QuackDisplay::QuackDisplay() : display{DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET} {
}

void
QuackDisplay::begin() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        DEBUGGING_PRINT(F("[DISPLAY] [!] SSD1306 allocation failed.\n"));
        for(;;); // Don't proceed, loop forever
    }

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Initializing with RESET pin on "));
    DEBUGGING_PRINT(OLED_ADDRESS, HEX);
    DEBUGGING_PRINT(F(".\n"));
#endif

    display.setTextSize(OLED_TEXT_SIZE);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Setting text scale to "));
    DEBUGGING_PRINT(OLED_TEXT_SIZE);
    DEBUGGING_PRINT(F(".\n"));
#endif

    display.setTextColor(OLED_TEXT_COLOR);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Setting text color to "));
    DEBUGGING_PRINT(OLED_TEXT_COLOR, HEX);
    DEBUGGING_PRINT(F(".\n"));
#endif

    display.setCursor(OLED_CURSOR_POS_X, OLED_CURSOR_POS_Y);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Setting cursor to ("));
    DEBUGGING_PRINT(OLED_CURSOR_POS_X);
    DEBUGGING_PRINT(F(", "));
    DEBUGGING_PRINT(OLED_CURSOR_POS_Y);
    DEBUGGING_PRINT(F(").\n"));
#endif

    display.cp437(true);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Setting cp to "));
    DEBUGGING_PRINT(OLED_CP437);
    DEBUGGING_PRINT(F(".\n"));
#endif
}

void
QuackDisplay::write(const u8* const str, const u16 len) {
    display.clearDisplay();

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Clearing.\n"));
#endif

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Writing '"));
    DEBUGGING_PRINTSTR(str, len);
    DEBUGGING_PRINT(F("'.\n"));
#endif

    for(u16 i = 0; i < len; i++) {
        display.write(str[i]);
    }

    display.display();

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINT(F("[DISPLAY] Displaying.\n"));
#endif
}