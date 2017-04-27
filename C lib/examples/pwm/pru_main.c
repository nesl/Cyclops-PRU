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

void pwm(enum pru_pin pin,
        unsigned int period_us, unsigned int pulse_width_us)
{
    // Keep sending PWM signal until P8_45 is asserted.
    while(read_pin(P8_45) == 0) {
        assert_pin(pin);
        WAIT_US(pulse_width_us);
        deassert_pin(pin);
        WAIT_US(period_us - pulse_width_us);
    }
}

int main()
{
    // Ouput PWM signal with period of 1 second and
    // pulse width of 0.5 seconds.
    pwm(P8_46, 1000000, 500000);

    // Exiting the application.
    TRIG_INTC(3);
    __halt(); // halt the PRU.
}
