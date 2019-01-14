#!/bin/sh

sudo ip link add dum0 type dummy
sudo ip addr add 1.1.1.1/24 dev dum0
sudo ip addr del 1.1.1.1/24 dev dum0
sudo ip addr add 2001:12::1/64 dev dum0
sudo ip addr del 2001:12::1/64 dev dum0
sudo ip link del dum0
