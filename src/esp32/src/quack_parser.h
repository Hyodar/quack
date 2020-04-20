
#ifndef QUACK_PARSER_H_
#define QUACK_PARSER_H_

/*****************************************************************************
 * quack_parser.h:
 *  Parses a string and sends the commands to the CommandManager.
 * 
*****************************************************************************/

#include <FastCRC.h>

#include <quack_config.h>
#include <quack_utils.h>

#include "quack_frame.h"
#include "quack_hid_locale.h"

#include <FS.h>
#include <SPIFFS.h>

class QuackDisplay;

class QuackParser {

public:
    enum State {
        NONE           = 0,
        BUFFER_PARSING = 1,
        FILE_PARSING   = 2,
    };

    struct QuackLine {
        enum State {
            FREE_TO_PARSE,
            PARSING,
            DONE_PARSING,
            SENDING,
            WAITING_RESPONSE,
        };

        u16 lineOrder;
        State state;
        QuackFrame data;

        QuackLine();
    };

private:

    State state;
    
    QuackLine quackLines[QUACKLINES_BUFFER];

    u8 activeLine;

    u16 nextOrder;
    u16 orderCount;

    FastCRC16 CRC16;
    QuackHIDLocale* keyboardLocale;

    u8 buffer[1000 + 1];

    fs::File activeFile;
    u8 activeCommand;

    QuackDisplay* quackDisplay;

    const bool updateActiveLine();
    const u8 getCommandCode(const u8* const str);
    void parseKeysParams(const u8* const str);
    void parseStringParams(const u8* const str);
    void replaceKeyword(const u8* const str, const u16 len);
    void replaceKey(const u8 str);
    
    const bool parse(const u8* const str);
    const bool canParse();

    const bool runLocalCommand(const u8* const params);

public:
    QuackParser();

    void begin(QuackDisplay* _quackDisplay);

    QuackParser::QuackLine* getProcessedLine();
    void parsingLoop();

    void fillBuffer(const u8* const str);
    void setFile(const char* const filename);
    
    void stop();
};

#endif