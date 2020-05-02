
#ifndef QUACK_H_
#define QUACK_H_

#include <quack_config.h>
#include "quack_parser.h"
#include "quack_interface.h"
#include "quack_display.h"
#include "quack_event_launcher.h"

#ifdef BLUETOOTH_ENABLED
#include "quack_bluetooth.h"
#endif

class Quack {

#ifndef QUACK_DEBUGGING
private:
#else
public:
#endif
    QuackParser quackParser;
    QuackInterface quackInterface;
    QuackDisplay quackDisplay;
    QuackEventLauncher quackEventLauncher;
#ifdef BLUETOOTH_ENABLED
    QuackBluetooth quackBluetooth;
#endif

public:
    void begin();
    void runParser();
    void runInterface();
    void runDisplay();
#ifdef BLUETOOTH_ENABLED
    void runBluetooth();
    const bool checkBluetooth();
#endif
    QuackParser* getParser();
    QuackEventLauncher* getEventLauncher();
};

#endif