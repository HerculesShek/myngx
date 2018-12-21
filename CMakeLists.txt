cmake_minimum_required(VERSION 3.12)
project(myngx C)

#set(CMAKE_C_STANDARD 11)

set(CMAKE_C_FLAGS "-pipe -Wall -Wextra -Wpointer-arith -Wconditional-uninitialized -Wno-unused-parameter -Wno-deprecated-declarations")


include_directories(src/auto
        src/core
        src/event
        src/event/modules
        src/http
        src/http/modules
        src/http/modules/perl
        src/mysql
        src/os
        objs
        src/os/unix
        src/http/modules/proxy
        src/imap)

set(SOURCE_FILES
        src/auto/ngx_auto_config.h
        src/auto/ngx_auto_headers.h
        src/auto/ngx_modules.c

        src/core/nginx.c
        src/core/nginx.h
        src/core/ngx_config.h
        src/core/ngx_conf_file.h
        src/core/ngx_core.h
        src/core/ngx_string.c
        src/core/ngx_string.h


        src/event/ngx_event.h


        src/os/unix/ngx_channel.c
        src/os/unix/ngx_channel.h
        src/os/unix/ngx_errno.h
        src/os/unix/ngx_posix_config.h
        src/os/unix/ngx_process.h
        src/os/unix/ngx_process.c
        src/os/unix/ngx_socket.h
        src/os/unix/ngx_socket.c
        src/os/unix/ngx_process_cycle.h
        src/os/unix/ngx_process_cycle.c
        src/os/unix/ngx_time.h
        src/os/unix/ngx_types.h


        )


add_executable(myngx ${SOURCE_FILES})
