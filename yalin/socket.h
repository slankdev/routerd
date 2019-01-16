
#ifndef _NETLINK_SOCKET_H_
#define _NETLINK_SOCKET_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>

#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>

#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/socket.h>

#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#ifndef NETLINK_EXT_ACK
#define NETLINK_EXT_ACK 11
#endif

#ifndef IFA_RTA
#error IFA_RTA isnt defined
#endif

#ifndef RTM_RTA
#error RTM_RTA isnt defined
#endif

#ifndef IFLA_RTA
#error IFLA_RTA isnt defined
#endif

#ifndef NDM_RTA
#define NDM_RTA(r) ((struct rtattr*)(((char*)(r))+NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif

typedef struct netlink_s {
  int32_t fd;
  struct sockaddr_nl local;
  struct sockaddr_nl peer;
  uint32_t seq;
  uint32_t dump;
  int32_t proto;
  FILE *dump_fp;
  int flags;
} netlink_t;

struct rtnl_ctrl_data {
  int nsid;
};

typedef int (*rtnl_listen_filter_t)(
             const struct sockaddr_nl*,
             struct rtnl_ctrl_data *,
             struct nlmsghdr *n, void *);

static inline netlink_t*
netlink_open(uint32_t subscriptions, int32_t protocol)
{
  netlink_t *nl = (netlink_t*)malloc(sizeof(netlink_t));
  memset(nl, 0, sizeof(*nl));
  nl->proto = protocol;
  nl->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, protocol);
  if (nl->fd < 0) {
    perror("socket");
    return NULL;
  }

  int sendbuf = 32768;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_SNDBUF,
           &sendbuf, sizeof(sendbuf)) < 0) {
    perror("setsockopt(SO_SNDBUF)");
    return NULL;
  }

  int recvbuf = 1024 * 1024;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_RCVBUF,
           &recvbuf, sizeof(recvbuf)) < 0) {
    perror("setsockopt(SO_RCVBUF)");
    return NULL;
  }

  int one = 1;
  setsockopt(nl->fd, SOL_NETLINK,
      NETLINK_EXT_ACK, &one, sizeof(one));

  memset(&nl->local, 0, sizeof(nl->local));
  nl->local.nl_family = AF_NETLINK;
  nl->local.nl_groups = subscriptions;
  if (bind(nl->fd, (struct sockaddr *)&nl->local,
     sizeof(nl->local)) < 0) {
    perror("bind local addr");
    return NULL;
  }

  socklen_t addr_len = sizeof(nl->local);
  if (getsockname(nl->fd, (struct sockaddr *)&nl->local,
      &addr_len) < 0) {
    perror("getsockname");
    return NULL;
  }

  return nl;
}

static inline void
netlink_close(netlink_t *nl)
{
  if (nl->fd >= 0) {
    close(nl->fd);
    nl->fd = -1;
  }
  free(nl);
}

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


#endif /* _NETLINK_SOCKET_H_ */

