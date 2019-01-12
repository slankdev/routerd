
#ifndef _NETLINK_RTATTR_H_
#define _NETLINK_RTATTR_H_

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>
#include <net/if.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <linux/socket.h>
#include <linux/netlink.h>
#include <linux/rtnetlink.h>
#include <net/if_arp.h>
#include "flags.h"
#include "hexdump.h"

inline static size_t
align(size_t blocklen, size_t alignlen)
{
  /* printf("blocklen: %zd \n", blocklen); */
  /* printf("alignlen: %zd \n", alignlen); */
  return (blocklen + alignlen - 1) & ~(alignlen - 1);
}

inline static uint16_t
rtattr_type(const struct rtattr* rta)
{ return rta->rta_type; }

inline static size_t
rtattr_len(const struct rtattr* rta)
{ return rta->rta_len; }

inline static size_t
rtattr_payload_len(const struct rtattr* rta)
{ return rta->rta_len-sizeof(struct rtattr); }

inline static uint8_t*
rtattr_payload_ptr(const struct rtattr* rta)
{ return (uint8_t*)(rta + 1); }

inline static const struct rtattr*
rtattr_next(const struct rtattr* rta, ssize_t* buflen)
{
  size_t seek_len = align(rtattr_len(rta), 4);
  if (*buflen <= 0) return NULL;
  if (seek_len <= 0) return NULL;

  if ((*buflen - seek_len) > sizeof(struct rtattr)) {
    struct rtattr* next_rta = (struct rtattr*)(((uint8_t*)rta) + seek_len);
    /* printf("%s: buflen=%zd, state(len,type): cur(0x%lx,0x%x) --seek(%zd)--> nxt(0x%lx,0x%x)\n", */
    /*     __func__, *buflen, */
    /*     rtattr_len(rta), rtattr_type(rta), seek_len, */
    /*     rtattr_len(next_rta), rtattr_type(next_rta)); */
    *buflen -= seek_len;
    return next_rta;
  } else {
    return NULL;
  }
}

inline static const struct rtattr*
rtattr_nest_next(const struct rtattr* rta)
{
  struct rtattr* nested_rta = (struct rtattr*)rtattr_payload_ptr(rta);
  return nested_rta;
}

inline static uint8_t
rtattr_read_8bit (const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint8_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), rtattr_type(attr));
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint8_t val = *(uint8_t*)rtattr_payload_ptr(attr);
  /* printf("%s: val=%u,0x%x\n", __func__, val, val); */
  return val;
}

inline static uint16_t
rtattr_read_16bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint16_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), rtattr_type(attr));
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint16_t val = *(uint16_t*)rtattr_payload_ptr(attr);
  /* printf("%s: val=%u,0x%x\n", __func__, val, val); */
  return val;
}

inline static uint32_t
rtattr_read_32bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint32_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), rtattr_type(attr));
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint32_t val = *(uint32_t*)rtattr_payload_ptr(attr);
  /* printf("%s: val=%u,0x%x\n", __func__, val, val); */
  return val;
}

inline static uint64_t
rtattr_read_64bit(const struct rtattr* attr)
{
  if (rtattr_payload_len(attr) > sizeof(uint64_t)) {
    fprintf(stderr, "%s: read miss (l,t)=(%zd,%u)\n", __func__,
        rtattr_payload_len(attr), rtattr_type(attr));
    fprintf(stderr, " - payload_len: %zd\n", rtattr_payload_len(attr));
    exit(1);
  }
  uint64_t val = *(uint64_t*)rtattr_payload_ptr(attr);
  /* printf("%s: val=%lu,0x%lx\n", __func__, val, val); */
  return val;
}

inline static size_t
rtattr_read_str(const struct rtattr* attr, char* str, size_t strbuflen)
{
  if (rtattr_payload_len(attr) > strbuflen) {
    fprintf(stderr, "%s: read miss (type=%u)\n", __func__, rtattr_type(attr));
    printf("payloadlen: %zd\n", rtattr_payload_len(attr));
    printf("strbuflen: %zd\n", strbuflen);
    exit(1);
  }
  snprintf(str, strbuflen, "%s", rtattr_payload_ptr(attr));
  return align(rtattr_payload_len(attr), 4);
}

