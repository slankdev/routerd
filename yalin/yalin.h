
#ifndef _YALIN_H_
#define _YALIN_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>

#ifndef SOL_NETLINK
#define SOL_NETLINK 270
#endif

#ifndef NETLINK_EXT_ACK
#define NETLINK_EXT_ACK 11
#endif


inline static std::string
strfmt(const char* fmt, ...)
{
  char str[1000];
  va_list args;
  va_start(args, fmt);
  vsprintf(str, fmt, args);
  va_end(args);
  return str;
}

inline static std::string
inetpton(const void* ptr, int afi)
{
  char buf[256];
  memset(buf, 0, sizeof(buf));
  inet_ntop(afi, ptr, buf, sizeof(buf));
  return buf;
}

inline static std::string
ifindex2str(uint32_t ifindex)
{
  char str[IF_NAMESIZE];
  char* ret = if_indextoname(ifindex, str);
  return ret?ret:"none";
}

inline static size_t
rta_payload(const struct rtattr* rta)
{ return rta->rta_len-sizeof(struct rtattr); }

inline static uint8_t*
rtattr_payload_ptr(const struct rtattr* rta)
{ return (uint8_t*)(rta + 1); }

inline static uint8_t
rta_read8(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint8_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint8_t val = *(uint8_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static void*
rta_readptr(const struct rtattr* rta)
{ return (void*)(rta + 1); }

inline static uint16_t
rta_read16(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint16_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint16_t val = *(uint16_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint32_t
rta_read32(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint32_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint32_t val = *(uint32_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static uint64_t
rta_read64(const struct rtattr* attr)
{
  if (rta_payload(attr) > sizeof(uint64_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rta_payload(attr), attr->rta_type);
    fprintf(stderr, " - payload_len: %zd\n", rta_payload(attr));
    exit(1);
  }
  uint64_t val = *(uint64_t*)rtattr_payload_ptr(attr);
  return val;
}

inline static const char*
rta_readstr(const struct rtattr* attr)
{
  const char* strptr = (const char*)rtattr_payload_ptr(attr);
  const size_t strbuflen = strlen(strptr);
  const size_t payloadlen = rta_payload(attr);
  if (payloadlen < strbuflen) {
    fprintf(stderr, "%s: read miss (type=%u)\n", __func__, attr->rta_type);
    printf("payloadlen: %zd\n", payloadlen);
    printf("strbuflen: %zd\n", strbuflen);
    exit(1);
  }
  return strptr;
}

inline static const char*
nlmsg_type_to_str(uint16_t type)
{
  switch (type) {
    case RTM_NEWLINK: return "RTM_NEWLINK";
    case RTM_DELLINK: return "RTM_DELLINK";
    case RTM_GETLINK: return "RTM_GETLINK";
    case RTM_NEWADDR: return "RTM_NEWADDR";
    case RTM_DELADDR: return "RTM_DELADDR";
    case RTM_GETADDR: return "RTM_GETADDR";
    case RTM_NEWROUTE: return "RTM_NEWROUTE";
    case RTM_DELROUTE: return "RTM_DELROUTE";
    case RTM_GETROUTE: return "RTM_GETROUTE";
    case RTM_NEWNEIGH: return "RTM_NEWNEIGH";
    case RTM_DELNEIGH: return "RTM_DELNEIGH";
    case RTM_GETNEIGH: return "RTM_GETNEIGH";
    case RTM_NEWRULE: return "RTM_NEWRULE";
    case RTM_DELRULE: return "RTM_DELRULE";
    case RTM_GETRULE: return "RTM_GETRULE";
    case RTM_NEWQDISC: return "RTM_NEWQDISC";
    case RTM_DELQDISC: return "RTM_DELQDISC";
    case RTM_GETQDISC: return "RTM_GETQDISC";
    case RTM_NEWTCLASS: return "RTM_NEWTCLASS";
    case RTM_DELTCLASS: return "RTM_DELTCLASS";
    case RTM_GETTCLASS: return "RTM_GETTCLASS";
    case RTM_NEWTFILTER: return "RTM_NEWTFILTER";
    case RTM_DELTFILTER: return "RTM_DELTFILTER";
    case RTM_GETTFILTER: return "RTM_GETTFILTER";
    case RTM_NEWACTION: return "RTM_NEWACTION";
    case RTM_DELACTION: return "RTM_DELACTION";
    case RTM_GETACTION: return "RTM_GETACTION";
    case RTM_NEWPREFIX: return "RTM_NEWPREFIX";
    case RTM_GETMULTICAST: return "RTM_GETMULTICAST";
    case RTM_GETANYCAST: return "RTM_GETANYCAST";
    case RTM_NEWNEIGHTBL: return "RTM_NEWNEIGHTBL";
    case RTM_GETNEIGHTBL: return "RTM_GETNEIGHTBL";
    case RTM_SETNEIGHTBL: return "RTM_SETNEIGHTBL";
    case RTM_NEWNDUSEROPT: return "RTM_NEWNDUSEROPT";
    case RTM_NEWADDRLABEL: return "RTM_NEWADDRLABEL";
    case RTM_DELADDRLABEL: return "RTM_DELADDRLABEL";
    case RTM_GETADDRLABEL: return "RTM_GETADDRLABEL";
    case RTM_GETDCB: return "RTM_GETDCB";
    case RTM_SETDCB: return "RTM_SETDCB";
    case RTM_NEWNETCONF: return "RTM_NEWNETCONF";
    case RTM_DELNETCONF: return "RTM_DELNETCONF";
    case RTM_GETNETCONF: return "RTM_GETNETCONF";
    case RTM_NEWMDB: return "RTM_NEWMDB";
    case RTM_DELMDB: return "RTM_DELMDB";
    case RTM_GETMDB: return "RTM_GETMDB";
    case RTM_NEWNSID: return "RTM_NEWNSID";
    case RTM_DELNSID: return "RTM_DELNSID";
    case RTM_GETNSID: return "RTM_GETNSID";
    case RTM_NEWSTATS: return "RTM_NEWSTATS";
    case RTM_GETSTATS: return "RTM_GETSTATS";
    case RTM_NEWCACHEREPORT: return "RTM_NEWCACHEREPORT";
    default: return "RTM_XXXUNKNOWNXXX";
  }
}

#include "addr.h"
#include "link.h"
#include "neigh.h"
#include "route.h"
#include "netconf.h"

typedef struct netlink_s {
  int32_t fd;
  struct sockaddr_nl local;
  struct sockaddr_nl peer;
  uint32_t seq;
  uint32_t dump;
  int32_t proto;
  FILE *dump_fp;
  int flags;
} netlink_t;

struct rtnl_ctrl_data {
  int nsid;
};

struct buffer {
  std::vector<uint8_t> raw;
  uint8_t* data() { return raw.data(); }
  size_t size() { return raw.size(); }
  void memcpy(const void* src, size_t len)
  {
    raw.clear();
    raw.resize(len);
    ::memcpy(raw.data(), src, len);
  }
};

typedef struct netlink_cache_s {
  std::vector<buffer> links;
  // std::vector<uint8_t> addr;
  // std::vector<uint8_t> route;
  // std::vector<uint8_t> neigh;
} netlink_cache_t;

static inline void
netlink_cache_update_link(netlink_cache_t* nlc,
    const struct ifinfomsg* ifm, size_t rta_len);

static inline void
netlink_dump_link(netlink_t* nl)
{
  /*
   * REFERENCES
   * + https://gist.github.com/cl4u2/5204374
   * + https://linuxjm.osdn.jp/html/LDP_man-pages/man3/rtnetlink.3.html
   */

  struct {
    struct nlmsghdr hdr;
    struct rtgenmsg gen;
  } req;
  memset(&req, 0x0, sizeof(req));

  req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
  req.hdr.nlmsg_type = RTM_GETLINK;
  req.hdr.nlmsg_flags = NLM_F_REQUEST | NLM_F_DUMP;
  req.hdr.nlmsg_seq = random();
  req.hdr.nlmsg_pid = getpid();
  req.gen.rtgen_family = AF_PACKET;

  int ret = write(nl->fd, &req, sizeof(req));
  if (ret < 0) {
    fprintf(stderr, "OKASHII..\n");
    abort();
  }
}

inline static int
cache_callback(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  netlink_cache_t* nlc = (netlink_cache_t*)arg;
  if (n->nlmsg_type == NLMSG_DONE) {
      return -1;
  }
  switch (n->nlmsg_type) {
    case RTM_NEWLINK:
    case RTM_DELLINK:
    {
      const struct ifinfomsg* ifi = (struct ifinfomsg*)(n + 1);
      const size_t ifa_payload_len = IFA_PAYLOAD(n);
      netlink_cache_update_link(nlc, ifi, ifa_payload_len);
      break;
    }
  }
  return 0;
}

static inline void
netlink_cache_free(netlink_cache_t* nlc)
{ delete nlc; }

typedef int (*rtnl_listen_filter_t)(
             const struct sockaddr_nl*,
             struct rtnl_ctrl_data *,
             struct nlmsghdr *n, void *);


static inline netlink_t*
netlink_open(uint32_t subscriptions, int32_t protocol)
{
  netlink_t *nl = (netlink_t*)malloc(sizeof(netlink_t));
  memset(nl, 0, sizeof(*nl));
  nl->proto = protocol;
  nl->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, protocol);
  if (nl->fd < 0) {
    perror("socket");
    return NULL;
  }

  int sendbuf = 32768;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_SNDBUF,
           &sendbuf, sizeof(sendbuf)) < 0) {
    perror("setsockopt(SO_SNDBUF)");
    return NULL;
  }

  int recvbuf = 1024 * 1024;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_RCVBUF,
           &recvbuf, sizeof(recvbuf)) < 0) {
    perror("setsockopt(SO_RCVBUF)");
    return NULL;
  }

  int one = 1;
  setsockopt(nl->fd, SOL_NETLINK,
      NETLINK_EXT_ACK, &one, sizeof(one));

  memset(&nl->local, 0, sizeof(nl->local));
  nl->local.nl_family = AF_NETLINK;
  nl->local.nl_groups = subscriptions;
  if (bind(nl->fd, (struct sockaddr *)&nl->local,
     sizeof(nl->local)) < 0) {
    perror("bind local addr");
    return NULL;
  }

  socklen_t addr_len = sizeof(nl->local);
  if (getsockname(nl->fd, (struct sockaddr *)&nl->local,
      &addr_len) < 0) {
    perror("getsockname");
    return NULL;
  }

  return nl;
}

static inline void
netlink_close(netlink_t *nl)
{
  if (nl->fd >= 0) {
    close(nl->fd);
    nl->fd = -1;
  }
  free(nl);
}

static inline int
netlink_listen(netlink_t *rtnl,
    rtnl_listen_filter_t handler, void *jarg)
{
  struct sockaddr_nl sa = { .nl_family = AF_NETLINK };
  struct iovec iov;
  struct msghdr msg;
  memset(&msg, 0x0, sizeof(struct msghdr));
  msg.msg_name = &sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char   buf[16384];
  iov.iov_base = buf;
  while (true) {
    struct rtnl_ctrl_data ctrl;
    iov.iov_len = sizeof(buf);
    int status = recvmsg(rtnl->fd, &msg, 0);
    if (status < 0) {
      if (errno == EINTR
       || errno == EAGAIN
       || errno == ENOBUFS)
        continue;
      return -1;
    }
    if (status == 0) {
      fprintf(stderr, "EOF on netlink\n");
      return -1;
    }

    for (struct nlmsghdr* h = (struct nlmsghdr *)buf;
         size_t(status) >= sizeof(*h);) {
      int len = h->nlmsg_len;
      int err = handler(&sa, &ctrl, h, jarg);
      if (err < 0)
        return err;

      status -= NLMSG_ALIGN(len);
      h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
    }

    if (msg.msg_flags & MSG_TRUNC) {
      // Message truncated
      continue;
    }
    if (status) {
      // Remnant of size
      exit(1);
    }
  }
}

inline static std::string
nlmsghdr_summary(const struct nlmsghdr* hdr)
{
  char buf[256];
  const char* type = nlmsg_type_to_str(hdr->nlmsg_type);
  snprintf(buf, sizeof(buf), "%-12s f=0x%04x s=%010u p=%010u",
      type?type:"unknown", hdr->nlmsg_flags, hdr->nlmsg_seq, hdr->nlmsg_pid);
  return buf;
}

inline static int
rtnl_summary(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
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

inline static void
parse_rtattr(const void* buf, size_t buflen, struct rtattr* attrs[], size_t max_attrs)
{
  size_t rta_len = buflen;
  for (struct rtattr* rta = (struct rtattr*)buf;
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    if (rta->rta_type >= max_attrs)
      printf("rta->rta_type:%u, max_attr:%zd\n",
          rta->rta_type, max_attrs);
    assert(rta->rta_type < max_attrs);
    attrs[rta->rta_type] = rta;
  }
}

static inline netlink_cache_t*
netlink_cache_alloc(netlink_t* nl)
{
  netlink_cache_t* nlc = new netlink_cache_s;
  if (!nlc) {
    perror("malloc");
    return NULL;
  }
  netlink_dump_link(nl);
  netlink_listen(nl, cache_callback, nlc);

  // printf("---Cache-INFO-BEGIN---\n");
  // printf("link: %zd\n", nlc->links.size());
  // for (size_t i=0; i<nlc->links.size(); i++) {
  //   auto& raw = nlc->links[i];
  //   const struct ifinfomsg* ifi = (struct ifinfomsg*)raw.data();
  //   const size_t rta_len = raw.size();
  //   slankdev::hexdump(stdout, ifi, rta_len);
  // }
  // printf("---Cache-INFO-END-----\n");

  return nlc;
}

static inline const ifinfomsg*
netlink_cache_get_link(netlink_cache_t* nlc, uint16_t index)
{
  const size_t n_links = nlc->links.size();
  for (size_t i=0; i<n_links; i++) {
    const struct ifinfomsg* ifi =
      (const struct ifinfomsg*)nlc->links[i].data();
    // printf("--- ifindex:%u ---\n", ifi->ifi_index);
    if (ifi->ifi_index == index)
      return ifi;
  }
  return nullptr;
}

static inline size_t
netlink_cachelen_get_link(netlink_cache_t* nlc, uint16_t index)
{
  const size_t n_links = nlc->links.size();
  for (size_t i=0; i<n_links; i++) {
    const struct ifinfomsg* ifi =
      (const struct ifinfomsg*)nlc->links[i].data();
    // printf("--- ifindex:%u ---\n", ifi->ifi_index);
    if (ifi->ifi_index == index)
      return nlc->links[i].size();
  }
  return 0;
}

static inline void
netlink_cache_update_link(netlink_cache_t* nlc,
    const struct ifinfomsg* ifm, size_t rta_len)
{
  const size_t n_links = nlc->links.size();

  /* update */
  for (size_t i=0; i<n_links; i++) {
    const struct ifinfomsg* ifi =
      (const struct ifinfomsg*)nlc->links[i].data();
    if (ifi->ifi_index == ifm->ifi_index)
      nlc->links[i].memcpy(ifm, rta_len);
  }

  /* add new entry */
  buffer buf;
  buf.memcpy(ifm, rta_len);
  nlc->links.push_back(buf);
}

#endif /* _YALIN_H_ */

