
#ifndef QUACK_PARSER_H_
#define QUACK_PARSER_H_

/*****************************************************************************
 * quack_parser.h:
 *  Parses a string and sends the commands to the CommandManager.
 * 
*****************************************************************************/

#include <FastCRC.h>

#include "quack_utils.h"

#include "command_manager.h"
#include "quack_frame.h"

class QuackParser {

enum ParsingState {
    NONE,
    COMMAND,
    PARAMS,
    CONTINUE
};

private:
    CommandManager commandManager;
    
    QuackFrame quackFrame;
    FastCRC16 CRC16;

    void sendCommand();

public:
    QuackParser();

    void begin();

    const bool parse(const u8* const str, const u16 len);
    const u32 parseU32(const u8* const str, const u16 len);

};

#endif