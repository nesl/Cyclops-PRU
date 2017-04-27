/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on helloworld_capture.c, a simple C example showing how to register
 * to listen for capture events by Fatima Anwar and Andrew Symington.
 */

/*
 * Based on code from Derek Molloy for the book "Exploring BeagleBone:
 * Tools and Techniques for Building with Embedded Linux" by John Wiley & Sons,
 * 2014 ISBN 9781118935125. Please see the file ATTRIB in the repository root
 * directory for copyright and GNU GPLv3 license information.
 */

#include <errno.h>
#include <fcntl.h>
#include <inttypes.h>
#include <math.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ioctl.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <pthread.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

#include "shared_conf.h"
#include "nesl_pru_rbuffer.h"
#include "host_qot.h"

void *report_thread(void *value)
{
    int err = 0;
    uint64_t nano_ts;
    int i = 1;

    while (1) {
        int channel;
        nano_ts = qot_read_event_ts(&err, &channel);
        if (channel == 0) {
            printf("%lld, chan %d\n", nano_ts, channel);
        } else if (channel == 1) {
            printf("%lld, chan %d\n", nano_ts, channel);
        }
    }
}

/*
 * Tried asserting P8.9 and P8.10 simultaneously.
 * QoT timestamps from both input captures are very accurate.
 * Only a few hundred nanoseconds apart.
 *
 * 1425252214579059335, chan 1
 * 1425252214579058961, chan 0
 * 1425252214579159376, chan 1
 * 1425252214579159002, chan 0
 * 1425252214829071585, chan 1
 * 1425252214829071211, chan 0
 * 1425252214829171585, chan 1
 * 1425252214829171211, chan 0
 * 1425252215079083793, chan 1
 * 1425252215079083419, chan 0
 * 1425252215079183835, chan 1
 * 1425252215079183461, chan 0
 * 1425252215329096043, chan 1
 * 1425252215329095669, chan 0
 * 1425252215329196085, chan 1
 * 1425252215329195711, chan 0
 * 1425252215579108293, chan 1
 * 1425252215579107919, chan 0
 * 1425252215579208335, chan 1
 * 1425252215579207961, chan 0
 *
 */
int main (void)
{
    int err;

    if(getuid()!=0){
        printf("You must run this program as root. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    pthread_t thread;

    // Setup QoT
    if (init_qot("/dev/ptp1")) {
        printf("Initialize QoT time sync failed\n");
        exit(EXIT_FAILURE);
    }

    /* Thread for sending messages to the PRU */
    if (pthread_create(&thread, NULL, &report_thread, NULL)){
        printf("Failed to create thread!\n");
        exit(EXIT_FAILURE);
    }

    while(1) {}

    // Teardown QoT
    if (deinit_qot()) {
        printf("Deinitialize QoT time sync failed\n");
    }

    return(EXIT_SUCCESS);
}
