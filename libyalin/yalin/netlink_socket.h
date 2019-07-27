#ifndef NETLINK_SOCKET_H
#define NETLINK_SOCKET_H

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
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef int (*rtnl_listen_filter_t)(
             const struct sockaddr_nl*,
             struct rtnl_ctrl_data *,
             struct nlmsghdr *n, void *);

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

int netlink_listen(netlink_t *rtnl, rtnl_listen_filter_t handler, void *arg);
int netlink_listen_until_done(netlink_t *rtnl, rtnl_listen_filter_t handler, void *arg);
netlink_t* netlink_open(uint32_t subscriptions, int32_t protocol);
void netlink_close(netlink_t *nl);

#ifdef __cplusplus
}
#endif
#endif /* NETLINK_SOCKET_H */
