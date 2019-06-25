
#include <stdio.h>
#include <pthread.h>
#include <vui/vui.h>

#include "yalin/yalin.h"
#include "netlink.h"
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif
void setup_netlink_node(vui_t *vui);
void setup_vpp_node(vui_t *vui);
void setup_tap_node(vui_t *vui);
#ifdef __cplusplus
}
#endif

void *th1(void *ptr)
{
  vui_t *vui = vui_new();
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_serv_sock(vui, NULL, 9077);
  vui_serv_stdio(vui);
  setup_netlink_node(vui);
  setup_vpp_node(vui);
  setup_tap_node(vui);

  vui_run();
  vui_delete(vui);
  return NULL;
}

netlink_cache_t* nlc;
void *th2(void *ptr)
{
  uint32_t groups = ~0U;
  netlink_t *nl = netlink_open(groups, NETLINK_ROUTE);
  nlc = netlink_cache_alloc(nl);
  netlink_listen(nl, monitor, nullptr);
  netlink_close(nl);
  log_info("routerd-end\n");
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t thread1, thread2;
  pthread_create(&thread1, NULL, th1, NULL);
  pthread_create(&thread2, NULL, th2, NULL);
  pthread_join(thread1, NULL);
  pthread_join(thread2, NULL);
}

