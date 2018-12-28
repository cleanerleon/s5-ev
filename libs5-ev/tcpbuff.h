#ifndef SS5_EV_TCPBUFF_H
#define SS5_EV_TCPBUFF_H


#include "buff.h"

class TcpBuff : public Buff {
public:
    TcpBuff();

    int recv(int fd) override;

    void set_least_len(uint32_t len);

    void set_required_len(uint32_t len);

    bool meet_least_len();

    void reset() override;

private:
    uint32_t least_;
    uint32_t required_;
};


#endif //SS5_EV_TCPBUFF_H
