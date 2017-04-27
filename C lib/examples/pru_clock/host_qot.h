/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on helloworld_capture.c, a simple C example showing how to register
 * to listen for capture events by Fatima Anwar and Andrew Symington.
 */

#ifndef HOST_QOT_H
#define HOST_QOT_H

#include <stdint.h>

int init_qot(char* dev, int channel);
int deinit_qot();
uint64_t qot_read_event_ts(int* err);

#endif
