#include <unistd.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>
#include "listener.h"
#include "common.h"

Listener::Listener() :
        closed_(false), fd_(-1) {
    io_.set<Listener, &Listener::io_cb>(this);
}

Listener::~Listener() {
    close();
}

void Listener::close() {
    if (!closed_) {
        closed_ = true;
        io_.stop();
        sig_.stop();
    }
}

void Listener::io_cb(ev::io &w, int events) {
    if (fd_ < 0) {
        return;
    }
    if (events & EV_READ) {
        struct sockaddr_in addr = {0};
        socklen_t addr_len = sizeof(addr);
        int cfd = accept4(fd_, (struct sockaddr *) &addr, &addr_len, SOCK_CLOEXEC | SOCK_NONBLOCK);
        if (cfd >= 0) {
            post_accept(cfd);
        } else {
            err_print_no("accept4");
        }
    }
}

void Listener::start_loop() {
    ev::default_loop loop;
    loop.run(0);
}

int Listener::start_listen(char const *host, uint16_t port) {
    if ((fd_ = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP)) == -1) {
        err_print_no("listen");
        return -E_SOCK;
    }
    int reuse = 1;
    setsockopt(fd_, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));

    struct hostent *ht = nullptr;
    struct sockaddr_in addr = {0};
    addr.sin_family = AF_INET;

    if (host == nullptr) {
        addr.sin_addr.s_addr = htonl(INADDR_ANY);
    } else {
        ht = gethostbyname(host);
        if (ht != nullptr && ht->h_length == 4) {
            memcpy(&(addr.sin_addr), ht->h_addr, sizeof(addr));
        } else {
            inet_pton(AF_INET, host, &(addr.sin_addr));
        }
    }
    addr.sin_port = htons(port);
    while (bind(fd_, (struct sockaddr *) &addr, sizeof(addr)) == -1) {
        err_print_no("bind");
        if (errno == EADDRINUSE) {
            sleep(1);
            continue;
        }
        return -E_SOCK;
    }
    if (listen(fd_, MAX_CONN) == -1) {
        err_print_no("listen");
        return -E_SOCK;
    }
    io_.start(fd_, EV_READ);
    return 0;
}

void Listener::set_exit_signals(uint signals) {
    sig_.set<Listener, &Listener::signal_cb>(this);
    sig_.start(signals | SIGPIPE);
}

void Listener::signal_cb(ev::sig &w, int events) {
    if ((events & EV_SIGNAL) > 0 && (w.signum & signals_) > 0) {
        if (events & SIGPIPE) {

        } else {
            close();
        }
    }
}

