#pragma once
#include <stdio.h>
#include <stdint.h>
#include <stddef.h>
#include <vector>
#include <slankdev/hexdump.h>

struct mbuf {
  std::vector<uint8_t> raw;
  size_t pos;
  size_t len_;
  uint8_t* data() const { return (uint8_t*)raw.data() + pos; }
  size_t pkt_len() const { return len_; }
  size_t buf_len() const { return raw.size() - pos; }
  void set_len(size_t l) { len_ = l; }
  static const size_t default_data_room_size = 10000;
  static const size_t default_head_room_size = 1000;
  mbuf()
  {
    raw.resize(default_data_room_size);
    pos = default_head_room_size;
    len_ = 0;
  }
  void dump(FILE* fp=stdout) const
  {
    fprintf(fp, "PKG len=%zd\n", pkt_len());
    slankdev::hexdump(fp, data(), pkt_len());
  }
};

