
#include "vui.h"
#include "lib/command.h"
#include "lib/zebra.h"

#include <stdio.h>
#include <string.h>
#include <stdint.h>
#include <stddef.h>
#include <stdlib.h>

struct thread_master *master = NULL;

vui_t *vui_new(void)
{
  vui_t *vui = malloc(sizeof(vui_t));
  memset(vui, 0, sizeof(vui_t));
  master = thread_master_create(NULL);
  cmd_init();
  vty_init(master, false);
  return vui;
}

void vui_delete(vui_t *vui)
{
  free(vui);
}

int vui_serv_stdio(vui_t *vui)
{
  (void*)(vui);
  vty_stdio(NULL);
  return 0;
}

int vui_serv_sock(vui_t *vui, const char *addr, uint16_t port)
{
  (void*)(vui);
  vty_serv_sock(NULL, port, "/var/run/frr/slank.vty");
  return 0;
}

int vui_set_password(vui_t *vui, const char* password)
{
  (void*)(vui);
  cmd_password_set(password);
  return 0;
}

int vui_set_hostname(vui_t *vui, const char* hostname)
{
  (void*)(vui);
  cmd_hostname_set(hostname);
  return 0;
}

void vui_install_node(vui_t *vui, struct cmd_node *node)
{
  (void*)(vui);
  install_node(node, NULL);
}

void vui_install_element(vui_t *vui,
    enum node_type ntype, struct cmd_element *cmd)
{
  (void*)(vui);
  install_element(ntype, cmd);
}

void vui_run(void)
{
  struct thread thread;
  while (thread_fetch(master, &thread))
    thread_call(&thread);
}

int vui_alloc_new_node_id(vui_t *vui, const char *name, int parent)
{
  return alloc_new_node_id(name, parent);
}

vui_node_t *vui_node_new(void)
{
  vui_node_t *node = malloc(sizeof(vui_node_t));
  memset(node, 0, sizeof(vui_node_t));
  node->impl = malloc(sizeof(struct cmd_node));
  memset(node->impl, 0, sizeof(struct cmd_node));
  return node;
}

void vui_node_delete(vui_node_t *node)
{
  free(node->impl);
  free(node);
}

void vui_node_install(vui_t *vui, vui_node_t *node)
{
  node->node = alloc_new_node_id(node->name, node->parent);
  node->impl->node = node->node;
  node->impl->prompt = node->prompt;
  node->impl->vtysh = 1;
  install_node(node->impl, NULL);
}

void vui_install_default_element(vui_t *vui, int node)
{
  install_default(node);
}
