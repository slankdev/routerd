#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include "yalin/yalin.h"

void monitor_NEWLINK(const struct nlmsghdr* hdr);
void monitor_DELLINK(const struct nlmsghdr* hdr);
void monitor_NEWADDR(const struct nlmsghdr* hdr);
void monitor_DELADDR(const struct nlmsghdr* hdr);
void monitor_NEWROUTE(const struct nlmsghdr* hdr);
void monitor_DELROUTE(const struct nlmsghdr* hdr);
void monitor_NEWNEIGH(const struct nlmsghdr* hdr);
void monitor_DELNEIGH(const struct nlmsghdr* hdr);

int monitor(const struct sockaddr_nl *who [[gnu::unused]],
         struct rtnl_ctrl_data* _dum_ [[gnu::unused]],
         struct nlmsghdr *n, void *arg [[gnu::unused]]);

#endif /* _NETLINK_H_ */
