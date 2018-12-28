#include "proxy.h"


Proxy::Proxy(S5Responser *s5) : TcpWatcher(-1), s5p_(s5) {
}

void Proxy::on_read() {
    auto buff = rbuffs_->get_read_buff();
    if (buff == nullptr) {
        stop_read();
        return;
    }

    int ret = buff->recv(fd_);
    dbg_print("%s recv %d", type(), ret);
    if (ret < 0) {
        if (ret == -E_RET0) {
            close_read();
        } else {
            close();
        }
    } else if (s5p_->fd() >= 0) {
        s5p_->start(EV_WRITE);
        dbg_print("s5p start write");
    }
}

void Proxy::on_write() {
    auto buff = wbuffs_->get_write_buff();
    if (buff == nullptr) {
        stop_write();
        return;
    }
    int ret = buff->send(fd_);
    dbg_print("%s send %d", type(), ret);
    if (ret < 0 && ret != -E_RET0) {
        close();
    } else if (s5p_->fd() >= 0) {
        s5p_->start(EV_READ);
    }
}

const char *Proxy::type() {
    return "Proxy";
}

void Proxy::close() {
    dbg_print("%s close fd %d", type(), fd());
    if (!closed_) {
        Watcher::close();
        if (s5p_ != nullptr) {

            s5p_->close();
            s5p_ = nullptr;
        }
    }
}
