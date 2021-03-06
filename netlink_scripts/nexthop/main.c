
#include "netlink.h"
#include "hexdump.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/nexthop.h>
#include <linux/lwtunnel.h>
#include <linux/seg6.h>
#include <linux/seg6_local.h>
#include <linux/seg6_iptunnel.h>

static void adddel_nexthop(int fd,
    uint32_t oif_idx, uint32_t nh_id,
    bool install)
{
  struct {
    struct nlmsghdr  n;
    struct nhmsg nh;
    char buf[4096];
  } req = {
    .n.nlmsg_len = NLMSG_LENGTH(sizeof(struct nhmsg)),
    .n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL | NLM_F_ACK,
    .n.nlmsg_type = install ? RTM_NEWNEXTHOP : RTM_DELNEXTHOP,
    .nh.nh_family = AF_INET6,
    .nh.nh_scope = 0x00,
    .nh.nh_protocol = 0x03,
    .nh.resvd = 0x00,
    .nh.nh_flags = 0,
  };

  addattr32(&req.n, sizeof(req), NHA_OIF, oif_idx);
  addattr32(&req.n, sizeof(req), NHA_ID, nh_id);

  char buf[1024];
  struct rtattr *rta = (void *)buf;
  rta->rta_type = NHA_ENCAP;
  rta->rta_len = RTA_LENGTH(0);
  struct rtattr *nest = rta_nest(rta, sizeof(buf), NHA_ENCAP);
  rta_addattr32(rta, sizeof(buf), SEG6_LOCAL_ACTION, SEG6_LOCAL_ACTION_END);
  rta_nest_end(rta, nest);
  addraw_l(&req.n, 1024 , RTA_DATA(rta), RTA_PAYLOAD(rta));

  addattr16(&req.n, sizeof(req), NHA_ENCAP_TYPE, LWTUNNEL_ENCAP_SEG6_LOCAL);

  char answer[1000] = {0};
  hexdump(stdout, &req.n, req.n.nlmsg_len);
  int ret = nl_talk(fd, &req.n, (void*)answer, sizeof(answer));
  if (ret < 0)
    exit(1);

  printf("==========\n");
  hexdump(stdout, answer, ret);
}

void main() {
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (fd < 0)
    exit(1);

#define SOL_NETLINK 270
  const int one = 1;
  int ret = setsockopt(fd, SOL_NETLINK, NETLINK_EXT_ACK, &one, sizeof(one));
  if (ret < 0) {
    perror("setsockopt(NETLINK_F_EXT_ACK)");
    exit(1);
  }

  uint32_t oif_idx = 4;
  uint32_t nh_id = 22;
  adddel_nexthop(fd, oif_idx, nh_id, true);
  close(fd);
}

