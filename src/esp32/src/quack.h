
#ifndef QUACK_H_
#define QUACK_H_

#include <quack_config.h>
#include "quack_parser.h"
#include "quack_interface.h"
#include "quack_display.h"

class Quack {

#ifndef QUACK_DEBUGGING
private:
#else
public:
#endif
    QuackParser quackParser;
    QuackInterface quackInterface;
    QuackDisplay quackDisplay;

public:
    void begin();
    void runParser();
    void runInterface();
    void runDisplay();
    QuackParser* getParser();
};

#endif