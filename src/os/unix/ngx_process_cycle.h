//
// Created by Will on 2018-12-21.
//

#ifndef MYNGX_NGX_PROCESS_CYCLE_H
#define MYNGX_NGX_PROCESS_CYCLE_H

#include <ngx_config.h>
#include <ngx_core.h>

extern ngx_uint_t ngx_process;
extern ngx_pid_t  ngx_pid;


#define NGX_CMD_OPEN_CHANNEL   1
#define NGX_CMD_CLOSE_CHANNEL  2
#define NGX_CMD_QUIT           3
#define NGX_CMD_TERMINATE      4
#define NGX_CMD_REOPEN         5


#define NGX_PROCESS_SINGLE   0
#define NGX_PROCESS_MASTER   1
#define NGX_PROCESS_WORKER   2

//void ngx_master_process_cycle(ngx_cycle_t *cycle);
void ngx_master_process_cycle();

void delay(int number_of_seconds);



extern ngx_uint_t      ngx_process;
extern ngx_pid_t       ngx_pid;
extern ngx_pid_t       ngx_new_binary;

#endif //MYNGX_NGX_PROCESS_CYCLE_H
