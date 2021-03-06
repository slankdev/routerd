/*
 * CLI backend interface.
 *
 * --
 * Copyright (C) 2019 Hiroki Shirokura / slankdev
 * Copyright (C) 2016 Cumulus Networks, Inc.
 * Copyright (C) 1997, 98, 99 Kunihiro Ishiguro
 * Copyright (C) 2013 by Open Source Routing.
 * Copyright (C) 2013 by Internet Systems Consortium, Inc. ("ISC")
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * GNU Zebra is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <zebra.h>
#include <lib/version.h>

#include "command.h"
#include "frrstr.h"
#include "memory.h"
#include "vector.h"
#include "linklist.h"
#include "vty.h"
#include "command_match.h"
#include "command_graph.h"
#include "qobj.h"
#include "hash.h"

#define debug_printf(fmt, ...) \
  if (false) printf(fmt, __VA_ARGS__)

#define DFLT_NAME "traditional"
#ifdef HAVE_DATACENTER
#define DFLT_BGP_IMPORT_CHECK      1
#define DFLT_BGP_TIMERS_CONNECT      10
#define DFLT_BGP_HOLDTIME      9
#define DFLT_BGP_KEEPALIVE      3
#define DFLT_BGP_LOG_NEIGHBOR_CHANGES    1
#define DFLT_BGP_SHOW_HOSTNAME      1
#define DFLT_BGP_DETERMINISTIC_MED    1
#define DFLT_OSPF_LOG_ADJACENCY_CHANGES    1
#define DFLT_OSPF6_LOG_ADJACENCY_CHANGES  1
#else  /* !HAVE_DATACENTER */
#define DFLT_BGP_IMPORT_CHECK      0
#define DFLT_BGP_TIMERS_CONNECT      120
#define DFLT_BGP_HOLDTIME      180
#define DFLT_BGP_KEEPALIVE      60
#define DFLT_BGP_LOG_NEIGHBOR_CHANGES    0
#define DFLT_BGP_SHOW_HOSTNAME      0
#define DFLT_BGP_DETERMINISTIC_MED    0
#define DFLT_OSPF_LOG_ADJACENCY_CHANGES    0
#define DFLT_OSPF6_LOG_ADJACENCY_CHANGES  0
#endif /* !HAVE_DATACENTER */

DEFINE_MTYPE(LIB, HOST, "Host config")
DEFINE_MTYPE(LIB, COMPLETION, "Completion item")

#define item(x) { x, #x }

/*
 * Command vector which includes some level
 * of command lists. Normally each daemon
 * maintains each own cmdvec.
 */
vector cmdvec = NULL;

struct host host; /* Host information structure. */

const char *cmd_hostname_get(void)
{ return host.name; }
int cmd_password_set(const char *password)
{
  host.password = strdup(password);
  return CMD_SUCCESS;
}
int cmd_hostname_set(const char *hostname)
{
  host.name = strdup(hostname);
  return CMD_SUCCESS;
}

/* Standard command node structures. */
static struct cmd_node auth_node = { AUTH_NODE, "Password: ", };
static struct cmd_node view_node = { VIEW_NODE, "%s> ", };
static struct cmd_node auth_enable_node = { AUTH_ENABLE_NODE, "Password: ", };
static struct cmd_node enable_node = { ENABLE_NODE, "%s# ", };
static struct cmd_node config_node = {CONFIG_NODE, "%s(config)# ", 1};

/* Default motd string. */
static const char *default_motd = FRR_DEFAULT_MOTD;

static const struct facility_map {
  int facility;
  const char *name;
  size_t match;
} syslog_facilities[] = {
  {LOG_KERN, "kern", 1},
  {LOG_USER, "user", 2},
  {LOG_MAIL, "mail", 1},
  {LOG_DAEMON, "daemon", 1},
  {LOG_AUTH, "auth", 1},
  {LOG_SYSLOG, "syslog", 1},
  {LOG_LPR, "lpr", 2},
  {LOG_NEWS, "news", 1},
  {LOG_UUCP, "uucp", 2},
  {LOG_CRON, "cron", 1},
#ifdef LOG_FTP
  {LOG_FTP, "ftp", 1},
#endif
  {LOG_LOCAL0, "local0", 6},
  {LOG_LOCAL1, "local1", 6},
  {LOG_LOCAL2, "local2", 6},
  {LOG_LOCAL3, "local3", 6},
  {LOG_LOCAL4, "local4", 6},
  {LOG_LOCAL5, "local5", 6},
  {LOG_LOCAL6, "local6", 6},
  {LOG_LOCAL7, "local7", 6},
  {0, NULL, 0},
};

static const char *facility_name(int facility)
{
  for (const struct facility_map *fm =
       syslog_facilities;
       fm->name; fm++)
    if (fm->facility == facility)
      return fm->name;
  return "";
}

static int facility_match(const char *str)
{
  const struct facility_map *fm;

  for (fm = syslog_facilities; fm->name; fm++)
    if (!strncmp(str, fm->name, fm->match))
      return fm->facility;
  return -1;
}

/* This is called from main when a daemon is invoked with -v or --version. */
void print_version(const char *progname)
{
  printf("%s version %s\n", progname, FRR_VERSION);
  printf("%s\n", FRR_COPYRIGHT);
}

char *argv_concat(struct cmd_token **argv, int argc, int shift)
{
  int cnt = MAX(argc - shift, 0);
  const char *argstr[cnt + 1];

  if (!cnt)
    return NULL;

  for (int i = 0; i < cnt; i++)
    argstr[i] = argv[i + shift]->arg;

  return frrstr_join(argstr, cnt, " ");
}

vector cmd_make_strvec(const char *string)
{
  if (!string)
    return NULL;

  const char *copy = string;

  /* skip leading whitespace */
  while (isspace((int)*copy) && *copy != '\0')
    copy++;

  /* if the entire string was whitespace or a comment, return */
  if (*copy == '\0' || *copy == '!' || *copy == '#')
    return NULL;

  vector result = frrstr_split_vec(copy, "\n\r\t ");

  for (unsigned int i = 0; i < vector_active(result); i++) {
    if (strlen(vector_slot(result, i)) == 0) {
      XFREE(MTYPE_TMP, vector_slot(result, i));
      vector_unset(result, i);
    }
  }

  vector_compact(result);

  return result;
}

void cmd_free_strvec(vector v)
{
  frrstr_strvec_free(v);
}

/**
 * Convenience function for accessing argv data.
 *
 * @param argc
 * @param argv
 * @param text definition snippet of the desired token
 * @param index the starting index, and where to store the
 *        index of the found token if it exists
 * @return 1 if found, 0 otherwise
 */
