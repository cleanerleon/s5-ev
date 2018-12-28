#ifndef SS5_EV_BUFFLIST_H
#define SS5_EV_BUFFLIST_H


#include "tcpbuff.h"

class BuffList {
public:
    BuffList();

    TcpBuff *get_read_buff();

    TcpBuff *get_write_buff();

    TcpBuff *getBuffList() { return buffs_; }

private:
    TcpBuff buffs_[BUFF_LIST_SIZE];
    int reading_id_;
    int writing_id_;
    int list_id_;
};


#endif //SS5_EV_BUFFLIST_H
