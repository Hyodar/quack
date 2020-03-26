
#ifndef QUACK_PARSER_H_
#define QUACK_PARSER_H_

#include "utils.h"
#include "command_manager.h"

class QuackParser {
    enum ParsingState {
        NONE,
        COMMAND,
        PARAMS,
        CONTINUE
    };

    private:
        CommandManager commandManager;
        ParsingState parsingState;
        
        u8 currentCommand;

        void sendCommand(const u8* params, const u16 len);
        u32 parseU32(const u8* params, u16 len);

    public:
        QuackParser();

        void parse(u8* str, const u16 len);

};

#endif