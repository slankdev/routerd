
# Router API Sample

ip command of iproute2 is following.
netlinkd should be hookable following action very-very correctry.
```
# basic net
ip link set dev dum0 up
ip link set dev dum0 down
ip link add dum0 name dum0.10 type vlan id 10
ip link del dum0 name dum0.10 type vlan id 10
ip addr add 10.0.0.1/24 dev net0
ip addr del 10.0.0.1/24 dev net0
ip addr add fc00:1::1/64 dev net0
ip addr del fc00:1::1/64 dev net0
ip route add 20.0.0.1/24 via 10.0.0.254
ip route add 20.0.0.1/24 dev net0
ip link add vrf10 type vrf table 10
ip link del vrf10 type vrf table 10
ip link set dev dum0 master vrf10
ip link set dev dum0 nomaster

# basic switching
ip link add br0 type bridge
ip link set dev dum0 master br0
ip link set dev dum0 nomaster

# basic vxlan vtep
ip link add vx100 type vxlan id 100 local 1.1.1.1 remote 2.2.2.2 dev dum0 dstport 4789
ip link del vx100

# segment routing
ip sr tunsrc set fc00:1::1
ip route add 2001:12::1/64 encap seg6 mode encap segs fc00:1::10 dev dum0
ip route del 2001:12::1/64 encap seg6 mode encap segs fc00:1::10 dev dum0
ip route add fc00:1::10/32 encap seg6local action End.DX4 nh4 10.0.0.1 dev dum0
ip route del fc00:1::10/32 encap seg6local action End.DX4 nh4 10.0.0.1 dev dum0
```


