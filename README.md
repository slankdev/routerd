
# Netlink for DUMMIES

- Hiroki Shirokura <slankdev@coe.ad.jp>
- 2019.01.13 - present

## Architecture of Netlink

- rtnetlink is composed of link,addr,route,etc..
  - each operation associated to 3messages(link,addr,route)

opening the netlink monitor socket
```
int open_rtnl_sock(uint32_t groups) {
  int sock = socket(AF_NETLINK, SOCK_RAW, NETLINK_ROUTE);
  struct sockaddr_nl sa;
  memset(&sa, 0x0, sizeof(sa));
  sa.nl_family = AF_NETLINK;
  sa.nl_groups = groups;
  bind(sock, (struct sockaddr*)&sa, sizeof(sa));
  return sock;
}

uint32_t grp = 0x00;
grp |= RTMGRP_LINK;
grp |= RTMGRP_NEIGH;
grp |= RTMGRP_IPV4_ROUTE;
grp |= RTMGRP_IPV4_IFADDR;
int sock = open_rtnl_sock(grp);
```

Tipical Netlink packet format is following.
```
bit0 --------> bitN(N>0)
Netlink
MsgHdr
```

## Use cases

**When I operate `ip link set dum0 up`**<br>
- please refer the `pcap/link_up.pcap`
- I operate `sudo ip link set dum0 up`
- before execution, dum0 is down state of link
- after execution, dum0 will be up state of link
- How to filter the link-up operation packet for hook action
- Flow
  - (Rout) get network route (seq=566030)
    -  send get network route
    - receive all network route's info
    - receive end-of-a-dump to close stream?
  - (Link) get network interface (seq=1547355678)
    -  send get network interface info
    - receive all network interface's info
    - receive end-of-a-dump to close stream?
  - (Addr) get ip address (seq=1547355679)
    - send get ip address
    - receive all ip address's
    - receive end-of-a-dump to close stream?
  - (Link) set network interface (seq=1547355685)
    - **THIS-PART-IS-THE-MOST-IMPORTANT**
    - send create network interface
      - flag: 0x000100c3
      - with device change flags = 1
    - receive end-of-a-dump to close stream?
  - (Addr) set ip address (seq=1547355686)
    - receive end-of-a-dump to close stream?
  - (Rout) set network route (seq=566031)
    - receive end-of-a-dump to close stream?

**When I operate `ip link add dum0 type dummy`**<br>
- please refer the `pcap/link_add.pcap`
- How to filter the link-add operation packet for hook action
- Flow
  - (Rout) (seq=566036)
  - (Link) (seq=1547355744)
  - (Addr) (seq=1547355745)
  - (Link) (seq=1547355756)
  - (Addr) (seq=1547355757)
  - (Rout) (seq=566037)

**When I operate `ip route add 8.8.8.8/32 via 10.0.0.1`**<br>
- please refer the `pcap/route_add.pcap`
- I operate `sudo ip route add 8.8.8.8/32 via 10.10.10.11`
- before execution, there is no route to 8.8.8.8
- after execution, there is a route to 8.8.8.8 via 10.10.10.11
- How to filter the route-add operation packet for hook action
- Flow
  - (Rout) (seq=566052)
  - (Link) (seq=1547355931)
  - (Addr) (seq=1547355932)
  - (Link) (seq=1547355935)
  - (Addr) (seq=1547355936)
  - (Rout) (seq=566053)

## Netlink Message Format cheat-sheet

### well known format/value

- Linux Netlink Cooked header
  - Netlink Family: [ref](#netlink-family) (wireshark: netlink.family)
- Netlink message (rtnetlink protocol)
  - Netlink Message Header
    - Length: uINT-value
    - Message type: [ref](#rtnetlink-message-type)
- Netlink message (genetlink protocol)
  - Netlink Message Header
    - Message type: [ref](#genetlink-message-type)

<a name="netlink-foramt-type"></a>
**netlink-format-type**
- Netlink message header ([RFC3549](https://tools.ietf.org/html/rfc3549#section-2.3.2))
```
/*
 * 0                   1                   2                   3
 * 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1 2 3 4 5 6 7 8 9 0 1
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                          Length                             |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |            Type              |           Flags              |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Sequence Number                        |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 * |                      Process ID (PID)                       |
 * +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 */
strcut nlmsghdr {
	uint32_t nlmsg_len;   /* length  */
	uint16_t nlmsg_type;  /* type    */
	uint16_t nlmsg_flags; /* flags   */
	uint32_t nlmsg_seq;   /* seq-num */
	uint32_t nlmsg_pid;   /* proc-id */
};
```

<a name="netlink-family"></a>
**netlink-family** defined at `/usr/include/linux/netlink.h`
- 0x00: rtnetlink
- 0x10: genetlink

<a name="rtnetlink-multicast-group"></a>
**rtnetlink-multicast-group** defined at `/usr/include/rtnetlink.h`
- `0x001`: link
- `0x002`: notify
- `0x004`: neigh
- `0x008`: tc
- `0x010`: ipv4 ifaddr
- `0x020`: ipv4 mroute
- `0x040`: ipv4 route
- `0x080`: ipv4 rule
- `0x100`: ipv6 ifaddr
- `0x200`: ipv6 mroute
- `0x400`: ipv6 route
- `0x800`: ipv6 rule

<a name="rtnetlink-message-type"></a>
**rtnetlink-message-type** defined at `/usr/include/rtnetlink.h`
- `0x02,02`: error
- `0x03,03`: end of a dump
- `0x10,16`: `NEW_LINK`, create network interface
- `0x11,17`: `DEL_LINK`, remove network interface
- `0x12,18`: `GET_LINK`, get network interface info
- `0x13,19`: `SET_LINK`
- `0x14,20`: add ip address
- `0x15,21`: del ip address
- `0x16,22`: get ip address
- `0x18,24`: add network route
- `0x19,25`: del network route
- `0x20,26`: get network route

<a name="rtnetlink-attribute-type"></a>
**rtnetlink-attribute-type**

<a name="genetlink-message-type"></a>
**rtnetlink-message-type** defined at `/usr/include/genetlink.h`
- write soon

## iproute2 cheat-sheet
```
# ip netns add net1
# ip netns add net2
# ip link add veth1 netns net1 type veth peer name veth2 netns net2
# ip link add nlmon0 type nlmon
# ip link set nlmon0 up
# tcpdump -i nlmon0 -w nlmsg.pcap
# ip link add dummy1 type dummy
# ip addr add 1.1.1.1/24 dev dummy1
# ip link set dummy1 up
```

## References

1. RFC3549, Linux Netlink as an IP Services Protocol, https://tools.ietf.org/html/rfc3549
2. N.Horman, "Understanding And Programming With Netlink Sockets", https://people.redhat.com/nhorman/papers/netlink.pdf

