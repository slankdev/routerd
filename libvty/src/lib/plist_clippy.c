/* ip_prefix_list => "ip prefix-list WORD [seq (1-4294967295)] <deny|permit>$action <any$dest|A.B.C.D/M$dest [{ge (0-32)|le (0-32)}]>" */
DEFUN_CMD_FUNC_DECL(ip_prefix_list)
#define funcdecl_ip_prefix_list static int ip_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	long seq,\
	const char * seq_str __attribute__ ((unused)),\
	const char * action,\
	const char * dest,\
	long ge,\
	const char * ge_str __attribute__ ((unused)),\
	long le,\
	const char * le_str __attribute__ ((unused)))
funcdecl_ip_prefix_list;
DEFUN_CMD_FUNC_TEXT(ip_prefix_list)
{
#if 6 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	long seq = 0;
	const char *seq_str = NULL;
	const char *action = NULL;
	const char *dest = NULL;
	long ge = 0;
	const char *ge_str = NULL;
	long le = 0;
	const char *le_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "seq")) {
			seq_str = argv[_i]->arg;
			char *_end;
			seq = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "action")) {
			action = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dest")) {
			dest = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "ge")) {
			ge_str = argv[_i]->arg;
			char *_end;
			ge = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "le")) {
			le_str = argv[_i]->arg;
			char *_end;
			le = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!action) {
		vty_out(vty, "Internal CLI error [%s]\n", "action");
		return CMD_WARNING;
	}
	if (!dest) {
		vty_out(vty, "Internal CLI error [%s]\n", "dest");
		return CMD_WARNING;
	}

	return ip_prefix_list_magic(self, vty, argc, argv, prefix_list, seq, seq_str, action, dest, ge, ge_str, le, le_str);
}

/* no_ip_prefix_list => "no ip prefix-list WORD [seq (1-4294967295)] <deny|permit>$action <any$dest|A.B.C.D/M$dest [{ge (0-32)|le (0-32)}]>" */
DEFUN_CMD_FUNC_DECL(no_ip_prefix_list)
#define funcdecl_no_ip_prefix_list static int no_ip_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	long seq,\
	const char * seq_str __attribute__ ((unused)),\
	const char * action,\
	const char * dest,\
	long ge,\
	const char * ge_str __attribute__ ((unused)),\
	long le,\
	const char * le_str __attribute__ ((unused)))
funcdecl_no_ip_prefix_list;
DEFUN_CMD_FUNC_TEXT(no_ip_prefix_list)
{
#if 6 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	long seq = 0;
	const char *seq_str = NULL;
	const char *action = NULL;
	const char *dest = NULL;
	long ge = 0;
	const char *ge_str = NULL;
	long le = 0;
	const char *le_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "seq")) {
			seq_str = argv[_i]->arg;
			char *_end;
			seq = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "action")) {
			action = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dest")) {
			dest = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "ge")) {
			ge_str = argv[_i]->arg;
			char *_end;
			ge = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "le")) {
			le_str = argv[_i]->arg;
			char *_end;
			le = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!action) {
		vty_out(vty, "Internal CLI error [%s]\n", "action");
		return CMD_WARNING;
	}
	if (!dest) {
		vty_out(vty, "Internal CLI error [%s]\n", "dest");
		return CMD_WARNING;
	}

	return no_ip_prefix_list_magic(self, vty, argc, argv, prefix_list, seq, seq_str, action, dest, ge, ge_str, le, le_str);
}

/* no_ip_prefix_list_seq => "no ip prefix-list WORD seq (1-4294967295)" */
DEFUN_CMD_FUNC_DECL(no_ip_prefix_list_seq)
#define funcdecl_no_ip_prefix_list_seq static int no_ip_prefix_list_seq_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	long seq,\
	const char * seq_str __attribute__ ((unused)))
funcdecl_no_ip_prefix_list_seq;
DEFUN_CMD_FUNC_TEXT(no_ip_prefix_list_seq)
{
#if 2 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	long seq = 0;
	const char *seq_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "seq")) {
			seq_str = argv[_i]->arg;
			char *_end;
			seq = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!seq_str) {
		vty_out(vty, "Internal CLI error [%s]\n", "seq_str");
		return CMD_WARNING;
	}

	return no_ip_prefix_list_seq_magic(self, vty, argc, argv, prefix_list, seq, seq_str);
}

/* no_ip_prefix_list_all => "no ip prefix-list WORD" */
DEFUN_CMD_FUNC_DECL(no_ip_prefix_list_all)
#define funcdecl_no_ip_prefix_list_all static int no_ip_prefix_list_all_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_no_ip_prefix_list_all;
DEFUN_CMD_FUNC_TEXT(no_ip_prefix_list_all)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}

	return no_ip_prefix_list_all_magic(self, vty, argc, argv, prefix_list);
}

