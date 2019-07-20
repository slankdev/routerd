#ifndef _NETLINK_CLI_H_
#define _NETLINK_CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>
#include <vui/vui.h>

#ifdef __cplusplus
extern "C" {
#endif

void setup_netlink_node(vui_t *vui);

#ifdef __cplusplus
}
#endif
#endif /* _NETLINK_CLI_H_ */
