#include <unistd.h>
#include <sys/socket.h>
#include "watcher.h"

Watcher::Watcher(int fd) : fd_(fd), closed_(false), read_closed_(false) {
    io_.set<Watcher, &Watcher::io_cb>(this);
}

void Watcher::start(uint32_t events) {
    if (read_closed_ && events == EV_READ) {
        return;
    }
    events |= io_.events;
    io_.start(fd_, events);
}

void Watcher::stop_read() {
    if ((io_.events & EV_READ) > 0) {
        io_.start(io_.fd, io_.events & ~EV_READ);
    }
}

void Watcher::stop_write() {
    if ((io_.events & EV_WRITE) > 0) {
        io_.start(io_.fd, io_.events & ~EV_WRITE);
    }
}

void Watcher::io_cb(ev::io &w, int events) {
    if (events & EV_WRITE) {
        on_write();

    }
    if (closed_) {
        return;
    }
    if (events & EV_READ) {
        on_read();
    }
}

void Watcher::close() {
    if (!closed_) {
        closed_ = true;
        io_.stop();
        if (fd_ > 0) {
            ::close(fd_);
            fd_ = -fd_;
        }
    }
}

void Watcher::start() {
    if (fd_ >= 0) {
        io_.start(fd_, EV_READ | EV_WRITE);
    }
}

Watcher::~Watcher() {
    close();
}

int Watcher::fd() {
    return fd_;
}

void Watcher::close_read() {
    if (!read_closed_) {
        read_closed_ = true;
        stop_read();
        shutdown(fd_, SHUT_RD);
    }
}
