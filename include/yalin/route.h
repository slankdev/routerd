#ifndef _YALIN_ROUTE_H_
#define _YALIN_ROUTE_H_

#include <assert.h>
#include <linux/lwtunnel.h>
#include <linux/seg6.h>
#include <linux/seg6_local.h>
#include <linux/seg6_iptunnel.h>

#include <slankdev/hexdump.h>

#ifndef RTM_RTA
#error RTM_RTA isnt defined
#endif

inline static const char*
rta_type_ROUTE_to_str(uint16_t type)
{
  /* defined at /usr/include/linux/rtnetlink.h */
  switch (type) {
    case RTA_UNSPEC       : return "RTA_UNSPEC";
    case RTA_DST          : return "RTA_DST";
    case RTA_SRC          : return "RTA_SRC";
    case RTA_IIF          : return "RTA_IIF";
    case RTA_OIF          : return "RTA_OIF";
    case RTA_GATEWAY      : return "RTA_GATEWAY";
    case RTA_PRIORITY     : return "RTA_PRIORITY";
    case RTA_PREFSRC      : return "RTA_PREFSRC";
    case RTA_METRICS      : return "RTA_METRICS";
    case RTA_MULTIPATH    : return "RTA_MULTIPATH";
    case RTA_PROTOINFO    : return "RTA_PROTOINFO";
    case RTA_FLOW         : return "RTA_FLOW";
    case RTA_CACHEINFO    : return "RTA_CACHEINFO";
    case RTA_SESSION      : return "RTA_SESSION";
    case RTA_MP_ALGO      : return "RTA_MP_ALGO";
    case RTA_TABLE        : return "RTA_TABLE";
    case RTA_MARK         : return "RTA_MARK";
    case RTA_MFC_STATS    : return "RTA_MFC_STATS";
    case RTA_VIA          : return "RTA_VIA";
    case RTA_NEWDST       : return "RTA_NEWDST";
    case RTA_PREF         : return "RTA_PREF";
    case RTA_ENCAP_TYPE   : return "RTA_ENCAP_TYPE";
    case RTA_ENCAP        : return "RTA_ENCAP";
    case RTA_EXPIRES      : return "RTA_EXPIRES";
    case RTA_PAD          : return "RTA_PAD";
    case RTA_UID          : return "RTA_UID";
    case RTA_TTL_PROPAGATE: return "RTA_TTL_PROPAGATE";
    default: return "RTA_XXXUNKNOWNXXX";
  }
}

inline static const char*
lwtunnel_encap_types_to_str(uint16_t type)
{
  switch (type)
  {
    /* defined at /usr/include/linux/lwtunnel.h */
    case LWTUNNEL_ENCAP_NONE      : return "LWTUNNEL_ENCAP_NONE";
    case LWTUNNEL_ENCAP_MPLS      : return "LWTUNNEL_ENCAP_MPLS";
    case LWTUNNEL_ENCAP_IP        : return "LWTUNNEL_ENCAP_IP";
    case LWTUNNEL_ENCAP_ILA       : return "LWTUNNEL_ENCAP_ILA";
    case LWTUNNEL_ENCAP_IP6       : return "LWTUNNEL_ENCAP_IP6";
    case LWTUNNEL_ENCAP_SEG6      : return "LWTUNNEL_ENCAP_SEG6";
    case LWTUNNEL_ENCAP_BPF       : return "LWTUNNEL_ENCAP_BPF";
    case LWTUNNEL_ENCAP_SEG6_LOCAL: return "LWTUNNEL_ENCAP_SEG6_LOCAL";
    default: return "LWTUNNEL_ENCAP_UNKNOWN";
  }
}

inline static const char*
SEG6_LOCAL_ACTION_to_str2(uint16_t act)
{
  switch (act) {
    /* define at /usr/include/linux/seg6_local.h */
    case SEG6_LOCAL_ACTION_UNSPEC      : return "unspec ";
    case SEG6_LOCAL_ACTION_END         : return "end";
    case SEG6_LOCAL_ACTION_END_X       : return "end.x";
    case SEG6_LOCAL_ACTION_END_T       : return "end.t";
    case SEG6_LOCAL_ACTION_END_DX2     : return "end.dx2";
    case SEG6_LOCAL_ACTION_END_DX6     : return "end.dx6";
    case SEG6_LOCAL_ACTION_END_DX4     : return "end.dx4";
    case SEG6_LOCAL_ACTION_END_DT6     : return "end.dt6";
    case SEG6_LOCAL_ACTION_END_DT4     : return "end.dt4";
    case SEG6_LOCAL_ACTION_END_B6      : return "end.b6";
    case SEG6_LOCAL_ACTION_END_B6_ENCAP: return "end.b6_encap";
    case SEG6_LOCAL_ACTION_END_BM      : return "end.bm";
    case SEG6_LOCAL_ACTION_END_S       : return "end.s";
    case SEG6_LOCAL_ACTION_END_AS      : return "end.as";
    case SEG6_LOCAL_ACTION_END_AM      : return "end.am";
    default: return "SEG6_LOCAL_ACTION_UNKNOWN";
  }
}