int argv_find(struct cmd_token **argv, int argc, const char *text, int *index)
{
  int found = 0;
  for (int i = *index; i < argc && found == 0; i++)
    if ((found = strmatch(text, argv[i]->text)))
      *index = i;
  return found;
}

/* Return prompt character of specified node. */
const char *cmd_prompt(enum node_type node)
{
  struct cmd_node *cnode;
  cnode = vector_slot(cmdvec, node);
  return cnode->prompt;
}

/* Install a command into a node. */
void install_element(enum node_type ntype, struct cmd_element *cmd)
{
  struct cmd_node *cnode;

  /* cmd_init hasn't been called */
  if (!cmdvec) {
    fprintf(stderr, "%s called before cmd_init, breakage likely\n",
      __func__);
    return;
  }

  cnode = vector_lookup(cmdvec, ntype);

  if (cnode == NULL) {
    fprintf(stderr,
      "%s[%s]:\n"
      "\tnode %d (%s) does not exist.\n"
      "\tplease call install_node() before install_element()\n",
      cmd->name, cmd->string, ntype, node_names[ntype]);
    exit(EXIT_FAILURE);
  }

  if (hash_lookup(cnode->cmd_hash, cmd) != NULL) {
    fprintf(stderr,
      "%s[%s]:\n"
      "\tnode %d (%s) already has this command installed.\n"
      "\tduplicate install_element call?\n",
      cmd->name, cmd->string, ntype, node_names[ntype]);
    return;
  }

  assert(hash_get(cnode->cmd_hash, cmd, hash_alloc_intern));

  struct graph *graph = graph_new();
  struct cmd_token *token =
    cmd_token_new(START_TKN, CMD_ATTR_NORMAL, NULL, NULL);
  graph_new_node(graph, token, (void (*)(void *)) & cmd_token_del);

  cmd_graph_parse(graph, cmd);
  cmd_graph_names(graph);
  cmd_graph_merge(cnode->cmdgraph, graph, +1);
  graph_delete_graph(graph);

  vector_set(cnode->cmd_vector, cmd);

  if (ntype == VIEW_NODE)
    install_element(ENABLE_NODE, cmd);
}

void uninstall_element(enum node_type ntype, struct cmd_element *cmd)
{
  struct cmd_node *cnode;

  /* cmd_init hasn't been called */
  if (!cmdvec) {
    fprintf(stderr, "%s called before cmd_init, breakage likely\n",
      __func__);
    return;
  }

  cnode = vector_lookup(cmdvec, ntype);

  if (cnode == NULL) {
    fprintf(stderr,
      "%s[%s]:\n"
      "\tnode %d (%s) does not exist.\n"
      "\tplease call install_node() before uninstall_element()\n",
      cmd->name, cmd->string, ntype, node_names[ntype]);
    exit(EXIT_FAILURE);
  }

  if (hash_release(cnode->cmd_hash, cmd) == NULL) {
    fprintf(stderr,
      "%s[%s]:\n"
      "\tnode %d (%s) does not have this command installed.\n"
      "\tduplicate uninstall_element call?\n",
      cmd->name, cmd->string, ntype, node_names[ntype]);
    return;
  }

  vector_unset_value(cnode->cmd_vector, cmd);

  struct graph *graph = graph_new();
  struct cmd_token *token =
    cmd_token_new(START_TKN, CMD_ATTR_NORMAL, NULL, NULL);
  graph_new_node(graph, token, (void (*)(void *)) & cmd_token_del);

  cmd_graph_parse(graph, cmd);
  cmd_graph_names(graph);
  cmd_graph_merge(cnode->cmdgraph, graph, -1);
  graph_delete_graph(graph);

  if (ntype == VIEW_NODE)
    uninstall_element(ENABLE_NODE, cmd);
}


static const unsigned char itoa64[] =
  "./0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz";

static void to64(char *s, long v, int n)
{
  while (--n >= 0) {
    *s++ = itoa64[v & 0x3f];
    v >>= 6;
  }
}

static char *zencrypt(const char *passwd)
{
  char salt[6];
  struct timeval tv;
  char *crypt(const char *, const char *);

  gettimeofday(&tv, 0);

  to64(&salt[0], random(), 3);
  to64(&salt[3], tv.tv_usec, 3);
  salt[5] = '\0';

  return crypt(passwd, salt);
}

/* Utility function for getting command graph. */
static struct graph *cmd_node_graph(vector v, enum node_type ntype)
{
  struct cmd_node *cnode = vector_slot(v, ntype);
  return cnode->cmdgraph;
}

static int cmd_try_do_shortcut(enum node_type node, char *first_word)
{
  if (first_word != NULL && node != AUTH_NODE && node != VIEW_NODE
      && node != AUTH_ENABLE_NODE && 0 == strcmp("do", first_word))
    return 1;
  return 0;
}

/**
 * Compare function for cmd_token.
 * Used with qsort to sort command completions.
 */
static int compare_completions(const void *fst, const void *snd)
{
  const struct cmd_token *first = *(const struct cmd_token * const *)fst,
             *secnd = *(const struct cmd_token * const *)snd;
  return strcmp(first->text, secnd->text);
}

/**
 * Takes a list of completions returned by command_complete,
 * dedeuplicates them based on both text and description,
 * sorts them, and returns them as a vector.
 *
 * @param completions linked list of cmd_token
 * @return deduplicated and sorted vector with
 */
vector completions_to_vec(struct list *completions)
{
  vector comps = vector_init(VECTOR_MIN_SIZE);

  struct listnode *ln;
  struct cmd_token *token, *cr = NULL;
  unsigned int i, exists;
  for (ALL_LIST_ELEMENTS_RO(completions, ln, token)) {
    if (token->type == END_TKN && (cr = token))
      continue;

    // linear search for token in completions vector
    exists = 0;
    for (i = 0; i < vector_active(comps) && !exists; i++) {
      struct cmd_token *curr = vector_slot(comps, i);
#ifdef VTYSH_DEBUG
      exists = !strcmp(curr->text, token->text)
         && !strcmp(curr->desc, token->desc);
#else
      exists = !strcmp(curr->text, token->text);
#endif /* VTYSH_DEBUG */
    }

    if (!exists)
      vector_set(comps, token);
  }

  // sort completions
  qsort(comps->index, vector_active(comps), sizeof(void *),
        &compare_completions);

  // make <cr> the first element, if it is present
  if (cr) {
    vector_set_index(comps, vector_active(comps), NULL);
    memmove(comps->index + 1, comps->index,
      (comps->alloced - 1) * sizeof(void *));
    vector_set_index(comps, 0, cr);
  }

  return comps;
}
/**
 * Generates a vector of cmd_token representing possible completions
 * on the current input.
 *
 * @param vline the vectorized input line
 * @param vty the vty with the node to match on
 * @param status pointer to matcher status code
 * @return vector of struct cmd_token * with possible completions
 */
