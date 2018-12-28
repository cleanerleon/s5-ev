#include <sys/socket.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>

#include "s5responser.h"
#include "tcpbuff.h"

S5Responser::S5Responser(int fd, S5Listener *listener) : TcpWatcher(fd), step_(INIT), listener_(listener) {
}
//
//int S5Responser::on_data(TcpBuff &buff) {
//    buff.set_status(Buff::READ);
//    return 0;
//}

void S5Responser::on_read() {
    auto buff = rbuffs_->get_read_buff();
    if (buff == nullptr) {
        stop_read();
        return;
    }
    if (buff->status() == Buff::UNUSED) {
        switch (step_) {
            case INIT:
                buff->set_least_len(2);
                break;
            case REQ:
                buff->set_least_len(5);
                break;
        }
    }
    int ret = buff->recv(fd_);
    dbg_print("%s fd %d step %d recv %d", type(), fd(), step_, ret);
    if (ret > 0) {
        switch (step_) {
            case INIT:
                ret = on_init(*buff);
                break;
            case REQ:
                ret = on_req(*buff);
                break;
//            case DATA:
//                ret = on_data(*buff);
//                break;
        }
        if (ret < 0) {
            close();
            return;
        }
        if (buff->status() == Buff::READ &&
            proxy_ != nullptr &&
            proxy_->fd() >= 0) {
            proxy_->start(EV_WRITE);
        }

    } else if (ret == 0) {

    } else {
        if (-E_RET0 == ret) {
            close_read();
        } else {
            close();
        }
    }
}

void S5Responser::on_write() {
    auto buff = wbuffs_->get_write_buff();
    if (buff == nullptr) {
        stop_write();
        return;
    }
    int ret = buff->send(fd_);
    dbg_print("%s fd %d step %d send %d", type(), fd(), step_, ret);
    if (ret < 0) {
        close();
        return;
    }
    if (buff->status() == Buff::UNUSED && proxy_ != nullptr && proxy_->fd() >= 0) {
        proxy_->start(EV_READ);
    }
}

int S5Responser::on_init(TcpBuff &buff) {
    int ret = 0;
    switch (buff.status()) {
        case Buff::READING:
            if (buff.meet_least_len()) {
                auto head = (method_req_t *) buff.data();
                if (head->ver != 0x05) {
                    ret = -E_REQ;
                    break;
                }
                uint32_t size = 2 + head->nmethods;
                buff.set_required_len(size);
            }
            break;
        case Buff::READ:
            auto req_head = (method_req_t *) buff.data();
            dbg_print("ver:%d, nmethods:%d", req_head->ver, req_head->nmethods);
            method_resp_t head;
            memset(&head, 0, sizeof(head));
            head.ver = 0x05;
            head.method = 0x00;
            ::send(fd_, &head, sizeof(head), 0);
            buff.reset();
            step_ = REQ;
            break;
    }
    return ret;
}

int S5Responser::on_req(TcpBuff &buff) {
    int ret = 0;
    switch (buff.status()) {
        case Buff::READING:
            if (buff.meet_least_len()) {
                auto head = (socks5_req_t *) buff.data();
                if (head->ver != 0x05) {
                    ret = -E_REQ;
                    break;
                }
                if (head->cmd != 0x01) {
                    ret = -E_REQ;
                    break;
                }
                switch (head->atyp) {
                    case 1:
                        buff.set_required_len(10);
                        break;
                    case 3:
                        buff.set_required_len(head->dat[0] + 7);
                        break;
//                    case 4:
//                        buff.set_required_len(22);
//                        break;
                    default:
                        ret = -E_REQ;
                        break;
                }
            }
            break;
        case Buff::READ:
            auto head = (socks5_req_t *) buff.data();
            struct sockaddr_in addr = {0};
            int len = 0;
            addr.sin_family = AF_INET;
            if (head->atyp == 1) {
                memcpy(&addr.sin_addr, head->dat, 4);
                len = 4;
            } else {
                char host[256];
                memcpy(host, head->dat + 1, head->dat[0]);
                len = head->dat[0];
                host[len] = '\0';
                struct hostent *ht = gethostbyname(host);
                if (ht != nullptr && ht->h_length == 4) {
                    memcpy(&(addr.sin_addr), ht->h_addr, sizeof(addr));
                } else {
                    ret = -E_REQ;
                    break;
                }
                len += 1;
            }
            addr.sin_port = *((uint16_t *) (head->dat + len));
            dbg_print("host %s, port %d", inet_ntoa(addr.sin_addr), ntohs(addr.sin_port));
            ret = new_proxy(addr);
            if (ret >= 0) {
                step_ = DATA;
            }
            break;
    }
    if (ret < 0) {
        auto head = (socks5_resp_t *) buff.data();
        head->resp = 0x01;
        ::send(fd_, buff.data(), buff.len(), 0);
    } else if (buff.status() == Buff::READ) {
        auto head = (socks5_resp_t *) buff.data();
        head->resp = 0x0;
        ::send(fd_, buff.data(), buff.len(), 0);
        buff.reset();
    }
    return ret;
}

int S5Responser::new_proxy(struct sockaddr_in &addr) {
    proxy_ = move(unique_ptr<Proxy>(new(nothrow) Proxy(this)));
    if (proxy_ == nullptr) {
        return -E_MEM;
    }
    proxy_->attach_buffs(wbuffs_, rbuffs_);
    int ret = proxy_->connect(&addr);
    if (ret >= 0) {
        proxy_->start();
        dbg_print("sock pair responser fd %d proxy fd %d", fd(), proxy_->fd());
    } else {
        proxy_.reset(nullptr);
    }
    return ret;
}

const char *S5Responser::type() {
    return "S5Responser";
}

void S5Responser::close() {
    dbg_print("%s close fd %d", type(), fd());
    if (!closed_) {
        Watcher::close();
        listener_->rm_responser(this);
    }
}

int S5Responser::run() {
    wbuffs_ = make_shared<BuffList>();
    rbuffs_ = make_shared<BuffList>();
    if (wbuffs_ == nullptr || rbuffs_ == nullptr) {
        return -E_MEM;
    }

    start();
    return 0;
}
