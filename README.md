# s5-ev

a Socks5 sever implemented with libev

```
//sample code
S5Server server(nullptr, port);
server.set_exit_signals(SIGINT | SIGTERM | SIGKILL);
server.run()
```
