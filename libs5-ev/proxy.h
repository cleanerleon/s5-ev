#ifndef SS5_EV_PROXY_H
#define SS5_EV_PROXY_H

#include "tcpwatcher.h"
#include "s5responser.h"

class S5Responser;

class Proxy : public TcpWatcher {
public:
    explicit Proxy(S5Responser *s5);

    void on_read() override;

    void on_write() override;

    const char *type() override;

    void close() override;

private:
    S5Responser *s5p_;

};


#endif //SS5_EV_PROXY_H
