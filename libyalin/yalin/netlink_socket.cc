
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <time.h>
#include <net/if.h>
#include <net/if_arp.h>
#include <netinet/in.h>
#include <asm/types.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <sys/uio.h>
#include <sys/socket.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/genetlink.h>

#include <vector>
#include <string>
#include "netlink_socket.h"

#include <slankdev/hexdump.h>

int
netlink_listen_until_done(netlink_t *rtnl,
    rtnl_listen_filter_t handler, void *jarg)
{
  struct sockaddr_nl sa;
  sa.nl_family = AF_NETLINK;
  struct iovec iov;
  struct msghdr msg;
  memset(&msg, 0x0, sizeof(struct msghdr));
  msg.msg_name = &sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char   buf[16384];
  iov.iov_base = buf;
  while (true) {
    struct rtnl_ctrl_data ctrl;
    iov.iov_len = sizeof(buf);
    int status = recvmsg(rtnl->fd, &msg, 0);
    if (status < 0) {
      if (errno == EINTR
       || errno == EAGAIN
       || errno == ENOBUFS)
        continue;
      return -1;
    }
    if (status == 0) {
      fprintf(stderr, "EOF on netlink\n");
      return -1;
    }

    for (struct nlmsghdr* h = (struct nlmsghdr *)buf;
         size_t(status) >= sizeof(*h);) {
      if (h->nlmsg_type == NLMSG_ERROR ||
          h->nlmsg_type == NLMSG_DONE)
        return 0;

      int len = h->nlmsg_len;
      int err = handler(&sa, &ctrl, h, jarg);
      if (err < 0)
        return err;

      status -= NLMSG_ALIGN(len);
      h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
    }

    if (msg.msg_flags & MSG_TRUNC) {
      // Message truncated
      continue;
    }
    if (status) {
      // Remnant of size
      exit(1);
    }
  }
}

int
ge_netlink_listen(ge_netlink_t *rtnl,
    rtnl_listen_filter_t handler, void *jarg)
{
  struct sockaddr_nl sa;
  sa.nl_family = AF_NETLINK;
  struct iovec iov;
  struct msghdr msg;
  memset(&msg, 0x0, sizeof(struct msghdr));
  msg.msg_name = &sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char   buf[16384];
  iov.iov_base = buf;
  while (true) {
    struct rtnl_ctrl_data ctrl;
    iov.iov_len = sizeof(buf);
    int status = recvmsg(rtnl->fd, &msg, 0);
    if (status < 0) {
      if (errno == EINTR
       || errno == EAGAIN
       || errno == ENOBUFS)
        continue;
      return -1;
    }
    if (status == 0) {
      fprintf(stderr, "EOF on netlink\n");
      return -1;
    }

    for (struct nlmsghdr* h = (struct nlmsghdr *)buf;
         size_t(status) >= sizeof(*h);) {
      int len = h->nlmsg_len;
      int err = handler(&sa, &ctrl, h, jarg);
      if (err < 0)
        return err;

      status -= NLMSG_ALIGN(len);
      h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
    }

    if (msg.msg_flags & MSG_TRUNC) {
      // Message truncated
      continue;
    }
    if (status) {
      // Remnant of size
      exit(1);
    }
  }
}

int
netlink_listen(netlink_t *rtnl,
    rtnl_listen_filter_t handler, void *jarg)
{
  struct sockaddr_nl sa;
  sa.nl_family = AF_NETLINK;
  struct iovec iov;
  struct msghdr msg;
  memset(&msg, 0x0, sizeof(struct msghdr));
  msg.msg_name = &sa;
  msg.msg_namelen = sizeof(sa);
  msg.msg_iov = &iov;
  msg.msg_iovlen = 1;

  char   buf[16384];
  iov.iov_base = buf;
  while (true) {
    struct rtnl_ctrl_data ctrl;
    iov.iov_len = sizeof(buf);
    int status = recvmsg(rtnl->fd, &msg, 0);
    if (status < 0) {
      if (errno == EINTR
       || errno == EAGAIN
       || errno == ENOBUFS)
        continue;
      return -1;
    }
    if (status == 0) {
      fprintf(stderr, "EOF on netlink\n");
      return -1;
    }

    for (struct nlmsghdr* h = (struct nlmsghdr *)buf;
         size_t(status) >= sizeof(*h);) {
      int len = h->nlmsg_len;
      int err = handler(&sa, &ctrl, h, jarg);
      if (err < 0)
        return err;

      status -= NLMSG_ALIGN(len);
      h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
    }

    if (msg.msg_flags & MSG_TRUNC) {
      // Message truncated
      continue;
    }
    if (status) {
      // Remnant of size
      exit(1);
    }
  }
}

