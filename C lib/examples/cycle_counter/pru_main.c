/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on code from Derek Molloy for the book "Exploring BeagleBone:
 * Tools and Techniques for Building with Embedded Linux" by John Wiley & Sons,
 * 2014 ISBN 9781118935125. Please see the file ATTRIB in the repository root
 * directory for copyright and GNU GPLv3 license information.
 */

#include "nesl_pru_intc.h"
#include "nesl_pru_ticks.h"
#include "nesl_pru_wait.h"
#include <stdint.h>

int main()
{
    uint32_t *shared_mem = (void*) (unsigned long) 0x10000;
    uint32_t *mem1 = shared_mem;
    uint32_t *mem2 = shared_mem+1;
    uint32_t *mem3 = shared_mem+2;

    ENABLE_TICKS();

    TICKS = 0;
    WAIT_CYCLES(200); // 1 us
    *mem1 = TICKS;

    TICKS = 0;
    WAIT_US(1); // 1 us
    *mem2 = TICKS;

    TICKS = 0;
    // Wait 1 us using inline assembly
    __asm__ __volatile__
        (
         " LDI32 r0, 99\n"
         "WAIT_LABEL: SUB r0, r0, 1\n"
         " QBNE WAIT_LABEL, r0, 0\n"
        );
    *mem3 = TICKS;

    // Exiting the application - send the interrupt
    TRIG_INTC(3); // Trigger interrupt PRUEVENT_0
    __halt(); // halt the PRU
}
