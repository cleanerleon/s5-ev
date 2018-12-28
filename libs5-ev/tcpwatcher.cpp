#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "tcpwatcher.h"

TcpWatcher::TcpWatcher(int fd) : Watcher(fd), connected_(false) {
}

int TcpWatcher::connect(struct sockaddr_in *addr) {
    fd_ = socket(AF_INET, SOCK_STREAM | SOCK_CLOEXEC | SOCK_NONBLOCK, IPPROTO_TCP);
    if (fd_ == -1) {
        err_print_no("socket");
        return -E_SOCK;
    }

    int r = ::connect(fd_, (sockaddr *) addr, sizeof(*addr));
    if (r < 0) {
        if (errno != EINPROGRESS) {
            err_print_no("connect");
            return -E_SOCK;
        } else {
            dbg_print("connecting");
        }
    }
    return 0;
}

int TcpWatcher::connect(char const *host, uint16_t port) {
    struct sockaddr_in addr = {0};
    struct hostent *ht = nullptr;
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
    return connect(&addr);
}

void TcpWatcher::io_cb(ev::io &w, int events) {
    if (!connected_ && (events & EV_WRITE) && (events && EV_READ)) {
        char error;
        socklen_t len = sizeof(error);
        getsockopt(fd_, SOL_SOCKET, SO_ERROR, (char *) &error, &len);
        if (error == 0) {
            connected_ = true;
            dbg_print("connected");
        } else {
            close();
            return;
        }
    }
    Watcher::io_cb(w, events);
}

void TcpWatcher::attach_buffs(shared_ptr<BuffList> rbuffs, shared_ptr<BuffList> wbuffs) {
    rbuffs_ = rbuffs;
    wbuffs_ = wbuffs;
}

