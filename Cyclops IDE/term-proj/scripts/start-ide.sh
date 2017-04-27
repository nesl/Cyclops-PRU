#!/bin/bash

CURRENT_DIR="${0%/*}"

cd $CURRENT_DIR

sudo ./activate-pruss.sh
sudo ./install-BBB-kernel-headers.sh
if [ ! -e "/etc/udev/rules.d/99-pinpirate.rules" ]; then
	sudo cp ../driver/99-pinpirate.rules /etc/udev/rules.d/	
fi
if ! lsmod | grep --quiet pin_pirate; then
	echo pin-pirate LKM not loaded 
	pushd .
	cd ../driver	
	make load
	popd
else
	echo pin-pirate LKM loaded
fi

cd ../ide

python start.py
