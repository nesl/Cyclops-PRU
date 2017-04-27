#!/bin/bash

#
# If you are using ethernet over USB.
# Then run this on the host to allow the BBB to access the internet.
#

# Network interface between host <--> internet
HOST_NET_IFACE="ens33"
#HOST_NET_IFACE="eth0"
# Network interface between host <--> BBB
HOST_BBB_IFACE="enx8030dc55737f"
#HOST_BBB_IFACE="eth2"

sudo iptables --table nat --append POSTROUTING --out-interface $HOST_NET_IFACE -j MASQUERADE
sudo iptables --append FORWARD --in-interface $HOST_BBB_IFACE -j ACCEPT
sudo sh -c 'echo 1 > /proc/sys/net/ipv4/ip_forward'
