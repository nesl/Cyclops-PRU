#!/bin/bash

#
#Copyright (c) Regents of the University of California, 2017. All rights reserved.
#See LICENSE and ATTRIB in the repository root.
#
#
#
#Based on code from Derek Molloy for the book "Exploring BeagleBone:
#Tools and Techniques for Building with Embedded Linux" by John Wiley & Sons,
#2014 ISBN 9781118935125. Please see the file ATTRIB in the repository root
#directory for copyright and GNU GPLv3 license information.
#

PRU_NUM=1; # which PRU 0 or 1
#PRU_SDK=/root/pru_2.0.0B2
PRU_SRC="pru_main.c"
HOST_SRC="host_main.c"
PRU_TARGET=pru.out
HOST_TARGET=host
GEN=gen
SCRIPT_DIR=${0%/*}

cd $SCRIPT_DIR
mkdir -p $GEN

echo "Compiling and linking the PRU firmware"
clpru --silicon_version=3 --keep_asm --c_src_interlist \
    --asm_directory=$GEN --obj_directory=$GEN --temp_directory=$GEN \
    -DPRU_NUM=$PRU_NUM \
    $PRU_SRC \
    -i../../include \
    -z AM3359_PRU.cmd \
    -o $GEN/$PRU_TARGET \
    -m $GEN/$PRU_TARGET.map
#-i$PRU_SDK/include
#-i$PRU_SDK/lib

if [ $? -ne 0 ]; then
    echo "Compile and link FAILED"
    exit 1
fi

pushd .
cd $GEN

# The output of the compiler/linker is an ELF executable.
# We cannot directly load the ELF file into the PRU because unlike Linux,
# the PRU does not have a ELF loader.
# In theory, we could parse the ELF file in our host application,
# but that is too much work.
# Instead, we break out the text and data segments and so that we can
# manually map them into PRU memory.
echo "Generating text and data segments (text.bin and data.bin)"
hexpru ../bin.cmd $PRU_TARGET

if [ $? -ne 0 ]; then
    echo "Generating text and data segments FAILED"
    exit 1
fi

popd

echo "Compiling and linking the host application"
gcc -I../../include -DPRU_NUM=$PRU_NUM $HOST_SRC -o $GEN/$HOST_TARGET -lpthread -lprussdrv

if [ $? -ne 0 ]; then
    echo "Compiling and linking the host application FAILED"
    exit 1
fi

echo "Build SUCCESSFUL."
echo "To run you must change to the gen directory:"
echo "cd gen/"
echo "./host"
