#!/bin/bash

if ! which ntpdate; then
    sudo apt-get update
    sudo apt-get -y install ntpdate
fi

sudo ntpdate -b time.nist.gov