inline static const char*
SEG6_LOCAL_ACTION_to_str(uint16_t act)
{
  switch (act) {
    /* define at /usr/include/linux/seg6_local.h */
    case SEG6_LOCAL_ACTION_UNSPEC      : return "SEG6_LOCAL_ACTION_UNSPEC ";
    case SEG6_LOCAL_ACTION_END         : return "SEG6_LOCAL_ACTION_END";
    case SEG6_LOCAL_ACTION_END_X       : return "SEG6_LOCAL_ACTION_END_X";
    case SEG6_LOCAL_ACTION_END_T       : return "SEG6_LOCAL_ACTION_END_T";
    case SEG6_LOCAL_ACTION_END_DX2     : return "SEG6_LOCAL_ACTION_END_DX2";
    case SEG6_LOCAL_ACTION_END_DX6     : return "SEG6_LOCAL_ACTION_END_DX6";
    case SEG6_LOCAL_ACTION_END_DX4     : return "SEG6_LOCAL_ACTION_END_DX4";
    case SEG6_LOCAL_ACTION_END_DT6     : return "SEG6_LOCAL_ACTION_END_DT6";
    case SEG6_LOCAL_ACTION_END_DT4     : return "SEG6_LOCAL_ACTION_END_DT4";
    case SEG6_LOCAL_ACTION_END_B6      : return "SEG6_LOCAL_ACTION_END_B6";
    case SEG6_LOCAL_ACTION_END_B6_ENCAP: return "SEG6_LOCAL_ACTION_END_B6_ENCAP";
    case SEG6_LOCAL_ACTION_END_BM      : return "SEG6_LOCAL_ACTION_END_BM";
    case SEG6_LOCAL_ACTION_END_S       : return "SEG6_LOCAL_ACTION_END_S";
    case SEG6_LOCAL_ACTION_END_AS      : return "SEG6_LOCAL_ACTION_END_AS";
    case SEG6_LOCAL_ACTION_END_AM      : return "SEG6_LOCAL_ACTION_END_AM";
    default: return "SEG6_LOCAL_ACTION_UNKNOWN";
  }
}

inline static const char*
SEG6_LOCAL_to_str(uint16_t type)
{
  switch (type) {
    case SEG6_LOCAL_UNSPEC: return "SEG6_LOCAL_UNSPEC";
    case SEG6_LOCAL_ACTION: return "SEG6_LOCAL_ACTION";
    case SEG6_LOCAL_SRH   : return "SEG6_LOCAL_SRH";
    case SEG6_LOCAL_TABLE : return "SEG6_LOCAL_TABLE ";
    case SEG6_LOCAL_NH4   : return "SEG6_LOCAL_NH4";
    case SEG6_LOCAL_NH6   : return "SEG6_LOCAL_NH6";
    case SEG6_LOCAL_IIF   : return "SEG6_LOCAL_IIF";
    case SEG6_LOCAL_OIF   : return "SEG6_LOCAL_OIF";
    default: return "SEG6_LOCAL_UKNOWN";
  }
}

inline static const char*
SEG6_IPTUNNEL_to_str(uint16_t type)
{
  switch (type) {
    case SEG6_IPTUNNEL_UNSPEC: return "SEG6_IPTUNNEL_UNSPEC";
    case SEG6_IPTUNNEL_SRH   : return "SEG6_IPTUNNEL_SRH";
    default: return "SEG6_IPTUNNEL_UNKNOWN";
  }
}

inline static const char*
SEG6_IPTUN_MODE_to_str(uint16_t type)
{
  switch (type) {
    case SEG6_IPTUN_MODE_INLINE : return "SEG6_IPTUN_MODE_INLINE";
    case SEG6_IPTUN_MODE_ENCAP  : return "SEG6_IPTUN_MODE_ENCAP";
    case SEG6_IPTUN_MODE_L2ENCAP: return "SEG6_IPTUN_MODE_L2ENCAP";
    default: return "SEG6_IPTUN_MODE_UNKNOWN";
  }
}

inline static std::string
ipv6_sr_hdr_summary(const struct ipv6_sr_hdr* srh)
{
  std::string str = strfmt("nh=%u hl=%u t=%u sl=%u [",
      srh->nexthdr, srh->hdrlen, srh->type,
      srh->segments_left, srh->first_segment);
  const size_t n = ((srh->hdrlen*8+8)-8)/16;
  for (size_t i=0; i<n; i++) {
    const struct in6_addr* addr = &srh->segments[i];
    str += strfmt("%s%s",
        inetpton(addr, AF_INET6).c_str(),
        i+1<n?",":"]");
  }
  return str;
}

