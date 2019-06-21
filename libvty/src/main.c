
#include <zebra.h>
#include "command.h"

int dump_args(struct vty *vty,
    const char *descr, int argc,
    struct cmd_token *argv[])
{
  vty_out(vty, "%s with %d args.\n", descr, argc);
  for (int i = 0; i < argc; i++)
    vty_out(vty, "[%02d] %s@%s: %s\n", i, argv[i]->text,
      argv[i]->varname, argv[i]->arg);
  return CMD_SUCCESS;
}

#define DUMMY_HELPSTR                         \
  "00\n01\n02\n03\n04\n05\n06\n07\n08\n09\n"  \
  "10\n11\n12\n13\n14\n15\n16\n17\n18\n19\n"  \
  "20\n21\n22\n23\n24\n25\n26\n27\n28\n29\n"
#define DUMMY_DEFUN(name, cmdstr)                \
  DEFUN(name, name##_cmd, cmdstr, DUMMY_HELPSTR) \
  {                                              \
    return dump_args(vty, #name, argc, argv);    \
  }

DUMMY_DEFUN(cmd0, "slank ipv4 A.B.C.D");
DUMMY_DEFUN(cmd1, "show int");
DUMMY_DEFUN(cmd2, "arg ipv6 X:X::X:X$foo");
DUMMY_DEFUN(cmd3, "arg ipv6m X:X::X:X/M");
DUMMY_DEFUN(cmd4, "arg range (5-15)");
DUMMY_DEFUN(cmd5, "pat a < a|b>");
DUMMY_DEFUN(cmd6, "pat b  <a|b A.B.C.D$bar>");
DUMMY_DEFUN(cmd7, "pat c <a | b|c> A.B.C.D");
DUMMY_DEFUN(cmd8, "pat d {  foo A.B.C.D$foo|bar   X:X::X:X$bar| baz } [final]");
DUMMY_DEFUN(cmd9, "pat e [ WORD ]");
DUMMY_DEFUN(cmd10, "pat f [key]");
DUMMY_DEFUN(cmd11, "alt a WORD");
DUMMY_DEFUN(cmd12, "alt a A.B.C.D");
DUMMY_DEFUN(cmd13, "alt a X:X::X:X");
DUMMY_DEFUN(cmd14, "pat g {  foo A.B.C.D$foo|foo|bar   X:X::X:X$bar| baz } [final]");

void install_commands()
{
  install_element(ENABLE_NODE, &cmd0_cmd);
  install_element(ENABLE_NODE, &cmd1_cmd);
  install_element(ENABLE_NODE, &cmd2_cmd);
  install_element(ENABLE_NODE, &cmd3_cmd);
  install_element(ENABLE_NODE, &cmd4_cmd);
  install_element(ENABLE_NODE, &cmd5_cmd);
  install_element(ENABLE_NODE, &cmd6_cmd);
  install_element(ENABLE_NODE, &cmd7_cmd);
  install_element(ENABLE_NODE, &cmd8_cmd);
  install_element(ENABLE_NODE, &cmd9_cmd);
  install_element(ENABLE_NODE, &cmd10_cmd);
  install_element(ENABLE_NODE, &cmd11_cmd);
  install_element(ENABLE_NODE, &cmd12_cmd);
  install_element(ENABLE_NODE, &cmd13_cmd);
  install_element(ENABLE_NODE, &cmd14_cmd);
}

struct thread_master *master = NULL;

static void
vty_do_exit(int isexit)
{
  printf("\nend.\n");
  cmd_terminate();
  vty_terminate();
  thread_master_free(master);
  if (!isexit)
    exit(0);
}

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

  install_commands();
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