/* ip_prefix_list_sequence_number => "[no] ip prefix-list sequence-number" */
DEFUN_CMD_FUNC_DECL(ip_prefix_list_sequence_number)
#define funcdecl_ip_prefix_list_sequence_number static int ip_prefix_list_sequence_number_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * no)
funcdecl_ip_prefix_list_sequence_number;
DEFUN_CMD_FUNC_TEXT(ip_prefix_list_sequence_number)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *no = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "no")) {
			no = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return ip_prefix_list_sequence_number_magic(self, vty, argc, argv, no);
}

/* show_ip_prefix_list => "show ip prefix-list [WORD [seq$dseq (1-4294967295)$arg]]" */
DEFUN_CMD_FUNC_DECL(show_ip_prefix_list)
#define funcdecl_show_ip_prefix_list static int show_ip_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const char * dseq,\
	long arg,\
	const char * arg_str __attribute__ ((unused)))
funcdecl_show_ip_prefix_list;
DEFUN_CMD_FUNC_TEXT(show_ip_prefix_list)
{
#if 3 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	const char *dseq = NULL;
	long arg = 0;
	const char *arg_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dseq")) {
			dseq = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "arg")) {
			arg_str = argv[_i]->arg;
			char *_end;
			arg = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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

	return show_ip_prefix_list_magic(self, vty, argc, argv, prefix_list, dseq, arg, arg_str);
}

/* show_ip_prefix_list_prefix => "show ip prefix-list WORD A.B.C.D/M$prefix [longer$dl|first-match$dfm]" */
DEFUN_CMD_FUNC_DECL(show_ip_prefix_list_prefix)
#define funcdecl_show_ip_prefix_list_prefix static int show_ip_prefix_list_prefix_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const struct prefix_ipv4 * prefix,\
	const char * prefix_str __attribute__ ((unused)),\
	const char * dl,\
	const char * dfm)
funcdecl_show_ip_prefix_list_prefix;
DEFUN_CMD_FUNC_TEXT(show_ip_prefix_list_prefix)
{
#if 4 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	struct prefix_ipv4 prefix = { };
	const char *prefix_str = NULL;
	const char *dl = NULL;
	const char *dfm = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "prefix")) {
			prefix_str = argv[_i]->arg;
			_fail = !str2prefix_ipv4(argv[_i]->arg, &prefix);
		}
		if (!strcmp(argv[_i]->varname, "dl")) {
			dl = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dfm")) {
			dfm = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!prefix_str) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_str");
		return CMD_WARNING;
	}

	return show_ip_prefix_list_prefix_magic(self, vty, argc, argv, prefix_list, &prefix, prefix_str, dl, dfm);
}

/* show_ip_prefix_list_summary => "show ip prefix-list summary [WORD$prefix_list]" */
DEFUN_CMD_FUNC_DECL(show_ip_prefix_list_summary)
#define funcdecl_show_ip_prefix_list_summary static int show_ip_prefix_list_summary_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_show_ip_prefix_list_summary;
DEFUN_CMD_FUNC_TEXT(show_ip_prefix_list_summary)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return show_ip_prefix_list_summary_magic(self, vty, argc, argv, prefix_list);
}

/* show_ip_prefix_list_detail => "show ip prefix-list detail [WORD$prefix_list]" */
DEFUN_CMD_FUNC_DECL(show_ip_prefix_list_detail)
#define funcdecl_show_ip_prefix_list_detail static int show_ip_prefix_list_detail_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_show_ip_prefix_list_detail;
DEFUN_CMD_FUNC_TEXT(show_ip_prefix_list_detail)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return show_ip_prefix_list_detail_magic(self, vty, argc, argv, prefix_list);
}

/* clear_ip_prefix_list => "clear ip prefix-list [WORD [A.B.C.D/M$prefix]]" */
DEFUN_CMD_FUNC_DECL(clear_ip_prefix_list)
#define funcdecl_clear_ip_prefix_list static int clear_ip_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const struct prefix_ipv4 * prefix,\
	const char * prefix_str __attribute__ ((unused)))
funcdecl_clear_ip_prefix_list;
DEFUN_CMD_FUNC_TEXT(clear_ip_prefix_list)
{
#if 2 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	struct prefix_ipv4 prefix = { };
	const char *prefix_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "prefix")) {
			prefix_str = argv[_i]->arg;
			_fail = !str2prefix_ipv4(argv[_i]->arg, &prefix);
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

	return clear_ip_prefix_list_magic(self, vty, argc, argv, prefix_list, &prefix, prefix_str);
}

