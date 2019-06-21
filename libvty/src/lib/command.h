/*
 * Zebra configuration command interface routine
 * Copyright (C) 1997, 98 Kunihiro Ishiguro
 *
 * This file is part of GNU Zebra.
 *
 * GNU Zebra is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published
 * by the Free Software Foundation; either version 2, or (at your
 * option) any later version.
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

#ifndef _ZEBRA_COMMAND_H
#define _ZEBRA_COMMAND_H

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

DECLARE_MTYPE(HOST)
DECLARE_MTYPE(COMPLETION)

/*
 * From RFC 1123 (Requirements for Internet Hosts), Section 2.1 on hostnames:
 * One aspect of host name syntax is hereby changed: the restriction on
 * the first character is relaxed to allow either a letter or a digit.
 * Host software MUST support this more liberal syntax.
 *
 * Host software MUST handle host names of up to 63 characters and
 * SHOULD handle host names of up to 255 characters.
 */
#define HOSTNAME_LEN   255

/* Host configuration variable */
struct host {
	/* Host name of this router. */
	char *name;

	/* Password for vty interface. */
	char *password;
	char *password_encrypt;

	/* Enable password */
	char *enable;
	char *enable_encrypt;

	/* System wide terminal lines. */
	int lines;

	/* Log filename. */
	char *logfile;

	/* config file name of this host */
	char *config;
	int noconfig;

	/* Flags for services */
	int advanced;
	int encrypt;

	/* Banner configuration. */
	const char *motd;
	char *motdfile;
};

extern vector cmdvec;

/* Return value of the commands. */
#define CMD_SUCCESS              0
#define CMD_WARNING              1
#define CMD_ERR_NO_MATCH         2
#define CMD_ERR_AMBIGUOUS        3
#define CMD_ERR_INCOMPLETE       4
#define CMD_ERR_EXEED_ARGC_MAX   5
#define CMD_ERR_NOTHING_TODO     6
#define CMD_COMPLETE_FULL_MATCH  7
#define CMD_COMPLETE_MATCH       8
#define CMD_COMPLETE_LIST_MATCH  9
#define CMD_SUCCESS_DAEMON      10
#define CMD_ERR_NO_FILE         11
#define CMD_SUSPEND             12
#define CMD_WARNING_CONFIG_FAILED 13
#define CMD_NOT_MY_INSTANCE	14

/* Argc max counts. */
#define CMD_ARGC_MAX   256

/* Turn off these macros when uisng cpp with extract.pl */
#ifndef VTYSH_EXTRACT_PL

/* helper defines for end-user DEFUN* macros */
#define DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attrs, dnum)     \
	static struct cmd_element cmdname = {                                  \
		.string = cmdstr,                                              \
		.func = funcname,                                              \
		.doc = helpstr,                                                \
		.attr = attrs,                                                 \
		.daemon = dnum,                                                \
		.name = #cmdname,                                              \
	};

#define DEFUN_CMD_FUNC_DECL(funcname)                                          \
	static int funcname(const struct cmd_element *, struct vty *, int,     \
			    struct cmd_token *[]);

#define DEFUN_CMD_FUNC_TEXT(funcname)                                          \
	static int funcname(const struct cmd_element *self                     \
			    __attribute__((unused)),                           \
			    struct vty *vty __attribute__((unused)),           \
			    int argc __attribute__((unused)),                  \
			    struct cmd_token *argv[] __attribute__((unused)))

#define DEFPY(funcname, cmdname, cmdstr, helpstr)                              \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, 0)            \
	funcdecl_##funcname

#define DEFPY_NOSH(funcname, cmdname, cmdstr, helpstr)                         \
	DEFPY(funcname, cmdname, cmdstr, helpstr)

#define DEFPY_ATTR(funcname, cmdname, cmdstr, helpstr, attr)                   \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, 0)         \
	funcdecl_##funcname

#define DEFPY_HIDDEN(funcname, cmdname, cmdstr, helpstr)                       \
	DEFPY_ATTR(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN)

