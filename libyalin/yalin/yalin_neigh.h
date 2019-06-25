#ifndef _NEIGH_H_
#define _NEIGH_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "netlink_helper.h"
#include "core/neigh.h"

namespace routerd {

struct neigh {
  const struct ndmsg* ndm;
  size_t len;
  rta_array* rtas;

  neigh(const struct ndmsg* ndm_, size_t rta_len)
  {
    this->ndm = ndm_;
    this->len = rta_len;
    this->rtas = new rta_array(NDM_RTA(ndm), len);
  }
  std::string summary() const
  {
    return strfmt("afi=%u index=%u state=%u flag=0x%01x type=%u",
        ndm->ndm_family, ndm->ndm_ifindex, ndm->ndm_state,
        ndm->ndm_flags, ndm->ndm_type);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    std::string lladdr;
    if (rtas->get(NDA_LLADDR)) {
      const struct rtattr* rta = rtas->get(NDA_LLADDR);
      assert(rta->rta_len == 10);
      uint8_t* ptr = (uint8_t*)(rta+1);
      lladdr = strfmt("%02x:%02x:%02x:%02x:%02x:%02x",
          ptr[0], ptr[1], ptr[2], ptr[3], ptr[4], ptr[5]);
    }

    std::string dst;
    if (rtas->get(NDA_DST)) {
      const struct rtattr* rta = rtas->get(NDA_DST);
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      int afi = addr_len==4?AF_INET:AF_INET6;
      dst = inetpton(addr_ptr, afi);
    }

    return strfmt("ip nei %s %s lladdr %s dev %s",
        nlmsg_type==RTM_NEWNEIGH?"add":"del",
        dst.c_str(), lladdr.c_str(),
        ifindex2str(ndm->ndm_ifindex).c_str());
  }
}; /* struct neigh */

} /* namespace routerd */

#endif /* _NEIGH_H_ */
