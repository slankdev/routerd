
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
#include "slankdev/filefd.h"

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

int monitor_from_file(const std::string& filename)
{
  typedef struct {
    uint32_t ts_sec;
    uint32_t ts_usec;
    uint32_t incl_len;
    uint32_t orig_len;
  } pcap_pkt_hdr_t;

  typedef struct {
    uint32_t magic_number;
    uint16_t version_major;
    uint16_t version_minor;
    int32_t  thiszone;
    uint32_t sigfigs;
    uint32_t snaplen;
    uint32_t network;
  } pcap_file_hdr_t;

  typedef struct {
    uint16_t magic;
    uint16_t lladdr;
    uint16_t dum1[2];
    uint16_t dum2[2];
    uint16_t dum3;
    uint16_t family;
  } nl_cooked_hdr_t;

  slankdev::filefd file;
  file.fopen(filename.c_str(), "rb");

  pcap_file_hdr_t hdr;
  file.fread(&hdr, sizeof(hdr), 1);
  if (hdr.network != 0xfd) {
    fprintf(stderr, "invalid network-type: %#x\n", hdr.network);
    return 1;
  }

  while (true) {
    if (file.feof())
      return 0;

    pcap_pkt_hdr_t pp;
    file.fread(&pp, sizeof(pp), 1);

    nl_cooked_hdr_t ph;
    file.fread(&ph, sizeof(ph), 1);

    uint8_t buf[10000];
    file.fread(buf, pp.incl_len-sizeof(nl_cooked_hdr_t), 1);
    rtnl_summary(NULL, NULL, (struct nlmsghdr*)buf, NULL);
  }
}

int
main(int argc, const char **argv)
{
  ArgumentParser parser;
  parser.addArgument("-g", "--rtmgrp", 1, false);
  parser.addArgument("-r", "--readfile", 1, true);
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

  auto readfile = parser.retrieve<std::string>("readfile");
  if (readfile != "") {

    printf("read from file %s\n", readfile.c_str());
    int ret = monitor_from_file(readfile);
    return ret;

  } else {
    printf("monitoring group(RTMGRP) is 0x%08x ...\n", group);
    netlink_t* nl = netlink_open(group, NETLINK_ROUTE);
    if (nl == NULL)
      return 1;

    int ret = netlink_listen(nl, rtnl_summary, NULL);
    if (ret < 0)
      return 1;

    netlink_close(nl);
  }
}

