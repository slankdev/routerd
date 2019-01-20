#!/bin/sh

sudo ip addr add 1.1.1.1/32 dev dum0
sudo ip addr del 1.1.1.1/32 dev dum0
