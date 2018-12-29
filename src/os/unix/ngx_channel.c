
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

    n = sendmsg(s, &msg, 0); // MSG_CTRUNC
    printf("sendmsg to [%i] res is %zi\n", s, n);

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



ngx_int_t
ngx_read_channel(ngx_socket_t s, ngx_channel_t *ch, size_t size)
{
    ssize_t             n;
    ngx_err_t           err;
    struct iovec        iov[1];
    struct msghdr       msg;

#if (NGX_HAVE_MSGHDR_MSG_CONTROL)
    union {
        struct cmsghdr  cm;
        char            space[CMSG_SPACE(sizeof(int))];
    } cmsg;
#else
    int                 fd;
#endif

    iov[0].iov_base = (char *) ch;
    iov[0].iov_len = size;

    msg.msg_name = NULL;
    msg.msg_namelen = 0;
    msg.msg_iov = iov;
    msg.msg_iovlen = 1;

#if (NGX_HAVE_MSGHDR_MSG_CONTROL)
    msg.msg_control = (caddr_t) &cmsg;
    msg.msg_controllen = sizeof(cmsg);
#else
    msg.msg_accrights = (caddr_t) &fd;
    msg.msg_accrightslen = sizeof(int);
#endif

    n = recvmsg(s, &msg, 0);

    if (n == -1) {
        err = ngx_errno;
        if (err == NGX_EAGAIN) {
            return NGX_AGAIN;
        }

//        ngx_log_error(NGX_LOG_ALERT, log, err, "recvmsg() failed");
        return NGX_ERROR;
    }

    if (n == 0) {
//        ngx_log_debug0(NGX_LOG_DEBUG_CORE, log, 0, "recvmsg() returned zero");
        return NGX_ERROR;
    }

    if ((size_t) n < sizeof(ngx_channel_t)) {
//        ngx_log_error(NGX_LOG_ALERT, log, 0,
//                      "recvmsg() returned not enough data: %uz", n);
        return NGX_ERROR;
    }

#if (NGX_HAVE_MSGHDR_MSG_CONTROL)

    printf(" [%i]: ch->command is %i\n", ngx_pid, ch->command);
    if (ch->command == NGX_CMD_OPEN_CHANNEL) {

        if (cmsg.cm.cmsg_len < (socklen_t) sizeof(cmsg)) {
//            ngx_log_error(NGX_LOG_ALERT, log, 0,
//                          "recvmsg() returned too small ancillary data");
            return NGX_ERROR;
        }

        if (cmsg.cm.cmsg_level != SOL_SOCKET || cmsg.cm.cmsg_type != SCM_RIGHTS)
        {
//            ngx_log_error(NGX_LOG_ALERT, log, 0,
//                          "recvmsg() returned invalid ancillary data "
//                          "level %d or type %d",
//                          cmsg.cm.cmsg_level, cmsg.cm.cmsg_type);
            return NGX_ERROR;
        }

        ch->fd = *(int *) CMSG_DATA(&cmsg.cm);
    }

    if (msg.msg_flags & (MSG_TRUNC|MSG_CTRUNC)) {
//        ngx_log_error(NGX_LOG_ALERT, log, 0,
//                      "recvmsg() truncated data");
    }

#else

    if (ch->command == NGX_CMD_OPEN_CHANNEL) {
        if (msg.msg_accrightslen != sizeof(int)) {
            ngx_log_error(NGX_LOG_ALERT, log, 0,
                          "recvmsg() returned no ancillary data");
            return NGX_ERROR;
        }

        ch->fd = fd;
    }

#endif

    return n;
}


