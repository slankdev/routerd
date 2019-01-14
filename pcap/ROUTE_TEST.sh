#!/bin/sh

sudo ip link add dum0 type dummy
sudo ip link set dum0 up
sudo ip addr add 10.0.0.1/24 dev dum0
sudo ip addr add 2001::1/64 dev dum0
sudo ip route add 10.0.0.2/32 dev dum0
sudo ip route add 1.1.1.1/32 via 10.0.0.2
sudo ip route add 20.0.0.0/24 via 10.0.0.2
sudo ip route add fc00:1::1/128 via 2001::2
sudo ip link del dum0

