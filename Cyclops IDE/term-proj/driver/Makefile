#
# kbuild Makefile based on example code from
# http://derekmolloy.ie/writing-a-linux-kernel-module-part-1-introduction/
# Linux Device Drivers 3rd Edition by Jonathan Corbet, Alessandro Rubini,
# and Greg Kroah-Hartman.
#

# If KERNELRELEASE is defined, we've been invoked from the
# kernel build system and can use its language.
ifneq ($(KERNELRELEASE),)
	obj-m := pin-pirate.o
# Otherwise we were called directly from the command
# line; invoke the kernel build system.
else
	KERNELDIR ?= /lib/modules/$(shell uname -r)/build
	PWD := $(shell pwd)

default:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) modules

.PHONY: clean
clean:
	$(MAKE) -C $(KERNELDIR) M=$(PWD) clean

.PHONY: load
load: default
	sudo insmod pin-pirate.ko
	lsmod

.PHONY: unload
unload:
	sudo rmmod pin-pirate.ko	
endif
