#include <sys/socket.h>
#include "buff.h"

Buff::Buff() : status_(UNUSED), offset_(0), len_(0) {

}

void Buff::reset() {
    status_ = UNUSED;
    offset_ = 0;
    len_ = 0;
}

int Buff::recv(int fd) {
    if (status_ == UNUSED) {
        offset_ = 0;
    }
    int ret = ::recv(fd, data_ + offset_, BUFF_SIZE - offset_, 0);
    if (ret > 0) {
        len_ += static_cast<uint32_t>(ret);
        offset_ = len_;
        status_ = READ;
        return ret;
    } else {
        err_print_no("fd %d recv buff ret %d", fd, ret);
        return process_ret(ret);
    }
}

int Buff::send(int fd) {
    int written = 0;
    if (status_ == READ) {
        offset_ = 0;
    }
    if (len_ > 0) {
        status_ = WRITING;
        int ret = ::send(fd, data_ + offset_, len_ - offset_, 0);
        if (ret >= 0) {
            auto uret = static_cast<uint32_t>(ret);
            len_ -= uret;
            offset_ += uret;
            written += ret;
            if (len_ == 0) {
                status_ = UNUSED;
            }
            return written;
        } else {
            err_print_no("fd %d send buff ret %d", fd, ret);
            return process_ret(ret);
        }
    }
    status_ = UNUSED;
    return written;
}

int Buff::process_ret(int ret) {
    if (ret == 0) {
        return -E_RET0;
    } else {
        if (errno == EAGAIN || errno == EWOULDBLOCK) {
            return 0;
        } else {
            return -E_SOCK;
        }
    }
}
