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
sudo ip neigh add 10.0.0.2 lladdr 11:11:11:11:11:11 dev dum0
sudo ip neigh add 2001:1::2 lladdr 22:22:22:22:22:22 dev dum0
ip neigh
echo -------
sudo ip neigh del 10.0.0.2 lladdr 11:11:11:11:11:11 dev dum0
sudo ip neigh del 2001:1::2 lladdr 22:22:22:22:22:22 dev dum0

# LINK & ADDR
sudo ip addr del 10.0.0.1/24 dev dum0
sudo ip addr del 2001:1::1/64 dev dum0
sudo ip link del dum0
