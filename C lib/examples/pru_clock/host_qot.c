/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on helloworld_capture.c, a simple C example showing how to register
 * to listen for capture events by Fatima Anwar and Andrew Symington.
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
#include <sys/time.h>
#include <sys/timex.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <stdint.h>
#include <linux/ptp_clock.h>

#include "host_qot.h"

#define PTP_PIN_SETFUNC    _IOW(PTP_CLK_MAGIC, 7, struct ptp_pin_desc)

//struct ptp_pin_desc {
//    /*
//     * Hardware specific human readable pin name. This field is
//     * set by the kernel during the PTP_PIN_GETFUNC ioctl and is
//     * ignored for the PTP_PIN_SETFUNC ioctl.
//     */
//    char name[64];
//    /*
//     * Pin index in the range of zero to ptp_clock_caps.n_pins - 1.
//     */
//    unsigned int index;
//    /*
//     * Which of the PTP_PF_xxx functions to use on this pin.
//     */
//    unsigned int func;
//    /*
//     * The specific channel to use for this function.
//     * This corresponds to the 'index' field of the
//     * PTP_EXTTS_REQUEST and PTP_PEROUT_REQUEST ioctls.
//     */
//    unsigned int chan;
//    /*
//     * Reserved for future use.
//     */
//    unsigned int rsv[5];
//};

char *device_m;             /* PTP device */
int index_m;                /* Channel index, '1' corresponds to 'TIMER6' */
int fd_m;                   /* device file descriptor */
struct ptp_pin_desc desc;   /* Pin configuration */
struct ptp_extts_event event;   /* PTP event */

/**
 * Initialize the QoT system for capturing input event timestamps.
 *
 * Params:
 * dev - PTP device file
 * channel - Channel index, '1' corresponds to 'TIMER6'
 */
int
init_qot(char* dev, int channel)
{
    struct ptp_extts_request extts_request;     /* External timestamp req */

    device_m = dev;                             /* PTP device */
    index_m = channel;                          /* Channel index */

    /* Open the character device */
    fd_m = open(device_m, O_RDWR);
    if (fd_m < 0) {
        fprintf(stderr, "opening device %s: %s\n",
                device_m, strerror(errno));
        return -1;
    }
    printf("Device opened %d\n", fd_m);

    memset(&desc, 0, sizeof(desc));
    desc.index = index_m;
    desc.func = 1;              // '1' corresponds to external timestamp
    desc.chan = index_m;
    if (ioctl(fd_m, PTP_PIN_SETFUNC, &desc)) {
        printf("Set pin func failed for %d\n", fd_m);
        return -1;
    }
    printf("Set pin func successful for %d\n", fd_m);

    // Setup to request timestamps from the pin
    memset(&extts_request, 0, sizeof(extts_request));
    extts_request.index = index_m;
    extts_request.flags = PTP_ENABLE_FEATURE;
    if (ioctl(fd_m, PTP_EXTTS_REQUEST, &extts_request)) {
        printf("Requesting timestamps failed for %d\n", fd_m);
        return -1;
    }
    printf("Requesting timestamps success for %d\n", fd_m);

    return 0;
}

int
deinit_qot()
{
    int ret = 0;

    /* Disable the pin */
    memset(&desc, 0, sizeof(desc));
    desc.index = index_m;
    desc.func = 0;              // '0' corresponds to no function
    desc.chan = index_m;
    if (ioctl(fd_m, PTP_PIN_SETFUNC, &desc)) {
        printf("Disable pin func failed for %d\n", fd_m);
        ret = -1;
    }

    /* Close the character device */
    close(fd_m);

    return ret;
}

/**
 * Reads QoT timestamp of event.
 * This call will block until an event is triggered!
 *
 * Params:
 * err - pointer to error, -1 on error
 * Return:
 * Time of event in nanoseconds
 */
uint64_t
qot_read_event_ts(int* err)
{
    /* Read events coming in */

    // Debugging
    //printf("Trying to read events %d\n", running++);

    // This call to read is blocking
    int cnt = read(fd_m, &event, sizeof(event));
    if (cnt != sizeof(event)) {
        printf("Cannot read QoT event");
        if (err != NULL) *err = -1;
        return 0;
    }
    uint64_t nano_ts = event.t.sec * 1000000000;
    nano_ts += event.t.nsec;

    // Debugging
    // printf("Sync Time - %lld.%09u - %llu ns\n",
    //          event.t.sec, event.t.nsec, nano_ts);

    //printf("QoT event time: %llu ns\n", nano_ts);

    return nano_ts;
}
