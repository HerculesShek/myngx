//
// Created by Will on 2018-12-20.
//

#ifndef MYNGX_NGX_CONF_FILE_H
#define MYNGX_NGX_CONF_FILE_H

#include <ngx_config.h>
#include <ngx_core.h>

#define NGX_MODULE_V1          0, 0, 0, 0, 0, 0, 1
#define NGX_MODULE_V1_PADDING  0, 0, 0, 0, 0, 0, 0, 0

//
//struct ngx_module_s {
//    ngx_uint_t ctx_index;
//    ngx_uint_t index;
//
//    ngx_uint_t spare0;
//    ngx_uint_t spare1;
//    ngx_uint_t spare2;
//    ngx_uint_t spare3;
//
//    ngx_uint_t version;
//
//    void          *ctx;
//    ngx_command_t *commands;
//    ngx_uint_t    type;
//
//    ngx_int_t (*init_master)(ngx_log_t *log);
//
//    ngx_int_t (*init_module)(ngx_cycle_t *cycle);
//
//    ngx_int_t (*init_process)(ngx_cycle_t *cycle);
//
//    ngx_int_t (*init_thread)(ngx_cycle_t *cycle);
//
//    void (*exit_thread)(ngx_cycle_t *cycle);
//
//    void (*exit_process)(ngx_cycle_t *cycle);
//
//    void (*exit_master)(ngx_cycle_t *cycle);
//
//    uintptr_t spare_hook0;
//    uintptr_t spare_hook1;
//    uintptr_t spare_hook2;
//    uintptr_t spare_hook3;
//    uintptr_t spare_hook4;
//    uintptr_t spare_hook5;
//    uintptr_t spare_hook6;
//    uintptr_t spare_hook7;
//};

//
//typedef struct {
//    ngx_str_t             name;
//    void               *(*create_conf)(ngx_cycle_t *cycle);
//    char               *(*init_conf)(ngx_cycle_t *cycle, void *conf);
//} ngx_core_module_t;


#endif //MYNGX_NGX_CONF_FILE_H
