#ifndef SS5_EV_LISTENER_H
#define SS5_EV_LISTENER_H


#include <cstdint>
#include <ev++.h>

class Listener {
public:
    Listener();

    virtual ~Listener();

    virtual void close();

    void start_loop();

    int start_listen(char const *host, uint16_t port);

    void set_exit_signals(uint signals);

protected:
    int fd_;
    bool closed_;
    ev::io io_;
    ev::sig sig_;
    uint signals_;

private:
    void io_cb(ev::io &w, int events);

    void signal_cb(ev::sig &w, int events);

    virtual int post_accept(int fd) = 0;

};


#endif //SS5_EV_LISTENER_H
