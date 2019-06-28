#ifndef _ROUTERD_H_
#define _ROUTERD_H_

#include <stdio.h>
#include <vector>

struct routerd_interface {
  uint32_t kern_ifindex;
  uint32_t vpp_ifindex;
  routerd_interface(uint32_t k, uint32_t v) :
    kern_ifindex(k), vpp_ifindex(v) {}
};

struct routerd_context {
  std::vector<struct routerd_interface> interfaces;
  void add_interface(uint32_t k, uint32_t v)
  {
    routerd_interface iface(k, v);
    interfaces.push_back(iface);
  }
};

extern struct routerd_context rd_ctx;

#endif /* _ROUTERD_H_ */
