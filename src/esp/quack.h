
#ifndef QUACK_H_
#define QUACK_H_

#include "quack_config.h"
#include "quack_parser.h"
#include "quack_interface.h"

class Quack {

#ifndef QUACK_DEBUGGING
private:
#else
public:
#endif
    QuackParser quackParser;
    QuackInterface quackInterface;

public:
    void begin();
    void runParser();
    void runInterface();
};

#endif