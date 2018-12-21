#ifndef MYNGX_NGX_CHANNEL_H
#define MYNGX_NGX_CHANNEL_H


#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_event.h>

// 封装了父子进程之间传递的信息。
typedef struct {
    ngx_uint_t  command;   // 对端将要做得命令。
    ngx_pid_t   pid;       // 当前的子进程id
    ngx_int_t   slot;      // 在全局进程表中的位置
    ngx_fd_t    fd;        // 传递的fd
} ngx_channel_t;

//
ngx_int_t ngx_write_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size);
//ngx_int_t ngx_read_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size);
//ngx_int_t ngx_add_channel_event(ngx_cycle_t *cycle, ngx_fd_t fd,
//                                ngx_int_t event, ngx_event_handler_pt handler);
void ngx_close_channel(ngx_fd_t *fd );


#endif //MYNGX_NGX_CHANNEL_H
