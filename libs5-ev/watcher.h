#ifndef SS5_EV_WATCHER_H
#define SS5_EV_WATCHER_H


#include <ev++.h>
#include <memory>
#include "bufflist.h"

using namespace std;

class Watcher {
public:

    explicit Watcher(int fd = -1);

    virtual ~Watcher();

    void start(uint32_t events);

    void start();

    void stop_read();

    void stop_write();

    virtual const char *type() = 0;

    virtual void close();

    virtual void close_read();

    int fd();

protected:
    virtual void io_cb(ev::io &w, int events);

    virtual void on_write() = 0;

    virtual void on_read() = 0;

    int fd_;
    bool closed_;
    bool read_closed_;

private:

    ev::io io_;

};


#endif //SS5_EV_WATCHER_H
