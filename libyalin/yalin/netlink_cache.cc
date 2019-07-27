
#include "netlink_socket.h"
#include "netlink_cache.h"

void
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


inline static int
cache_callback(const struct sockaddr_nl *who __attribute__((unused)),
         struct rtnl_ctrl_data* _dum_ __attribute__((unused)),
         struct nlmsghdr *n, void *arg __attribute__((unused)))
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

void
netlink_dump_addr(netlink_t* nl)
{
  struct {
    struct nlmsghdr hdr;
    struct rtgenmsg gen;
  } req;
  memset(&req, 0x0, sizeof(req));

  req.hdr.nlmsg_len = NLMSG_LENGTH(sizeof(struct rtgenmsg));
  req.hdr.nlmsg_type = RTM_GETADDR;
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

void
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

netlink_cache_t*
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

size_t
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

const ifinfomsg*
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

void
netlink_cache_free(netlink_cache_t* nlc)
{ delete nlc; }
