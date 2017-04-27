#!/bin/bash

CAPE_MGR="/sys/devices/platform/bone_capemgr/slots"
CAPE_MGR_OLD="/sys/devices/bone_capemgr.9/slots"

if [ ! -e $CAPE_MGR ]; then
    CAPE_MGR=$CAPE_MGR_OLD
fi

NAME=${1:-NESL-PRU}

OVERLAYS=$(cat $CAPE_MGR | grep "$NAME")
if [ ! -z "$OVERLAYS" ]; then
    echo "$OVERLAYS"
    read -p "Confirm, unload these overlays? [y/n]: " user_in
    if [ "$user_in" == "y" ]; then
        cat $CAPE_MGR | grep "$NAME" | cut -d':' -f1 | xargs -I{} sh -c "echo -{} > $CAPE_MGR"

        sleep 1

        cat $CAPE_MGR
    fi
fi
