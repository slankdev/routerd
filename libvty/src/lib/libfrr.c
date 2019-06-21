/*
 * libfrr overall management functions
 *
 * Copyright (C) 2016  David Lamparter for NetDEF, Inc.
 *
 * This program is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the Free
 * Software Foundation; either version 2 of the License, or (at your option)
 * any later version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or
 * FITNESS FOR A PARTICULAR PURPOSE.  See the GNU General Public License for
 * more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include "config.h"
#include <zebra.h>
#include <sys/un.h>

#include <sys/types.h>
#include <sys/wait.h>

#include "libfrr.h"
#include "getopt.h"
#include "privs.h"
#include "vty.h"
#include "command.h"
#include "version.h"
#include "memory_vty.h"
#include "module.h"
#include "network.h"

const char frr_sysconfdir[] = SYSCONFDIR;
char frr_vtydir[256];
#ifdef HAVE_SQLITE3
const char frr_dbdir[] = DAEMON_DB_DIR;
#endif
const char frr_moduledir[] = MODULE_PATH;

char frr_protoname[256] = "NONE";
char frr_protonameinst[256] = "NONE";

char config_default[512];
char frr_zclientpath[256];
static char pidfile_default[1024];
#ifdef HAVE_SQLITE3
static char dbfile_default[512];
#endif
static char vtypath_default[512];

bool debug_memstats_at_exit = false;
static bool nodetach_term, nodetach_daemon;

static char comb_optstr[256];
static struct option comb_lo[64];
static struct option *comb_next_lo = &comb_lo[0];
static char comb_helpstr[4096];

struct optspec {
	const char *optstr;
	const char *helpstr;
	const struct option *longopts;
};

static void opt_extend(const struct optspec *os)
{
	const struct option *lo;

	strlcat(comb_optstr, os->optstr, sizeof(comb_optstr));
	strlcat(comb_helpstr, os->helpstr, sizeof(comb_helpstr));
	for (lo = os->longopts; lo->name; lo++)
		memcpy(comb_next_lo++, lo, sizeof(*lo));
}


#define OPTION_VTYSOCK   1000
#define OPTION_MODULEDIR 1002
#define OPTION_LOG       1003
#define OPTION_LOGLEVEL  1004
#define OPTION_TCLI      1005
#define OPTION_DB_FILE   1006
#define OPTION_LOGGING   1007

static const struct option lo_always[] = {
	{"help", no_argument, NULL, 'h'},
	{"version", no_argument, NULL, 'v'},
	{"daemon", no_argument, NULL, 'd'},
	{"module", no_argument, NULL, 'M'},
	{"vty_socket", required_argument, NULL, OPTION_VTYSOCK},
	{"moduledir", required_argument, NULL, OPTION_MODULEDIR},
	{"log", required_argument, NULL, OPTION_LOG},
	{"log-level", required_argument, NULL, OPTION_LOGLEVEL},
	{"tcli", no_argument, NULL, OPTION_TCLI},
	{"command-log-always", no_argument, NULL, OPTION_LOGGING},
	{NULL}};
static const struct optspec os_always = {
	"hvdM:",
	"  -h, --help         Display this help and exit\n"
	"  -v, --version      Print program version\n"
	"  -d, --daemon       Runs in daemon mode\n"
	"  -M, --module       Load specified module\n"
	"      --vty_socket   Override vty socket path\n"
	"      --moduledir    Override modules directory\n"
	"      --log          Set Logging to stdout, syslog, or file:<name>\n"
	"      --log-level    Set Logging Level to use, debug, info, warn, etc\n"
	"      --tcli         Use transaction-based CLI\n",
	lo_always};


static const struct option lo_cfg_pid_dry[] = {
	{"pid_file", required_argument, NULL, 'i'},
	{"config_file", required_argument, NULL, 'f'},
#ifdef HAVE_SQLITE3
	{"db_file", required_argument, NULL, OPTION_DB_FILE},
#endif
	{"pathspace", required_argument, NULL, 'N'},
	{"dryrun", no_argument, NULL, 'C'},
	{"terminal", no_argument, NULL, 't'},
	{NULL}};
static const struct optspec os_cfg_pid_dry = {
	"f:i:CtN:",
	"  -f, --config_file  Set configuration file name\n"
	"  -i, --pid_file     Set process identifier file name\n"
#ifdef HAVE_SQLITE3
	"      --db_file      Set database file name\n"
#endif
	"  -N, --pathspace    Insert prefix into config & socket paths\n"
	"  -C, --dryrun       Check configuration for validity and exit\n"
	"  -t, --terminal     Open terminal session on stdio\n"
	"  -d -t              Daemonize after terminal session ends\n",
	lo_cfg_pid_dry};


static const struct option lo_zclient[] = {
	{"socket", required_argument, NULL, 'z'},
	{NULL}};
static const struct optspec os_zclient = {
	"z:", "  -z, --socket       Set path of zebra socket\n", lo_zclient};


static const struct option lo_vty[] = {
	{"vty_addr", required_argument, NULL, 'A'},
	{"vty_port", required_argument, NULL, 'P'},
	{NULL}};
static const struct optspec os_vty = {
	"A:P:",
	"  -A, --vty_addr     Set vty's bind address\n"
	"  -P, --vty_port     Set vty's port number\n",
	lo_vty};


static const struct option lo_user[] = {{"user", required_argument, NULL, 'u'},
					{"group", required_argument, NULL, 'g'},
					{NULL}};
static const struct optspec os_user = {"u:g:",
				       "  -u, --user         User to run as\n"
				       "  -g, --group        Group to run as\n",
				       lo_user};

static struct frr_daemon_info *di = NULL;

void frr_init_vtydir(void)
{
	snprintf(frr_vtydir, sizeof(frr_vtydir), DAEMON_VTY_DIR, "", "");
}

void frr_preinit(struct frr_daemon_info *daemon, int argc, char **argv)
{
	di = daemon;

	/* basename(), opencoded. */
	char *p = strrchr(argv[0], '/');
	di->progname = p ? p + 1 : argv[0];

	umask(0027);

	opt_extend(&os_always);
	if (!(di->flags & FRR_NO_CFG_PID_DRY))
		opt_extend(&os_cfg_pid_dry);
	if (!(di->flags & FRR_NO_PRIVSEP))
		opt_extend(&os_user);
	if (!(di->flags & FRR_NO_ZCLIENT))
		opt_extend(&os_zclient);
	if (!(di->flags & FRR_NO_TCPVTY))
		opt_extend(&os_vty);
	if (di->flags & FRR_DETACH_LATER)
		nodetach_daemon = true;

	frr_init_vtydir();
	snprintf(config_default, sizeof(config_default), "%s/%s.conf",
		 frr_sysconfdir, di->name);
	snprintf(pidfile_default, sizeof(pidfile_default), "%s/%s.pid",
		 frr_vtydir, di->name);
	snprintf(frr_zclientpath, sizeof(frr_zclientpath),
		 ZEBRA_SERV_PATH, "", "");
