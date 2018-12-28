#ifndef SS5_EV_TCPWATCHER_H
#define SS5_EV_TCPWATCHER_H


#include "tcpbuff.h"
#include "watcher.h"

class TcpWatcher : public Watcher {
public:
    explicit TcpWatcher(int fd = -1);

    int connect(struct sockaddr_in *addr);

    int connect(char const *host, uint16_t port);

    void attach_buffs(shared_ptr<BuffList> rbuffs, shared_ptr<BuffList> wbuffs);

protected:
    void io_cb(ev::io &w, int events) override;

    bool connected_;
    shared_ptr<BuffList> wbuffs_;
    shared_ptr<BuffList> rbuffs_;

};


#endif //SS5_EV_TCPWATCHER_H
