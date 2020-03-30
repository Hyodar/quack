
#include "quack_serial.h"
#include "quack.h"

#include "fastcrc/FastCRC.h"

#define DECLARE_STR(str) (u8*) str, sizeof(str) - 1

Quack quack;

void
setup() {
    quack.begin();
}

void
loop() {
    quack.run();
}

int
main(void) {

    setup();

    const u8 payload[] = {'\x27', '\x04', '\x00', '1', '0', '0', '0', '\x00'};

    FastCRC16 CRC16;
    printf("%d\n", CRC16.ccitt((uint8_t*) payload, sizeof(payload) - 1));
    // printf("%d\n", *((uint16_t*) "\xd7H"));
    
    u16 checksum = CRC16.ccitt((uint8_t*) payload, sizeof(payload) - 1);

    // separator (start)
    Serial1.write(DECLARE_STR("\x00"));

    // checksum
    Serial1.write((u8*) &checksum, sizeof(u16));

    // command
    Serial1.write(DECLARE_STR("\x27"));

    // length
    Serial1.write(DECLARE_STR("\x04\x00"));

    // params
    Serial1.write(DECLARE_STR("1000"));

    // separator (end)
    Serial1.write(DECLARE_STR("\x00"));

    for(;;) {
        loop();
    }

    return 0;
}