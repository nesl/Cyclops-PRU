#!/bin/sh

CURRENT_DIR="${0%/*}"

cd $CURRENT_DIR

DTS_NAME="PRU-ACTIVATE"
FIRMWARE_DIR="/lib/firmware/"
CAPEMGR_SLOTS="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPEMGR_SLOTS ]; then
	CAPEMGR_SLOTS="/sys/devices/platform/bone_capemgr/slots"
	if [ ! -e $CAPEMGR_SLOTS ]; then
		echo "Bone Cape Manager not found"
	fi
fi

is_active()
{
	for f in /sys/class/uio/*
	do
		if cat "${f}/name" 2>/dev/null | grep --quiet "pruss*"; then
			return 0
		fi
	done

	return 1
}

if [ "$1" = "unload" ]; then
	if is_active; then
		if cat $CAPEMGR_SLOTS | grep --quiet $DTS_NAME ; then
			SLOT_UNLOAD="sudo sh -c 'echo -$(($(cat $CAPEMGR_SLOTS | grep $DTS_NAME | cut -d':' -f1))) > $CAPEMGR_SLOTS'"
			eval $SLOT_UNLOAD
			sleep 1
		fi
	fi
else
	if ! is_active; then
	
		if [ ! -f "${FIRMWARE_DIR}${DTS_NAME}-00A0.dtbo" ]; then
			sudo cp ../device-tree/${DTS_NAME}-00A0.dts $FIRMWARE_DIR
			sudo dtc -O dtb -I dts -o ${FIRMWARE_DIR}${DTS_NAME}-00A0.dtbo -b 0 -@ ${FIRMWARE_DIR}${DTS_NAME}-00A0.dts
		fi
		
		if ! cat $CAPEMGR_SLOTS | grep --quiet $DTS_NAME ; then
			SLOT_LOAD="sudo sh -c 'echo $DTS_NAME > $CAPEMGR_SLOTS'"
			eval $SLOT_LOAD
			sleep 1
		fi
	fi
fi

if is_active; then
	echo "PRUSS Active"
else
	echo "PRUSS Disabled"
fi
