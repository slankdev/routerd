
#ifndef _YALIN_H_
#define _YALIN_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
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

#include <vector>
#include <string>

// #include "netlink_helper.h"
// #include "netlink_cache.h"
#include "yalin_route.h"
#include "yalin_neigh.h"
#include "yalin_addr.h"
#include "yalin_link.h"

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#ifndef NETLINK_EXT_ACK
#define NETLINK_EXT_ACK 11
#endif

netlink_t* netlink_open(uint32_t subscriptions, int32_t protocol);
void netlink_close(netlink_t *nl);
int netlink_listen(netlink_t *rtnl, rtnl_listen_filter_t handler, void *jarg);
std::string nlmsghdr_summary(const struct nlmsghdr* hdr);
int rtnl_summary(const struct sockaddr_nl *who __attribute__((unused)),
         struct rtnl_ctrl_data* _dum_ __attribute__((unused)),
         struct nlmsghdr *n, void *arg __attribute__((unused)));

std::string nlmsghdr_summary(const struct nlmsghdr* hdr);
int rtnl_summary(const struct sockaddr_nl *who __attribute__((unused)),
         struct rtnl_ctrl_data* _dum_ __attribute__((unused)),
         struct nlmsghdr *n, void *arg __attribute__((unused)));

#endif /* _YALIN_H_ */

