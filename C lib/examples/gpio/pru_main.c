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
#include "nesl_pru_gpio.h"
#include "nesl_pru_wait.h"

int main()
{
#if PRU_NUM == 0
    // Set pin to high
    assert_pin(P9_27);

    // Blink pin P9_27 every half of a second
    // Stop when P9_28 is set high
    while(!read_pin(P9_28)){
        toggle_pin(P9_27);
        WAIT_MS(500);
    }

    // Set pin to low before we exit
    deassert_pin(P9_27);
#else
    // Set pin to high
    assert_pin(P8_46);

    // Blink pin P9_27 every half of a second
    // Stop when P9_28 is set high
    while(!read_pin(P8_45)){
        toggle_pin(P8_46);
        WAIT_MS(500);
    }

    // Set pin to low before we exit
    deassert_pin(P8_46);
#endif

    // Exiting the application - send the interrupt
    TRIG_INTC(3); // PRUEVENT_0 on PRU0_R31_VEC_VALID
    __halt(); // halt the PRU
}
