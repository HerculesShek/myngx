//
// Created by Will on 2018-12-21.
//

#ifndef MYNGX_NGX_TYPES_H
#define MYNGX_NGX_TYPES_H


#include <ngx_config.h>


typedef int         ngx_fd_t;
typedef struct stat ngx_file_info_t;
typedef ino_t       ngx_file_uniq_t;

typedef struct {
    DIR           *dir;
    struct dirent *de;
    struct stat   info;

    ngx_uint_t valid_info:1;  /* unsigned  valid_info:1; */
}                   ngx_dir_t;

#endif //MYNGX_NGX_TYPES_H