/* ipv6_prefix_list => "ipv6 prefix-list WORD [seq (1-4294967295)] <deny|permit>$action <any$dest|X:X::X:X/M$dest [{ge (0-128)|le (0-128)}]>" */
DEFUN_CMD_FUNC_DECL(ipv6_prefix_list)
#define funcdecl_ipv6_prefix_list static int ipv6_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	long seq,\
	const char * seq_str __attribute__ ((unused)),\
	const char * action,\
	const char * dest,\
	long ge,\
	const char * ge_str __attribute__ ((unused)),\
	long le,\
	const char * le_str __attribute__ ((unused)))
funcdecl_ipv6_prefix_list;
DEFUN_CMD_FUNC_TEXT(ipv6_prefix_list)
{
#if 6 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	long seq = 0;
	const char *seq_str = NULL;
	const char *action = NULL;
	const char *dest = NULL;
	long ge = 0;
	const char *ge_str = NULL;
	long le = 0;
	const char *le_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "seq")) {
			seq_str = argv[_i]->arg;
			char *_end;
			seq = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "action")) {
			action = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dest")) {
			dest = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "ge")) {
			ge_str = argv[_i]->arg;
			char *_end;
			ge = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "le")) {
			le_str = argv[_i]->arg;
			char *_end;
			le = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!action) {
		vty_out(vty, "Internal CLI error [%s]\n", "action");
		return CMD_WARNING;
	}
	if (!dest) {
		vty_out(vty, "Internal CLI error [%s]\n", "dest");
		return CMD_WARNING;
	}

	return ipv6_prefix_list_magic(self, vty, argc, argv, prefix_list, seq, seq_str, action, dest, ge, ge_str, le, le_str);
}

/* no_ipv6_prefix_list => "no ipv6 prefix-list WORD [seq (1-4294967295)] <deny|permit>$action <any$dest|X:X::X:X/M$dest [{ge (0-128)|le (0-128)}]>" */
DEFUN_CMD_FUNC_DECL(no_ipv6_prefix_list)
#define funcdecl_no_ipv6_prefix_list static int no_ipv6_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	long seq,\
	const char * seq_str __attribute__ ((unused)),\
	const char * action,\
	const char * dest,\
	long ge,\
	const char * ge_str __attribute__ ((unused)),\
	long le,\
	const char * le_str __attribute__ ((unused)))
funcdecl_no_ipv6_prefix_list;
DEFUN_CMD_FUNC_TEXT(no_ipv6_prefix_list)
{
#if 6 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	long seq = 0;
	const char *seq_str = NULL;
	const char *action = NULL;
	const char *dest = NULL;
	long ge = 0;
	const char *ge_str = NULL;
	long le = 0;
	const char *le_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "seq")) {
			seq_str = argv[_i]->arg;
			char *_end;
			seq = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "action")) {
			action = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dest")) {
			dest = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "ge")) {
			ge_str = argv[_i]->arg;
			char *_end;
			ge = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "le")) {
			le_str = argv[_i]->arg;
			char *_end;
			le = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!action) {
		vty_out(vty, "Internal CLI error [%s]\n", "action");
		return CMD_WARNING;
	}
	if (!dest) {
		vty_out(vty, "Internal CLI error [%s]\n", "dest");
		return CMD_WARNING;
	}

	return no_ipv6_prefix_list_magic(self, vty, argc, argv, prefix_list, seq, seq_str, action, dest, ge, ge_str, le, le_str);
}

/* no_ipv6_prefix_list_all => "no ipv6 prefix-list WORD" */
DEFUN_CMD_FUNC_DECL(no_ipv6_prefix_list_all)
#define funcdecl_no_ipv6_prefix_list_all static int no_ipv6_prefix_list_all_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_no_ipv6_prefix_list_all;
DEFUN_CMD_FUNC_TEXT(no_ipv6_prefix_list_all)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}

	return no_ipv6_prefix_list_all_magic(self, vty, argc, argv, prefix_list);
}

/* ipv6_prefix_list_sequence_number => "[no] ipv6 prefix-list sequence-number" */
DEFUN_CMD_FUNC_DECL(ipv6_prefix_list_sequence_number)
#define funcdecl_ipv6_prefix_list_sequence_number static int ipv6_prefix_list_sequence_number_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * no)
funcdecl_ipv6_prefix_list_sequence_number;
DEFUN_CMD_FUNC_TEXT(ipv6_prefix_list_sequence_number)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *no = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "no")) {
			no = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return ipv6_prefix_list_sequence_number_magic(self, vty, argc, argv, no);
}

/* show_ipv6_prefix_list => "show ipv6 prefix-list [WORD [seq$dseq (1-4294967295)$arg]]" */
DEFUN_CMD_FUNC_DECL(show_ipv6_prefix_list)
#define funcdecl_show_ipv6_prefix_list static int show_ipv6_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const char * dseq,\
	long arg,\
	const char * arg_str __attribute__ ((unused)))
