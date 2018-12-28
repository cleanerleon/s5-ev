#ifndef SS5_EV_S5LISTENER_H
#define SS5_EV_S5LISTENER_H


#include <list>
#include "listener.h"
#include "s5responser.h"

class S5Responser;

class S5Listener : public Listener {
public:
    void rm_responser(S5Responser *item);

private:
    int post_accept(int fd) override;

    list <unique_ptr<S5Responser>> responsers_;
};


#endif //SS5_EV_S5LISTENER_H
