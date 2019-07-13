
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>
#include "routerd.h"

extern "C" {

void
routerd_context_add_interface(uint32_t kernl_index, uint32_t vpp_index)
{
  rd_ctx.add_interface(kernl_index, vpp_index);
}

} /* extern "C" */
