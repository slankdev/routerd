
#include <stdio.h>
#include <pthread.h>
#include <vui/vui.h>
#include "netlink.h"


void *th(void *ptr)
{
  vui_t *vui = vui_new();
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_serv_sock(vui, NULL, 9077);
  vui_serv_stdio(vui);
  setup_netlink_node(vui);
  vui_run();
  vui_delete(vui);
  return NULL;
}

int main(int argc, char **argv)
{
  pthread_t thread1;
  int ret = pthread_create(&thread1, NULL, th, NULL);
  pthread_join(thread1, NULL);
  return 0;
}