static vector cmd_complete_command_real(vector vline, struct vty *vty,
          int *status)
{
  struct list *completions;
  struct graph *cmdgraph = cmd_node_graph(cmdvec, vty->node);

  enum matcher_rv rv = command_complete(cmdgraph, vline, &completions);

  if (MATCHER_ERROR(rv)) {
    *status = CMD_ERR_NO_MATCH;
    return NULL;
  }

  vector comps = completions_to_vec(completions);
  list_delete(&completions);

  // set status code appropriately
  switch (vector_active(comps)) {
  case 0:
    *status = CMD_ERR_NO_MATCH;
    break;
  case 1:
    *status = CMD_COMPLETE_FULL_MATCH;
    break;
  default:
    *status = CMD_COMPLETE_LIST_MATCH;
  }

  return comps;
}

vector cmd_describe_command(vector vline, struct vty *vty, int *status)
{
  vector ret;

  if (cmd_try_do_shortcut(vty->node, vector_slot(vline, 0))) {
    enum node_type onode;
    int orig_xpath_index;
    vector shifted_vline;
    unsigned int index;

    onode = vty->node;
    orig_xpath_index = vty->xpath_index;
    vty->node = ENABLE_NODE;
    vty->xpath_index = 0;
    /* We can try it on enable node, cos' the vty is authenticated
     */

    shifted_vline = vector_init(vector_count(vline));
    /* use memcpy? */
    for (index = 1; index < vector_active(vline); index++) {
      vector_set_index(shifted_vline, index - 1,
           vector_lookup(vline, index));
    }

    ret = cmd_complete_command_real(shifted_vline, vty, status);

    vector_free(shifted_vline);
    vty->node = onode;
    vty->xpath_index = orig_xpath_index;
    return ret;
  }

  return cmd_complete_command_real(vline, vty, status);
}

static struct list *varhandlers = NULL;

void cmd_variable_complete(struct cmd_token *token, const char *arg,
         vector comps)
{
  struct listnode *ln;
  const struct cmd_variable_handler *cvh;
  size_t i, argsz;
  vector tmpcomps;

  tmpcomps = arg ? vector_init(VECTOR_MIN_SIZE) : comps;

  for (ALL_LIST_ELEMENTS_RO(varhandlers, ln, cvh)) {
    if (cvh->tokenname && strcmp(cvh->tokenname, token->text))
      continue;
    if (cvh->varname && (!token->varname
             || strcmp(cvh->varname, token->varname)))
      continue;
    cvh->completions(tmpcomps, token);
    break;
  }

  if (!arg)
    return;

  argsz = strlen(arg);
  for (i = vector_active(tmpcomps); i; i--) {
    char *item = vector_slot(tmpcomps, i - 1);
    if (strlen(item) >= argsz && !strncmp(item, arg, argsz))
      vector_set(comps, item);
    else
      XFREE(MTYPE_COMPLETION, item);
  }
  vector_free(tmpcomps);
}

char *cmd_variable_comp2str(vector comps, unsigned short cols)
{
  size_t bsz = 16;
  char *buf = XCALLOC(MTYPE_TMP, bsz);
  const int autocomp_indent = 5;
  int lc = autocomp_indent;
  size_t cs = autocomp_indent;
  size_t itemlen;
  snprintf(buf, bsz, "%*s", autocomp_indent, "");
  for (size_t j = 0; j < vector_active(comps); j++) {
    char *item = vector_slot(comps, j);
    itemlen = strlen(item);

    if (cs + itemlen + autocomp_indent + 3 >= bsz)
      buf = XREALLOC(MTYPE_TMP, buf, (bsz *= 2));

    if (lc + itemlen + 1 >= cols) {
      cs += snprintf(&buf[cs], bsz - cs, "\n%*s",
               autocomp_indent, "");
      lc = autocomp_indent;
    }

    size_t written = snprintf(&buf[cs], bsz - cs, "%s ", item);
    lc += written;
    cs += written;
    XFREE(MTYPE_COMPLETION, item);
    vector_set_index(comps, j, NULL);
  }
  return buf;
}

void cmd_variable_handler_register(const struct cmd_variable_handler *cvh)
{
  if (!varhandlers)
    return;

  for (; cvh->completions; cvh++)
    listnode_add(varhandlers, (void *)cvh);
}

DEFUN_HIDDEN (autocomplete,
              autocomplete_cmd,
              "autocomplete TYPE TEXT VARNAME",
              "Autocompletion handler (internal, for vtysh)\n"
              "cmd_token->type\n"
              "cmd_token->text\n"
              "cmd_token->varname\n")
{
  struct cmd_token tok;
  vector comps = vector_init(32);
  size_t i;

  memset(&tok, 0, sizeof(tok));
  tok.type = atoi(argv[1]->arg);
  tok.text = argv[2]->arg;
  tok.varname = argv[3]->arg;
  if (!strcmp(tok.varname, "-"))
    tok.varname = NULL;

  cmd_variable_complete(&tok, NULL, comps);

  for (i = 0; i < vector_active(comps); i++) {
    char *text = vector_slot(comps, i);
    vty_out(vty, "%s\n", text);
    XFREE(MTYPE_COMPLETION, text);
  }

  vector_free(comps);
  return CMD_SUCCESS;
}

/**
 * Generate possible tab-completions for the given input. This function only
 * returns results that would result in a valid command if used as Readline
 * completions (as is the case in vtysh). For instance, if the passed vline ends
 * with '4.3.2', the strings 'A.B.C.D' and 'A.B.C.D/M' will _not_ be returned.
 *
 * @param vline vectorized input line
 * @param vty the vty
 * @param status location to store matcher status code in
 * @return set of valid strings for use with Readline as tab-completions.
 */

