
#ifndef QUACK_FRAME_H_
#define QUACK_FRAME_H_

#include "quack_config.h"
#include "quack_utils.h"

#include "fastcrc/FastCRC.h"

#define HEADER_SIZE sizeof(u16) + sizeof(u8) + sizeof(u16)

class QuackFrame {

// | CHECKSUM | COMMAND_CODE | PARAMETER_LENGTH | PARAMETERS |

private:
    u8 buffer[BUFFER_SIZE + HEADER_SIZE];
    u16 length;
    
public:
    QuackFrame();

    void setCommandCode(const u8 commandCode);
    void addParameterByte(const u8 byte);
    void copyBuffer(const u8* const buf, const u16 bufLength);
    void serialize(FastCRC16* CRC16);

    #ifdef FRAME_DEBUGGING
    const u8* const getBuffer() const { return buffer; }
    #endif
};

#endif