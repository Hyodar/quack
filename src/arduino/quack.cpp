
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

    quackParser.parse(BUFFER_PTR_TO_STR(buffer));

    quackInterface.flush();
}