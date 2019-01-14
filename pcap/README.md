
# Command

```
ip link add dum0 type dummy
ip link add br0 type bridge
ip link add link dum0 name dum0.10 type vlan id 10
ip link add vxlan0 type vxlan id 100 local 1.1.1.1 remote 2.2.2.2 dev dum0 dstport 4789
ip link del dum0
ip link set dum0 up
ip link set dum0 up
ip addr add 1.1.1.1/24 dev dum0
ip addr del 1.1.1.1/24 dev dum0
ip route add 2.2.2.2 via 1.1.1.10
ip route del 2.2.2.2 via 1.1.1.10
ip link set dum0 mtu 8000
ip link set dum0 promisc on
ip link set dum0 promisc off
ip neigh add 1.1.1.2 lladdr 11:22:33:44:55:66 dev dum0
ip neigh del 1.1.1.2 lladdr 11:22:33:44:55:66 dev dum0
ip -6 route add fc00:1::1 encap seg6local action End.DX4 nh4 10.1.1.1 dev dum0
ip -6 route del fc00:1::1 encap seg6local action End.DX4 nh4 10.1.1.1 dev dum0
ip route add 1.1.2.0/24 encap seg6 mode encap segs fc00:1::1,fc00:2::1,fc00:3::1 dev dum0
ip route del 1.1.2.0/24 encap seg6 mode encap segs fc00:1::1,fc00:2::1,fc00:3::1 dev dum0
```
