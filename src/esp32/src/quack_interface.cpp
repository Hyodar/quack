
#include "quack_interface.h"

#include "quack_codes.h"
#include "quack_serial.h"

QuackInterface::QuackInterface() : quackLine{nullptr}, status{RESPONSE_SUCCESS} {
    // no-op
}

void
QuackInterface::begin() const {
    Serial.begin(SERIAL_BAUDRATE);
    Serial2.begin(SERIAL2_BAUDRATE);
    Serial.setDebugOutput(true);
    delay(500);
}

const QuackInterface::Status
QuackInterface::getStatus() const {
    return status;
}

void
QuackInterface::setStatus(QuackInterface::Status _status) {
    status = _status;
}

void
QuackInterface::waitResponse() {
    if(Serial2.available()) {
        const u8 byte = Serial2.read();

        if(byte == FRAME_SEPARATOR) {
            status = GET_RESPONSE;
            return;
        }

        if(status == Status::GET_RESPONSE) {
            if(byte == WRONG_CHECKSUM) {
                status = Status::RESPONSE_RESEND;
            }
            else if(byte == FINISHED_PARSING) {
                status = Status::RESPONSE_SUCCESS;
                quackLine->state = QuackParser::QuackLine::State::FREE_TO_PARSE;
            }
        }
    }
}

void
QuackInterface::send(QuackParser::QuackLine* line) {
    quackLine = line;
    send();
}

void
QuackInterface::send() const {
    const u8* const buf = quackLine->data.getBuffer();
    const u16 length = quackLine->data.getLength() + FRAME_HEADER_SIZE;

    Serial2.write(FRAME_SEPARATOR);
    for(u16 i = 0; i < length; i++) Serial2.write(buf[i]);
    Serial2.write(FRAME_SEPARATOR);

    Serial.write(0);
    DEBUGGING_PRINTSTR(buf, length + FRAME_HEADER_SIZE);
    DEBUGGING_PRINTF("\n");
}

void
QuackInterface::freeLine() {
    if(quackLine) {
        DEBUGGING_PRINTF("[INTERFACE] Setting quackLine to FREE_TO_PARSE.\n");
        quackLine->state = QuackParser::QuackLine::State::FREE_TO_PARSE;
        quackLine = nullptr;
    }
}