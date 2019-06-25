#ifndef _NETLINK_CACHE_H_
#define _NETLINK_CACHE_H_

#include <vector>
#include <string>

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

#include "netlink_socket.h"

// typedef struct netlink_s netlink_t;

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

netlink_cache_t* netlink_cache_alloc(netlink_t* nl);
void netlink_cache_free(netlink_cache_t* nlc);
size_t netlink_cachelen_get_link(netlink_cache_t* nlc, uint16_t index);
const ifinfomsg* netlink_cache_get_link(netlink_cache_t* nlc, uint16_t index);
void netlink_cache_update_link(netlink_cache_t* nlc,
    const struct ifinfomsg* ifm, size_t rta_len);
void netlink_dump_link(netlink_t* nl);

#endif /* _NETLINK_CACHE_H_ */
