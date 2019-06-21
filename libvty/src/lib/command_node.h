#ifndef _COMMAND_NODE_H
#define _COMMAND_NODE_H

#include "vector.h"
#include "vty.h"
#include "graph.h"
#include "memory.h"
#include "hash.h"
#include "command_graph.h"
#include "command_node.h"

#ifdef __cplusplus
extern "C" {
#endif

/*
 * List of CLI nodes.
 * Please remember to update the
 * name array in command.c.
 */
enum node_type {
  AUTH_NODE,              /* Authentication mode of vty interface. */
  VIEW_NODE,              /* View node. Default mode of vty interface. */
  AUTH_ENABLE_NODE,       /* Authentication mode for change enable. */
  ENABLE_NODE,            /* Enable node. */
  CONFIG_NODE,            /* Config node. Default mode of config file. */
  DEBUG_NODE,             /* Debug node. */
  VTY_NODE,               // VTY_NODE
  NETLINK_NODE,           // netlink
  NODE_TYPE_MAX,          /* maximum */
};

extern const char *node_names[];

/*
 * Node which has some commands and
 * prompt string and configuration
 * function pointer .
 */
struct cmd_node {
  enum node_type node;       /* Node index. */
  const char *prompt;        /* Prompt character at vty interface. */
  int vtysh;                 /* Is this node's configuration goes to vtysh ? */
  int (*func)(struct vty *); /* Node's configuration write function */
  struct graph *cmdgraph;    /* Node's command graph */
  vector cmd_vector;         /* Vector of this node's command list. */
  struct hash *cmd_hash;     /* Hashed index of command node list, for de-dupping primarily */
};

#ifdef __cplusplus
}
#endif
#endif /* _COMMAND_NODE_H */
