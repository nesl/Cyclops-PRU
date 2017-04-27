/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */
#ifndef NESL_PRU_GPIO_H
#define NESL_PRU_GPIO_H

#include "nesl_pru_regs.h"

enum pru_pin
{
#if PRU_NUM == 0
    // PRU0
    P8_11 = 1 << 15, // output only
    P8_12 = 1 << 14, // output only
    P8_15 = 1 << 15, // input only
    P8_16 = 1 << 14, // input only
    P9_24 = 1 << 16, // input only
    P9_25 = 1 << 7,
    P9_27 = 1 << 5,
    P9_28 = 1 << 3,
    P9_29 = 1 << 1,
    P9_30 = 1 << 2,
    P9_31 = 1 << 0,
    P9_41A = 1 << 16, // input only
    P9_41B = 1 << 6,
    P9_42B = 1 << 4,
#else
    // PRU1
    P8_20 = 1 << 13,
    P8_21 = 1 << 12,
    P8_27 = 1 << 8,
    P8_28 = 1 << 10,
    P8_29 = 1 << 9,
    P8_39 = 1 << 6,
    P8_40 = 1 << 7,
    P8_41 = 1 << 4,
    P8_42 = 1 << 5,
    P8_43 = 1 << 2,
    P8_44 = 1 << 3,
    P8_45 = 1 << 0,
    P8_46 = 1 << 1,
    P9_26 = 1 << 16, // input only
#endif
};

inline unsigned int read_pin(enum pru_pin pin)
{
    return __R31 & pin;
}

inline void toggle_pin(enum pru_pin pin)
{
    __R30 ^= pin;
}

inline void assert_pin(enum pru_pin pin)
{
    __R30 |= pin;
}

inline void deassert_pin(enum pru_pin pin)
{
    __R30 &= ~pin;
}
#endif