char **cmd_complete_command(vector vline, struct vty *vty, int *status)
{
  char **ret = NULL;
  int original_node = vty->node;
  vector input_line = vector_init(vector_count(vline));

  // if the first token is 'do' we'll want to execute the command in the
  // enable node
  int do_shortcut = cmd_try_do_shortcut(vty->node, vector_slot(vline, 0));
  vty->node = do_shortcut ? ENABLE_NODE : original_node;

  // construct the input line we'll be matching on
  unsigned int offset = (do_shortcut) ? 1 : 0;
  for (unsigned index = 0; index + offset < vector_active(vline); index++)
    vector_set_index(input_line, index,
         vector_lookup(vline, index + offset));

  // get token completions -- this is a copying operation
  vector comps = NULL, initial_comps;
  initial_comps = cmd_complete_command_real(input_line, vty, status);

  if (!MATCHER_ERROR(*status)) {
    assert(initial_comps);
    // filter out everything that is not suitable for a
    // tab-completion
    comps = vector_init(VECTOR_MIN_SIZE);
    for (unsigned int i = 0; i < vector_active(initial_comps);
         i++) {
      struct cmd_token *token = vector_slot(initial_comps, i);
      if (token->type == WORD_TKN)
        vector_set(comps, XSTRDUP(MTYPE_COMPLETION,
                token->text));
      else if (IS_VARYING_TOKEN(token->type)) {
        const char *ref = vector_lookup(
          vline, vector_active(vline) - 1);
        cmd_variable_complete(token, ref, comps);
      }
    }
    vector_free(initial_comps);

    // since we filtered results, we need to re-set status code
    switch (vector_active(comps)) {
    case 0:
      *status = CMD_ERR_NO_MATCH;
      break;
    case 1:
      *status = CMD_COMPLETE_FULL_MATCH;
      break;
    default:
      *status = CMD_COMPLETE_LIST_MATCH;
    }

    // copy completions text into an array of char*
    ret = XMALLOC(MTYPE_TMP,
            (vector_active(comps) + 1) * sizeof(char *));
    unsigned int i;
    for (i = 0; i < vector_active(comps); i++) {
      ret[i] = vector_slot(comps, i);
    }
    // set the last element to NULL, because this array is used in
    // a Readline completion_generator function which expects NULL
    // as a sentinel value
    ret[i] = NULL;
    vector_free(comps);
    comps = NULL;
  } else if (initial_comps)
    vector_free(initial_comps);

  // comps should always be null here
  assert(!comps);

  // free the adjusted input line
  vector_free(input_line);

  // reset vty->node to its original value
  vty->node = original_node;

  return ret;
}

