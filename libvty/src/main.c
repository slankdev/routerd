
#include <zebra.h>
#include "command.h"

struct thread_master *master = NULL;
void setup_netlink_node();

#if 0
void vui_install_commands(vui_t *vui)
{
  vui_install_element(vui, ENABLE_NODE, &cmd0_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd1_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd2_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd3_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd4_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd5_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd6_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd7_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd8_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd9_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd10_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd11_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd12_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd13_cmd);
  vui_install_element(vui, ENABLE_NODE, &cmd14_cmd);
}
#endif

void *th(void *ptr)
{
#if 0
  vui_t *vui = vui_new();
  vui_init(vui, 0, 9077);
  vui_serv_stdio(NULL);
  vui_serv_sock(vui, 0, 9077)
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_install_commands(vui)
  vui_run();
#else
  master = thread_master_create(NULL);
  cmd_init();
  cmd_password_set("slank");
  cmd_hostname_set("test");
  vty_init(master, false);
  vty_serv_sock(NULL, 9077, "/var/run/frr/slank.vty");
  setup_netlink_node();;

  vty_stdio(NULL);
  struct thread thread;
  while (thread_fetch(master, &thread))
    thread_call(&thread);
  return NULL;
#endif
}

int main(int argc, char **argv)
{
  pthread_t thread1;
  int ret = pthread_create(&thread1, NULL, th, NULL);
  pthread_join(thread1, NULL);
}