#define DEFUN(funcname, cmdname, cmdstr, helpstr)                              \
	DEFUN_CMD_FUNC_DECL(funcname)                                          \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, 0)            \
	DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUN_ATTR(funcname, cmdname, cmdstr, helpstr, attr)                   \
	DEFUN_CMD_FUNC_DECL(funcname)                                          \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, 0)         \
	DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUN_HIDDEN(funcname, cmdname, cmdstr, helpstr)                       \
	DEFUN_ATTR(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN)

/* DEFUN_NOSH for commands that vtysh should ignore */
#define DEFUN_NOSH(funcname, cmdname, cmdstr, helpstr)                         \
	DEFUN(funcname, cmdname, cmdstr, helpstr)

/* DEFSH for vtysh. */
#define DEFSH(daemon, cmdname, cmdstr, helpstr)                                \
	DEFUN_CMD_ELEMENT(NULL, cmdname, cmdstr, helpstr, 0, daemon)

#define DEFSH_HIDDEN(daemon, cmdname, cmdstr, helpstr)                         \
	DEFUN_CMD_ELEMENT(NULL, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN,     \
			  daemon)

/* DEFUN + DEFSH */
#define DEFUNSH(daemon, funcname, cmdname, cmdstr, helpstr)                    \
	DEFUN_CMD_FUNC_DECL(funcname)                                          \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, daemon)       \
	DEFUN_CMD_FUNC_TEXT(funcname)

/* DEFUN + DEFSH with attributes */
#define DEFUNSH_ATTR(daemon, funcname, cmdname, cmdstr, helpstr, attr)         \
	DEFUN_CMD_FUNC_DECL(funcname)                                          \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, daemon)    \
	DEFUN_CMD_FUNC_TEXT(funcname)

#define DEFUNSH_HIDDEN(daemon, funcname, cmdname, cmdstr, helpstr)             \
	DEFUNSH_ATTR(daemon, funcname, cmdname, cmdstr, helpstr,               \
		     CMD_ATTR_HIDDEN)

#define DEFUNSH_DEPRECATED(daemon, funcname, cmdname, cmdstr, helpstr)         \
	DEFUNSH_ATTR(daemon, funcname, cmdname, cmdstr, helpstr,               \
		     CMD_ATTR_DEPRECATED)

/* ALIAS macro which define existing command's alias. */
#define ALIAS(funcname, cmdname, cmdstr, helpstr)                              \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, 0)

#define ALIAS_ATTR(funcname, cmdname, cmdstr, helpstr, attr)                   \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, attr, 0)

#define ALIAS_HIDDEN(funcname, cmdname, cmdstr, helpstr)                       \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN, \
			  0)

#define ALIAS_DEPRECATED(funcname, cmdname, cmdstr, helpstr)                   \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr,                  \
			  CMD_ATTR_DEPRECATED, 0)

#define ALIAS_SH(daemon, funcname, cmdname, cmdstr, helpstr)                   \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, 0, daemon)

#define ALIAS_SH_HIDDEN(daemon, funcname, cmdname, cmdstr, helpstr)            \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr, CMD_ATTR_HIDDEN, \
			  daemon)

#define ALIAS_SH_DEPRECATED(daemon, funcname, cmdname, cmdstr, helpstr)        \
	DEFUN_CMD_ELEMENT(funcname, cmdname, cmdstr, helpstr,                  \
			  CMD_ATTR_DEPRECATED, daemon)

#else /* VTYSH_EXTRACT_PL */
#define DEFPY(funcname, cmdname, cmdstr, helpstr)                              \
	DEFUN(funcname, cmdname, cmdstr, helpstr)

#define DEFPY_NOSH(funcname, cmdname, cmdstr, helpstr)                         \
	DEFUN_NOSH(funcname, cmdname, cmdstr, helpstr)

#define DEFPY_ATTR(funcname, cmdname, cmdstr, helpstr, attr)                   \
	DEFUN_ATTR(funcname, cmdname, cmdstr, helpstr, attr)

