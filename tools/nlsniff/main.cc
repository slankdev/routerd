
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "yalin/yalin.h"
#include "argparse.hpp"

#include <string> // std::string, std::char_traits
#include <vector> // std::vector

namespace std {
  static inline vector<string>
  split(const string &s, char delim)
  {
    vector<string> elems;
    stringstream ss(s);
    string item;
    while (getline(ss, item, delim))
      if (!item.empty())
        elems.push_back(item);
    return elems;
  }
} /* namespace  std */

int
main(int argc, const char **argv)
{
  ArgumentParser parser;
  parser.addArgument("-g", "--rtmgrp", 1, false);
  parser.parse(argc, argv);

  auto grpstr = parser.retrieve<std::string>("rtmgrp");
  auto grps = std::split(grpstr, ',');
  uint32_t group = 0;
  for (size_t i=0; i<grps.size(); i++) {
    if (grps[i] == "all") group |= ~0U;
    else if (grps[i] == "link")        group |= RTMGRP_LINK;
    else if (grps[i] == "notify")      group |= RTMGRP_NOTIFY;
    else if (grps[i] == "neigh")       group |= RTMGRP_NEIGH;
    else if (grps[i] == "tc")          group |= RTMGRP_TC;
    else if (grps[i] == "ipv4_ifaddr") group |= RTMGRP_IPV4_IFADDR;
    else if (grps[i] == "ipv6_ifaddr") group |= RTMGRP_IPV6_IFADDR;
    else if (grps[i] == "ipv4_route")  group |= RTMGRP_IPV4_ROUTE;
    else if (grps[i] == "ipv6_route")  group |= RTMGRP_IPV6_ROUTE;
    else if (grps[i] == "ipv4_mroute") group |= RTMGRP_IPV4_MROUTE;
    else if (grps[i] == "ipv6_mroute") group |= RTMGRP_IPV6_MROUTE;
    else if (grps[i] == "ipv4_rule")   group |= RTMGRP_IPV4_RULE;
    else if (grps[i] == "ipv6_ifinfo") group |= RTMGRP_IPV6_IFINFO;
  }

  printf("monitoring group(RTMGRP) is 0x%08x ...\n", group);
  netlink_t* nl = netlink_open(group, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, rtnl_summary, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

