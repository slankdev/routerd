/* setsockopt functions
 * Copyright (C) 1999 Kunihiro Ishiguro
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

#ifdef SUNOS_5
#include <ifaddrs.h>
#endif

#include "sockopt.h"
#include "sockunion.h"

void setsockopt_so_recvbuf(int sock, int size)
{
	int orig_req = size;

	while (setsockopt(sock, SOL_SOCKET, SO_RCVBUF, &size, sizeof(size))
	       == -1)
		size /= 2;

	if (size != orig_req)
		fprintf(stderr,
			 "%s: fd %d: SO_RCVBUF set to %d (requested %d)",
			 __func__, sock, size, orig_req);
}

void setsockopt_so_sendbuf(const int sock, int size)
{
	int orig_req = size;

	while (setsockopt(sock, SOL_SOCKET, SO_SNDBUF, &size, sizeof(size))
	       == -1)
		size /= 2;

	if (size != orig_req)
		fprintf(stderr,
			 "%s: fd %d: SO_SNDBUF set to %d (requested %d)",
			 __func__, sock, size, orig_req);
}

int getsockopt_so_sendbuf(const int sock)
{
	uint32_t optval;
	socklen_t optlen = sizeof(optval);
	int ret = getsockopt(sock, SOL_SOCKET, SO_SNDBUF, (char *)&optval,
			     &optlen);
	if (ret < 0) {
		fprintf(stderr,
			     "fd %d: can't getsockopt SO_SNDBUF: %d (%s)", sock,
			     errno, strerror(errno));
		return ret;
	}
	return optval;
}

static void *getsockopt_cmsg_data(struct msghdr *msgh, int level, int type)
{
	struct cmsghdr *cmsg;
	void *ptr = NULL;

	for (cmsg = CMSG_FIRSTHDR(msgh); cmsg != NULL;
	     cmsg = CMSG_NXTHDR(msgh, cmsg))
		if (cmsg->cmsg_level == level && cmsg->cmsg_type == type)
			return (ptr = CMSG_DATA(cmsg));

	return NULL;
}

static int getsockopt_ipv6_ifindex(struct msghdr *msgh)
{
	struct in6_pktinfo *pktinfo;

	pktinfo = getsockopt_cmsg_data(msgh, IPPROTO_IPV6, IPV6_PKTINFO);

	return pktinfo->ipi6_ifindex;
}

static int setsockopt_ipv4_ifindex(int sock, ifindex_t val)
{
	int ret;

#if defined(IP_PKTINFO)
	if ((ret = setsockopt(sock, IPPROTO_IP, IP_PKTINFO, &val, sizeof(val)))
	    < 0)
		fprintf(stderr,
			 "Can't set IP_PKTINFO option for fd %d to %d: %s",
			 sock, val, strerror(errno));
#elif defined(IP_RECVIF)
	if ((ret = setsockopt(sock, IPPROTO_IP, IP_RECVIF, &val, sizeof(val)))
	    < 0)
		fprintf(stderr,
			 "Can't set IP_RECVIF option for fd %d to %d: %s", sock,
			 val, strerror(errno));
#else
#warning "Neither IP_PKTINFO nor IP_RECVIF is available."
#warning "Will not be able to receive link info."
#warning "Things might be seriously broken.."
	/* XXX Does this ever happen?  Should there be a zlog_warn message here?
	 */
	ret = -1;
#endif
	return ret;
}

int setsockopt_ipv4_tos(int sock, int tos)
{
	int ret;

	ret = setsockopt(sock, IPPROTO_IP, IP_TOS, &tos, sizeof(tos));
	if (ret < 0)
		fprintf(stderr,
			 "Can't set IP_TOS option for fd %d to %#x: %s", sock,
			 tos, strerror(errno));
	return ret;
}

/*
 * Requires: msgh is not NULL and points to a valid struct msghdr, which
 * may or may not have control data about the incoming interface.
 *
 * Returns the interface index (small integer >= 1) if it can be
 * determined, or else 0.
 */
