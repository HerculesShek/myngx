#include <ngx_config.h>
#include <ngx_core.h>


int
ngx_nonblocking(ngx_socket_t s) {
    u_long nb;

    nb = 1;

    return ioctl(s, FIONBIO, &nb);
}


int
ngx_blocking(ngx_socket_t s) {
    u_long nb;

    nb = 0;

    return ioctl(s, FIONBIO, &nb);
}