static int parse_rtattr(struct rtattr *tb[], int max, struct rtattr *rta,
		       int len)
{
	unsigned short type;
	uint16_t flags = 0;

	memset(tb, 0, sizeof(struct rtattr *) * (max + 1));
	while (RTA_OK(rta, len)) {
		type = rta->rta_type & ~flags;
		if ((type <= max) && (!tb[type]))
			tb[type] = rta;
		rta = RTA_NEXT(rta, len);
	}
	if (len)
		fprintf(stderr, "!!!Deficit %d, rta_len=%d\n",
			len, rta->rta_len);
	return 0;
}

static inline __u16 rta_getattr_u16(const struct rtattr *rta)
{
	return *(__u16 *)RTA_DATA(rta);
}

static int genl_parse_getfamily(struct nlmsghdr *nlh)
{
	printf("%s\n", __func__);
	struct rtattr *tb[CTRL_ATTR_MAX + 1];
	struct genlmsghdr *ghdr = (struct genlmsghdr *)NLMSG_DATA(nlh);
	int len = nlh->nlmsg_len;
	struct rtattr *attrs;

	if (nlh->nlmsg_type != GENL_ID_CTRL) {
		fprintf(stderr, "Not a controller message, nlmsg_len=%d "
			"nlmsg_type=0x%x\n", nlh->nlmsg_len, nlh->nlmsg_type);
		return -1;
	}

	len -= NLMSG_LENGTH(GENL_HDRLEN);

	if (len < 0) {
		fprintf(stderr, "wrong controller message len %d\n", len);
		return -1;
	}

	if (ghdr->cmd != CTRL_CMD_NEWFAMILY) {
		fprintf(stderr, "Unknown controller command %d\n", ghdr->cmd);
		return -1;
	}

	attrs = (struct rtattr *) ((char *) ghdr + GENL_HDRLEN);
	parse_rtattr(tb, CTRL_ATTR_MAX, attrs, len);

	if (tb[CTRL_ATTR_FAMILY_ID] == NULL) {
		fprintf(stderr, "Missing family id TLV\n");
		return -1;
	}

	return rta_getattr_u16(tb[CTRL_ATTR_FAMILY_ID]);
}

