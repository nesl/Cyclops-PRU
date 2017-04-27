/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on code from Derek Molloy for the book "Exploring BeagleBone:
 * Tools and Techniques for Building with Embedded Linux" by John Wiley & Sons, * 2014 ISBN 9781118935125. Please see the file ATTRIB in the repository root * directory for copyright and GNU GPLv3 license information.
 */

#include "nesl_pru_intc.h"
#include "nesl_pru_iep.h"
#include "nesl_pru_wait.h"
#include "nesl_pru_rbuffer.h"
#include "nesl_pru_ticks.h"
#include "nesl_pru_gpio.h"
#include <stdint.h>
#include <stdlib.h>

#include "shared_conf.h"

#define SHARED_MEM_BASE 0x10000
#define SYNC_PERIOD_NS 8000000000

void
terminate()
{
    TRIG_INTC(4); // Trigger interrupt PRUEVENT_1
    __halt(); // halt the PRU
}

int num_samples = 8000;
int input_high = 0;

void check_input_pin(struct iep_time *time, struct rbuffer *send_buf)
{
    if (!input_high) {
        if (read_pin(P8_45)) {
            input_high = 1;
            // Send current pru time back to host for printing
            rbuf_write_uint64(send_buf, iep_get_time(time));
            // Interrupt the host: there is a message in the rbuffer
            TRIG_INTC(3); // Trigger interrupt PRUEVENT_0
            num_samples--;
        }
    } else {
        if (!read_pin(P8_45)) {
            input_high = 0;
        }
    }
}

int main()
{
    // Wait enough time for host to startup
    WAIT_MS(1000);

    // Send PRU time back to host for printing
    struct rbuffer *send_buf =
        (struct rbuffer *) (uint32_t) (SHARED_MEM_BASE + RBUF_ADDR);
    init_rbuffer(send_buf);

    // Required for getting host timestamp
    struct rbuffer *rec_buf =
        (struct rbuffer *) (uint32_t) (SHARED_MEM_BASE + RBUF_ADDR
                + sizeof(struct rbuffer));

    // IEP is our clock source
    struct iep_time time;
    init_iep_time(&time);

    short status = -1;
    int first = 1;
    uint64_t last_ts = 0;
    uint64_t ts = 0;
    uint64_t host_ts = 0;

    while(num_samples > 0) {
        ts = iep_get_time(&time);
        // Do synchronization every SYNC_PERIOD_NS
        if (((ts - last_ts) > SYNC_PERIOD_NS) || first) {
            first = 0;

#if PRU_NUM == 0
            // Send a pulse on P9_27
            assert_pin(P9_27);
            deassert_pin(P9_27);
#else
            // Send a pulse on P8_46
            assert_pin(P8_46);
            deassert_pin(P8_46);
#endif
            // time of sync event
            ts = iep_get_time(&time);

            // Get the time when the host received the pulse
            do {
                host_ts = rbuf_read_uint64(rec_buf, &status);
                check_input_pin(&time, send_buf);
            } while(status);

            // Calculate the offset between host and pru
            iep_adj_time(&time, ts, host_ts);

            last_ts = iep_get_time(&time);
            // Debugging. If you want to print the ts and delta.
            //rbuf_write_uint64(send_buf, ts);
            //rbuf_write_uint64(send_buf, delta);
        }

        check_input_pin(&time, send_buf);

        //WAIT_MS(1000);
    }

    DISABLE_IEP_TMR();

    // Exiting the application
    terminate();
}
