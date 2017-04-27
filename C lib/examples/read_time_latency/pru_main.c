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

void
terminate()
{
    TRIG_INTC(4); // Trigger interrupt PRUEVENT_1
    __halt(); // halt the PRU
}

/*
 * Measure the latency for read_pru_time() and adj_pru_time().
 *
 * PRU is 200 MHz -> 1 cycle is 5 ns
 *
 * ie_get_time = 124 cycles = 620 ns
 * adj_pru_time = 58 cycles = 290 ns
 * read IEP counter = 23 cycles = 115 ns
 */
int main()
{
    uint32_t *shared_mem = (void*) (unsigned long) 0x10000;

    ENABLE_TICKS();

    // IEP is our clock source
    DISABLE_IEP_TMR();
    ENABLE_IEP_TMR();
    IEP_CNT = 0;

    uint64_t ts = 0;
    uint64_t host_ts = 0;

    struct iep_time time;
    init_iep_time(&time);

    unsigned int i = 1000;
    while(i--) {
        TICKS = 0;
        uint64_t ts_pru = iep_get_time(&time);
        //iep_adj_time(&time, ts, host_ts);
        //uint32_t iep_cnt = IEP_CNT;
        *shared_mem = TICKS;
        shared_mem++;
    }

    DISABLE_IEP_TMR();

    // Exiting the application
    terminate();
}
