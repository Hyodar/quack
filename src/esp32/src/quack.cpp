
#include "quack.h"

#include <Arduino.h>

void
Quack::begin() {
    quackInterface.begin();
    quackParser.begin(&quackDisplay);
    quackDisplay.begin();
}

void
Quack::runParser() {
    quackParser.parsingLoop();
}

void
Quack::runInterface() {
    const QuackInterface::Status status = quackInterface.getStatus();
    
    if(status == QuackInterface::Status::RESPONSE_SUCCESS) {
        quackInterface.freeLine();
        QuackParser::QuackLine* const line = quackParser.getProcessedLine();
        if(line) {
            DEBUGGING_PRINTF("[QUACK] RESPONSE_SUCCESS, sending next.\n");
            quackInterface.send(line);
            line->state = QuackParser::QuackLineState::WAITING_RESPONSE;
            quackInterface.setStatus(QuackInterface::Status::WAITING_RESPONSE);
        }
    }
    else if(status == QuackInterface::Status::RESPONSE_RESEND) {
        DEBUGGING_PRINTF("[QUACK] RESPONSE_RESEND, resending frame.\n");
        quackInterface.send();
        quackInterface.setStatus(QuackInterface::Status::WAITING_RESPONSE);
    }
    else {
        // WAITING_RESPONSE
        quackInterface.waitResponse();
    }
}

void
Quack::runDisplay() {
    quackDisplay.scroll();
}

QuackParser*
Quack::getParser() {
    return &quackParser;
}