/* Execute command by argument vline vector. */
static int cmd_execute_command_real(vector vline, enum cmd_filter_type filter,
            struct vty *vty,
            const struct cmd_element **cmd)
{
  debug_printf("%s: (1): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
  struct list *argv_list;
  enum matcher_rv status;
  const struct cmd_element *matched_element = NULL;

  struct graph *cmdgraph = cmd_node_graph(cmdvec, vty->node);
  status = command_match(cmdgraph, vline, &argv_list, &matched_element);
  debug_printf("%s: (2): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);

  if (cmd)
    *cmd = matched_element;

  // if matcher error, return corresponding CMD_ERR
  if (MATCHER_ERROR(status)) {
    if (argv_list)
      list_delete(&argv_list);
    switch (status) {
    case MATCHER_INCOMPLETE:
      return CMD_ERR_INCOMPLETE;
    case MATCHER_AMBIGUOUS:
      return CMD_ERR_AMBIGUOUS;
    default:
      return CMD_ERR_NO_MATCH;
    }
  }

  // build argv array from argv list
  struct cmd_token **argv = XMALLOC(
    MTYPE_TMP, argv_list->count * sizeof(struct cmd_token *));
  struct listnode *ln;
  struct cmd_token *token;
  unsigned int i = 0;
  for (ALL_LIST_ELEMENTS_RO(argv_list, ln, token))
    argv[i++] = token;

  int argc = argv_list->count;

  int ret;
  if (matched_element->daemon)
    ret = CMD_SUCCESS_DAEMON;
  else {
    if (vty->config) {
      /* Clear array of enqueued configuration changes. */
      vty->num_cfg_changes = 0;
      memset(&vty->cfg_changes, 0, sizeof(vty->cfg_changes));
    }

    debug_printf("%s: (3): vty->node=%d\n\r", __func__, vty->node);
    ret = matched_element->func(matched_element, vty, argc, argv);
    debug_printf("%s: (4): vty->node=%d\n\r", __func__, vty->node);
  }

  // delete list and cmd_token's in it
  list_delete(&argv_list);
  XFREE(MTYPE_TMP, argv);

  return ret;
}

/**
 * Execute a given command, handling things like "do ..." and checking
 * whether the given command might apply at a parent node if doesn't
 * apply for the current node.
 *
 * @param vline Command line input, vector of char* where each element is
 *              one input token.
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element of the matched command
 *            will be stored, if any. May be set to NULL if this info is
 *            not needed.
 * @param vtysh If set != 0, don't lookup the command at parent nodes.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int cmd_execute_command(vector vline, struct vty *vty,
      const struct cmd_element **cmd, int vtysh)
{
  debug_printf("%s: (1): vty->node=%d\n\r", __func__, vty->node);
  int ret, saved_ret = 0;
  enum node_type onode, try_node;
  int orig_xpath_index;

  onode = try_node = vty->node;
  orig_xpath_index = vty->xpath_index;

  if (cmd_try_do_shortcut(vty->node, vector_slot(vline, 0))) {
    vector shifted_vline;
    unsigned int index;

    vty->node = ENABLE_NODE;
    vty->xpath_index = 0;
    /* We can try it on enable node, cos' the vty is authenticated
     */

    shifted_vline = vector_init(vector_count(vline));
    /* use memcpy? */
    for (index = 1; index < vector_active(vline); index++)
      vector_set_index(shifted_vline, index - 1,
           vector_lookup(vline, index));

    debug_printf("%s: (3:1): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
    ret = cmd_execute_command_real(shifted_vline, FILTER_RELAXED, vty, cmd);
    debug_printf("%s: (3:2): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);

    vector_free(shifted_vline);
    vty->node = onode;
    vty->xpath_index = orig_xpath_index;
    return ret;
  }

  debug_printf("%s: (4:1): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
  saved_ret = ret =
    cmd_execute_command_real(vline, FILTER_RELAXED, vty, cmd);
  debug_printf("%s: (4:2): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);

  if (vtysh)
    return saved_ret;

  if (ret != CMD_SUCCESS && ret != CMD_WARNING
      && ret != CMD_NOT_MY_INSTANCE && ret != CMD_WARNING_CONFIG_FAILED) {
    /* This assumes all nodes above CONFIG_NODE are childs of
     * CONFIG_NODE */
    while (vty->node > CONFIG_NODE) {
      try_node = node_parent(try_node);
      vty->node = try_node;
      if (vty->xpath_index > 0)
        vty->xpath_index--;
      debug_printf("%s: (5:1): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
      ret = cmd_execute_command_real(vline, FILTER_RELAXED, vty, cmd);
      debug_printf("%s: (5:2): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
      if (ret == CMD_SUCCESS || ret == CMD_WARNING
          || ret == CMD_NOT_MY_INSTANCE
          || ret == CMD_WARNING_CONFIG_FAILED)
        return ret;
    }
    /* no command succeeded, reset the vty to the original node */
    vty->node = onode;
    vty->xpath_index = orig_xpath_index;
  }

  /* return command status for original node */
  return saved_ret;
}

/**
 * Execute a given command, matching it strictly against the current node.
 * This mode is used when reading config files.
 *
 * @param vline Command line input, vector of char* where each element is
 *              one input token.
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element* of the matched command
 *            will be stored, if any. May be set to NULL if this info is
 *            not needed.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int cmd_execute_command_strict(vector vline, struct vty *vty,
             const struct cmd_element **cmd)
{
  debug_printf("%s: (1:1): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
  return cmd_execute_command_real(vline, FILTER_STRICT, vty, cmd);
  debug_printf("%s: (1:2): vty=%p vty->node=%d\n\r", __func__, vty, vty->node);
}

/*
 * cmd_execute hook subscriber to handle `|` actions.
 */
static int handle_pipe_action(struct vty *vty, const char *cmd_in,
            char **cmd_out)
{
  /* look for `|` */
  char *orig, *working, *token, *u;
  char *pipe = strstr(cmd_in, "| ");

  if (!pipe)
    return 0;

  /* duplicate string for processing purposes, not including pipe */
  orig = working = XSTRDUP(MTYPE_TMP, pipe + 2);

  /* retrieve action */
  token = strsep(&working, " ");
  assert(token);

  /* match result to known actions */
  if (strmatch(token, "include")) {
    /* the remaining text should be a regexp */
    char *regexp = working;

    if (!regexp) {
      vty_out(vty, "%% Need a regexp to filter with\n");
      goto fail;
    }

    bool succ = vty_set_include(vty, regexp);

    if (!succ) {
      vty_out(vty, "%% Bad regexp '%s'\n", regexp);
      goto fail;
    }
    *cmd_out = XSTRDUP(MTYPE_TMP, cmd_in);
    u = *cmd_out;
    strsep(&u, "|");
  } else {
    vty_out(vty, "%% Unknown action '%s'\n", token);
    goto fail;
  }

fail:
  XFREE(MTYPE_TMP, orig);
  return 0;
}

static int handle_pipe_action_done(struct vty *vty, const char *cmd_exec)
{
  if (vty->filter)
    vty_set_include(vty, NULL);

  return 0;
}

int cmd_execute(struct vty *vty, const char *cmd,
    const struct cmd_element **matched, int vtysh)
{
  int ret;
  char *cmd_out = NULL;
  const char *cmd_exec;
  vector vline;

  cmd_exec = cmd_out ? (const char *)cmd_out : cmd;

  vline = cmd_make_strvec(cmd_exec);
  debug_printf("%s(1): cmd=\'%s\' vty=%p vty->node=%u\n\r", __func__, cmd, vty, vty->node);

  if (vline) {
    ret = cmd_execute_command(vline, vty, matched, vtysh);
    cmd_free_strvec(vline);
  } else {
    ret = CMD_SUCCESS;
  }

  XFREE(MTYPE_TMP, cmd_out);
  debug_printf("%s(2): cmd=\'%s\' vty=%p vty->node=%u\n\r", __func__, cmd, vty, vty->node);
  return ret;
}


/**
 * Parse one line of config, walking up the parse tree attempting to find a
 * match
 *
 * @param vty The vty context in which the command should be executed.
 * @param cmd Pointer where the struct cmd_element* of the match command
 *            will be stored, if any.  May be set to NULL if this info is
 *            not needed.
 * @param use_daemon Boolean to control whether or not we match on
 * CMD_SUCCESS_DAEMON
 *                   or not.
 * @return The status of the command that has been executed or an error code
 *         as to why no command could be executed.
 */
int command_config_read_one_line(struct vty *vty,
         const struct cmd_element **cmd,
         uint32_t line_num, int use_daemon)
{
  vector vline;
  int ret;

  vline = cmd_make_strvec(vty->buf);

  /* In case of comment line */
  if (vline == NULL)
    return CMD_SUCCESS;

  /* Execute configuration command : this is strict match */
  ret = cmd_execute_command_strict(vline, vty, cmd);

  // Climb the tree and try the command again at each node
  if (!(use_daemon && ret == CMD_SUCCESS_DAEMON)
      && !(!use_daemon && ret == CMD_ERR_NOTHING_TODO)
      && ret != CMD_SUCCESS && ret != CMD_WARNING
      && ret != CMD_NOT_MY_INSTANCE && ret != CMD_WARNING_CONFIG_FAILED
      && vty->node != CONFIG_NODE) {
    int saved_node = vty->node;
    int saved_xpath_index = vty->xpath_index;

    while (!(use_daemon && ret == CMD_SUCCESS_DAEMON)
           && !(!use_daemon && ret == CMD_ERR_NOTHING_TODO)
           && ret != CMD_SUCCESS && ret != CMD_WARNING
           && vty->node > CONFIG_NODE) {
      vty->node = node_parent(vty->node);
      if (vty->xpath_index > 0)
        vty->xpath_index--;
      ret = cmd_execute_command_strict(vline, vty, cmd);
    }

    // If climbing the tree did not work then ignore the command and
    // stay at the same node
    if (!(use_daemon && ret == CMD_SUCCESS_DAEMON)
        && !(!use_daemon && ret == CMD_ERR_NOTHING_TODO)
        && ret != CMD_SUCCESS && ret != CMD_WARNING) {
      vty->node = saved_node;
      vty->xpath_index = saved_xpath_index;
    }
  }

  if (ret != CMD_SUCCESS &&
      ret != CMD_WARNING &&
      ret != CMD_SUCCESS_DAEMON) {
    struct vty_error *ve = XCALLOC(MTYPE_TMP, sizeof(*ve));

    memcpy(ve->error_buf, vty->buf, VTY_BUFSIZ);
    ve->line_num = line_num;
    if (!vty->error)
      vty->error = list_new();

    listnode_add(vty->error, ve);
  }

  cmd_free_strvec(vline);

  return ret;
}

/* Configuration from terminal */
DEFUN (config_terminal,
       config_terminal_cmd,
       "configure [terminal]",
       "Configuration from vty interface\n"
       "Configuration terminal\n")
{
  return vty_config_enter(vty, false, false);
}

/* Enable command */
DEFUN (enable,
       config_enable_cmd,
       "enable",
       "Turn on privileged mode command\n")
{
  /* If enable password is NULL, change to ENABLE_NODE */
  if ((host.enable == NULL && host.enable_encrypt == NULL)
      || vty->type == VTY_SHELL_SERV)
    vty->node = ENABLE_NODE;
  else
    vty->node = AUTH_ENABLE_NODE;

  return CMD_SUCCESS;
}

/* Disable command */
DEFUN (disable,
       config_disable_cmd,
       "disable",
       "Turn off privileged mode command\n")
{
  if (vty->node == ENABLE_NODE)
    vty->node = VIEW_NODE;
  return CMD_SUCCESS;
}

/* Down vty node level. */
DEFUN (config_exit,
       config_exit_cmd,
       "exit",
       "Exit current mode and down to previous mode\n")
{
  cmd_exit(vty);
  return CMD_SUCCESS;
}

void cmd_exit(struct vty *vty)
{
  debug_printf("%s: (1): vty->node=%d\n\r", __func__, vty->node);
  switch (vty->node) {
  case VIEW_NODE:
  case ENABLE_NODE:
    if (vty_shell(vty))
      exit(0);
    else
      vty->status = VTY_CLOSE;
    break;
  case CONFIG_NODE:
    vty->node = ENABLE_NODE;
    vty_config_exit(vty);
    break;
  case VTY_NODE:
    vty->node = CONFIG_NODE;
    break;
  default:
    vty->node = node_parent(vty->node);
    break;
  }

  if (vty->xpath_index > 0)
    vty->xpath_index--;
}

/* ALIAS_FIXME */
DEFUN (config_quit,
       config_quit_cmd,
       "quit",
       "Exit current mode and down to previous mode\n")
{
  return config_exit(self, vty, argc, argv);
}


/* End of configuration. */
DEFUN (config_end,
       config_end_cmd,
       "end",
       "End current mode and change to enable mode.\n")
{
  if (vty->config) {
    vty_config_exit(vty);
    vty->node = ENABLE_NODE;
  }

  return CMD_SUCCESS;
}

/* Help display function for all node. */
DEFUN (config_help,
       config_help_cmd,
       "help",
       "I will tell you what a want to say\n")
{
  vty_out(vty,
    "I guess, you typed this command to find your helps, \n"
    "but this command will discribe you what I want to say.\n"
    "Software engineering is one of the most important thing \n"
    "for our company and world right?. \n"
    "But some people can't (or don't wanna) understand that philosophy.\n"
    "That's nonsence for creators to inovate something to new.\n\n");
  return CMD_SUCCESS;
}

static void permute(struct graph_node *start, struct vty *vty)
{
  static struct list *position = NULL;
  if (!position)
    position = list_new();

  struct cmd_token *stok = start->data;
  struct graph_node *gnn;
  struct listnode *ln;

  // recursive dfs
  listnode_add(position, start);
  for (unsigned int i = 0; i < vector_active(start->to); i++) {
    struct graph_node *gn = vector_slot(start->to, i);
    struct cmd_token *tok = gn->data;
    if (tok->attr == CMD_ATTR_HIDDEN
        || tok->attr == CMD_ATTR_DEPRECATED)
      continue;
    else if (tok->type == END_TKN || gn == start) {
      vty_out(vty, " ");
      for (ALL_LIST_ELEMENTS_RO(position, ln, gnn)) {
        struct cmd_token *tt = gnn->data;
        if (tt->type < SPECIAL_TKN)
          vty_out(vty, " %s", tt->text);
      }
      if (gn == start)
        vty_out(vty, "...");
      vty_out(vty, "\n");
    } else {
      bool skip = false;
      if (stok->type == FORK_TKN && tok->type != FORK_TKN)
        for (ALL_LIST_ELEMENTS_RO(position, ln, gnn))
          if (gnn == gn) {
            skip = true;
            break;
          }
      if (!skip)
        permute(gn, vty);
    }
  }
  list_delete_node(position, listtail(position));
}

int cmd_list_cmds(struct vty *vty, int do_permute)
{
  struct cmd_node *node = vector_slot(cmdvec, vty->node);

  if (do_permute)
    permute(vector_slot(node->cmdgraph->nodes, 0), vty);
  else {
    /* loop over all commands at this node */
    struct cmd_element *element = NULL;
    for (unsigned int i = 0; i < vector_active(node->cmd_vector);
         i++)
      if ((element = vector_slot(node->cmd_vector, i))
          && element->attr != CMD_ATTR_DEPRECATED
          && element->attr != CMD_ATTR_HIDDEN)
        vty_out(vty, "    %s\n", element->string);
  }
  return CMD_SUCCESS;
}

/* Help display function for all node. */
DEFUN (config_list,
       config_list_cmd,
       "list [permutations]",
       "Print command list\n"
       "Print all possible command permutations\n")
{
  return cmd_list_cmds(vty, argc == 2);
}

DEFUN (show_commandtree,
       show_commandtree_cmd,
       "show commandtree [permutations]",
       SHOW_STR
       "Show command tree\n"
       "Permutations that we are interested in\n")
{
  return cmd_list_cmds(vty, argc == 3);
}

DEFUN_HIDDEN(show_cli_graph,
             show_cli_graph_cmd,
             "show cli graph",
             SHOW_STR
             "CLI reflection\n"
             "Dump current command space as DOT graph\n")
{
  struct cmd_node *cn = vector_slot(cmdvec, vty->node);
  char *dot = cmd_graph_dump_dot(cn->cmdgraph);

  vty_out(vty, "%s\n", dot);
  XFREE(MTYPE_TMP, dot);
  return CMD_SUCCESS;
}

/* Hostname configuration */
DEFUN (config_hostname,
       hostname_cmd,
       "hostname WORD",
       "Set system's network name\n"
       "This system's network name\n")
{
  struct cmd_token *word = argv[1];

  if (!isalnum((int)word->arg[0])) {
    vty_out(vty,
        "Please specify string starting with alphabet or number\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  /* With reference to RFC 1123 Section 2.1 */
  if (strlen(word->arg) > HOSTNAME_LEN) {
    vty_out(vty, "Hostname length should be less than %d chars\n",
      HOSTNAME_LEN);
    return CMD_WARNING_CONFIG_FAILED;
  }

  return cmd_hostname_set(word->arg);
}

DEFUN (config_no_hostname,
       no_hostname_cmd,
       "no hostname [HOSTNAME]",
       NO_STR
       "Reset system's network name\n"
       "Host name of this router\n")
{
  return cmd_hostname_set(NULL);
}

/* VTY interface password set. */
DEFUN (config_password,
       password_cmd,
       "password [(8-8)] WORD",
       "Modify the terminal connection password\n"
       "Specifies a HIDDEN password will follow\n"
       "The password string\n")
{
  int idx_8 = 1;
  int idx_word = 2;
  if (argc == 3) // '8' was specified
  {
    if (host.password)
      XFREE(MTYPE_HOST, host.password);
    host.password = NULL;
    if (host.password_encrypt)
      XFREE(MTYPE_HOST, host.password_encrypt);
    host.password_encrypt =
      XSTRDUP(MTYPE_HOST, argv[idx_word]->arg);
    return CMD_SUCCESS;
  }

  if (!isalnum((int)argv[idx_8]->arg[0])) {
    vty_out(vty,
      "Please specify string starting with alphanumeric\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  if (host.password)
    XFREE(MTYPE_HOST, host.password);
  host.password = NULL;

  if (host.encrypt) {
    if (host.password_encrypt)
      XFREE(MTYPE_HOST, host.password_encrypt);
    host.password_encrypt =
      XSTRDUP(MTYPE_HOST, zencrypt(argv[idx_8]->arg));
  } else
    host.password = XSTRDUP(MTYPE_HOST, argv[idx_8]->arg);

  return CMD_SUCCESS;
}

/* VTY interface password delete. */
DEFUN (no_config_password,
       no_password_cmd,
       "no password",
       NO_STR
       "Modify the terminal connection password\n")
{
  bool warned = false;

  if (host.password) {
    if (!vty_shell_serv(vty)) {
      vty_out(vty, NO_PASSWD_CMD_WARNING);
      warned = true;
    }
    XFREE(MTYPE_HOST, host.password);
  }
  host.password = NULL;

  if (host.password_encrypt) {
    if (!warned && !vty_shell_serv(vty))
      vty_out(vty, NO_PASSWD_CMD_WARNING);
    XFREE(MTYPE_HOST, host.password_encrypt);
  }
  host.password_encrypt = NULL;

  return CMD_SUCCESS;
}

/* VTY enable password set. */
DEFUN (config_enable_password,
       enable_password_cmd,
       "enable password [(8-8)] WORD",
       "Modify enable password parameters\n"
       "Assign the privileged level password\n"
       "Specifies a HIDDEN password will follow\n"
       "The HIDDEN 'enable' password string\n")
{
  int idx_8 = 2;
  int idx_word = 3;

  /* Crypt type is specified. */
  if (argc == 4) {
    if (argv[idx_8]->arg[0] == '8') {
      if (host.enable)
        XFREE(MTYPE_HOST, host.enable);
      host.enable = NULL;

      if (host.enable_encrypt)
        XFREE(MTYPE_HOST, host.enable_encrypt);
      host.enable_encrypt =
        XSTRDUP(MTYPE_HOST, argv[idx_word]->arg);

      return CMD_SUCCESS;
    } else {
      vty_out(vty, "Unknown encryption type.\n");
      return CMD_WARNING_CONFIG_FAILED;
    }
  }

  if (!isalnum((int)argv[idx_8]->arg[0])) {
    vty_out(vty,
      "Please specify string starting with alphanumeric\n");
    return CMD_WARNING_CONFIG_FAILED;
  }

  if (host.enable)
    XFREE(MTYPE_HOST, host.enable);
  host.enable = NULL;

  /* Plain password input. */
  if (host.encrypt) {
    if (host.enable_encrypt)
      XFREE(MTYPE_HOST, host.enable_encrypt);
    host.enable_encrypt =
      XSTRDUP(MTYPE_HOST, zencrypt(argv[idx_8]->arg));
  } else
    host.enable = XSTRDUP(MTYPE_HOST, argv[idx_8]->arg);

  return CMD_SUCCESS;
}

/* VTY enable password delete. */
DEFUN (no_config_enable_password,
       no_enable_password_cmd,
       "no enable password",
       NO_STR
       "Modify enable password parameters\n"
       "Assign the privileged level password\n")
{
  bool warned = false;

  if (host.enable) {
    if (!vty_shell_serv(vty)) {
      vty_out(vty, NO_PASSWD_CMD_WARNING);
      warned = true;
    }
    XFREE(MTYPE_HOST, host.enable);
  }
  host.enable = NULL;

  if (host.enable_encrypt) {
    if (!warned && !vty_shell_serv(vty))
      vty_out(vty, NO_PASSWD_CMD_WARNING);
    XFREE(MTYPE_HOST, host.enable_encrypt);
  }
  host.enable_encrypt = NULL;

  return CMD_SUCCESS;
}

DEFUN (service_password_encrypt,
       service_password_encrypt_cmd,
       "service password-encryption",
       "Set up miscellaneous service\n"
       "Enable encrypted passwords\n")
{
  if (host.encrypt)
    return CMD_SUCCESS;

  host.encrypt = 1;

  if (host.password) {
    if (host.password_encrypt)
      XFREE(MTYPE_HOST, host.password_encrypt);
    host.password_encrypt =
      XSTRDUP(MTYPE_HOST, zencrypt(host.password));
  }
  if (host.enable) {
    if (host.enable_encrypt)
      XFREE(MTYPE_HOST, host.enable_encrypt);
    host.enable_encrypt =
      XSTRDUP(MTYPE_HOST, zencrypt(host.enable));
  }

  return CMD_SUCCESS;
}

DEFUN (no_service_password_encrypt,
       no_service_password_encrypt_cmd,
       "no service password-encryption",
       NO_STR
       "Set up miscellaneous service\n"
       "Enable encrypted passwords\n")
{
  if (!host.encrypt)
    return CMD_SUCCESS;

  host.encrypt = 0;

  if (host.password_encrypt)
    XFREE(MTYPE_HOST, host.password_encrypt);
  host.password_encrypt = NULL;

  if (host.enable_encrypt)
    XFREE(MTYPE_HOST, host.enable_encrypt);
  host.enable_encrypt = NULL;

  return CMD_SUCCESS;
}

DEFUN (config_terminal_length,
       config_terminal_length_cmd,
       "terminal length (0-512)",
       "Set terminal line parameters\n"
       "Set number of lines on a screen\n"
       "Number of lines on screen (0 for no pausing)\n")
{
  int idx_number = 2;

  vty->lines = atoi(argv[idx_number]->arg);

  return CMD_SUCCESS;
}

DEFUN (config_terminal_no_length,
       config_terminal_no_length_cmd,
       "terminal no length",
       "Set terminal line parameters\n"
       NO_STR
       "Set number of lines on a screen\n")
{
  vty->lines = -1;
  return CMD_SUCCESS;
}

DEFUN (service_terminal_length,
       service_terminal_length_cmd,
       "service terminal-length (0-512)",
       "Set up miscellaneous service\n"
       "System wide terminal length configuration\n"
       "Number of lines of VTY (0 means no line control)\n")
{
  int idx_number = 2;

  host.lines = atoi(argv[idx_number]->arg);

  return CMD_SUCCESS;
}

DEFUN (no_service_terminal_length,
       no_service_terminal_length_cmd,
       "no service terminal-length [(0-512)]",
       NO_STR
       "Set up miscellaneous service\n"
       "System wide terminal length configuration\n"
       "Number of lines of VTY (0 means no line control)\n")
{
  host.lines = -1;
  return CMD_SUCCESS;
}

DEFUN_HIDDEN (do_echo,
              echo_cmd,
              "echo MESSAGE...",
              "Echo a message back to the vty\n"
              "The message to echo\n")
{
  char *message;

  vty_out(vty, "%s\n",
    ((message = argv_concat(argv, argc, 1)) ? message : ""));
  if (message)
    XFREE(MTYPE_TMP, message);
  return CMD_SUCCESS;
}

DEFUN (banner_motd_default,
       banner_motd_default_cmd,
       "banner motd default",
       "Set banner string\n"
       "Strings for motd\n"
       "Default string\n")
{
  host.motd = default_motd;
  return CMD_SUCCESS;
}

DEFUN (no_banner_motd,
       no_banner_motd_cmd,
       "no banner motd",
       NO_STR
       "Set banner string\n"
       "Strings for motd\n")
{
  host.motd = NULL;
  if (host.motdfile)
    XFREE(MTYPE_HOST, host.motdfile);
  host.motdfile = NULL;
  return CMD_SUCCESS;
}

DEFUN(find,
      find_cmd,
      "find COMMAND...",
      "Find CLI command containing text\n"
      "Text to search for\n")
{
  char *text = argv_concat(argv, argc, 1);
  const struct cmd_node *node;
  const struct cmd_element *cli;
  vector clis;

  for (unsigned int i = 0; i < vector_active(cmdvec); i++) {
    node = vector_slot(cmdvec, i);
    if (!node)
      continue;
    clis = node->cmd_vector;
    for (unsigned int j = 0; j < vector_active(clis); j++) {
      cli = vector_slot(clis, j);
      if (strcasestr(cli->string, text))
        vty_out(vty, "  (%s)  %s\n",
          node_names[node->node], cli->string);
    }
  }

  XFREE(MTYPE_TMP, text);

  return CMD_SUCCESS;
}

/* Set config filename.  Called from vty.c */
void host_config_set(const char *filename)
{
  XFREE(MTYPE_HOST, host.config);
  host.config = XSTRDUP(MTYPE_HOST, filename);
}

const char *host_config_get(void)
{
  return host.config;
}

void install_default(enum node_type node)
{
  install_element(node, &config_exit_cmd);
  install_element(node, &config_quit_cmd);
  install_element(node, &config_end_cmd);
  install_element(node, &config_help_cmd);
  install_element(node, &config_list_cmd);
  install_element(node, &show_cli_graph_cmd);
  install_element(node, &find_cmd);
  install_element(node, &autocomplete_cmd);
}

void cmd_init()
{
  struct utsname names;
  uname(&names);
  qobj_init();

  /* Allocate initial top vector of commands. */
  varhandlers = list_new();
  cmdvec = vector_init(VECTOR_MIN_SIZE);

  /* Default host value settings. */
  host.name = XSTRDUP(MTYPE_HOST, names.nodename);
  host.password = NULL;
  host.enable = NULL;
  host.logfile = NULL;
  host.config = NULL;
  host.noconfig = false;
  host.lines = -1;
  host.motd = default_motd;
  host.motdfile = NULL;

  /* Install top nodes. */
  install_node(&view_node, NULL);
  install_node(&enable_node, NULL);
  install_node(&auth_node, NULL);
  install_node(&auth_enable_node, NULL);
  install_node(&config_node, NULL);

  /* Each node's basic commands. */
  install_element(VIEW_NODE, &config_list_cmd);
  install_element(VIEW_NODE, &config_exit_cmd);
  install_element(VIEW_NODE, &config_quit_cmd);
  install_element(VIEW_NODE, &config_help_cmd);
  install_element(VIEW_NODE, &config_enable_cmd);
  install_element(VIEW_NODE, &config_terminal_length_cmd);
  install_element(VIEW_NODE, &config_terminal_no_length_cmd);
  install_element(VIEW_NODE, &show_commandtree_cmd);
  install_element(VIEW_NODE, &echo_cmd);
  install_element(VIEW_NODE, &autocomplete_cmd);
  install_element(VIEW_NODE, &find_cmd);
  install_element(ENABLE_NODE, &config_end_cmd);
  install_element(ENABLE_NODE, &config_disable_cmd);
  install_element(ENABLE_NODE, &config_terminal_cmd);
  install_element(CONFIG_NODE, &hostname_cmd);
  install_element(CONFIG_NODE, &no_hostname_cmd);
  install_element(CONFIG_NODE, &password_cmd);
  install_element(CONFIG_NODE, &no_password_cmd);
  install_element(CONFIG_NODE, &enable_password_cmd);
  install_element(CONFIG_NODE, &no_enable_password_cmd);
  install_element(CONFIG_NODE, &service_password_encrypt_cmd);
  install_element(CONFIG_NODE, &no_service_password_encrypt_cmd);
  install_element(CONFIG_NODE, &banner_motd_default_cmd);
  install_element(CONFIG_NODE, &no_banner_motd_cmd);
  install_element(CONFIG_NODE, &service_terminal_length_cmd);
  install_element(CONFIG_NODE, &no_service_terminal_length_cmd);
  install_default(CONFIG_NODE);
}

void cmd_terminate(void)
{
  struct cmd_node *cmd_node;

  if (cmdvec) {
    for (unsigned int i = 0; i < vector_active(cmdvec); i++)
      if ((cmd_node = vector_slot(cmdvec, i)) != NULL) {
        graph_delete_graph(cmd_node->cmdgraph);
        vector_free(cmd_node->cmd_vector);
        hash_clean(cmd_node->cmd_hash, NULL);
        hash_free(cmd_node->cmd_hash);
        cmd_node->cmd_hash = NULL;
      }

    vector_free(cmdvec);
    cmdvec = NULL;
  }

  XFREE(MTYPE_HOST, host.name);
  XFREE(MTYPE_HOST, host.password);
  XFREE(MTYPE_HOST, host.password_encrypt);
  XFREE(MTYPE_HOST, host.enable);
  XFREE(MTYPE_HOST, host.enable_encrypt);
  XFREE(MTYPE_HOST, host.logfile);
  XFREE(MTYPE_HOST, host.motdfile);
  XFREE(MTYPE_HOST, host.config);

  list_delete(&varhandlers);
  qobj_finish();
}

/* Configuration make from file. */
int config_from_file(struct vty *vty, FILE *fp, unsigned int *line_num)
{
	int ret, error_ret = 0;
	*line_num = 0;

	while (fgets(vty->buf, VTY_BUFSIZ, fp)) {
		++(*line_num);

		ret = command_config_read_one_line(vty, NULL, *line_num, 0);

		if (ret != CMD_SUCCESS && ret != CMD_WARNING
		    && ret != CMD_ERR_NOTHING_TODO)
			error_ret = ret;
	}

	if (error_ret) {
		return error_ret;
	}

	return CMD_SUCCESS;
}