inline static const char*
rta_to_str_IFLA_LINKINFO(const struct rtattr* root_rta, char* str, size_t len)
{
  assert(rtattr_type(root_rta) == IFLA_LINKINFO);
  const struct rtattr* rta = rtattr_nest_next(root_rta);
  assert(rta != NULL);
  assert(rtattr_type(rta) == IFLA_INFO_KIND);

  char strbuf[100];
  rtattr_read_str(rta, strbuf, sizeof(strbuf));

  if (strcmp(strbuf, "vlan") == 0) {
    struct interface_info_vlan {
      uint16_t proto;
      uint16_t id;
      uint64_t flags;
    } vlaninfo = {0};

    ssize_t l = rtattr_len(rta);
    rta = rtattr_next(rta, &l);
    bool ret_cond0 = rta != NULL;
    bool ret_cond1 = rtattr_type(rta) == IFLA_INFO_DATA;
    if (ret_cond0 && ret_cond1) {
      const struct rtattr* nrta = (const struct rtattr*)rtattr_payload_ptr(rta);
      ssize_t nbuflen = rtattr_payload_len(rta);

      while (nrta) {

        uint16_t type = rtattr_type(nrta);
        switch (type) {
          case IFLA_VLAN_ID:
            vlaninfo.id = rtattr_read_16bit(nrta);
            break;
          case IFLA_VLAN_FLAGS:
            vlaninfo.flags = rtattr_read_64bit(nrta);
            break;
          case IFLA_VLAN_PROTOCOL:
            vlaninfo.proto = rtattr_read_16bit(nrta);
            break;
          case IFLA_VLAN_UNSPEC:
          case IFLA_VLAN_EGRESS_QOS:
          case IFLA_VLAN_INGRESS_QOS:
            printf("unsupport ... skip type=0x%04x @%p\n", type, nrta);
            break;
        }

        nrta = rtattr_next(nrta, &nbuflen);
        if (!nrta || nbuflen<=0) break;
      }

      snprintf(str, len, "IFLA_LINKINFO vlan(id:%u, proto:0x%04x, flags:0x%08lx)",
          vlaninfo.id, vlaninfo.proto, vlaninfo.flags);
      return str;
    }
  }

  snprintf(str, len, "UNKNOWN LINK KIND (%s)", strbuf);
  carrydump(stdout, "IFLA_LINKINFO Unknown-link-kind", root_rta, rtattr_len(root_rta));
  return str;
}

inline static const char*
rta_to_str_IFLA_AF_SPEC(const struct rtattr* rta, char* str, size_t len)
{
  snprintf(str, len, "IFLA_AF_SPEC <unsupported yet>");
  return str;
}

