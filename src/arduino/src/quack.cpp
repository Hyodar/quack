
#include "quack.h"

void
Quack::begin() {
    quackInterface.begin();
    quackParser.begin();
}

void
Quack::run() {
    if(!quackInterface.update()) return;

    const QuackInterface::QuackBuffer* buffer = quackInterface.getBuffer();

    if(!quackParser.parse(UNPACK_BUFFER(buffer))) {
        quackInterface.requestResend();
    }
    else {
        quackInterface.requestNext();
    }

    quackInterface.flush();
}