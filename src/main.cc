
#include <stdio.h>
#include <getopt.h>
#include <thread>
#include <vui/vui.h>

#include "netlink.h"
#include "netlink_cli.h"
#include "vpp_cli.h"
#include "tap.h"
#include "debug.h"
#include "routerd.h"

static void
vui_manager(bool interactive, const char* config_path)
{
  vui_t *vui = vui_new();
  vui_set_password(vui, "slank");
  vui_set_hostname(vui, "slank");
  vui_serv_sock(vui, NULL, 9077);
  if (interactive)
    vui_serv_stdio(vui);
  setup_netlink_node(vui);
  setup_vpp_node(vui);
  setup_tap_node(vui);
  setup_debug_node(vui);
  vui_read_conf(vui, config_path);

  vui_run();
  vui_delete(vui);
}

static void
usage(const char *progname)
{
  FILE *fp = stdout;
  fprintf(fp, "Usage: %s\n", progname);
}

static int
parse_args(int argc, char **argv)
{
  const static struct option longopts[] = {
      { "interactive", no_argument, NULL, 'i' },
      { "configfile", required_argument, NULL, 'c' },
      { 0, 0, 0, 0 },
  };

  while (true) {
    int opt_index;
    int opt = getopt_long(argc, argv, "ic:", longopts, &opt_index);
    if (opt == EOF)
      break;

    switch (opt) {
      case 'i':
        rd_ctx.global_config.enable_interactive = true;
        break;
      case 'c':
        rd_ctx.global_config.config_path = optarg;
        break;
        break;
      default:
        fprintf(stdout, "%s: invalid arg \'%c\'\n", __func__, opt);
        return -1;
        break;
    }
  }
  return 0;
}

int main(int argc, char **argv)
{
  int ret = parse_args(argc, argv);
  if (ret < 0) {
    usage(argv[0]);
    return 1;
  }
  // rd_ctx.dump(stdout);

  std::thread thrd1(vui_manager,
      rd_ctx.global_config.enable_interactive,
      rd_ctx.global_config.config_path.c_str());
  sleep(1); //XXX
  std::thread thrd2(netlink_manager);
  thrd1.join();
  thrd2.join();
}

