/* ecmp_nexthops => "[no] nexthop        <	  <A.B.C.D|X:X::X:X>$addr [INTERFACE$intf]	  |INTERFACE$intf	>	[nexthop-vrf NAME$name]" */
DEFUN_CMD_FUNC_DECL(ecmp_nexthops)
#define funcdecl_ecmp_nexthops static int ecmp_nexthops_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * no,\
	const union sockunion * addr,\
	const char * addr_str __attribute__ ((unused)),\
	const char * intf,\
	const char * name)
funcdecl_ecmp_nexthops;
DEFUN_CMD_FUNC_TEXT(ecmp_nexthops)
{
#if 4 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *no = NULL;
	union sockunion s__addr = { .sa.sa_family = AF_UNSPEC }, *addr = NULL;
	const char *addr_str = NULL;
	const char *intf = NULL;
	const char *name = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "no")) {
			no = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "addr")) {
			addr_str = argv[_i]->arg;
			if (argv[_i]->text[0] == 'X') {
				s__addr.sa.sa_family = AF_INET6;
				_fail = !inet_pton(AF_INET6, argv[_i]->arg, &s__addr.sin6.sin6_addr);
				addr = &s__addr;
			} else {
				s__addr.sa.sa_family = AF_INET;
				_fail = !inet_aton(argv[_i]->arg, &s__addr.sin.sin_addr);
				addr = &s__addr;
			}
		}
		if (!strcmp(argv[_i]->varname, "intf")) {
			intf = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "name")) {
			name = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 1 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 1 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return ecmp_nexthops_magic(self, vty, argc, argv, no, addr, addr_str, intf, name);
}

