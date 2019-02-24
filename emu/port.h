#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <thread>
#include <slankdev/poll.h>
#include <slankdev/string.h>
#include <slankdev/net/tuntap.h>
#include <boost/lockfree/queue.hpp>
#include "mbuf.h"

using slankdev::strfmt;

struct port {
  std::string name;
  slankdev::tap* tap;
  slankdev::tap* phy;
  boost::lockfree::queue<mbuf*> phy_queue;
  struct {
    size_t discard;
  } counter;

  port(const char* name_) : phy_queue(128)
  {
    this->name = name_;
    tap = new slankdev::tap(name_);
    phy = new slankdev::tap(strfmt("peer-%s",name_).c_str());
  }
  ~port()
  {
    delete tap;
    delete phy;
  }
  mbuf* read_tap()
  {
    mbuf* m = new mbuf;
    size_t ret = this->tap->read(m->data()-4, m->buf_len());
    m->set_len(ret -4);
    return m;
  }
  mbuf* read_phy()
  {
    mbuf* m = new mbuf;
    size_t ret = this->phy->read(m->data()-4, m->buf_len());
    m->set_len(ret -4);
    return m;
  }
  void redirect_tap(mbuf* m)
  {
    tap->write(m->data()-4, m->pkt_len()+4);
    delete m;
  }
  void discard_mbuf(mbuf* m)
  {
    counter.discard ++;
    // printf("discard\n");
    delete m;
  }
  void enqueue_phy(mbuf* m)
  {
    phy_queue.push(m);
  }
  void flush_phy()
  {
    while (!phy_queue.empty()) {
      mbuf* m;
      if (phy_queue.pop(m)) {
        phy->write(m->data()-4, m->pkt_len()+4);
        delete m;
      }
    }
  }
};

