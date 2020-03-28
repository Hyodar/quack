
#ifndef QUACK_H_
#define QUACK_H_

#include "quack_interface.h"
#include "quack_parser.h"

class Quack {

private:
    QuackInterface quackInterface;
    QuackParser quackParser;

public:
    void begin();
    void run();
};

#endif