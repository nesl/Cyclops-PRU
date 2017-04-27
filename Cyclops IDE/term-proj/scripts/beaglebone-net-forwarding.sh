#
# Run on host to allow the BBB to access the internet
#

# ens33 is the host interface between host <--> internet
# enx8030dc55737f is thehost interface between BBB <--> host
sudo ifconfig enx8030dc55737f 192.168.7.1
sudo iptables --table nat --append POSTROUTING --out-interface ens33 -j MASQUERADE
sudo iptables --append FORWARD --in-interface enx8030dc55737f -j ACCEPT
sudo sh -c 'echo 1 > /proc/sys/net/ipv4/ip_forward'
