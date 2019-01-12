
#include "socket.h"

static inline int
netlink_listen(netlink_t *rtnl,
    rtnl_listen_filter_t handler, void *jarg)
{
  struct sockaddr_nl sa = { .nl_family = AF_NETLINK };
  struct iovec iov;
  struct msghdr msg;
  memset(&msg, 0x0, sizeof(struct msghdr));
  msg.msg_name = &sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char   buf[16384];
  iov.iov_base = buf;
  while (true) {
    struct rtnl_ctrl_data ctrl;
    iov.iov_len = sizeof(buf);
    int status = recvmsg(rtnl->fd, &msg, 0);
    if (status < 0) {
      if (errno == EINTR
       || errno == EAGAIN
       || errno == ENOBUFS)
        continue;
      return -1;
    }
    if (status == 0) {
      fprintf(stderr, "EOF on netlink\n");
      return -1;
    }

    for (struct nlmsghdr* h = (struct nlmsghdr *)buf;
         size_t(status) >= sizeof(*h);) {
      int len = h->nlmsg_len;
      int err = handler(&sa, &ctrl, h, jarg);
      if (err < 0)
        return err;

      status -= NLMSG_ALIGN(len);
      h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
    }

    if (msg.msg_flags & MSG_TRUNC) {
      // Message truncated
      continue;
    }
    if (status) {
      // Remnant of size
      exit(1);
    }
  }
}

