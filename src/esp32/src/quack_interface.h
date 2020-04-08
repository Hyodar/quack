
#ifndef QUACK_INTERFACE_H_
#define QUACK_INTERFACE_H_

#include <quack_config.h>
#include <quack_utils.h>
#include "quack_parser.h"

class QuackInterface {

public:

enum Status {
    WAITING_RESPONSE,
    GET_RESPONSE,
    RESPONSE_SUCCESS,
    RESPONSE_RESEND
};

private:
    QuackParser::QuackLine* quackLine;
    enum Status status;

public:

    QuackInterface();
    void begin() const;

    const QuackInterface::Status getStatus() const;
    void setStatus(QuackInterface::Status _status);
    void waitResponse();

    void send(QuackParser::QuackLine* line);
    void send() const;

    void freeLine();

};

#endif