inline static const char*
rta_to_str(const struct rtattr* rta, char* str, size_t len)
{
  switch (rta->rta_type) {
    case IFLA_IFNAME:
    {
      snprintf(str, len, "%s <%s>",
          rta_type_to_str(rta->rta_type), (const char*)(rta+1));
      return str;
    }
    case IFLA_ADDRESS:
    case IFLA_BROADCAST:
    {
      char substr[1000];
      memset(substr, 0x0, sizeof(substr));
      size_t data_len = rta->rta_len-sizeof(struct rtattr);
      uint8_t* data_ptr = (uint8_t*)(rta + 1);
      for (size_t i=0; i<data_len; i++) {
        char subsubstr[100];
        snprintf(subsubstr, sizeof(subsubstr), "%02x:", data_ptr[i]);
        strncat(substr, subsubstr, strlen(subsubstr));
      }
      snprintf(str, len, "%s <%s>",
          rta_type_to_str(rta->rta_type), substr);
      return str;
    }
    case IFLA_STATS64:
    {
      struct rtnl_link_stats64* st = (struct rtnl_link_stats64*)(rta + 1);
      snprintf(str, len, "%s \n"
        "\trx_packets:%llu tx_packets:%llu rx_bytes:%llu tx_bytes:%llu \n"
        "\trx_errors:%llu tx_errors:%llu rx_dropped:%llu tx_dropped:%llu \n"
        "\tmulticast:%llu collisions:%llu rx_length_errors:%llu rx_over_errors:%llu \n"
        "\trx_crc_errors:%llu rx_frame_errors:%llu rx_fifo_errors:%llu \n"
        "\trx_missed_errors:%llu tx_aborted_errors:%llu tx_carrier_errors:%llu \n"
        "\ttx_fifo_errors:%llu " "tx_heartbeat_errors:%llu tx_window_errors:%llu \n"
        "\trx_compressed:%llu tx_compressed:%llu \n",
        rta_type_to_str(rta->rta_type),
        st->rx_packets, st->tx_packets, st->rx_bytes, st->tx_bytes,
        st->rx_errors, st->tx_errors, st->rx_dropped, st->tx_dropped,
        st->multicast, st->collisions, st->rx_length_errors, st->rx_over_errors,
        st->rx_crc_errors, st->rx_frame_errors, st->rx_fifo_errors,
        st->rx_missed_errors, st->tx_aborted_errors, st->tx_carrier_errors,
        st->tx_fifo_errors, st->tx_heartbeat_errors, st->tx_window_errors,
        st->rx_compressed, st->tx_compressed);
      return str;
    }
    case IFLA_STATS:
    {
      struct rtnl_link_stats *st = (struct rtnl_link_stats*)(rta + 1);
      snprintf(str, len, "%s \n"
        "\trx_packets:%u tx_packets:%u rx_bytes:%u tx_bytes:%u rx_errors:%u \n"
        "\ttx_errors:%u rx_dropped:%u tx_dropped:%u multicast:%u collisions:%u \n"
        "\trx_length_errors:%u rx_over_errors:%u rx_crc_errors:%u rx_frame_errors:%u \n"
        "\trx_fifo_errors:%u rx_missed_errors:%u tx_aborted_errors:%u \n"
        "\ttx_carrier_errors:%u tx_fifo_errors:%u tx_heartbeat_errors:%u \n"
        "\ttx_window_errors:%u rx_compressed:%u tx_compressed:%u",
        rta_type_to_str(rta->rta_type),
        st->rx_packets, st->tx_packets, st->rx_bytes, st->tx_bytes, st->rx_errors,
        st->tx_errors, st->rx_dropped, st->tx_dropped, st->multicast, st->collisions,
        st->rx_length_errors, st->rx_over_errors, st->rx_crc_errors, st->rx_frame_errors,
        st->rx_fifo_errors, st->rx_missed_errors, st->tx_aborted_errors, st->tx_carrier_errors,
        st->tx_fifo_errors, st->tx_heartbeat_errors, st->tx_window_errors, st->rx_compressed,
        st->tx_compressed);
      return str;
    }
    case IFLA_MAP:
    {
      struct rtnl_link_ifmap* ifmap = (struct rtnl_link_ifmap*)(rta + 1);
      snprintf(str, len, "%s \n"
        "\tmem_start:%llu mem_end:%llu \n"
        "\tbase_addr:%llu irq:%u dma:%u port:%u",
        rta_type_to_str(rta->rta_type),
        ifmap->mem_start, ifmap->mem_end, ifmap->base_addr,
        ifmap->irq, ifmap->dma, ifmap->port);
      return str;
    }
    case IFLA_PROMISCUITY:
    {
      uint32_t promisc = *(uint32_t*)(rta+1);
      snprintf(str, len, "%s <%s>",
          rta_type_to_str(rta->rta_type),
          promisc?"promisc=on":"promisc=off");
      return str;
    }

    case IFLA_PROTO_DOWN: // XXX ??? unknown..?
    case IFLA_OPERSTATE:
    case IFLA_LINKMODE:
    case IFLA_CARRIER:
    {
      uint8_t num = *(uint8_t*)(rta+1);
      snprintf(str, len, "%s <%u>",
          rta_type_to_str(rta->rta_type), num);
      return str;
    }
    case IFLA_NUM_TX_QUEUES:
    case IFLA_NUM_RX_QUEUES:
    case IFLA_MTU:
    case IFLA_LINK:
    case IFLA_GROUP:
    case IFLA_QDISC:
    case IFLA_CARRIER_CHANGES:
    case IFLA_TXQLEN:
    {
      uint32_t num = *(uint32_t*)(rta+1);
      snprintf(str, len, "%s <%u>",
          rta_type_to_str(rta->rta_type), num);
      return str;
    }
    case IFLA_LINKINFO:
    {
      rta_to_str_IFLA_LINKINFO(rta, str, len);
      return str;
    }
    case IFLA_AF_SPEC:
    {
      // carrydump(stdout, "IFLA_AF_SPEC", rta, rtattr_len(rta));
      rta_to_str_IFLA_AF_SPEC(rta, str, len);
      return str;
    }
    default:
    {
#if 0
      snprintf(str, len, "%s unsupport-data=<...skipped>",
          rta_type_to_str(rta->rta_type));
#else
      char dstr[10000];
      memset(dstr, 0, sizeof(dstr));
      uint8_t* ptr = (uint8_t*)(rta + 1);
      for (size_t i=0; i<rta->rta_len-sizeof(struct rtattr); i++) {
        char sub_str[6];
        snprintf(sub_str, sizeof(sub_str), "%02x", ptr[i]);
        strncat(dstr, sub_str, strlen(sub_str));
      }
      snprintf(str, len, "%s unsupport-data=<%s>",
          rta_type_to_str(rta->rta_type), dstr);
      hexdump(stdout, rta, rtattr_len(rta));
#endif
      return str;
    }
  }
}

#endif /* _NETLINK_RTATTR_H_ */
