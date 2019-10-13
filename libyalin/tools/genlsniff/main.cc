
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
  parser.addArgument("-f", "--family", 1, false);
  parser.addArgument("-r", "--readfile", 1, true);
  parser.parse(argc, argv);

  auto family_str = parser.retrieve<std::string>("family");

  auto readfile = parser.retrieve<std::string>("readfile");
  if (readfile != "") {

    printf("read from file %s\n", readfile.c_str());
    int ret = monitor_from_file(readfile);
    return ret;

  } else {
    ge_netlink_t* nl = ge_netlink_open(family_str.c_str());
    if (nl == NULL)
      return 1;

		printf("nl: %p\n", nl);
    int ret = ge_netlink_listen(nl, rtnl_summary, NULL);
    if (ret < 0)
      return 1;

    ge_netlink_close(nl);
  }
}

