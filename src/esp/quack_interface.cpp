
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
        }
        // else do nothing, probably a frame separator
    }
}

void
QuackInterface::send(const QuackFrame* frame) {
    quackFrame = frame;

    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(frame->getBuffer(), frame->getLength() + HEADER_SIZE);
    Serial1.write(FRAME_SEPARATOR);
}

void
QuackInterface::resend() const {
    Serial1.write(FRAME_SEPARATOR);
    Serial1.write(quackFrame->getBuffer(), quackFrame->getLength() + HEADER_SIZE);
    Serial1.write(FRAME_SEPARATOR);
}