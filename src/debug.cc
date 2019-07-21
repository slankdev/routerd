
#include "debug.h"
#include <vui/vui.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <stdint.h>
#include <stddef.h>

static uint64_t debug_bitmap = 0;

bool debug_enabled(debug_t debug)
{
  assert(debug < debug_t_max);
  return debug_bitmap & (0x01 << debug);
}

void set_debug(debug_t debug, bool is_on)
{
  assert(debug < debug_t_max);
  if (is_on)
    debug_bitmap |= 0x01 << debug;
  else
    debug_bitmap &= ~(0x01 << debug);
}

DEFUN (show_debug,
       show_debug_cmd,
       "show debug",
       SHOW_STR
       "Show debug information\n")
{
  vty_out(vty, "\ndebug_bitmap: 0x%08llx\n", debug_bitmap);
  for (size_t i=0; i<debug_t_max; i++) {
    const char* name = debug_name[i];
    if (!name)
      continue;
    vty_out(vty, " %-10s: %s\n", name,
        debug_enabled(debug_t(i)) ? "true" : "false");
  }
  vty_out(vty, "\n");
}

DEFUN (debug,
       debug_cmd,
       "[no] debug <netlink|vpp|cli>",
       "Negate information\n"
       "Configure debug information\n"
       "Set netlink debug options\n"
       "Set vpp debug options\n"
       "Set cli debug options\n")
{
  const bool is_negate = strcmp(argv[0]->arg, "no") == 0;
  const char *name = argv[is_negate ? 2 : 1]->arg;
  for (size_t i=0; i<debug_t_max; i++) {
    assert(debug_name[i] != NULL);
    if (strcmp(debug_name[i], name) != 0)
      continue;
    set_debug(debug_t(i), !is_negate);
    return CMD_SUCCESS;
  }
  vty_out(vty, "invalid debug option\n");
  return CMD_WARNING_CONFIG_FAILED;
}

void
setup_debug_node(vui_t *vui)
{
  vui_install_element(vui, ENABLE_NODE, &show_debug_cmd);
  vui_install_element(vui, ENABLE_NODE, &debug_cmd);
  vui_install_element(vui, CONFIG_NODE, &debug_cmd);
}