static int addattr_l(struct nlmsghdr *n, int maxlen, int type, const void *data,
	      int alen)
{
#define NLMSG_TAIL(nmsg) \
	((struct rtattr *) (((uint8_t *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

	int len = RTA_LENGTH(alen);
	struct rtattr *rta;

	if ((size_t)(NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len)) > (size_t)maxlen) {
		fprintf(stderr,
			"addattr_l ERROR: message exceeded bound of %d\n",
			maxlen);
		return -1;
	}
	rta = NLMSG_TAIL(n);
	rta->rta_type = type;
	rta->rta_len = len;
	if (alen)
		memcpy(RTA_DATA(rta), data, alen);
	n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len);
	return 0;
}

static char buf[100000];
int rtnl_talk(int fd, struct nlmsghdr *n, struct nlmsghdr **answer)
{
	struct iovec iov = {
		.iov_base = n,
		.iov_len = n->nlmsg_len
	};

	struct nlmsghdr *h = (struct nlmsghdr*)iov.iov_base;
	h->nlmsg_seq = random();
	if (answer == NULL)
		h->nlmsg_flags |= NLM_F_ACK;

	printf("%s: before sendmsg\n", __func__);
	int write_ret = write(fd, &iov.iov_base, iov.iov_len);
	if (write_ret < 0) {
		perror("Cannot talk to rtnetlink");
		return -1;
	}
	printf("%s: after sendmsg ret=%d\n", __func__, write_ret);

	printf("%s: before recvmsg\n", __func__);
	int read_ret = read(fd, buf, sizeof(buf));
	if (read_ret < 0) {
		perror("Cannot read");
		return -1;
	}
	printf("%s: after recvmsg status=%d\n", __func__, read_ret);

	int status = read_ret;
	for (struct nlmsghdr *h = (struct nlmsghdr *)buf; (size_t)status >= sizeof(*h); ) {
		printf("%s: for\n", __func__);
		int len = h->nlmsg_len;
		int l = len - sizeof(*h);

		if (l < 0 || len > status) {
			fprintf(stderr,
				"!!!malformed message: len=%d\n",
				len);
			exit(1);
		}

		if (h->nlmsg_type == NLMSG_ERROR) {
			struct nlmsgerr *err = (struct nlmsgerr *)NLMSG_DATA(h);
			int error = err->error;

			if ((size_t)l < sizeof(struct nlmsgerr)) {
				fprintf(stderr, "ERROR truncated\n");
				//free(buf);
				return -1;
			}

			if (answer)
				*answer = (struct nlmsghdr *)buf;

			return error;
		}

		if (answer) {
			*answer = (struct nlmsghdr *)buf;
			return 0;
		}
		status -= NLMSG_ALIGN(len);
		h = (struct nlmsghdr *)((char *)h + NLMSG_ALIGN(len));
	}

	printf("%s: after 100\n", __func__);
	//free(buf);

	printf("%s: after 200\n", __func__);
	return 0;
}

static int
genl_resolve_family(int fd, const char *family)
{
	struct {
		struct nlmsghdr		n;
		struct genlmsghdr	g;
		char   buf[1024];
	} req;
	req.n.nlmsg_type = GENL_ID_CTRL;
	req.n.nlmsg_flags = NLM_F_REQUEST;
	req.n.nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN);
	req.g.cmd = CTRL_CMD_GETFAMILY;
	req.g.version = 0;

	struct nlmsghdr *answer = NULL;

	addattr_l(&req.n, sizeof(req), CTRL_ATTR_FAMILY_NAME,
		  family, strlen(family) + 1);

	slankdev::hexdump(stdout, &req.n, 64);

	if (rtnl_talk(fd, &req.n, &answer) < 0) {
		fprintf(stderr, "Error talking to the kernel\n");
		return -2;
	}
	printf("%s: answer=%p\n", __func__, answer);

	//int fnum = 0;
	int fnum = genl_parse_getfamily(answer);
	//free(answer);
	printf("%s: fnum=%d\n", __func__, fnum);
	return fnum;
}

ge_netlink_t*
ge_netlink_open(const char* family_str)
{
	printf("%s: unimplmeneted family=%s\n", __func__, family_str);
	ge_netlink_t *nl = (ge_netlink_t*)malloc(sizeof(ge_netlink_t));

	nl->proto = NETLINK_GENERIC;
  nl->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, NETLINK_GENERIC);
  if (nl->fd < 0) {
    perror("socket");
    return NULL;
  }

	strcpy(nl->family, family_str);
	nl->genl_family = genl_resolve_family(nl->fd, family_str);
	if (nl->genl_family < 0) {
    perror("socket");
    return NULL;
	}
	//printf("fnum: %u\n", nl->genl_family);

	return nl;
}

netlink_t*
netlink_open(uint32_t subscriptions, int32_t protocol)
{
  netlink_t *nl = (netlink_t*)malloc(sizeof(netlink_t));
  memset(nl, 0, sizeof(*nl));
  nl->proto = protocol;
  nl->fd = socket(AF_NETLINK, SOCK_RAW | SOCK_CLOEXEC, protocol);
  if (nl->fd < 0) {
    perror("socket");
    return NULL;
  }

  int sendbuf = 32768;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_SNDBUF,
           &sendbuf, sizeof(sendbuf)) < 0) {
    perror("setsockopt(SO_SNDBUF)");
    return NULL;
  }

  int recvbuf = 1024 * 1024;
  if (setsockopt(nl->fd, SOL_SOCKET, SO_RCVBUF,
           &recvbuf, sizeof(recvbuf)) < 0) {
    perror("setsockopt(SO_RCVBUF)");
    return NULL;
  }

  int one = 1;
  setsockopt(nl->fd, SOL_NETLINK,
      NETLINK_EXT_ACK, &one, sizeof(one));

  memset(&nl->local, 0, sizeof(nl->local));
  nl->local.nl_family = AF_NETLINK;
  nl->local.nl_groups = subscriptions;
  if (bind(nl->fd, (struct sockaddr *)&nl->local,
     sizeof(nl->local)) < 0) {
    perror("bind local addr");
    return NULL;
  }

  socklen_t addr_len = sizeof(nl->local);
  if (getsockname(nl->fd, (struct sockaddr *)&nl->local,
      &addr_len) < 0) {
    perror("getsockname");
    return NULL;
  }

  return nl;
}

void
ge_netlink_close(ge_netlink_t *nl)
{
  free(nl);
}

void
netlink_close(netlink_t *nl)
{
  if (nl->fd >= 0) {
    close(nl->fd);
    nl->fd = -1;
  }
  free(nl);
}

