
#include "command_node.h"

size_t NODE_TYPE_MAX = _NODE_TYPE_MAX;
extern vector cmdvec;

const char *node_names[NODE_TYPE_LIMIT] = {
  [AUTH_NODE            ] = "auth"              , // AUTH_NODE
  [VIEW_NODE            ] = "view"              , // VIEW_NODE
  [AUTH_ENABLE_NODE     ] = "auth enable"       , // AUTH_ENABLE_NODE
  [ENABLE_NODE          ] = "enable"            , // ENABLE_NODE
  [CONFIG_NODE          ] = "config+"            , // CONFIG_NODE
  [DEBUG_NODE           ] = "debug"             , // DEBUG_NODE
  [VTY_NODE             ] = "vty"               , // VTY_NODE
};

int _node_parent[NODE_TYPE_LIMIT] = {
  [AUTH_NODE            ] = -1,
  [VIEW_NODE            ] = -1,
  [AUTH_ENABLE_NODE     ] = -1,
  [ENABLE_NODE          ] = -1,
  [CONFIG_NODE          ] = -1,
  [DEBUG_NODE           ] = CONFIG_NODE,
  [VTY_NODE             ] = CONFIG_NODE,
};

int node_parent(int node)
{
  /* assert(node > CONFIG_NODE); */
  return _node_parent[node];
}

static unsigned int
cmd_hash_key(const void *p)
{
  int size = sizeof(p);
  return jhash(p, size, 0);
}

static bool
cmd_hash_cmp(const void *a, const void *b)
{
  return a == b;
}

/*
 * Install top node of command vector.
 */
void install_node(struct cmd_node *node, int (*func)(struct vty *))
{
  vector_set_index(cmdvec, node->node, node);
  node->func = func;
  node->cmdgraph = graph_new();
  node->cmd_vector = vector_init(VECTOR_MIN_SIZE);
  // add start node
  struct cmd_token *token =
    cmd_token_new(START_TKN, CMD_ATTR_NORMAL, NULL, NULL);
  graph_new_node(node->cmdgraph, token,
           (void (*)(void *)) & cmd_token_del);
  node->cmd_hash = hash_create_size(16, cmd_hash_key, cmd_hash_cmp,
            "Command Hash");
}

int alloc_new_node_id(const char* name, int parent)
{
  assert(NODE_TYPE_MAX < NODE_TYPE_LIMIT);
  int next = NODE_TYPE_MAX++;
  node_names[next] = strdup(name);
  _node_parent[next] = parent;
  return next;
}
