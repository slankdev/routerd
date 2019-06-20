/*
 * CLI/command dummy handling tester
 *
 * Copyright (C) 2015 by David Lamparter,
 *                   for Open Source Routing / NetDEF, Inc.
 *
 * Quagga is free software; you can redistribute it and/or modify it
 * under the terms of the GNU General Public License as published by the
 * Free Software Foundation; either version 2, or (at your option) any
 * later version.
 *
 * Quagga is distributed in the hope that it will be useful, but
 * WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; see the file COPYING; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA 02110-1301 USA
 */

#include <zebra.h>

#include "prefix.h"
#include "common_cli.h"

DUMMY_DEFUN(cmd0, "slank ipv4 A.B.C.D");
DUMMY_DEFUN(cmd1, "show int");
/* DUMMY_DEFUN(cmd0, "arg ipv4 A.B.C.D"); */
/* DUMMY_DEFUN(cmd1, "arg ipv4m A.B.C.D/M"); */
DUMMY_DEFUN(cmd2, "arg ipv6 X:X::X:X$foo");
DUMMY_DEFUN(cmd3, "arg ipv6m X:X::X:X/M");
DUMMY_DEFUN(cmd4, "arg range (5-15)");
DUMMY_DEFUN(cmd5, "pat a < a|b>");
DUMMY_DEFUN(cmd6, "pat b  <a|b A.B.C.D$bar>");
DUMMY_DEFUN(cmd7, "pat c <a | b|c> A.B.C.D");
DUMMY_DEFUN(cmd8, "pat d {  foo A.B.C.D$foo|bar   X:X::X:X$bar| baz } [final]");
DUMMY_DEFUN(cmd9, "pat e [ WORD ]");
DUMMY_DEFUN(cmd10, "pat f [key]");
DUMMY_DEFUN(cmd11, "alt a WORD");
DUMMY_DEFUN(cmd12, "alt a A.B.C.D");
DUMMY_DEFUN(cmd13, "alt a X:X::X:X");
DUMMY_DEFUN(cmd14,
	    "pat g {  foo A.B.C.D$foo|foo|bar   X:X::X:X$bar| baz } [final]");

/* magic_test => "magic (0-100) {ipv4net A.B.C.D/M|X:X::X:X$ipv6}" */
DEFUN_CMD_FUNC_DECL(magic_test)
#define funcdecl_magic_test static int magic_test_magic(\
	const struct cmd_element *self __attribute__ ((unused)),\
	struct vty *vty __attribute__ ((unused)),\
	int argc __attribute__ ((unused)),\
	struct cmd_token *argv[] __attribute__ ((unused)),\
	long magic,\
	const char * magic_str __attribute__ ((unused)),\
	const struct prefix_ipv4 * ipv4net,\
	const char * ipv4net_str __attribute__ ((unused)),\
	struct in6_addr ipv6,\
	const char * ipv6_str __attribute__ ((unused)))
funcdecl_magic_test;
DEFUN_CMD_FUNC_TEXT(magic_test)
{
#if 3 /* anything to parse? */
	int _i;
#if 1 /* anything that can fail? */
	unsigned _fail = 0, _failcnt = 0;
#endif
	long magic = 0;
	const char *magic_str = NULL;
	struct prefix_ipv4 ipv4net = { };
	const char *ipv4net_str = NULL;
	struct in6_addr ipv6 = {};
	const char *ipv6_str = NULL;

	for (_i = 0; _i < argc; _i++) {
		if (!argv[_i]->varname)
			continue;
#if 1 /* anything that can fail? */
		_fail = 0;
#endif

		if (!strcmp(argv[_i]->varname, "magic")) {
			magic_str = argv[_i]->arg;
			char *_end;
			magic = strtol(argv[_i]->arg, &_end, 10);
			_fail = (_end == argv[_i]->arg) || (*_end != '\0');
		}
		if (!strcmp(argv[_i]->varname, "ipv4net")) {
			ipv4net_str = argv[_i]->arg;
			_fail = !str2prefix_ipv4(argv[_i]->arg, &ipv4net);
		}
		if (!strcmp(argv[_i]->varname, "ipv6")) {
			ipv6_str = argv[_i]->arg;
			_fail = !inet_pton(AF_INET6, argv[_i]->arg, &ipv6);
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
	if (!magic_str) {
		vty_out(vty, "Internal CLI error [%s]\n", "magic_str");
		return CMD_WARNING;
	}

	return magic_test_magic(self, vty, argc, argv, magic, magic_str, &ipv4net, ipv4net_str, ipv6, ipv6_str);
}

DEFPY(magic_test, magic_test_cmd,
	"magic (0-100) {ipv4net A.B.C.D/M|X:X::X:X$ipv6}",
	"1\n2\n3\n4\n5\n")
{
	char buf[256];
	vty_out(vty, "def: %s\n", self->string);
	vty_out(vty, "num: %ld\n", magic);
	vty_out(vty, "ipv4: %s\n", prefix2str(ipv4net, buf, sizeof(buf)));
	vty_out(vty, "ipv6: %s\n",
		inet_ntop(AF_INET6, &ipv6, buf, sizeof(buf)));
	return CMD_SUCCESS;
}

void test_init(int argc, char **argv)
{
	size_t repeat = argc > 1 ? strtoul(argv[1], NULL, 0) : 223;

	install_element(ENABLE_NODE, &cmd0_cmd);
	install_element(ENABLE_NODE, &cmd1_cmd);
	install_element(ENABLE_NODE, &cmd2_cmd);
	install_element(ENABLE_NODE, &cmd3_cmd);
	install_element(ENABLE_NODE, &cmd4_cmd);
	install_element(ENABLE_NODE, &cmd5_cmd);
	install_element(ENABLE_NODE, &cmd6_cmd);
	install_element(ENABLE_NODE, &cmd7_cmd);
	install_element(ENABLE_NODE, &cmd8_cmd);
	install_element(ENABLE_NODE, &cmd9_cmd);
	install_element(ENABLE_NODE, &cmd10_cmd);
	install_element(ENABLE_NODE, &cmd11_cmd);
	install_element(ENABLE_NODE, &cmd12_cmd);
	install_element(ENABLE_NODE, &cmd13_cmd);
	for (size_t i = 0; i < repeat; i++) {
		uninstall_element(ENABLE_NODE, &cmd5_cmd);
		install_element(ENABLE_NODE, &cmd5_cmd);
	}
	for (size_t i = 0; i < repeat; i++) {
		uninstall_element(ENABLE_NODE, &cmd13_cmd);
		install_element(ENABLE_NODE, &cmd13_cmd);
	}
	install_element(ENABLE_NODE, &cmd14_cmd);
	install_element(ENABLE_NODE, &magic_test_cmd);
}

