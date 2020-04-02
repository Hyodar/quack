
#include "quack_interface.h"

#include "quack_codes.h"
#include "quack_serial.h"

QuackInterface::QuackInterface() : quackFrame{nullptr}, status{RESPONSE_SUCCESS} {
    // no-op
}

void
QuackInterface::begin() const {
    // TODO
}

const QuackInterface::Status
QuackInterface::getStatus() const {
    return status;
}

void
QuackInterface::waitResponse() {
    if(Serial1.available()) {
        const u8 byte = Serial1.read();

        if(byte == WRONG_CHECKSUM) {
            status = RESPONSE_RESEND;
        }
        else if(byte == FINISHED_PARSING) {
            status = RESPONSE_SUCCESS;
            quackLine->state = QuackParser::QuackLineState::FREE_TO_PARSE;
        }
        // else do nothing, probably a frame separator
    }
}

void
QuackInterface::send(QuackParser::QuackLine* line) {
    quackLine = line;

    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(line->data.getBuffer(), line->data.getLength() + HEADER_SIZE);
    Serial1.write(FRAME_SEPARATOR);
}

void
QuackInterface::resend() const {
    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(quackLine->data.getBuffer(), quackLine->data.getLength() + HEADER_SIZE);
    Serial1.write(FRAME_SEPARATOR);
}