
#include <ngx_config.h>
#include <ngx_core.h>
#include <ngx_channel.h>

void
ngx_close_channel(ngx_fd_t *fd) {
//ngx_close_channel(ngx_fd_t *fd, ngx_log_t *log) {
//    if (close(fd[0]) == -1) {
//        ngx_log_error(NGX_LOG_ALERT, log, ngx_errno, "close() channel failed");
//    }
//
//    if (close(fd[1]) == -1) {
//        ngx_log_error(NGX_LOG_ALERT, log, ngx_errno, "close() channel failed");
//    }

    close(fd[0]);
    close(fd[1]);
}


ngx_int_t
ngx_write_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size) {
    ssize_t       n;
    ngx_err_t     err;
    struct iovec  iov[1];
    struct msghdr msg;

#if (NGX_HAVE_MSGHDR_MSG_CONTROL)

    union {
        struct cmsghdr cm;
        char           space[CMSG_SPACE(sizeof(int))];
    } cmsg;

    if (ch->fd == -1) {
        msg.msg_control    = NULL;
        msg.msg_controllen = 0;

    } else {
        msg.msg_control    = (caddr_t) &cmsg;
        msg.msg_controllen = sizeof(cmsg);

        cmsg.cm.cmsg_len   = sizeof(cmsg);
        cmsg.cm.cmsg_level = SOL_SOCKET;
        cmsg.cm.cmsg_type  = SCM_RIGHTS;
        *(int *) CMSG_DATA(&cmsg.cm) = ch->fd;
    }

    msg.msg_flags = 0;

#else

    if (ch->fd == -1) {
        msg.msg_accrights = NULL;
        msg.msg_accrightslen = 0;

    } else {
        msg.msg_accrights = (caddr_t) &ch->fd;
        msg.msg_accrightslen = sizeof(int);
    }

#endif

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len  = size;

    msg.msg_name    = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov     = iov;
    msg.msg_iovlen  = 1;

    n = sendmsg(s, &msg, 0);

    if (n == -1) {
        err = ngx_errno;
        if (err == NGX_EAGAIN) {
            return NGX_AGAIN;
        }

//        ngx_log_error(NGX_LOG_ALERT, log, err, "sendmsg() failed");
        return NGX_ERROR;
    }

    return NGX_OK;
}


