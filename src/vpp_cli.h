#ifndef _VPP_CLI_H_
#define _VPP_CLI_H_

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdint.h>
#include <stdarg.h>

#ifdef hash_create
#undef hash_create
#endif
#ifdef hash_get
#undef hash_get
#endif
#ifdef hash_free
#undef hash_free
#endif
#include <vui/vui.h>

#ifdef __cplusplus
extern "C" {
#endif

void setup_vpp_node(vui_t *vui);

#ifdef __cplusplus
}
#endif
#endif /* _VPP_CLI_H_ */
