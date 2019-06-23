#ifndef _VUI_H_
#define _VUI_H_

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>
#include "command.h"
#include "zebra.h"

typedef struct vui_s {
} vui_t;

typedef struct vui_node_s {
  int node;
  int parent;
  const char *name;
  const char *prompt;
  struct cmd_node impl;
} vui_node_t;

vui_t *vui_new(void);
vui_node_t *vui_node_new(void);
void vui_delete(vui_t *vui);
int vui_serv_stdio(vui_t *vui);
int vui_serv_sock(vui_t *vui, const char *addr, uint16_t port);
int vui_set_password(vui_t *vui, const char* password);
int vui_set_hostname(vui_t *vui, const char* hostname);
void vui_install_node(vui_t *vui, struct cmd_node *node);
void vui_install_element(vui_t *vui,
    enum node_type ntype, struct cmd_element *cmd);
void vui_run(void);
int vui_alloc_new_node_id(vui_t *vui, const char *name, int parent);
void vui_install_default_element(vui_t *vui, int node);
void vui_node_install(vui_t *vui, vui_node_t *node);

#endif /* _VUI_H_ */
