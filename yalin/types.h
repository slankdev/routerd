#ifndef _NETLINK_TYPES_H_
#define _NETLINK_TYPES_H_

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


inline static const char*
nlmsg_type_to_str(uint16_t type)
{
  switch (type) {
    case RTM_NEWLINK: return "RTM_NEWLINK";
    case RTM_DELLINK: return "RTM_DELLINK";
    case RTM_GETLINK: return "RTM_GETLINK";
    case RTM_NEWADDR: return "RTM_NEWADDR";
    case RTM_DELADDR: return "RTM_DELADDR";
    case RTM_GETADDR: return "RTM_GETADDR";
    case RTM_NEWROUTE: return "RTM_NEWROUTE";
    case RTM_DELROUTE: return "RTM_DELROUTE";
    case RTM_GETROUTE: return "RTM_GETROUTE";
    case RTM_NEWNEIGH: return "RTM_NEWNEIGH";
    case RTM_DELNEIGH: return "RTM_DELNEIGH";
    case RTM_GETNEIGH: return "RTM_GETNEIGH";
    case RTM_NEWRULE: return "RTM_NEWRULE";
    case RTM_DELRULE: return "RTM_DELRULE";
    case RTM_GETRULE: return "RTM_GETRULE";
    case RTM_NEWQDISC: return "RTM_NEWQDISC";
    case RTM_DELQDISC: return "RTM_DELQDISC";
    case RTM_GETQDISC: return "RTM_GETQDISC";
    case RTM_NEWTCLASS: return "RTM_NEWTCLASS";
    case RTM_DELTCLASS: return "RTM_DELTCLASS";
    case RTM_GETTCLASS: return "RTM_GETTCLASS";
    case RTM_NEWTFILTER: return "RTM_NEWTFILTER";
    case RTM_DELTFILTER: return "RTM_DELTFILTER";
    case RTM_GETTFILTER: return "RTM_GETTFILTER";
    case RTM_NEWACTION: return "RTM_NEWACTION";
    case RTM_DELACTION: return "RTM_DELACTION";
    case RTM_GETACTION: return "RTM_GETACTION";
    case RTM_NEWPREFIX: return "RTM_NEWPREFIX";
    case RTM_GETMULTICAST: return "RTM_GETMULTICAST";
    case RTM_GETANYCAST: return "RTM_GETANYCAST";
    case RTM_NEWNEIGHTBL: return "RTM_NEWNEIGHTBL";
    case RTM_GETNEIGHTBL: return "RTM_GETNEIGHTBL";
    case RTM_SETNEIGHTBL: return "RTM_SETNEIGHTBL";
    case RTM_NEWNDUSEROPT: return "RTM_NEWNDUSEROPT";
    case RTM_NEWADDRLABEL: return "RTM_NEWADDRLABEL";
    case RTM_DELADDRLABEL: return "RTM_DELADDRLABEL";
    case RTM_GETADDRLABEL: return "RTM_GETADDRLABEL";
    case RTM_GETDCB: return "RTM_GETDCB";
    case RTM_SETDCB: return "RTM_SETDCB";
    case RTM_NEWNETCONF: return "RTM_NEWNETCONF";
    case RTM_DELNETCONF: return "RTM_DELNETCONF";
    case RTM_GETNETCONF: return "RTM_GETNETCONF";
    case RTM_NEWMDB: return "RTM_NEWMDB";
    case RTM_DELMDB: return "RTM_DELMDB";
    case RTM_GETMDB: return "RTM_GETMDB";
    case RTM_NEWNSID: return "RTM_NEWNSID";
    case RTM_DELNSID: return "RTM_DELNSID";
    case RTM_GETNSID: return "RTM_GETNSID";
    case RTM_NEWSTATS: return "RTM_NEWSTATS";
    case RTM_GETSTATS: return "RTM_GETSTATS";
    case RTM_NEWCACHEREPORT: return "RTM_NEWCACHEREPORT";
    default: return "RTM_XXXUNKNOWNXXX";
  }
}

#endif /* _NETLINK_TYPES_H_ */
