
#include "vui.h"
#include <stdio.h>

void setup_netlink_node(vui_t *vui);

void *th(void *ptr)
{
  vui_t *vui = vui_new();
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_serv_sock(vui, NULL, 9077);
  vui_serv_stdio(vui);
  setup_netlink_node(vui);
  vui_run();
}

int main(int argc, char **argv)
{
  pthread_t thread1;
  int ret = pthread_create(&thread1, NULL, th, NULL);
  pthread_join(thread1, NULL);
}