static ifindex_t getsockopt_ipv4_ifindex(struct msghdr *msgh)
{
	ifindex_t ifindex;

#if defined(IP_PKTINFO)
	/* Linux pktinfo based ifindex retrieval */
	struct in_pktinfo *pktinfo;

	pktinfo = (struct in_pktinfo *)getsockopt_cmsg_data(msgh, IPPROTO_IP,
							    IP_PKTINFO);

	/* getsockopt_ifindex() will forward this, being 0 "not found" */
	if (pktinfo == NULL)
		return 0;

	ifindex = pktinfo->ipi_ifindex;

#elif defined(IP_RECVIF)

/* retrieval based on IP_RECVIF */

#ifndef SUNOS_5
	/* BSD systems use a sockaddr_dl as the control message payload. */
	struct sockaddr_dl *sdl;
#else
	/* SUNOS_5 uses an integer with the index. */
	ifindex_t *ifindex_p;
#endif /* SUNOS_5 */

#ifndef SUNOS_5
	/* BSD */
	sdl = (struct sockaddr_dl *)getsockopt_cmsg_data(msgh, IPPROTO_IP,
							 IP_RECVIF);
	if (sdl != NULL)
		ifindex = sdl->sdl_index;
	else
		ifindex = 0;
#else
	/*
	 * Solaris.  On Solaris 8, IP_RECVIF is defined, but the call to
	 * enable it fails with errno=99, and the struct msghdr has
	 * controllen 0.
	 */
	ifindex_p = (uint_t *)getsockopt_cmsg_data(msgh, IPPROTO_IP, IP_RECVIF);
	if (ifindex_p != NULL)
		ifindex = *ifindex_p;
	else
		ifindex = 0;
#endif /* SUNOS_5 */

#else
/*
 * Neither IP_PKTINFO nor IP_RECVIF defined - warn at compile time.
 * XXX Decide if this is a core service, or if daemons have to cope.
 * Since Solaris 8 and OpenBSD seem not to provide it, it seems that
 * daemons have to cope.
 */
#warning "getsockopt_ipv4_ifindex: Neither IP_PKTINFO nor IP_RECVIF defined."
#warning "Some daemons may fail to operate correctly!"
	ifindex = 0;

#endif /* IP_PKTINFO */

	return ifindex;
}

/* return ifindex, 0 if none found */
ifindex_t getsockopt_ifindex(int af, struct msghdr *msgh)
{
	switch (af) {
	case AF_INET:
		return (getsockopt_ipv4_ifindex(msgh));
		break;
	case AF_INET6:
		return (getsockopt_ipv6_ifindex(msgh));
		break;
	default:
		fprintf(stderr,
			 "getsockopt_ifindex: unknown address family %d", af);
		return 0;
	}
}

/* swab iph between order system uses for IP_HDRINCL and host order */
void sockopt_iphdrincl_swab_htosys(struct ip *iph)
{
/* BSD and derived take iph in network order, except for
 * ip_len and ip_off
 */
#ifndef HAVE_IP_HDRINCL_BSD_ORDER
	iph->ip_len = htons(iph->ip_len);
	iph->ip_off = htons(iph->ip_off);
#endif /* HAVE_IP_HDRINCL_BSD_ORDER */

	iph->ip_id = htons(iph->ip_id);
}

void sockopt_iphdrincl_swab_systoh(struct ip *iph)
{
#ifndef HAVE_IP_HDRINCL_BSD_ORDER
	iph->ip_len = ntohs(iph->ip_len);
	iph->ip_off = ntohs(iph->ip_off);
#endif /* HAVE_IP_HDRINCL_BSD_ORDER */

	iph->ip_id = ntohs(iph->ip_id);
}

int sockopt_tcp_rtt(int sock)
{
#ifdef TCP_INFO
	struct tcp_info ti;
	socklen_t len = sizeof(ti);

	if (getsockopt(sock, IPPROTO_TCP, TCP_INFO, &ti, &len) != 0)
		return 0;

	return ti.tcpi_rtt / 1000;
#else
	return 0;
#endif
}

