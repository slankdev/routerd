
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
using slankdev::strfmt;
std::array<port*, 2> ports;

void ipv4_input(port* port, mbuf* buf)
{ port->redirect_tap(buf); }

void ipv6_input(port* port, mbuf* buf)
{ port->redirect_tap(buf); }

void port_input(port* port, mbuf* buf)
{
  slankdev::ether* eh = (slankdev::ether*)(buf->data());
  switch (ntohs(eh->type)) {
    case 0x0806:
      port->redirect_tap(buf);
      break;
    case 0x0800:
      ipv4_input(port, buf);
      break;
    case 0x86dd:
      ipv6_input(port, buf);
      break;
    default:
      printf("unknown ether-type: 0x%04x\n", ntohs(eh->type));
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
    pfd.poll(-1);
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

