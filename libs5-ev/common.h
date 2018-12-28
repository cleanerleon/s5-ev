#ifndef SS5_EV_COMMON_H
#define SS5_EV_COMMON_H

#include <cstring>
#include <cerrno>
#include <cstdio>

#define BUFF_SIZE (1500)
#define BUFF_LIST_SIZE (5)
#define MAX_CONN (10)

enum RETURN {
    OK = 0,
    E_RET0,
    E_SOCK,
    E_REQ,
    E_MEM,
};

#ifdef _WIN32
#define FILE_BASENAME(file) strrchr("\\" file, '\\') + 1
#else
#define FILE_BASENAME(file) strrchr("/" file, '/') + 1
#endif
# define err_print_no(...)  \
do { \
    fprintf(stderr, "\033[31m%s:%d[%s]:", FILE_BASENAME(__FILE__), __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\033[33m errno[%d]:%s \033[0m\n", errno, strerror(errno));\
} while (0)


# define err_print(...)  \
do { \
    fprintf(stderr, "\033[31m%s:%d[%s]: ", FILE_BASENAME(__FILE__), __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\033[0m\n");\
} while (0)

#ifdef DEBUG
# define dbg_print(...) \
do { \
    fprintf(stderr, "%s:%d[%s]: ", FILE_BASENAME(__FILE__), __LINE__, __FUNCTION__);\
    fprintf(stderr, __VA_ARGS__);\
    fprintf(stderr, "\n");\
} while (0)
#else
# define dbg_print(...) do {} while(0)
#endif

#endif //SS5_EV_COMMON_H
