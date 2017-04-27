#!/bin/bash

# 
# Run on BBB to set the default gateway.
# You may also need to update your /etc/resolv.conf to point to a valid nameserver:
# nameserver 8.8.8.8
#

BBB_HOST_IFACE="usb0"

route add default gw $(arp -n | grep $BBB_HOST_IFACE | cut -d' ' -f1)