funcdecl_show_ipv6_prefix_list;
DEFUN_CMD_FUNC_TEXT(show_ipv6_prefix_list)
{
#if 3 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	const char *dseq = NULL;
	long arg = 0;
	const char *arg_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dseq")) {
			dseq = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "arg")) {
			arg_str = argv[_i]->arg;
			char *_end;
			arg = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
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

	return show_ipv6_prefix_list_magic(self, vty, argc, argv, prefix_list, dseq, arg, arg_str);
}

/* show_ipv6_prefix_list_prefix => "show ipv6 prefix-list WORD X:X::X:X/M$prefix [longer$dl|first-match$dfm]" */
DEFUN_CMD_FUNC_DECL(show_ipv6_prefix_list_prefix)
#define funcdecl_show_ipv6_prefix_list_prefix static int show_ipv6_prefix_list_prefix_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const struct prefix_ipv6 * prefix,\
	const char * prefix_str __attribute__ ((unused)),\
	const char * dl,\
	const char * dfm)
funcdecl_show_ipv6_prefix_list_prefix;
DEFUN_CMD_FUNC_TEXT(show_ipv6_prefix_list_prefix)
{
#if 4 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	struct prefix_ipv6 prefix = { };
	const char *prefix_str = NULL;
	const char *dl = NULL;
	const char *dfm = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "prefix")) {
			prefix_str = argv[_i]->arg;
			_fail = !str2prefix_ipv6(argv[_i]->arg, &prefix);
		}
		if (!strcmp(argv[_i]->varname, "dl")) {
			dl = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "dfm")) {
			dfm = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
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
	if (!prefix_list) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_list");
		return CMD_WARNING;
	}
	if (!prefix_str) {
		vty_out(vty, "Internal CLI error [%s]\n", "prefix_str");
		return CMD_WARNING;
	}

	return show_ipv6_prefix_list_prefix_magic(self, vty, argc, argv, prefix_list, &prefix, prefix_str, dl, dfm);
}

/* show_ipv6_prefix_list_summary => "show ipv6 prefix-list summary [WORD$prefix-list]" */
DEFUN_CMD_FUNC_DECL(show_ipv6_prefix_list_summary)
#define funcdecl_show_ipv6_prefix_list_summary static int show_ipv6_prefix_list_summary_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_show_ipv6_prefix_list_summary;
DEFUN_CMD_FUNC_TEXT(show_ipv6_prefix_list_summary)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return show_ipv6_prefix_list_summary_magic(self, vty, argc, argv, prefix_list);
}

/* show_ipv6_prefix_list_detail => "show ipv6 prefix-list detail [WORD$prefix-list]" */
DEFUN_CMD_FUNC_DECL(show_ipv6_prefix_list_detail)
#define funcdecl_show_ipv6_prefix_list_detail static int show_ipv6_prefix_list_detail_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list)
funcdecl_show_ipv6_prefix_list_detail;
DEFUN_CMD_FUNC_TEXT(show_ipv6_prefix_list_detail)
{
#if 1 /* anything to parse? */
	int _i;
#if 0 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 0 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
#if 0 /* anything that can fail? */
		if (_fail)
			vty_out (vty, "%% invalid input for %s: %s\n",
				   argv[_i]->varname, argv[_i]->arg);
		_failcnt += _fail;
#endif
	}
#if 0 /* anything that can fail? */
	if (_failcnt)
		return CMD_WARNING;
#endif
#endif

	return show_ipv6_prefix_list_detail_magic(self, vty, argc, argv, prefix_list);
}

/* clear_ipv6_prefix_list => "clear ipv6 prefix-list [WORD [X:X::X:X/M$prefix]]" */
DEFUN_CMD_FUNC_DECL(clear_ipv6_prefix_list)
#define funcdecl_clear_ipv6_prefix_list static int clear_ipv6_prefix_list_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	const char * prefix_list,\
	const struct prefix_ipv6 * prefix,\
	const char * prefix_str __attribute__ ((unused)))
funcdecl_clear_ipv6_prefix_list;
DEFUN_CMD_FUNC_TEXT(clear_ipv6_prefix_list)
{
#if 2 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	const char *prefix_list = NULL;
	struct prefix_ipv6 prefix = { };
	const char *prefix_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "prefix_list")) {
			prefix_list = (argv[_i]->type == WORD_TKN) ? argv[_i]->text : argv[_i]->arg;
		}
		if (!strcmp(argv[_i]->varname, "prefix")) {
			prefix_str = argv[_i]->arg;
			_fail = !str2prefix_ipv6(argv[_i]->arg, &prefix);
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

	return clear_ipv6_prefix_list_magic(self, vty, argc, argv, prefix_list, &prefix, prefix_str);
}

