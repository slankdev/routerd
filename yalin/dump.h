
#ifndef _NETLINK_DUMP_H_
#define _NETLINK_DUMP_H_

#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include "types.h"
#include "flags.h"
#include "rtattr.h"

#ifndef NDM_RTA
#define NDM_RTA(r) ((struct rtattr*)(((char*)(r))+NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif

#if 0
inline static void
netlink_neigh_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);

  size_t i=0;
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = NDM_RTA(ndm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    char str[512];
    printf("attr[%zd]: %s\n", i++, rta_to_str(rta, str, sizeof(str)));
  }
}
#endif

#endif /* _NETLINK_DUMP_H_ */