int sockopt_tcp_signature_ext(int sock, union sockunion *su, uint16_t prefixlen,
			      const char *password)
{
#ifndef HAVE_DECL_TCP_MD5SIG
	/*
	 * We have been asked to enable MD5 auth for an address, but our
	 * platform doesn't support that
	 */
	return -2;
#endif

#ifndef TCP_MD5SIG_EXT
	/*
	 * We have been asked to enable MD5 auth for a prefix, but our platform
	 * doesn't support that
	 */
	if (prefixlen > 0)
		return -2;
#endif

#if HAVE_DECL_TCP_MD5SIG
	int ret;

	int optname = TCP_MD5SIG;
#ifndef GNU_LINUX
	/*
	 * XXX Need to do PF_KEY operation here to add/remove an SA entry,
	 * and add/remove an SP entry for this peer's packet flows also.
	 */
	int md5sig = password && *password ? 1 : 0;
#else
	int keylen = password ? strlen(password) : 0;
	struct tcp_md5sig md5sig;
	union sockunion *su2, *susock;

	/* Figure out whether the socket and the sockunion are the same family..
	 * adding AF_INET to AF_INET6 needs to be v4 mapped, you'd think..
	 */
	if (!(susock = sockunion_getsockname(sock)))
		return -1;

	if (susock->sa.sa_family == su->sa.sa_family)
		su2 = su;
	else {
		/* oops.. */
		su2 = susock;

		if (su2->sa.sa_family == AF_INET) {
			sockunion_free(susock);
			return 0;
		}

		/* If this does not work, then all users of this sockopt will
		 * need to
		 * differentiate between IPv4 and IPv6, and keep seperate
		 * sockets for
		 * each.
		 *
		 * Sadly, it doesn't seem to work at present. It's unknown
		 * whether
		 * this is a bug or not.
		 */
		if (su2->sa.sa_family == AF_INET6
		    && su->sa.sa_family == AF_INET) {
			su2->sin6.sin6_family = AF_INET6;
			/* V4Map the address */
			memset(&su2->sin6.sin6_addr, 0,
			       sizeof(struct in6_addr));
			su2->sin6.sin6_addr.s6_addr32[2] = htonl(0xffff);
			memcpy(&su2->sin6.sin6_addr.s6_addr32[3],
			       &su->sin.sin_addr, 4);
		}
	}

	memset(&md5sig, 0, sizeof(md5sig));
	memcpy(&md5sig.tcpm_addr, su2, sizeof(*su2));

	md5sig.tcpm_keylen = keylen;
	if (keylen)
		memcpy(md5sig.tcpm_key, password, keylen);
	sockunion_free(susock);

	/*
	 * Handle support for MD5 signatures on prefixes, if available and
	 * requested. Technically the #ifdef check below is not needed because
	 * if prefixlen > 0 and we don't have support for this feature we would
	 * have already returned by now, but leaving it there to be explicit.
	 */
#ifdef TCP_MD5SIG_EXT
	if (prefixlen > 0) {
		md5sig.tcpm_prefixlen = prefixlen;
		md5sig.tcpm_flags = TCP_MD5SIG_FLAG_PREFIX;
		optname = TCP_MD5SIG_EXT;
	}
#endif /* TCP_MD5SIG_EXT */

#endif /* GNU_LINUX */

	if ((ret = setsockopt(sock, IPPROTO_TCP, optname, &md5sig,
			      sizeof md5sig))
	    < 0) {
		/* ENOENT is harmless.  It is returned when we clear a password
		   for which
		   one was not previously set. */
		if (ENOENT == errno)
			ret = 0;
		else
			fprintf(stderr,
				"sockopt_tcp_signature: setsockopt(%d): %s",
				sock, strerror(errno));
	}
	return ret;
#endif /* HAVE_TCP_MD5SIG */

	/*
	 * Making compiler happy.  If we get to this point we probably
	 * have done something really really wrong.
	 */
	return -2;
}

int sockopt_tcp_signature(int sock, union sockunion *su, const char *password)
{
	return sockopt_tcp_signature_ext(sock, su, 0, password);
}
