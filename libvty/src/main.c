
#include <zebra.h>
#include "thread.h"
#include "vty.h"
#include "command.h"
#include "memory.h"
#include "memory_vty.h"
#include "log.h"

#include "common_cli.h"

struct thread_master *master;

int dump_args(struct vty *vty, const char *descr, int argc,
	      struct cmd_token *argv[])
{
	int i;
	vty_out(vty, "%s with %d args.\n", descr, argc);
	for (i = 0; i < argc; i++) {
		vty_out(vty, "[%02d] %s@%s: %s\n", i, argv[i]->text,
			argv[i]->varname, argv[i]->arg);
	}

	return CMD_SUCCESS;
}

static void vty_do_exit(int isexit)
{
	printf("\nend.\n");
	cmd_terminate();
	vty_terminate();
	nb_terminate();
	yang_terminate();
	thread_master_free(master);
	closezlog();

	log_memstats(stderr, "testcli");
	if (!isexit)
		exit(0);
}

/* main routine. */
int main(int argc, char **argv)
{
	/* Set umask before anything for security */
	umask(0027);

	/* master init. */
	master = thread_master_create(NULL);
	openzlog("common-cli", "NONE", 0, LOG_CONS | LOG_NDELAY | LOG_PID,
		 LOG_DAEMON);
	zlog_set_level(ZLOG_DEST_SYSLOG, ZLOG_DISABLED);
	zlog_set_level(ZLOG_DEST_STDOUT, ZLOG_DISABLED);
	zlog_set_level(ZLOG_DEST_MONITOR, LOG_DEBUG);

	/* Library inits. */
	cmd_init(1);
	cmd_hostname_set("test");
	cmd_domainname_set("test.domain");

	vty_init(master, false);
	memory_init();
	yang_init();
	nb_init(master, NULL, 0);

	test_init(argc, argv);
	vty_stdio(vty_do_exit);

	/* Fetch next active thread. */
	struct thread thread;
	while (thread_fetch(master, &thread))
		thread_call(&thread);

	/* Not reached. */
	exit(0);
}