#define DEFPY_HIDDEN(funcname, cmdname, cmdstr, helpstr)                       \
	DEFUN_HIDDEN(funcname, cmdname, cmdstr, helpstr)
#endif /* VTYSH_EXTRACT_PL */

/* Some macroes */

/*
 * Sometimes #defines create maximum values that
 * need to have strings created from them that
 * allow the parser to match against them.
 * These macros allow that.
 */
#define CMD_CREATE_STR(s)  CMD_CREATE_STR_HELPER(s)
#define CMD_CREATE_STR_HELPER(s) #s
#define CMD_RANGE_STR(a,s) "(" CMD_CREATE_STR(a) "-" CMD_CREATE_STR(s) ")"

/* Common descriptions. */
#define SHOW_STR "Show running system information\n"
#define IP_STR "IP information\n"
#define IPV6_STR "IPv6 information\n"
#define NO_STR "Negate a command or set its defaults\n"
#define REDIST_STR "Redistribute information from another routing protocol\n"
#define CLEAR_STR "Reset functions\n"
#define RIP_STR "RIP information\n"
#define EIGRP_STR "EIGRP information\n"
#define BGP_STR "BGP information\n"
#define BGP_SOFT_STR "Soft reconfig inbound and outbound updates\n"
#define BGP_SOFT_IN_STR "Send route-refresh unless using 'soft-reconfiguration inbound'\n"
#define BGP_SOFT_OUT_STR "Resend all outbound updates\n"
#define BGP_SOFT_RSCLIENT_RIB_STR "Soft reconfig for rsclient RIB\n"
#define OSPF_STR "OSPF information\n"
#define NEIGHBOR_STR "Specify neighbor router\n"
#define DEBUG_STR "Debugging functions\n"
#define UNDEBUG_STR "Disable debugging functions (see also 'debug')\n"
#define ROUTER_STR "Enable a routing process\n"
#define AS_STR "AS number\n"
#define MAC_STR "MAC address\n"
#define MBGP_STR "MBGP information\n"
#define MATCH_STR "Match values from routing table\n"
#define SET_STR "Set values in destination routing protocol\n"
#define OUT_STR "Filter outgoing routing updates\n"
#define IN_STR  "Filter incoming routing updates\n"
#define V4NOTATION_STR "specify by IPv4 address notation(e.g. 0.0.0.0)\n"
#define OSPF6_NUMBER_STR "Specify by number\n"
#define INTERFACE_STR "Interface information\n"
#define IFNAME_STR "Interface name(e.g. ep0)\n"
#define IP6_STR "IPv6 Information\n"
#define OSPF6_STR "Open Shortest Path First (OSPF) for IPv6\n"
#define OSPF6_INSTANCE_STR "(1-65535) Instance ID\n"
#define SECONDS_STR "Seconds\n"
#define ROUTE_STR "Routing Table\n"
#define PREFIX_LIST_STR "Build a prefix list\n"
#define OSPF6_DUMP_TYPE_LIST "<neighbor|interface|area|lsa|zebra|config|dbex|spf|route|lsdb|redistribute|hook|asbr|prefix|abr>"
#define AREA_TAG_STR "[area tag]\n"
#define COMMUNITY_AANN_STR "Community number where AA and NN are (0-65535)\n"
#define COMMUNITY_VAL_STR  "Community number in AA:NN format (where AA and NN are (0-65535)) or local-AS|no-advertise|no-export|internet or additive\n"
#define MPLS_TE_STR "MPLS-TE specific commands\n"
#define LINK_PARAMS_STR "Configure interface link parameters\n"
#define OSPF_RI_STR "OSPF Router Information specific commands\n"
#define PCE_STR "PCE Router Information specific commands\n"
#define MPLS_STR "MPLS information\n"
#define SR_STR "Segment-Routing specific commands\n"
#define WATCHFRR_STR "watchfrr information\n"
#define ZEBRA_STR "Zebra information\n"

