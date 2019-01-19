
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <string>
#include <algorithm>
#include <arpa/inet.h>
#include "yalin/yalin.h"

inline static int
routerd_monitor(const struct sockaddr_nl *who,
         struct rtnl_ctrl_data* _dum_,
         struct nlmsghdr *n, void *arg)
{
  // TODO KOKOKARA
  printf("nlpacket\n");
  return 0;
}

int
main(int argc, char **argv)
{
  uint32_t groups = ~0U;
  netlink_t* nl = netlink_open(groups, NETLINK_ROUTE);
  if (nl == NULL)
    return 1;

  int ret = netlink_listen(nl, routerd_monitor, NULL);
  if (ret < 0)
    return 1;

  netlink_close(nl);
}