inline static std::string
rtmsg_rtattr_SEG6_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-20s :: ",
      rta->rta_type, SEG6_IPTUNNEL_to_str(rta->rta_type));
  switch (rta->rta_type) {
    case SEG6_IPTUNNEL_SRH:
    {
      const struct seg6_iptunnel_encap* sie;
      sie = (const struct seg6_iptunnel_encap*)(rta+1);
      return hdr + strfmt("mode=%u (%s) ", sie->mode,
          SEG6_IPTUN_MODE_to_str(sie->mode)) +
          ipv6_sr_hdr_summary(sie->srh);
    }
    default:
    {
      std::string val;
      val = strfmt("unknown-fmt(rta_len=%u,data=", rta->rta_len);
      const uint8_t* data = (const uint8_t*)(rta+1);
      size_t payload_len = size_t(rta->rta_len-sizeof(*rta));
      size_t n = std::min(size_t(4), payload_len);
      for (size_t i=0; i<n; i++)
        val += strfmt("%02x", data[i]);
      if (4 < payload_len) val += "...";
      val += ")";
      return hdr + val;
    }
  }
  return hdr + __func__;
}

inline static std::string
rtmsg_rtattr_SEG6_LOCAL_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-20s :: ",
      rta->rta_type, SEG6_LOCAL_to_str(rta->rta_type));
  switch (rta->rta_type) {

    case SEG6_LOCAL_ACTION:
    {
      assert(rta->rta_len == 8);
      uint32_t num = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u (%s)", num,
          SEG6_LOCAL_ACTION_to_str(num));
    }
    case SEG6_LOCAL_NH6:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==16);
      return hdr + inetpton(addr_ptr, AF_INET6);
    }
    case SEG6_LOCAL_NH4:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4);
      return hdr + inetpton(addr_ptr, AF_INET);
    }
    case SEG6_LOCAL_OIF:
    case SEG6_LOCAL_IIF:
    case SEG6_LOCAL_TABLE:
    {
      assert(rta->rta_len == 8);
      uint32_t num = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", num);
    }
    case SEG6_LOCAL_SRH:
    {
      const struct ipv6_sr_hdr* srh = (const struct ipv6_sr_hdr*)(rta+1);
      std::string str = ipv6_sr_hdr_summary(srh);
      return hdr + str;
    }
    default:
    {
      std::string val;
      val = strfmt("unknown-fmt(rta_len=%u,data=", rta->rta_len);
      const uint8_t* data = (const uint8_t*)(rta+1);
      size_t payload_len = size_t(rta->rta_len-sizeof(*rta));
      size_t n = std::min(size_t(4), payload_len);
      for (size_t i=0; i<n; i++)
        val += strfmt("%02x", data[i]);
      if (4 < payload_len) val += "...";
      val += ")";
      return hdr + val;
    }
  }
}

