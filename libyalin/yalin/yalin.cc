
#include "yalin.h"
#include "core/link.h"
#include "core/addr.h"
#include "core/route.h"
#include "core/neigh.h"
#include "core/netconf.h"
#include "core/nexthop.h"

int
rtnl_summary(const struct sockaddr_nl *who __attribute__((unused)),
         struct rtnl_ctrl_data* _dum_ __attribute__((unused)),
         struct nlmsghdr *n, void *arg __attribute__((unused)))
{
  std::string str = nlmsghdr_summary(n) + " :: ";
  switch (n->nlmsg_type) {

    case RTM_NEWLINK:
    case RTM_DELLINK:
    case RTM_GETLINK:
      str += rtnl_link_summary(n);
      break;

    case RTM_NEWADDR:
    case RTM_DELADDR:
    case RTM_GETADDR:
      str += rtnl_addr_summary(n);
      break;

    case RTM_NEWROUTE:
    case RTM_DELROUTE:
    case RTM_GETROUTE:
      str += rtnl_route_summary(n);
      break;

    case RTM_NEWNEIGH:
    case RTM_DELNEIGH:
    case RTM_GETNEIGH:
      str += rtnl_neigh_summary(n);
      break;

    case RTM_NEWNETCONF:
    case RTM_DELNETCONF:
    case RTM_GETNETCONF:
      str += rtnl_netconf_summary(n);
      break;

    case RTM_NEWNEXTHOP:
    case RTM_DELNEXTHOP:
    case RTM_GETNEXTHOP:
      str += rtnl_nexthop_summary(n);
      break;

    case RTM_NEWNSID:
    case RTM_DELNSID:
    case RTM_GETNSID:
      str += "unsupport nsid msg";
      break;

    /* Invalid Case */
    default:
      fprintf(stderr, "%s: unknown type(%u)\n", __func__, n->nlmsg_type);
      fprintf(stderr, "%s: type=%s\n", __func__, str.c_str());
      fprintf(stderr, "please check with "
          "\'grep %u /usr/include/linux/rtnetlink.h\'\n", n->nlmsg_type);
      exit(1);
      break;
  }
  printf("%s\n", str.c_str());
  return 0;
}

std::string
nlmsghdr_summary(const struct nlmsghdr* hdr)
{
  char buf[256];
  const char* type = nlmsg_type_to_str(hdr->nlmsg_type);
  snprintf(buf, sizeof(buf), "%-12s f=0x%04x s=%010u p=%010u",
      type?type:"unknown", hdr->nlmsg_flags, hdr->nlmsg_seq, hdr->nlmsg_pid);
  return buf;
}

