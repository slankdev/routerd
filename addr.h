#ifndef _ADDR_H_
#define _ADDR_H_

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

namespace routerd {

struct ifaddr {
  const struct ifaddrmsg* ifa;
  size_t len;
  rta_array* rtas;

  ifaddr(const struct ifaddrmsg* ifa_, size_t rta_len)
  {
    this->ifa = ifa_;
    this->len = rta_len;
    rtas = new rta_array(IFA_RTA(ifa), rta_len);
  }
  std::string summary() const
  {
    return strfmt("if=%zd afi=%d prefixlen=%d",
        ifa->ifa_index, ifa->ifa_family, ifa->ifa_prefixlen);
  }
  std::string to_iproute2_cli(uint16_t nlmsg_type) const
  {
    const uint8_t* addr_ptr = (const uint8_t*)rta_readptr(rtas->get(IFLA_ADDRESS));
    std::string addr = inetpton(addr_ptr, ifa->ifa_family);
    std::string ifname = ifindex2str(ifa->ifa_index);
    return strfmt("ip -%d addr %s %s/%d dev %s",
        ifa->ifa_family==AF_INET?4:6,
        nlmsg_type==RTM_NEWADDR?"add":"del",
        addr.c_str(), ifa->ifa_prefixlen, ifname.c_str());
  }
}; /* struct ifaddr */

} /* namespace routerd */

#endif /* _ADDR_H_ */
