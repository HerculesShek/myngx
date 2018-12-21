//
// Created by Will on 2018-12-21.
//

#ifndef MYNGX_NGX_SOCKET_H
#define MYNGX_NGX_SOCKET_H


typedef int ngx_socket_t;


int ngx_nonblocking(ngx_socket_t s);
int ngx_blocking(ngx_socket_t s);


#endif //MYNGX_NGX_SOCKET_H
