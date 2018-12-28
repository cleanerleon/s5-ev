#ifndef SS5_EV_S5RESPONSER_H
#define SS5_EV_S5RESPONSER_H

#include "buff.h"
#include "tcpwatcher.h"
#include "tcpbuff.h"
#include "proxy.h"
#include "s5listener.h"

typedef struct {
    uint8_t ver;
    uint8_t nmethods;
    uint8_t methods[0];
} __attribute__((packed, aligned(1))) method_req_t;

typedef struct {
    uint8_t ver;
    uint8_t method;
} __attribute__((packed, aligned(1))) method_resp_t;

typedef struct {
    uint8_t ver;
    uint8_t cmd;
    uint8_t rsv;
    uint8_t atyp;
    uint8_t dat[0];
} __attribute__((packed, aligned(1))) socks5_req_t;

typedef struct {
    uint8_t ver;
    uint8_t resp;
    uint8_t rsv;
    uint8_t atyp;
}__attribute__((packed, aligned(1))) socks5_resp_t;

class S5Listener;

class Proxy;

class S5Responser : public TcpWatcher {
public:
    enum STEP {
        INIT = 0,
        REQ,
        DATA,
    };

    S5Responser(int fd, S5Listener *listener);

    void close() override;

    const char *type() override;

    int run();


private:


    void on_read() override;

    void on_write() override;

    int on_init(TcpBuff &buff);

    int on_req(TcpBuff &buff);

//    int on_data(TcpBuff &buff);

    int new_proxy(struct sockaddr_in &addr);

    uint step_;
    unique_ptr<Proxy> proxy_;
    S5Listener *listener_;

};


#endif //SS5_EV_S5RESPONSER_H
