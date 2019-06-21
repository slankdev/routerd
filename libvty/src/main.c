
#include <zebra.h>
#include "command.h"

struct thread_master *master = NULL;
void setup_netlink_node();

void *th(void *ptr)
{
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
}

int main(int argc, char **argv)
{
  pthread_t thread1;
  int ret = pthread_create(&thread1, NULL, th, NULL);
  pthread_join(thread1, NULL);
}

