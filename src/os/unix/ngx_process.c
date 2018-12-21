#include <ngx_config.h>
//#include <ngx_process.h> // TODO 为何这里加上这行代码 ngx_process.h中的ngx_socket_t类型就无法识别？？？？
#include <ngx_core.h>

/**
 * ngx_process_slot     进程的slot
 * ngx_socket_t         进程的管道
 * ngx_last_process     最后一个子进程的索引
 * ngx_processes        全局进程表 包含所有的存活的子进程
 */
ngx_int_t     ngx_process_slot;
ngx_socket_t  ngx_channel;
ngx_int_t     ngx_last_process;
ngx_process_t ngx_processes[NGX_MAX_PROCESSES];