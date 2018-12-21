//
// Created by Will on 2018-12-21.
//

#ifndef MYNGX_NGX_ERRNO_H
#define MYNGX_NGX_ERRNO_H

typedef int ngx_err_t;


#define NGX_EPERM         EPERM
#define NGX_ENOENT        ENOENT
#define NGX_ESRCH         ESRCH
#define NGX_EINTR         EINTR
#define NGX_ECHILD        ECHILD
#define NGX_ENOMEM        ENOMEM
#define NGX_EACCES        EACCES
#define NGX_EBUSY         EBUSY
#define NGX_EEXIST        EEXIST
#define NGX_ENOTDIR       ENOTDIR
#define NGX_EISDIR        EISDIR
#define NGX_EINVAL        EINVAL
#define NGX_ENOSPC        ENOSPC
#define NGX_EPIPE         EPIPE
#define NGX_EAGAIN        EAGAIN
#define NGX_EINPROGRESS   EINPROGRESS
#define NGX_EADDRINUSE    EADDRINUSE
#define NGX_ECONNABORTED  ECONNABORTED
#define NGX_ECONNRESET    ECONNRESET
#define NGX_ENOTCONN      ENOTCONN
#define NGX_ETIMEDOUT     ETIMEDOUT
#define NGX_ECONNREFUSED  ECONNREFUSED
#define NGX_ENAMETOOLONG  ENAMETOOLONG
#define NGX_EHOSTUNREACH  EHOSTUNREACH
#define NGX_ENOSYS        ENOSYS
#define NGX_ECANCELED     ECANCELED
#define NGX_ENOMOREFILES  0


#define ngx_errno                  errno
#define ngx_socket_errno           errno
#define ngx_set_errno(err)         errno = err
#define ngx_set_socket_errno(err)  errno = err

#endif //MYNGX_NGX_ERRNO_H
