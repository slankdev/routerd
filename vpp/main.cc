
#include <stdio.h>
#include <thread>
#include <vui/vui.h>

#include "netlink.h"
#include "netlink_cli.h"
#include "vpp_cli.h"
#include "tap.h"

void th1()
{
  vui_t *vui = vui_new();
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_serv_sock(vui, NULL, 9077);
  vui_serv_stdio(vui);
  setup_netlink_node(vui);
  setup_vpp_node(vui);
  setup_tap_node(vui);
  vui_read_conf(vui, "./routerd.conf");

  vui_run();
  vui_delete(vui);
}

int main(int argc, char **argv)
{
  std::thread thrd1(th1);
  std::thread thrd2(netlink_manager);
  thrd1.join();
  thrd2.join();
  // pthread_join(thread1, NULL);
  // pthread_join(thread2, NULL);
}

