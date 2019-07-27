
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "routerd.h"

extern "C" {

void routerd_context_add_interface(
    uint32_t kernl_index, const char *kern_name,
    uint32_t vpp_index, const char *vpp_name)
{
  rd_ctx.add_interface(
      kernl_index, kern_name,
      vpp_index, vpp_name);
}

} /* extern "C" */
