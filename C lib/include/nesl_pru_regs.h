/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on code from the Beaglebone PRU IO project.
 * https://github.com/outer-space-sounds/beaglebone-pruio
 * Please see the file ATTRIB in the repository root
 * directory for copyright and GNU GPLv3 license information.
 */

#ifndef NESL_PRU_REGS_H
#define NESL_PRU_REGS_H

#include <stdint.h>

volatile register unsigned int __R31, __R30;

#if PRU_NUM == 0
static uint32_t *CTRL_REG = (uint32_t*) 0x22000;
#else
static uint32_t *CTRL_REG = (uint32_t*) 0x24000;
#endif

#define HWREG(addr) (*((volatile uint32_t *)(addr)))

// PRU Module Registers
#define PRU_ICSS_CFG 0x26000
#define PRU_ICSS_CFG_SYSCFG 0x04

// Enable OCP so we can access the whole memory map for the
// device from the PRU. Clear bit 4 of SYSCFG register
void init_ocp()
{
    HWREG(PRU_ICSS_CFG + PRU_ICSS_CFG_SYSCFG) &= ~(1 << 4);
}

#endif