#define CMD_VNI_RANGE "(1-16777215)"
#define CONF_BACKUP_EXT ".sav"

/* Command warnings. */
#define NO_PASSWD_CMD_WARNING                                                  \
	"Please be aware that removing the password is a security risk and you should think twice about this command.\n"

/* IPv4 only machine should not accept IPv6 address for peer's IP
   address.  So we replace VTY command string like below. */
#define NEIGHBOR_ADDR_STR  "Neighbor address\nIPv6 address\n"
#define NEIGHBOR_ADDR_STR2 "Neighbor address\nNeighbor IPv6 address\nInterface name or neighbor tag\n"
#define NEIGHBOR_ADDR_STR3 "Neighbor address\nIPv6 address\nInterface name\n"

/* Prototypes. */
extern void install_node(struct cmd_node *, int (*)(struct vty *));
extern void install_default(enum node_type);
extern void install_element(enum node_type, struct cmd_element *);

/* known issue with uninstall_element:  changes to cmd_token->attr (i.e.
 * deprecated/hidden) are not reversed. */
extern void uninstall_element(enum node_type, struct cmd_element *);

/* Concatenates argv[shift] through argv[argc-1] into a single NUL-terminated
   string with a space between each element (allocated using
   XMALLOC(MTYPE_TMP)).  Returns NULL if shift >= argc. */
extern char *argv_concat(struct cmd_token **argv, int argc, int shift);

/*
 * It is preferred that you set the index initial value
 * to a 0.  This way in the future if you modify the
 * cli then there is no need to modify the initial
 * value of the index
 */
extern int argv_find(struct cmd_token **argv, int argc, const char *text, int *index);
extern vector cmd_make_strvec(const char *);
extern void cmd_free_strvec(vector);
extern vector cmd_describe_command(vector, struct vty *, int *status);
extern char **cmd_complete_command(vector, struct vty *, int *status);
extern const char *cmd_prompt(enum node_type);
extern int command_config_read_one_line(struct vty *vty,
					const struct cmd_element **,
					uint32_t line_num, int use_config_node);
extern int config_from_file(struct vty *, FILE *, unsigned int *line_num);
/*
 * Execute command under the given vty context.
 *
 * vty
 *    The vty context to execute under.
 *
 * cmd
 *    The command string to execute.
 *
 * matched
 *    If non-null and a match was found, the address of the matched command is
 *    stored here. No action otherwise.
 *
 * vtysh
 *    Whether or not this is being called from vtysh. If this is nonzero,
 *    XXX: then what?
 *
 * Returns:
 *    XXX: what does it return
 */
extern int cmd_execute(struct vty *vty, const char *cmd,
		       const struct cmd_element **matched, int vtysh);
extern int cmd_execute_command(vector, struct vty *,
			       const struct cmd_element **, int);
extern int cmd_execute_command_strict(vector, struct vty *,
				      const struct cmd_element **);
extern void cmd_init(void);
extern void cmd_terminate(void);
extern void cmd_exit(struct vty *vty);
extern int cmd_list_cmds(struct vty *vty, int do_permute);

extern int cmd_password_set(const char *password);
extern int cmd_hostname_set(const char *hostname);
extern const char *cmd_hostname_get(void);

extern vector completions_to_vec(struct list *completions);

/* Export typical functions. */
extern const char *host_config_get(void);
extern void host_config_set(const char *);
extern void print_version(const char *);
extern int cmd_banner_motd_file(const char *);

/* struct host global, ick */
extern struct host host;

struct cmd_variable_handler {
	const char *tokenname, *varname;
	void (*completions)(vector out, struct cmd_token *token);
};

extern void cmd_variable_complete(struct cmd_token *token, const char *arg, vector comps);
extern void cmd_variable_handler_register(const struct cmd_variable_handler *cvh);
extern char *cmd_variable_comp2str(vector comps, unsigned short cols);
extern void command_setup_early_logging(const char *dest, const char *level);

#ifdef __cplusplus
}
#endif
#endif /* _ZEBRA_COMMAND_H */
