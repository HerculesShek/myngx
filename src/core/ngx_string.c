
#include <ngx_config.h>
#include <ngx_core.h>


u_char *
ngx_cpystrn(u_char *dst, u_char *src, size_t n) {
    if (n == 0) {
        return dst;
    }

    for ( /* void */ ; --n; dst++, src++) {
        *dst = *src;

        if (*dst == '\0') {
            return dst;
        }
    }

    *dst = '\0';

    return dst;
}