#ifdef HAVE_SQLITE3
	snprintf(dbfile_default, sizeof(dbfile_default), "%s/%s.db",
		 frr_dbdir, di->name);
#endif

	strlcpy(frr_protoname, di->logname, sizeof(frr_protoname));
	strlcpy(frr_protonameinst, di->logname, sizeof(frr_protonameinst));

	di->cli_mode = FRR_CLI_CLASSIC;
}

void frr_help_exit(int status)
{
	FILE *target = status ? stderr : stdout;

	if (status != 0)
		fprintf(target, "Invalid options.\n\n");

	if (di->printhelp)
		di->printhelp(target);
	else
		fprintf(target, "Usage: %s [OPTION...]\n\n%s%s%s\n\n%s",
			di->progname, di->proghelp, di->copyright ? "\n\n" : "",
			di->copyright ? di->copyright : "", comb_helpstr);
	fprintf(target, "\nReport bugs to %s\n", FRR_BUG_ADDRESS);
	exit(status);
}

static int errors = 0;

static void frr_mkdir(const char *path, bool strip)
{
	char buf[256];
	mode_t prev;
	int ret;
	struct zprivs_ids_t ids;

	if (strip) {
		char *slash = strrchr(path, '/');
		size_t plen;
		if (!slash)
			return;
		plen = slash - path;
		if (plen > sizeof(buf) - 1)
			return;
		memcpy(buf, path, plen);
		buf[plen] = '\0';
		path = buf;
	}

	/* o+rx (..5) is needed for the frrvty group to work properly;
	 * without it, users in the frrvty group can't access the vty sockets.
	 */
	prev = umask(0022);
	ret = mkdir(path, 0755);
	umask(prev);

	if (ret != 0) {
		/* if EEXIST, return without touching the permissions,
		 * so user-set custom permissions are left in place
		 */
		if (errno == EEXIST)
			return;

		fprintf(stderr, "failed to mkdir \"%s\": %s", path,
			 strerror(errno));
		return;
	}

	zprivs_get_ids(&ids);
	if (chown(path, ids.uid_normal, ids.gid_normal))
		fprintf(stderr, "failed to chown \"%s\": %s", path,
			 strerror(errno));
}

const char *frr_get_progname(void)
{
	return di ? di->progname : NULL;
}

enum frr_cli_mode frr_get_cli_mode(void)
{
	return di ? di->cli_mode : FRR_CLI_CLASSIC;
}

static int rcvd_signal = 0;

static void rcv_signal(int signum)
{
	rcvd_signal = signum;
	/* poll() is interrupted by the signal; handled below */
}

static int daemon_ctl_sock = -1;

static void frr_check_detach(void)
{
	if (nodetach_term || nodetach_daemon)
		return;

	if (daemon_ctl_sock != -1)
		close(daemon_ctl_sock);
	daemon_ctl_sock = -1;
}

static void frr_terminal_close(int isexit)
{
	int nullfd;

	nodetach_term = false;
	frr_check_detach();

	if (!di->daemon_mode || isexit) {
		printf("\n%s exiting\n", di->name);
		if (!isexit)
			raise(SIGINT);
		return;
	} else {
		printf("\n%s daemonizing\n", di->name);
		fflush(stdout);
	}

	nullfd = open("/dev/null", O_RDONLY | O_NOCTTY);
	if (nullfd == -1) {
		fprintf(stderr,
			     "%s: failed to open /dev/null: %s", __func__,
			     strerror(errno));
	} else {
		dup2(nullfd, 0);
		dup2(nullfd, 1);
		dup2(nullfd, 2);
		close(nullfd);
	}
}

static struct thread *daemon_ctl_thread = NULL;

