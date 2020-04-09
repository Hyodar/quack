
#include "quack_display.h"

const u8 DISPLAY_WIDTH      = 0x80;
const u8 DISPLAY_HEIGHT     = 0x20;

const u8 OLED_ADDRESS       = 0x3C;
const u8 OLED_TEXT_SIZE     = 2;
const u8 OLED_TEXT_COLOR    = SSD1306_WHITE;
const u8 OLED_CURSOR_POS_X  = 0;
const u8 OLED_CURSOR_POS_Y  = 5;
const u8 OLED_SCROLL_STEP   = 1;
const i8 OLED_RESET         = -1;
const bool OLED_CP437       = true;

QuackDisplay::QuackDisplay() : display{DISPLAY_WIDTH, DISPLAY_HEIGHT, &Wire, OLED_RESET},
                               displayString{0}, displayStringLen{0}, displayPosition{0},
                               minDisplayPosition{0} {
    // no-op
}

void
QuackDisplay::begin() {
    if(!display.begin(SSD1306_SWITCHCAPVCC, OLED_ADDRESS)) {
        DEBUGGING_PRINTF("[DISPLAY] [!] SSD1306 allocation failed.\n");
        for(;;); // Don't proceed, loop forever
    }

    display.clearDisplay();

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINTF("[DISPLAY] Initializing with address on ");
    DEBUGGING_PRINT(OLED_ADDRESS, HEX);
    DEBUGGING_PRINT(".\n");
#endif

    display.setTextSize(OLED_TEXT_SIZE);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINTF("[DISPLAY] Setting text scale to ");
    DEBUGGING_PRINT(OLED_TEXT_SIZE);
    DEBUGGING_PRINTF(".\n");
#endif

    display.setTextColor(OLED_TEXT_COLOR);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINTF("[DISPLAY] Setting text color to ");
    DEBUGGING_PRINT(OLED_TEXT_COLOR, HEX);
    DEBUGGING_PRINTF(".\n");
#endif

    display.setCursor(OLED_CURSOR_POS_X, OLED_CURSOR_POS_Y);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINTF("[DISPLAY] Setting cursor to (");
    DEBUGGING_PRINT(OLED_CURSOR_POS_X);
    DEBUGGING_PRINTF(", ");
    DEBUGGING_PRINT(OLED_CURSOR_POS_Y);
    DEBUGGING_PRINTF(").\n");
#endif

    display.cp437(true);

#ifdef DISPLAY_DEBUGGING
    DEBUGGING_PRINTF("[DISPLAY] Setting cp to ");
    DEBUGGING_PRINT(OLED_CP437);
    DEBUGGING_PRINTF(".\n");
#endif

    display.setTextWrap(false);
}

void
QuackDisplay::write(const u8* const str, const u16 len) {

    displayStringLen = (len <= 100)? len : 100;

    for(u16 i = 0; i < displayStringLen; i++) displayString[i] = str[i];
    minDisplayPosition = - 6 * OLED_TEXT_SIZE * len;
    displayPosition = minDisplayPosition;
}

void
QuackDisplay::write(const u8* const str) {
    u16 i = 0;
    for(; i < 100 && str[i]; i++) displayString[i] = str[i];
    
    displayStringLen = i;
    minDisplayPosition = - 6 * OLED_TEXT_SIZE * displayStringLen;
    displayPosition = minDisplayPosition;
}

void
QuackDisplay::scroll() {
    if(NOT displayString[0]) return;

    display.clearDisplay();

    display.setCursor(OLED_CURSOR_POS_X + displayPosition, OLED_CURSOR_POS_Y);

    for(u16 i = 0; i < displayStringLen; i++) {
        display.write(displayString[i]);
    }

    display.display();

    displayPosition += OLED_SCROLL_STEP;
    if(displayPosition >= DISPLAY_WIDTH) {
        displayPosition = minDisplayPosition;
    }
}