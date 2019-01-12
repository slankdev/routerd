
#ifndef _YALIN_FILTER_H_
#define _YALIN_FILTER_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stdbool.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

inline static bool
netlink_is_link_msg(const struct nlmsghdr* hdr)
{
  uint16_t type = hdr->nlmsg_type;
  return type == RTM_NEWLINK
      || type == RTM_DELLINK
      || type == RTM_GETLINK;
}

#endif /* _YALIN_FILTER_H_ */

