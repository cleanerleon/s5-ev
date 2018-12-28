#include <sys/socket.h>
#include "tcpbuff.h"


TcpBuff::TcpBuff() : least_(0), required_(0) {
}

int TcpBuff::recv(int fd) {
    if (UNUSED == status_) {
        offset_ = 0;
    }
    if (least_ == 0) {
        return Buff::recv(fd);
    }
    int read = 0;
    while (len_ < least_) {
        int ret = ::recv(fd, data_ + offset_, least_ - offset_, 0);
        if (ret > 0) {
            len_ += static_cast<uint32_t>(ret);
            offset_ = len_;
            status_ = READING;
            read += ret;
        } else {
            ret = process_ret(ret);
            if (ret == 0) {
                return read;
            } else {
                return ret;
            }
        }
    }
    if (least_ > 0 && required_ == 0) {
        return read;
    }
    while (len_ < required_) {
        int ret = ::recv(fd, data_ + offset_, required_ - offset_, 0);
        if (ret > 0) {
            len_ += static_cast<uint32_t>(ret);
            offset_ = len_;
            read += ret;
        } else {
            ret = process_ret(ret);
            if (ret == 0) {
                return read;
            } else {
                return ret;
            }
        }
    }
    status_ = READ;
    return read;
}

void TcpBuff::set_least_len(uint32_t len) {
    least_ = len;
}

void TcpBuff::set_required_len(uint32_t len) {
    required_ = len;
}

bool TcpBuff::meet_least_len() {
    return least_ == len_;
}

void TcpBuff::reset() {
    required_ = 0;
    least_ = 0;
    Buff::reset();
}

