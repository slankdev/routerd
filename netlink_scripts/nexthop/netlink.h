#ifndef _NETLINK_H_
#define _NETLINK_H_

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <linux/genetlink.h>

#define GENL_REQUEST(_req, _bufsiz, _family, _hdrsiz, _ver, _cmd, _flags) \
struct {                                                \
  struct nlmsghdr    n;                                 \
  struct genlmsghdr  g;                                 \
  char buf[NLMSG_ALIGN(_hdrsiz) + (_bufsiz)];           \
} _req = {                                              \
  .n = {                                                \
    .nlmsg_type = (_family),                            \
    .nlmsg_flags = (_flags),                            \
    .nlmsg_len = NLMSG_LENGTH(GENL_HDRLEN + (_hdrsiz)), \
  },                                                    \
  .g = {                                                \
    .cmd = (_cmd),                                      \
    .version = (_ver),                                  \
  },                                                    \
}

#define NLMSG_TAIL(nmsg) \
  ((struct rtattr *) (((void *) (nmsg)) + NLMSG_ALIGN((nmsg)->nlmsg_len)))

#define RTA_TAIL(rta) \
	((struct rtattr *) (((void *) (rta)) + \
					RTA_ALIGN((rta)->rta_len)))

static inline __u8 rta_getattr_u8(const struct rtattr *rta) { return *(__u8 *)RTA_DATA(rta); }
static inline __u16 rta_getattr_u16(const struct rtattr *rta) { return *(__u16 *)RTA_DATA(rta); }
static inline __u32 rta_getattr_u32(const struct rtattr *rta) { return *(__u32 *)RTA_DATA(rta); }
static inline __u64 rta_getattr_u64(const struct rtattr *rta) { return *(__u64 *)RTA_DATA(rta); }
static inline __s8 rta_getattr_s8(const struct rtattr *rta) { return *(__s8 *)RTA_DATA(rta); }
static inline __u16 rta_getattr_s16(const struct rtattr *rta) { return *(__s16 *)RTA_DATA(rta); }
static inline __s32 rta_getattr_s32(const struct rtattr *rta) { return *(__s32 *)RTA_DATA(rta); }
static inline __s64 rta_getattr_s64(const struct rtattr *rta) { return *(__s64 *)RTA_DATA(rta); }
static inline const char *rta_getattr_str(const struct rtattr *rta) { return (const char *)RTA_DATA(rta); }

static inline int
parse_rtattr_flags(struct rtattr *tb[],
    int max, struct rtattr *rta,
    int len, unsigned short flags)
{
  unsigned short type;
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

static inline int
parse_rtattr(struct rtattr *tb[],
    int max, struct rtattr *rta, int len)
{ return parse_rtattr_flags(tb, max, rta, len, 0); }

static inline int
addattr_l(struct nlmsghdr *n, int maxlen,
    int type, const void *data, int alen)
{
  int len = RTA_LENGTH(alen);
  struct rtattr *rta;

  if (NLMSG_ALIGN(n->nlmsg_len) + RTA_ALIGN(len) > maxlen) {
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

static inline int
addattr(struct nlmsghdr *n, int maxlen, int type)
{ return addattr_l(n, maxlen, type, NULL, 0); }
static inline int
addattr8(struct nlmsghdr *n, int maxlen, int type, __u8 data)
{ return addattr_l(n, maxlen, type, &data, sizeof(__u8)); }
static inline int
addattr16(struct nlmsghdr *n, int maxlen, int type, __u16 data)
{ return addattr_l(n, maxlen, type, &data, sizeof(__u16)); }
static inline int
addattr32(struct nlmsghdr *n, int maxlen, int type, __u32 data)
{ return addattr_l(n, maxlen, type, &data, sizeof(__u32)); }
static inline int
addattr64(struct nlmsghdr *n, int maxlen, int type, __u64 data)
{ return addattr_l(n, maxlen, type, &data, sizeof(__u64)); }
static inline int
addattrstrz(struct nlmsghdr *n, int maxlen, int type, const char *str)
{ return addattr_l(n, maxlen, type, str, strlen(str)+1); }

static inline int
nl_talk(int fd, struct nlmsghdr *n,
                struct nlmsghdr *answer, size_t answer_buf_siz)
{
  static char buf[10000];
  if (answer == NULL) {
    n->nlmsg_flags |= NLM_F_ACK;
    answer = (struct nlmsghdr*)buf;
    answer_buf_siz = sizeof(buf);
  }
  int ret = send(fd, n, n->nlmsg_len, 0);
  if (ret < 0) {
    perror("send");
    exit(1);
  }
  ret = recv(fd, answer, answer_buf_siz, 0);
  if (ret < 0) {
    perror("recv");
    exit(1);
  }
  return ret;
}

static inline int
rta_addattr_l(struct rtattr *rta, int maxlen, int type,
      const void *data, int alen)
{
  struct rtattr *subrta;
  int len = RTA_LENGTH(alen);

  if (RTA_ALIGN(rta->rta_len) + RTA_ALIGN(len) > maxlen) {
    fprintf(stderr,
      "rta_addattr_l: Error! max allowed bound %d exceeded\n",
      maxlen);
    return -1;
  }
  subrta = (struct rtattr *)(((char *)rta) + RTA_ALIGN(rta->rta_len));
  subrta->rta_type = type;
  subrta->rta_len = len;
  if (alen)
    memcpy(RTA_DATA(subrta), data, alen);
  rta->rta_len = NLMSG_ALIGN(rta->rta_len) + RTA_ALIGN(len);
  return 0;
}

static inline int
rta_addattr8(struct rtattr *rta, int maxlen, int type, __u8 data)
{ return rta_addattr_l(rta, maxlen, type, &data, sizeof(__u8)); }

static inline int
rta_addattr16(struct rtattr *rta, int maxlen, int type, __u16 data)
{ return rta_addattr_l(rta, maxlen, type, &data, sizeof(__u16)); }

static inline int
rta_addattr32(struct rtattr *rta, int maxlen, int type, __u32 data)
{ return rta_addattr_l(rta, maxlen, type, &data, sizeof(__u32)); }

static inline int
rta_addattr64(struct rtattr *rta, int maxlen, int type, __u64 data)
{ return rta_addattr_l(rta, maxlen, type, &data, sizeof(__u64)); }

static inline struct rtattr *
rta_nest(struct rtattr *rta, int maxlen, int type)
{
  struct rtattr *nest = RTA_TAIL(rta);
  rta_addattr_l(rta, maxlen, type, NULL, 0);
  nest->rta_type |= NLA_F_NESTED;
  return nest;
}

static inline int
rta_nest_end(struct rtattr *rta, struct rtattr *nest)
{
  nest->rta_len = (void *)RTA_TAIL(rta) - (void *)nest;
  return rta->rta_len;
}

static inline int
addraw_l(struct nlmsghdr *n, int maxlen, const void *data, int len)
{
  if (NLMSG_ALIGN(n->nlmsg_len) + NLMSG_ALIGN(len) > maxlen) {
    fprintf(stderr,
      "addraw_l ERROR: message exceeded bound of %d\n",
      maxlen);
    return -1;
  }

  memcpy(NLMSG_TAIL(n), data, len);
  memset((void *) NLMSG_TAIL(n) + len, 0, NLMSG_ALIGN(len) - len);
  n->nlmsg_len = NLMSG_ALIGN(n->nlmsg_len) + NLMSG_ALIGN(len);
  return 0;
}

#endif /* _NETLINK_H_ */
