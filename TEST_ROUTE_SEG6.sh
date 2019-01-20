#!/bin/sh
set -ue

# LINK & ADDR
echo -------
sudo ip link add dum0 type dummy
sudo ip link set dum0 up
sudo ip addr add 10.0.0.1/24 dev dum0
sudo ip addr add 2001:1::1/64 dev dum0
ip addr show dev dum0
echo -------

# ROUTE
sudo ip route add 20.0.0.0/24 encap seg6 mode encap segs fc00:2::10 dev dum0
sudo ip route add fc00:1::10/128 encap seg6local action End.DX4 nh4 10.0.0.1 dev dum0
ip route show
ip -6 route show
echo -------

# ROUTE
sudo ip route del 20.0.0.0/24
sudo ip route del fc00:1::10/128

# LINK & ADDR
sudo ip addr del 10.0.0.1/24 dev dum0
sudo ip addr del 2001:1::1/64 dev dum0
sudo ip link del dum0
