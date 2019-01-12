
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
#include "hexdump.h"
#include "rtattr.h"

#ifndef NDM_RTA
#define NDM_RTA(r) ((struct rtattr*)(((char*)(r))+NLMSG_ALIGN(sizeof(struct ndmsg))))
#endif

inline static void
netlink_link_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  char strbuf[100];
  struct ifinfomsg* ifm = (struct ifinfomsg*)(hdr + 1);
  printf("ifi_family: %u <%s>\n", ifm->ifi_family,
      ifa_family_to_str(ifm->ifi_family));
  printf("ifi_type: %u <%s>\n", ifm->ifi_type,
      ifi_type_to_str(ifm->ifi_type));
  printf("ifi_index: %d\n", ifm->ifi_index);
  printf("ifi_flags: 0x%x <%s>\n", ifm->ifi_flags,
      ifi_flags_to_str(ifm->ifi_flags, strbuf, sizeof(strbuf)));
  printf("ifi_change: 0x%x\n", ifm->ifi_change);

  size_t i=0;
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = IFLA_RTA(ifm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    char str[1000];
    printf("attr[%zd]: %s\n", i++, rta_to_str(rta, str, sizeof(str)));
  }
}

inline static void
netlink_addr_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  char strbuf[256];
  struct ifaddrmsg* ifa = (struct ifaddrmsg*)(hdr + 1);
  printf("ifa_family: %u (%s)\n",
      ifa->ifa_family, ifa_family_to_str(ifa->ifa_family));
  printf("ifa_prefixlen: %u\n", ifa->ifa_prefixlen);
  printf("ifa_flags: 0x%x\n", ifa->ifa_flags);
  printf("ifa_scope: %u\n", ifa->ifa_scope    );
  printf("ifa_index: %d (%s)\n", ifa->ifa_index,
      if_indextoname(ifa->ifa_index, strbuf));

  size_t i=0;
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = IFA_RTA(ifa);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    char str[512];
    printf("attr[%zd]: %s\n", i++, rta_to_str(rta, str, sizeof(str)));
  }
}

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

inline static void
netlink_msg_dump(FILE* fp, const struct nlmsghdr* hdr)
{
  char strbuf[100];
  printf("-----NLMSG-BEGIN---------------------------\n");
  printf("nlmsg_len: %u\n", hdr->nlmsg_len  );
  printf("nlmsg_type: %u <%s>\n", hdr->nlmsg_type,
      nlmsg_type_to_str(hdr->nlmsg_type)?
      nlmsg_type_to_str(hdr->nlmsg_type):"unknwon");
  printf("nlmsg_flags(get): 0x%x <%s>\n", hdr->nlmsg_flags,
    nlmsg_flags_to_str_get(hdr->nlmsg_flags, strbuf, sizeof(strbuf)));
  printf("nlmsg_flags(new): 0x%x <%s>\n", hdr->nlmsg_flags,
    nlmsg_flags_to_str_new(hdr->nlmsg_flags, strbuf, sizeof(strbuf)));
  printf("nlmsg_seq: %u\n", hdr->nlmsg_seq  );
  printf("nlmsg_pid: %u\n", hdr->nlmsg_pid  );

  uint16_t type = hdr->nlmsg_type;
  switch (type) {
    case RTM_NEWLINK:
    case RTM_DELLINK:
    case RTM_GETLINK:
      netlink_link_msg_dump(fp, hdr);
      break;
    case RTM_NEWADDR:
    case RTM_DELADDR:
    case RTM_GETADDR:
      netlink_addr_msg_dump(fp, hdr);
      break;
    case RTM_NEWROUTE:
    case RTM_DELROUTE:
    case RTM_GETROUTE:
      netlink_route_msg_dump(fp, hdr);
      break;
    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
    case RTM_GETNEIGH:
      netlink_neigh_msg_dump(fp, hdr);
      break;
    default:
      fprintf(stderr, "%s: unknown type(%u)\n", __func__, type);
      hexdump(stderr, hdr, hdr->nlmsg_len);
      break;
  }
  printf("-----NLMSG-END-----------------------------\n");
}

#endif /* _NETLINK_DUMP_H_ */

