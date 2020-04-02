
#include "quack_interface.h"

#include "quack_codes.h"
#include "quack_serial.h"

QuackInterface::QuackInterface() : quackLine{nullptr}, status{RESPONSE_SUCCESS} {
    // no-op
}

void
QuackInterface::begin() const {
    Serial.begin(BAUDRATE);
#ifdef SERIAL_DEBUG_OUTPUT
    Serial.setDebugOutput(true);
#endif
}

const QuackInterface::Status
QuackInterface::getStatus() const {
    return status;
}

void
QuackInterface::waitResponse() {
    if(Serial.available()) {
        const u8 byte = Serial.read();

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

    Serial.write(FRAME_SEPARATOR);
    Serial.write(line->data.getBuffer(), line->data.getLength() + HEADER_SIZE);
    Serial.write(FRAME_SEPARATOR);
}

void
QuackInterface::resend() const {
    Serial.write(FRAME_SEPARATOR);
    Serial.write(quackLine->data.getBuffer(), quackLine->data.getLength() + HEADER_SIZE);
    Serial.write(FRAME_SEPARATOR);
}