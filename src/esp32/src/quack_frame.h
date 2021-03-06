
#ifndef QUACK_FRAME_H_
#define QUACK_FRAME_H_

#include <quack_config.h>
#include <quack_utils.h>

#include <FastCRC.h>

class QuackFrame {

// | CHECKSUM | COMMAND_CODE | PARAMETER_LENGTH | PARAMETERS |

private:
    u8 buffer[FRAME_BUFFER_SIZE];
    u16 length;
    
public:
    QuackFrame();

    void setCommandCode(const u8 commandCode);
    void addParameterByte(const u8 byte);
    void copyBuffer(const u8* const buf);
    void serialize(FastCRC16* CRC16);
    void reset();

    const u8* const getBuffer() const { return buffer; }
    const u16 getLength() const { return length; }
};

#endif