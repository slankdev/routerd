
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include "yalin/yalin.h"

static int
dump_msg(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  netlink_msg_dump(stdout, n);
  return 0;
}

int
main(int argc, char **argv)
{
  uint32_t groups = ~0U;
  int32_t proto = NETLINK_ROUTE;
  netlink_t* nl = netlink_open(groups, proto);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, dump_msg, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

