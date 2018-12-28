#include "s5listener.h"

int S5Listener::post_accept(int fd) {
//    if (responsers_.size() > 0) {
//        return 0;
//    }
    unique_ptr<S5Responser> responser(new(nothrow)S5Responser(fd, this));
    if (responser == nullptr) {
        return -E_MEM;
    }
    int ret = responser->run();
    if (ret < 0) {
        return ret;
    }
    responsers_.push_back(move(responser));
    return 0;
}

void S5Listener::rm_responser(S5Responser *s5r) {
    responsers_.remove_if([s5r](unique_ptr<S5Responser> const &item) {
        return item.get() == s5r;
    });
}
