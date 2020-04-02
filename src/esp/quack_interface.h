
#ifndef QUACK_INTERFACE_H_
#define QUACK_INTERFACE_H_

#include "quack_config.h"
#include "quack_utils.h"
#include "quack_frame.h"

class QuackInterface {

public:

enum Status {
    WAITING_RESPONSE,
    RESPONSE_SUCCESS,
    RESPONSE_RESEND
};

private:
    const QuackFrame* quackFrame;
    enum Status status;

public:

    QuackInterface();
    void begin() const;

    const QuackInterface::Status getStatus() const;
    void waitResponse();

    void send(const QuackFrame* frame);
    void resend() const;

};

#endif