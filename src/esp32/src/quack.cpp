
#include "quack.h"

void
Quack::begin() {
    quackParser.begin();
    quackInterface.begin();
}

void
Quack::runParser() {
    quackParser.parsingLoop();
}

void
Quack::runInterface() {
    const QuackInterface::Status status = quackInterface.getStatus();
    
    if(status == QuackInterface::Status::RESPONSE_SUCCESS) {
        QuackParser::QuackLine* const line = quackParser.getProcessedLine();
        if(line) {
            quackInterface.send(line);
            line->state = QuackParser::QuackLineState::WAITING_RESPONSE;
        }
    }
    else if(status == QuackInterface::Status::RESPONSE_RESEND) {
        quackInterface.resend();
    }
    else {
        // WAITING_RESPONSE
        quackInterface.waitResponse();
    }
}

QuackParser*
Quack::getParser() {
    return &quackParser;
}