
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <string>
#include "yalin/yalin.h"

inline static std::string
my_rtnl_msghdr_summary(const struct nlmsghdr* hdr)
{
  char buf[256];
  const char* type = nlmsg_type_to_str(hdr->nlmsg_type);
  snprintf(buf, sizeof(buf), "%-12s f=0x%04x s=%010u p=%010u",
      type?type:"unknown", hdr->nlmsg_flags, hdr->nlmsg_seq, hdr->nlmsg_pid);
  return buf;
}

inline static std::string
my_rtnl_newlink_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_dellink_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_newaddr_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_deladdr_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_newroute_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_delroute_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_newneigh_summary(const struct nlmsghdr* hdr)
{ return __func__; }

inline static std::string
my_rtnl_delneigh_summary(const struct nlmsghdr* hdr)
{ return __func__; }

static int
dump_msg(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  std::string str = my_rtnl_msghdr_summary(n) + " :: ";
  switch (n->nlmsg_type) {
    case RTM_NEWLINK : str += my_rtnl_newlink_summary(n) ; break;
    case RTM_DELLINK : str += my_rtnl_newlink_summary(n) ; break;
    case RTM_NEWADDR : str += my_rtnl_newaddr_summary(n) ; break;
    case RTM_DELADDR : str += my_rtnl_newaddr_summary(n) ; break;
    case RTM_NEWROUTE: str += my_rtnl_newroute_summary(n); break;
    case RTM_DELROUTE: str += my_rtnl_newroute_summary(n); break;
    case RTM_NEWNEIGH: str += my_rtnl_newneigh_summary(n); break;
    case RTM_DELNEIGH: str += my_rtnl_newneigh_summary(n); break;

    /* Invalid Case */
    default:
      fprintf(stderr, "%s: unknown type(%u)\n", __func__, n->nlmsg_type);
      fprintf(stderr, "please check with "
          "\'grep %u /usr/include/linux/rtnetlink.h\'\n", n->nlmsg_type);
      hexdump(stderr, n, n->nlmsg_len);
      exit(1);
      break;
  }
  printf("%s\n", str.c_str());
  hexdump(stderr, n, n->nlmsg_len);
  printf("\n");
  return 0;
}

int
main(int argc, char **argv)
{
  uint32_t groups = ~0U;
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, dump_msg, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

