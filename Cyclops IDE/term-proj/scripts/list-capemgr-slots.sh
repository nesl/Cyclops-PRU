#!/bin/sh

CAPE_MGR_SLOTS="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPE_MGR_SLOTS ]; then
	CAPE_MGR_SLOTS="/sys/devices/platform/bone_capemgr/slots"
	if [ ! -e $CAPE_MGR_SLOTS ]; then
		echo "Bone Cape Manager not found"
	fi
fi

cat $CAPE_MGR_SLOTS
