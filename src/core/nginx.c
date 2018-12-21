

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <nginx.h>


int ngx_cdecl
main(int argc, char *const *argv) {


//    ngx_time_init();


    ngx_pid = ngx_getpid();

    printf("ngx_pid is %i\n", ngx_pid);


//    log = ngx_log_init();
//    if (log == NULL) {
//        return 1;
//    }




    return 0;
}

