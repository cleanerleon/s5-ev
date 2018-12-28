#include <iostream>
#include "../libs5-ev/s5server.h"

using namespace std;

int main(int argc, char *argv[]) {
    if (argc < 2) {
        cout << argv[0] << " port" << endl;
        return -1;
    }
    struct sigaction sa;
    sa.sa_handler = SIG_IGN;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    if (sigaction(SIGPIPE, &sa, NULL) < 0) {
        perror("cannot ignore SIGPIPE");
        return -1;
    }
    uint16_t port = atoi(argv[1]);
    unique_ptr<S5Server> server(new(nothrow) S5Server(nullptr, port));
    if (server == nullptr) {
        return -1;
    }
    server->set_exit_signals(SIGINT | SIGTERM | SIGKILL);
    if (server->start() < 0) {
        cerr << "cannot start Socks5 server" << endl;
    }
    return 0;
}