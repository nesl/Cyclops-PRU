#!/bin/bash

NAME=${1:-NESL-PRU}
DTBO=$NAME-00A0.dtbo
DTS=$NAME.dts

CAPE_MGR="/sys/devices/platform/bone_capemgr/slots"
CAPE_MGR_OLD="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPE_MGR ]; then
    CAPE_MGR=$CAPE_MGR_OLD
fi

cd ${0%/*}

echo "Compiling the overlay from .dts to .dtbo"

dtc -O dtb -o $DTBO -b 0 -@ $DTS

if [ $? -eq 0 ]; then
    echo "Installing into /lib/firmware"
    cp $DTBO /lib/firmware
fi

if [ $? -eq 0 ]; then
    echo "Activating PRU overlay"
    echo $NAME > $CAPE_MGR
    sleep 1
    cat $CAPE_MGR
fi
