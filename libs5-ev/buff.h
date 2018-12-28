#ifndef SS5_EV_BUFF_H
#define SS5_EV_BUFF_H


#include <cstdint>

#include "common.h"

class Buff {
public:
    typedef enum {
        UNUSED = 0,
        READING,
        READ,
        WRITING
    } STATUS;

    Buff();

    uint8_t *data() {
        return data_;
    }

    uint32_t len() {
        return len_;
    }

    uint32_t offset() {
        return offset_;
    }

    STATUS status() {
        return status_;
    }

    void set_status(STATUS status) {
        status_ = status;
    }

    void set_len(uint32_t len) {
        len_ = len;
    }

    void set_offset(uint32_t offset) {
        offset_ = offset;
    }

    virtual void reset();

    virtual int recv(int fd);

    virtual int send(int fd);

protected:
    int process_ret(int ret);

    uint8_t data_[BUFF_SIZE];
    uint32_t len_;
    uint32_t offset_;
    STATUS status_;

};


#endif //SS5_EV_BUFF_H
