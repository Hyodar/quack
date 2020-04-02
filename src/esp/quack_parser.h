
#ifndef QUACK_PARSER_H_
#define QUACK_PARSER_H_

/*****************************************************************************
 * quack_parser.h:
 *  Parses a string and sends the commands to the CommandManager.
 * 
*****************************************************************************/

#include "fastcrc/FastCRC.h"

#include "quack_config.h"
#include "quack_utils.h"

#include "quack_frame.h"
#include "quack_hid_locale.h"

class QuackParser {

public:
    enum QuackLineState {
        FREE_TO_PARSE,
        PARSING,
        DONE_PARSING,
        SENDING,
        WAITING_RESPONSE
    };

    struct QuackLine {
        u16 lineOrder;
        QuackLineState state;
        QuackFrame data;

        QuackLine();
    };

private:

    // CommandManager commandManager;
    // ParsingState parsingState;
    
    QuackLine quackLines[QUACKLINES_BUFFER];

    u8 activeLine;

    u16 nextOrder;
    u16 orderCount;

    FastCRC16 CRC16;
    QuackHIDLocale* keyboardLocale;

    const bool updateActiveLine();
    const u16 getCommandCode(const u8* const str, const u8 len, bool continuation) const;
    void parseKeysParams(const u8* const str, const u16 len);
    void parseStringParams(const u8* const str, const u16 len);
    void replaceKeyword(const u8* const str, const u16 len);
    void replaceKey(const u8 str);
    // void changeLocale(const u8* const str);

public:
    QuackParser();

    void begin();

    const bool parse(const u8* const str, const u16 len, const bool continuation=false);
    QuackParser::QuackLine* getProcessedLine();
};

#endif