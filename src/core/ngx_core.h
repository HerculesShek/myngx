//
// Created by Will on 2018-12-20.
//

#ifndef MYNGX_NGX_CORE_H
#define MYNGX_NGX_CORE_H

#include <ngx_errno.h>
#include <ngx_string.h>
#include <ngx_time.h>


#include <ngx_socket.h> // @note
/* 因为ngx_process.h中用到了ngx_socket_t类型 而ngx_socket_t类型是在ngx_socket.h中定义的
 * 所以ngx_socket.h必须在ngx_process.h之前
 */
#include <ngx_types.h>
#include <ngx_process.h>


#include <ngx_conf_file.h>

#include <ngx_process_cycle.h>



//typedef struct ngx_module_s ngx_module_t;


#define  NGX_OK          0
#define  NGX_ERROR      -1
#define  NGX_AGAIN      -2
#define  NGX_BUSY       -3
#define  NGX_DONE       -4
#define  NGX_DECLINED   -5
#define  NGX_ABORT      -6


#endif //MYNGX_NGX_CORE_H
