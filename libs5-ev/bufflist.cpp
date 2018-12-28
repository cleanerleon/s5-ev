#include "bufflist.h"

static int BL_ID = 0;

BuffList::BuffList() : reading_id_(0), writing_id_(-1), list_id_(BL_ID++) {
}


TcpBuff *BuffList::get_read_buff() {
    if (reading_id_ >= 0) {
        if (buffs_[reading_id_].status() == Buff::READING) {
            return &buffs_[reading_id_];
        }
    } else {
        reading_id_ = writing_id_;
    }
    for (int i = 0; i < BUFF_LIST_SIZE; i++) {
        reading_id_ = (reading_id_ + 1) % BUFF_LIST_SIZE;
        auto buff = buffs_ + reading_id_;
        if (buff->status() == Buff::UNUSED) {
            return buff;
        }
    }
    reading_id_ = -1;
    return nullptr;
}


TcpBuff *BuffList::get_write_buff() {
    if (writing_id_ >= 0) {
        if (buffs_[writing_id_].status() == Buff::WRITING) {
            return &buffs_[writing_id_];
        }
    } else {
        writing_id_ = reading_id_;
    }
    for (int i = 0; i < BUFF_LIST_SIZE; i++) {
        writing_id_ = (writing_id_ + 1) % BUFF_LIST_SIZE;
        auto buff = buffs_ + writing_id_;
        if (buff->status() == Buff::READ) {
            return buff;
        }
    }
    writing_id_ = -1;
    return nullptr;
}
