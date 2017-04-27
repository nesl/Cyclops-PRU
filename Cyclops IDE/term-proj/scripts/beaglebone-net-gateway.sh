# 
# Run on BBB to set the gateway
#

sudo ifconfig usb0 192.168.7.2
sudo route add default gw 192.168.7.1
