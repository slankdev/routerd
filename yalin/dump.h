
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
netlink_route_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  printf("rtm_family: %u (%s)\n",
      rtm->rtm_family, ifa_family_to_str(rtm->rtm_family));
  printf("rtm_dst_len: %u\n", rtm->rtm_dst_len);
  printf("rtm_src_len: %u\n", rtm->rtm_src_len);
  printf("rtm_tos: %u\n", rtm->rtm_tos);
  printf("rtm_table: %u\n", rtm->rtm_table);
  printf("rtm_protocol: %u\n", rtm->rtm_protocol);
  printf("rtm_scope: %u\n", rtm->rtm_scope);
  printf("rtm_type: %u (%s)\n", rtm->rtm_type,
      rtn_type_to_str(rtm->rtm_type));
  printf("rtm_flags: 0x%x\n", rtm->rtm_flags);

  size_t i=0;
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = RTM_RTA(rtm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    char str[512];
    printf("attr[%zd]: %s\n", i++, rta_to_str(rta, str, sizeof(str)));
  }
}

inline static void
netlink_neigh_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  struct ndmsg* ndm = (struct ndmsg*)(hdr + 1);
  printf("ndm_family: %u\n", ndm->ndm_family);
  printf("ndm_pad1: %u\n", ndm->ndm_pad1);
  printf("ndm_pad2: %u\n", ndm->ndm_pad2);
  printf("ndm_ifindex: %d\n", ndm->ndm_ifindex);
  printf("ndm_state: %u\n", ndm->ndm_state);
  printf("ndm_flags: 0x%x\n", ndm->ndm_flags);
  printf("ndm_type: %u\n", ndm->ndm_type);

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

