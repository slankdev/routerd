#ifndef _DEBUG_HH_
#define _DEBUG_HH_

#include <stdio.h>
#include <stdbool.h>

typedef enum {
  NETLINK = 0,
  VPP,
  debug_t_max,
} debug_t;

const char* const debug_name[] = {
  [NETLINK] = "netlink",
  [VPP] = "vpp",
  [debug_t_max] = NULL,
};

typedef struct vui_s vui_t;
bool debug_enabled(debug_t debug);
void set_debug(debug_t debug, bool is_on);
void setup_debug_node(vui_t *vui);

#endif /* _DEBUG_HH_ */
