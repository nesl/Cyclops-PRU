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
#include "nesl_pru_time.h"
#include "nesl_pru_ticks.h"
#include "nesl_pru_gpio.h"
#include <stdint.h>
#include <stdlib.h>

#include "shared_conf.h"

#define SHARED_MEM_BASE 0x10000

void
terminate()
{
    TRIG_INTC(4); // Trigger interrupt PRUEVENT_1
    __halt(); // halt the PRU
}

cycle_t
read_cc(const struct cyclecounter *cc)
{
    return IEP_CNT;
}

/*
 * Custom function that lets you slew the IEP counter.
 * This is just for demo purposes. You should define your own algorithm.
 */
int
slew_cc(s64 delta)
{
    uint8_t comp_cycles = 0;
    uint8_t comp_inc = 0;
    uint64_t abs_delta = llabs(delta);

    if (abs_delta < 100) {
        if (delta < 0) {
            // PRU clock was too fast
            comp_cycles = abs_delta / 5;
            comp_inc = 0;
        } else {
            // PRU clock was too slow
            comp_cycles = 1;
            comp_inc = abs_delta / 5;
        }

        SET_IEP_COMP_INC(comp_inc);
        IEP_COMPEN = comp_cycles;

        return 1;
    } else {
        return 0;
    }
}

/*
 * This program demonstrates time synchronization between the host and PRU.
 *
 * The PRU will send it's own time to the host every ~1 second and the host
 * will print that time out on the terminal.
 * Every ~5 seconds, we will do a time synchronization.
 * This will continue for ~30 seconds and then we terminate.
 */
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
    DISABLE_IEP_TMR();
    ENABLE_IEP_TMR();
    IEP_CNT = 0;

    struct pru_time time;

    // Initialize pru_time with IEP as the source
    // slew_cc is optional
    //init_pru_time(&time, 5, 0, 32, read_cc, slew_cc);
    init_pru_time(&time, 5, 0, 32, read_cc, NULL);

    short status = -1;
    uint64_t data = 0;

    // Run for ~30 seconds total.
    int i = 30;
    while(i--) {
        // Do synchronization every ~5 seconds
        if (i % 5 == 0) {
            // Time synchronization started
            u64 ts_pru = read_pru_time(&time);

#if PRU_NUM == 0
            // Send a pulse on P9_27
            assert_pin(P9_27);
            WAIT_US(10);
            deassert_pin(P9_27);
#else
            // Send a pulse on P9_27
            assert_pin(P8_46);
            WAIT_US(10);
            deassert_pin(P8_46);
#endif

            // Get the time when the host received the pulse
            uint64_t ts_host = 0;
            do {
                data = rbuf_read_uint64(rec_buf, &status);
            } while(status);
            ts_host = data;

            // Calculate the offset between host and pru
            s64 delta = adj_pru_time(&time, ts_pru, ts_host);

            // Debugging. If you want to print the ts_pru and delta.
            //rbuf_write_uint64(send_buf, ts_pru);
            //rbuf_write_uint64(send_buf, delta);
        }

        // Send current pru time back to host for printing
        rbuf_write_uint64(send_buf, read_pru_time(&time));
        // Interrupt the host: there is a message in the rbuffer
        TRIG_INTC(3); // Trigger interrupt PRUEVENT_0

        // Pause for a second. Don't want to flood the host with messages.
        WAIT_MS(1000);
    }

    DISABLE_IEP_TMR();

    // Exiting the application
    terminate();
}
