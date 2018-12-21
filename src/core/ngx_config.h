#ifndef MYNGX_NGX_CONFIG_H
#define MYNGX_NGX_CONFIG_H

/*
 * ngx_auto_headers.h 是由configure所生成 存在于默认的objs目录下
 */
#include <ngx_auto_headers.h>
#include <ngx_posix_config.h>

#define ngx_signal_helper(n)     SIG##n
#define ngx_signal_value(n)      ngx_signal_helper(n)


// 定义的信号值。
#define NGX_SHUTDOWN_SIGNAL      QUIT
#define NGX_TERMINATE_SIGNAL     TERM
#define NGX_NOACCEPT_SIGNAL      WINCH
#define NGX_RECONFIGURE_SIGNAL   HUP

#define NGX_REOPEN_SIGNAL        USR1
#define NGX_CHANGEBIN_SIGNAL     USR2

/* STUB: autoconf */
typedef int   ngx_int_t;
typedef u_int ngx_uint_t;
typedef int   ngx_flag_t;
#define NGX_INT_T_LEN   sizeof("-2147483648") - 1


#define ngx_cdecl
#define ngx_libc_cdecl


#endif //MYNGX_NGX_CONFIG_H
