#!/bin/bash

CAPE_MGR="/sys/devices/platform/bone_capemgr/slots"
CAPE_MGR_OLD="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPE_MGR ]; then
    CAPE_MGR=$CAPE_MGR_OLD
fi

NAME=${1:-NESL-PRU}

echo "$NAME" > $CAPE_MGR

sleep 1

cat $CAPE_MGR
