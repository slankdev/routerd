
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <vector>
#include <string>
#include <thread>
#include <slankdev/poll.h>
#include <slankdev/string.h>
#include <slankdev/hexdump.h>
#include <slankdev/net/hdr.h>
#include <slankdev/net/tuntap.h>
#include <boost/lockfree/queue.hpp>
#include "port.h"
#include "fib.h"
#include "log.h"
#include "config.h"
std::array<port*, 2> ports;
Fib* fib;

void ipv4_input(port* port, mbuf* buf)
{
  slankdev::ether* eh = (slankdev::ether*)(buf->data());
  slankdev::ip* ih = (slankdev::ip*)(eh + 1);
  const entry* ent = fib->lookup(ih);
  if (ent == NULL)
    port->redirect_tap(buf);

  switch (ent->type) {
    case forward:
    {
      if (conf::get_instance().debug_trace) {
        log_info("%p: %s: forward %s\n", buf,
            __func__, ent->to_str().c_str());
      }
      uint8_t dst_port_id = ent->port_id;
      memcpy(&eh->dst, ent->mac_addr, 6);
      ports[dst_port_id]->enqueue_phy(buf);
      break;
    }
    case redirect_tap:
    {
      if (conf::get_instance().debug_trace) {
        log_info("%p: %s: redirect_tap %s\n", buf,
            __func__, ent->to_str().c_str());
      }
      port->redirect_tap(buf);
      break;
    }
    case blackhole:
    {
      if (conf::get_instance().debug_trace) {
        log_info("%p: %s: blackhole %s\n", buf,
            __func__, ent->to_str().c_str());
      }
      port->discard_mbuf(buf);
      break;
    }
    default:
      assert(false);
      break;
  }
}

void port_input(port* port, mbuf* buf)
{
  slankdev::ether* eh = (slankdev::ether*)(buf->data());
  switch (ntohs(eh->type)) {
    case 0x0806:
      if (conf::get_instance().debug_trace)
        log_info("%p: %s: ether-type is arp\n",
            buf, __func__);
      port->redirect_tap(buf);
      break;
    case 0x0800:
      if (conf::get_instance().debug_trace)
        log_info("%p: %s: ether-type is ipv4\n",
            buf, __func__);
      ipv4_input(port, buf);
      break;
    default:
      if (conf::get_instance().debug_trace)
        log_info("%p: %s: ether-type is unknown\n",
            buf, __func__);
      port->discard_mbuf(buf);
      break;
  }
}

void port_process(struct port* port)
{
  slankdev::pollfd pfd;
  pfd.append_fd(port->tap->get_fd(), POLLIN|POLLERR);
  pfd.append_fd(port->phy->get_fd(), POLLIN|POLLERR);
  while (true) {
    pfd.poll(1);
    constexpr size_t tap_idx = 0;
    if (pfd.get_revents(tap_idx) & POLLIN) {
      mbuf* buf = port->read_tap();
      port->enqueue_phy(buf);
    }
    constexpr size_t phy_idx = 1;
    if (pfd.get_revents(phy_idx) & POLLIN) {
      mbuf* buf = port->read_phy();
      port_input(port, buf);
    }
    port->flush_phy();
  }
}

int main(int argc [[gnu::unused]], char** argv [[gnu::unused]])
{
  conf& conf = conf::get_instance();
  conf.load_file("./trio.json");
  conf.dump(stdout);

  fib = new Fib;
  ports[0] = new port("et-0-0-0");
  ports[1] = new port("et-0-0-1");
  std::thread t0(port_process, ports[0]);
  std::thread t1(port_process, ports[1]);
  while (true) ;
  t0.join();
  t1.join();
  delete ports[0];
  delete ports[1];
}

