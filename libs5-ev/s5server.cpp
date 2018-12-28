#include "s5server.h"

void S5Server::set_exit_signals(int signo) {
    return listener_.set_exit_signals(signo);
}

void S5Server::set_timeout(int ms) {
    timeout_ = ms;
}

int S5Server::start() {
    int ret = listener_.start_listen(host_, port_);
    dbg_print("start listening");
    if (ret < 0) {
        return ret;
    }
    listener_.start_loop();
    return 0;
}

S5Server::S5Server(const char *host, uint16_t port) : host_(host), port_(port), timeout_(0) {
}
