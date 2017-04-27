#!/bin/bash

#
# Run this on the BBB to install kernel headers for kernel module development
#

KERNEL_HEADERS_MAKEFILE="/lib/modules/$(uname -r)/build/Makefile"

if [ ! -e $KERNEL_HEADERS_MAKEFILE ]; then
	sudo apt-get update
	sudo apt-get install -y linux-headers-$(uname -r)
else
	echo Kernel headers already installed $KERNEL_HEADERS_MAKEFILE
fi
