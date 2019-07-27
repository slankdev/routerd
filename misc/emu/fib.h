#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <slankdev/net/hdr.h>
#include <slankdev/string.h>

enum entry_type : uint8_t {
  forward,
  redirect_tap,
  blackhole,
};

static const char*
entry_type2str(uint8_t t)
{
  switch (t) {
    case forward     : return "forward";
    case redirect_tap: return "redirect_tap";
    case blackhole   : return "blackhole";
    default: return NULL;
  }
}

struct entry {
  /* all of ipv4 addr are stored as HostEndian */
  uint32_t nexthop;
  uint32_t mask;
  enum entry_type type;
  uint8_t port_id;
  uint8_t mac_addr[8];
  bool match(uint32_t addr) const
  { return (nexthop & mask) == (addr & mask); }
  std::string to_str() const
  {
    using slankdev::strfmt;
    return strfmt("%08x/%08x -> %s port%u "
        "mac:%02x:%02x:%02x:%02x:%02x:%02x",
        nexthop, mask, entry_type2str(type), port_id,
        mac_addr[0], mac_addr[1], mac_addr[2],
        mac_addr[3], mac_addr[4], mac_addr[5]);
  }
};

struct Fib {
  std::vector<entry> ents;
  Fib()
  {
    ents.push_back({0x0a000001, 0xffffffff, redirect_tap,
        0, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } });
    ents.push_back({0x0a010001, 0xffffffff, redirect_tap,
        1, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } });
    ents.push_back({0x0a000000, 0xffffff00, forward,
        0, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } });
    ents.push_back({0x0a010000, 0xffffff00, forward,
        1, { 0xff, 0xff, 0xff, 0xff, 0xff, 0xff } });
  }
  const entry* lookup(const struct slankdev::ip* ih) const
  {
    const size_t n = ents.size();
    for (size_t i=0; i<n; i++) {
      uint32_t dst = ntohl(ih->dst);
      if (ents[i].match(dst))
        return &ents[i];
    }
    return NULL;
  }
};
