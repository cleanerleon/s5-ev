#ifndef SS5_EV_S5SERVER_H
#define SS5_EV_S5SERVER_H


#include <cstdint>
#include "listener.h"
#include "s5listener.h"

class S5Server {
public:
    S5Server(const char *host, uint16_t port);

    void set_timeout(int ms);

    void set_exit_signals(int signo);

    int start();

private:
    int timeout_;
    const char *host_;
    uint16_t port_;
    S5Listener listener_;

};


#endif //SS5_EV_S5SERVER_H
