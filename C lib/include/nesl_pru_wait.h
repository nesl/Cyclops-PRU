/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */
#ifndef NESL_PRU_WAIT_H
#define NESL_PRU_WAIT_H

#define WAIT_MS(ms) \
{ \
    if (ms == 1) { \
        __delay_cycles(199999); \
    } else if (ms > 0) { \
        unsigned int _ms = ms-1; \
        while(_ms--) { \
            __delay_cycles(199993); \
        } \
        __delay_cycles(199993); \
    } \
}

#define WAIT_US(us) \
{ \
    if (us == 1) { \
        __delay_cycles(199); \
    } else if (us > 0) { \
        unsigned int _us = us-1; \
        while(_us--) { \
            __delay_cycles(193); \
        } \
        __delay_cycles(193); \
    } \
}
#define WAIT_CYCLES(cycles) __delay_cycles(cycles);

#endif
