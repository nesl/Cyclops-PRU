#!/bin/bash

CAPE_MGR="/sys/devices/platform/bone_capemgr/slots"
CAPE_MGR_OLD="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPE_MGR ]; then
    CAPE_MGR=$CAPE_MGR_OLD
fi

cat $CAPE_MGR
