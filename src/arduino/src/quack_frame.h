
#ifndef QUACK_FRAME_H_
#define QUACK_FRAME_H_

#include <quack_config.h>
#include <quack_utils.h>

#define HEADER_SIZE sizeof(u16) + sizeof(u8) + sizeof(u16)

class FastCRC16;

class QuackFrame {

// | CHECKSUM | COMMAND_CODE | PARAMETER_LENGTH | PARAMETERS |
#ifndef FRAME_DEBUGGING
private:
#else
public:
#endif
    u8 commandCode;
    u16 length;
    const u8* params;
    
public:
    QuackFrame();

    const bool deserialize(const u8* const str, const u16 len, FastCRC16* CRC16);
};

#endif