
#include "netlink.h"
#include "hexdump.h"
#include <stdio.h>
#include <stdbool.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>

static void adddel_in6_route(int fd,
    struct in6_addr *pref, uint32_t plen,
                struct in6_addr *nh6, bool install)
{
  struct {
    struct nlmsghdr  n;
    struct rtmsg r;
    char buf[4096];
  } req = {
    .n.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtmsg)),
    .n.nlmsg_flags = NLM_F_REQUEST | NLM_F_CREATE | NLM_F_EXCL | NLM_F_ACK,
    .n.nlmsg_type = install ? RTM_NEWROUTE : RTM_DELROUTE,
    .r.rtm_family = AF_INET6,
    .r.rtm_dst_len = plen,
    .r.rtm_src_len = 0,
    .r.rtm_tos = 0,
    .r.rtm_table = RT_TABLE_MAIN,
    .r.rtm_protocol = 0x03,
    .r.rtm_scope = 0xfd,
    .r.rtm_type = RTN_UNICAST,
    .r.rtm_flags = 0,
  };

  /* set RTA_DST */
  addattr_l(&req.n, sizeof(req), RTA_DST, pref, sizeof(struct in6_addr));
  req.r.rtm_dst_len = plen;

  /* set RTA_GATEWAY */
  addattr_l(&req.n, sizeof(req), RTA_GATEWAY, nh6, sizeof(struct in6_addr));

  /* talk with netlink-bus */
  hexdump(stdout, &req.n, req.n.nlmsg_len);
  if (nl_talk(fd, &req.n, NULL, 0) < 0)
    exit(1);
}

void main() {
  int fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  if (fd < 0)
    exit(1);

  struct in6_addr pref, nh6;
  uint32_t plen = 128;
  inet_pton(AF_INET6, "20::", &pref);
  inet_pton(AF_INET6, "2001::2", &nh6);

  adddel_in6_route(fd, &pref, plen, &nh6, true);
  close(fd);
}