inline static std::string
rtmsg_rtattr_summary(const struct rtattr* rta)
{
  std::string hdr = strfmt("0x%04x %-16s :: ",
      rta->rta_type, rta_type_ROUTE_to_str(rta->rta_type));
  switch (rta->rta_type) {

    case RTA_PRIORITY:
    case RTA_PREFSRC:
    case RTA_TABLE:
    case RTA_IIF:
    case RTA_OIF:
    {
      assert(rta->rta_len == 8);
      uint32_t val = *(uint32_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }
    case RTA_ENCAP_TYPE:
    {
      assert(rta->rta_len == 6);
      uint16_t val = *(uint16_t*)(rta+1);
      return hdr + strfmt("%u (%s)", val,
          lwtunnel_encap_types_to_str(val));
    }
    case RTA_EXPIRES:
    {
      assert(rta->rta_len == 12);
      uint64_t val = *(uint64_t*)(rta+1);
      return hdr + strfmt("%lu", val);
    }
    case RTA_ENCAP:
    {
      uint16_t encap_kind;
      {
        const uint8_t* ptr = (const uint8_t*)rta;
        ptr += rta->rta_len;
        const struct rtattr* rtan = (const struct rtattr*)ptr;
        assert(rtan->rta_type == RTA_ENCAP_TYPE);
        assert(rtan->rta_len == 6);
        uint16_t val = *(uint16_t*)(rtan+1);
        encap_kind = val;
      }

      std::string str = "nested-data\n";
      size_t rta_len = rta->rta_len - sizeof(*rta);
      const uint8_t* data = (const uint8_t*)(rta + 1);
      for (const struct rtattr* rta = (const struct rtattr*)data;
           RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
        std::string attr_str;
        if (encap_kind==LWTUNNEL_ENCAP_SEG6_LOCAL)
          attr_str = rtmsg_rtattr_SEG6_LOCAL_summary(rta);
        else if (encap_kind==LWTUNNEL_ENCAP_SEG6)
          attr_str = rtmsg_rtattr_SEG6_summary(rta);

        if (attr_str != "")
          str += "    " + attr_str + "\n";
      }
      str.pop_back();
      return hdr + str;
    }
    case RTA_PREF:
    {
      assert(rta->rta_len == 5);
      uint8_t val = *(uint8_t*)(rta+1);
      return hdr + strfmt("%u", val);
    }

    case RTA_DST:
    case RTA_SRC:
    case RTA_GATEWAY:
    {
      uint8_t* addr_ptr = (uint8_t*)(rta+1);
      size_t addr_len = rta->rta_len - sizeof(*rta);
      assert(addr_len==4 || addr_len==16);
      if (addr_len == 4) return hdr + inetpton(addr_ptr, AF_INET);
      if (addr_len == 16) return hdr + inetpton(addr_ptr, AF_INET6);
      else return hdr + "unknown-addr-fmt";
    }

    case RTA_MFC_STATS:
    {
      assert(rta->rta_len == 28);
      struct rta_mfc_stats {
        uint64_t mfcs_packets;
        uint64_t mfcs_bytes;
        uint64_t mfcs_wrong_if;
      };
      const struct rta_mfc_stats* ms;
      ms = (const struct rta_mfc_stats*)(rta+1);
      return hdr + strfmt("pkt=%lu bytes=%lu wrong_if=%lu",
          ms->mfcs_packets, ms->mfcs_bytes, ms->mfcs_wrong_if);
    }

    case RTA_MULTIPATH:
    {
      std::string str = strfmt("nested");
      size_t plen = rta->rta_len - sizeof(struct rtattr);
      const struct rtnexthop* rtnh = (const struct rtnexthop*)(rta+1);
      for (; RTNH_OK(rtnh, plen);
           rtnh = RTNH_NEXT(rtnh)) {
        str += strfmt("\n%4snexthop flags=0x%x hops=%u ifindex=%d",
            " ", rtnh->rtnh_flags, rtnh->rtnh_hops, rtnh->rtnh_ifindex);
        size_t sub_plen = rtnh->rtnh_len - sizeof(struct rtnexthop);
        printf("SLANKDE!!!! sub_plen=%zd\n", sub_plen);
        for (struct rtattr* rta = RTNH_DATA(rtnh);
            RTA_OK(rta, sub_plen);
            rta = RTA_NEXT (rta, sub_plen)) {
          str += strfmt("\n%6s", " ");
          str += rtmsg_rtattr_summary(rta);
        }
      }
      return hdr + str;
    }

    // Others
    case RTA_METRICS:
    case RTA_PROTOINFO:
    case RTA_FLOW:
    case RTA_SESSION:
    case RTA_MP_ALGO:
    case RTA_MARK:
    case RTA_VIA:
    case RTA_NEWDST:
    case RTA_PAD:
    case RTA_UID:
    case RTA_TTL_PROPAGATE:
    case RTA_CACHEINFO:
    default:
    {
      std::string val;
      val = strfmt("unknown-fmt(rta_len=%u,data=", rta->rta_len);
      const uint8_t* data = (const uint8_t*)(rta+1);
      size_t payload_len = size_t(rta->rta_len-sizeof(*rta));
      size_t n = std::min(size_t(4), payload_len);
      for (size_t i=0; i<n; i++)
        val += strfmt("%02x", data[i]);
      if (4 < payload_len) val += "...";
      val += ")";
      return hdr + val;
    }
  }
}

inline static std::string
rtmsg_summary(const struct rtmsg* rtm)
{
  return strfmt("fmly=%u dl=%u sl=%u tos=%u tab=%u"
      " pro=%u scope=%u type=%u f=0x%x",
      rtm->rtm_family, rtm->rtm_dst_len, rtm->rtm_src_len,
      rtm->rtm_tos, rtm->rtm_table, rtm->rtm_protocol,
      rtm->rtm_scope, rtm->rtm_type, rtm->rtm_flags);
}

inline static std::string
rtnl_route_summary(const struct nlmsghdr* hdr)
{
  struct rtmsg* rtm = (struct rtmsg*)(hdr + 1);
  std::string str = rtmsg_summary(rtm) + "\n";
  size_t rta_len = IFA_PAYLOAD(hdr);
  for (struct rtattr* rta = RTM_RTA(rtm);
       RTA_OK(rta, rta_len); rta = RTA_NEXT(rta, rta_len)) {
    std::string attr_str = rtmsg_rtattr_summary(rta);
    if (attr_str != "")
      str += "  " + attr_str + "\n";
  }
  return str;
}

#endif /* _YALIN_ROUTE_H_ */
