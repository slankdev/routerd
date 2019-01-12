
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "yalin/yalin.h"
#include "slankdev/filefd.h"
#include "argparse.hpp"

typedef struct {
  uint16_t magic;
  uint16_t lladdr;
  uint16_t dum1[2];
  uint16_t dum2[2];
  uint16_t dum3;
  uint16_t family;
} nl_cooked_hdr_t;

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

inline static std::string
rtnl_msghdr_summary(const struct nlmsghdr* hdr)
{
  char buf[256];
  const char* type = nlmsg_type_to_str(hdr->nlmsg_type);
  snprintf(buf, sizeof(buf), "%-12s f=0x%04x s=%010u p=%010u",
      type?type:"unknown", hdr->nlmsg_flags, hdr->nlmsg_seq, hdr->nlmsg_pid);
  return buf;
}

static int
dump_msg(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  slankdev::filefd* file = (slankdev::filefd*)(arg);

  pcap_pkt_hdr_t pp;
  memset(&pp, 0x0, sizeof(pp));
  pp.ts_sec = 0;
  pp.ts_usec = 0;
  pp.incl_len = sizeof(nl_cooked_hdr_t) + n->nlmsg_len;
  pp.orig_len = sizeof(nl_cooked_hdr_t) + n->nlmsg_len;
  file->fwrite(&pp, sizeof(pp), 1);

  nl_cooked_hdr_t ph;
  memset(&ph, 0x0, sizeof(ph));
  ph.magic = 0x0400;
  ph.lladdr = 0x3803;
  ph.family = 0x0000;
  file->fwrite(&ph, sizeof(ph), 1);

  file->fwrite(n, n->nlmsg_len, 1);
  file->fflush();
  printf("write [%s]\n", rtnl_msghdr_summary(n).c_str());
  return 0;
}

int
main(int argc, const char **argv)
{
  ArgumentParser parser;
  parser.addArgument("-w", "--writefile", 1, false);
  parser.parse(argc, argv);
  std::string filename = parser.retrieve<std::string>("writefile");

  uint32_t groups = ~0U;
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  pcap_file_hdr_t hdr;
  memset(&hdr, 0x0, sizeof(hdr));
  hdr.magic_number = 0xa1b2c3d4;
  hdr.version_major = 0x0002;
  hdr.version_minor = 0x0004;
  hdr.thiszone = 0x0;
  hdr.sigfigs = 0x0;
  hdr.snaplen = 0x00040000;
  hdr.network = 0xfd;

  slankdev::filefd file;
  file.fopen(filename.c_str(), "w");
  file.fwrite(&hdr, sizeof(hdr), 1);
  file.fflush();

  int ret = netlink_listen(nl, dump_msg, &file);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

