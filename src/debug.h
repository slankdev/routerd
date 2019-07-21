#ifndef _DEBUG_HH_
#define _DEBUG_HH_

#include <stdio.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum {
  NETLINK = 0,
  VPP,
  CLI,
  debug_t_max,
} debug_t;

const char* const debug_name[] = {
  [NETLINK] = "netlink",
  [VPP] = "vpp",
  [CLI] = "cli",
  [debug_t_max] = NULL,
};

typedef struct vui_s vui_t;
bool debug_enabled(debug_t debug);
void set_debug(debug_t debug, bool is_on);
void setup_debug_node(vui_t *vui);

#ifdef __cplusplus
}
#endif
#endif /* _DEBUG_HH_ */
