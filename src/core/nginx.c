

#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>
#include <nginx.h>


int ngx_cdecl
main(int argc, char *const *argv) {


//    ngx_time_init();


    ngx_pid = ngx_getpid();

    printf("ngx_pid is %i\n", ngx_pid);

    ngx_argc = argc;

    ngx_master_process_cycle();


//    log = ngx_log_init();
//    if (log == NULL) {
//        return 1;
//    }




    return 0;
}


//u_long
//ngx_get_cpu_affinity(ngx_uint_t n)
//{
//    ngx_core_conf_t  *ccf;
//
//    ccf = (ngx_core_conf_t *) ngx_get_conf(ngx_cycle->conf_ctx,
//                                           ngx_core_module);
//
//    if (ccf->cpu_affinity == NULL) {
//        return 0;
//    }
//
//    if (ccf->cpu_affinity_n > n) {
//        return ccf->cpu_affinity[n];
//    }
//
//    return ccf->cpu_affinity[ccf->cpu_affinity_n - 1];
//}
