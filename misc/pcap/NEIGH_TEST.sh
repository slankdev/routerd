#!/bin/sh

sudo ip link add dum0 type dummy
sudo ip link set dum0 up
sudo ip addr add 10.0.0.1/24 dev dum0
sudo ip neigh add 10.0.0.2 lladdr 11:22:33:44:55:66 dev dum0
sudo ip neigh del 10.0.0.2 lladdr 11:22:33:44:55:66 dev dum0
sudo ip link del dum0
