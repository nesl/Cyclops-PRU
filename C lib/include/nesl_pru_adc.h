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

#ifndef NESL_PRU_ADC_H
#define NESL_PRU_ADC_H

#include <stdint.h>
#include "nesl_pru_adc.h"

// ADC Registers
#define ADC_TSC 0x44e0d000
#define ADC_TSC_IRQSTATUS 0x28
#define ADC_TSC_IRQENABLE_SET 0x2c
#define ADC_TSC_CTRL 0x40
#define ADC_TSC_ADCRANGE 0x48
#define ADC_TSC_CLKDIV 0x4c
#define ADC_TSC_STEPENABLE 0x54
#define ADC_TSC_STEPCONFIG1 0x64
#define ADC_TSC_STEPDELAY1 0x68
#define ADC_TSC_STEPCONFIG2 0x6c
#define ADC_TSC_STEPDELAY2 0x70
#define ADC_TSC_STEPCONFIG3 0x74
#define ADC_TSC_STEPDELAY3 0x78
#define ADC_TSC_STEPCONFIG4 0x7c
#define ADC_TSC_STEPDELAY4 0x80
#define ADC_TSC_STEPCONFIG5 0x84
#define ADC_TSC_STEPDELAY5 0x88
#define ADC_TSC_STEPCONFIG6 0x8c
#define ADC_TSC_STEPDELAY6 0x90
#define ADC_TSC_STEPCONFIG7 0x94
#define ADC_TSC_STEPDELAY7 0x98
#define ADC_TSC_STEPCONFIG8 0x9c
#define ADC_TSC_STEPDELAY8 0xa0
#define ADC_TSC_STEPCONFIG9 0xa4
#define ADC_TSC_STEPDELAY9 0xa8
#define ADC_TSC_STEPCONFIG10 0xac
#define ADC_TSC_STEPDELAY10 0xb0
#define ADC_TSC_STEPCONFIG11 0xb4
#define ADC_TSC_STEPDELAY11 0xb8
#define ADC_TSC_STEPCONFIG12 0xbc
#define ADC_TSC_STEPDELAY12 0xc0
#define ADC_TSC_STEPCONFIG13 0xc4
#define ADC_TSC_STEPDELAY13 0xc8
#define ADC_TSC_STEPCONFIG14 0xcc
#define ADC_TSC_STEPDELAY14 0xd0
#define ADC_TSC_STEPCONFIG15 0xd4
#define ADC_TSC_STEPDELAY15 0xd8
#define ADC_TSC_STEPCONFIG16 0xdc
#define ADC_TSC_STEPDELAY16 0xe0
#define ADC_TSC_FIFO0COUNT 0xe4
#define ADC_TSC_FIFO1COUNT 0xf0
#define ADC_TSC_FIFO0DATA 0x100
#define ADC_TSC_FIFO1DATA 0x200

// Clock Module registers
#define CM_PER 0x44e00000
#define CM_PER_GPIO1_CLKCTRL 0xac
#define CM_PER_GPIO2_CLKCTRL 0xb0
#define CM_PER_GPIO3_CLKCTRL 0xb4
#define CM_WKUP 0x44e00400
#define CM_WKUP_GPIO0_CLKCTRL 0x08
#define CM_WKUP_ADC_TSK_CLKCTL 0xbc

inline void wait_for_adc()
{
    // Wait for irqstatus[1] to go high
    while((HWREG(ADC_TSC + ADC_TSC_IRQSTATUS) & (1<<1)) == 0){
        // nothing
    }

    // Clear status (write 1)
    HWREG(ADC_TSC + ADC_TSC_IRQSTATUS) |= (1<<1);
}

inline void adc_start_sampling()
{
    // Enable steps 1 to 7
    HWREG(ADC_TSC + ADC_TSC_STEPENABLE) = 0xfe;
    /* HWREG(ADC_TSC + ADC_TSC_STEPENABLE) = 0b111110; // 1 to 5 */
}

inline unsigned int read_adc_count()
{
    unsigned int count = HWREG(ADC_TSC + ADC_TSC_FIFO0COUNT);

    return count;
}

inline unsigned int read_adc_data()
{
    unsigned int data = HWREG(ADC_TSC + ADC_TSC_FIFO0DATA);

    return data;
}

void init_adc()
{
    // Enable clock for adc module.
    HWREG(CM_WKUP + CM_WKUP_ADC_TSK_CLKCTL) = 0x02;

    // Disable ADC module temporarily.
    HWREG(ADC_TSC + ADC_TSC_CTRL) &= ~(0x01);

    // To calculate sample rate:
    // fs = 24MHz / (CLK_DIV*2*Channels*(OpenDly+Average*(14+SampleDly)))
    // We want 48KHz. (Compromising to 50KHz)
    unsigned int clock_divider = 1;
    unsigned int open_delay = 0;
    unsigned int average = 0;       // can be 0 (no average), 1 (2 samples),
    // 2 (4 samples),  3 (8 samples)
    // or 4 (16 samples)
    unsigned int sample_delay = 0;

    // Set clock divider (set register to desired value minus one).
    HWREG(ADC_TSC + ADC_TSC_CLKDIV) = clock_divider - 1;

    // Set values range from 0 to FFF.
    HWREG(ADC_TSC + ADC_TSC_ADCRANGE) = (0xfff << 16) & (0x000);

    // Disable all steps. STEPENABLE register
    HWREG(ADC_TSC + ADC_TSC_STEPENABLE) &= ~(0xff);

    // Unlock step config register.
    HWREG(ADC_TSC + ADC_TSC_CTRL) |= (1 << 2);

    // Set config and delays for step 1:
    // Sw mode, one shot mode, fifo0, channel 0.
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG1) = 0 | (0<<26) | (0<<19) | (0<<15) | (average<<2) | (0);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY1)  = 0 | (sample_delay - 1)<<24 | open_delay;

    // Set config and delays for step 2:
    // Sw mode, one shot mode, fifo0, channel 1.
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG2) = 0 | (0x0<<26) | (0x01<<19) | (0x01<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY2)  = 0 | (sample_delay - 1)<<24 | open_delay;

    // Set config and delays for step 3:
    // Sw mode, one shot mode, fifo0, channel 2.
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG3) = 0 | (0x0<<26) | (0x02<<19) | (0x02<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY3)  = 0 | ((sample_delay - 1)<<24) | open_delay;

    // Set config and delays for step 4:
    // Sw mode, one shot mode, fifo0, channel 3.
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG4) = 0 | (0x0<<26) | (0x03<<19) | (0x03<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY4)  = 0 | ((sample_delay - 1)<<24) | open_delay;

    // Set config and delays for step 5:
    // Sw mode, one shot mode, fifo0, channel 4.
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG5) = 0 | (0x0<<26) | (0x04<<19) | (0x04<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY5)  = 0 | ((sample_delay - 1)<<24) | open_delay;

    // Set config and delays for step 6:
    // Sw mode, one shot mode, fifo0, CHANNEL 6!
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG6) = 0 | (0x0<<26) | (0x06<<19) | (0x06<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY6)  = 0 | ((sample_delay - 1)<<24) | open_delay;

    // Set config and delays for step 7:
    // Sw mode, one shot mode, fifo0, CHANNEL 5!
    HWREG(ADC_TSC + ADC_TSC_STEPCONFIG7) = 0 | (0x0<<26) | (0x05<<19) | (0x05<<15) | (average<<2) | (0x00);
    HWREG(ADC_TSC + ADC_TSC_STEPDELAY7)  = 0 | ((sample_delay - 1)<<24) | open_delay;

    // Enable tag channel id. Samples in fifo will have channel id bits ADC_CTRL register
    HWREG(ADC_TSC + ADC_TSC_CTRL) |= (1 << 1);

    // Clear End_of_sequence interrupt
    HWREG(ADC_TSC + ADC_TSC_IRQSTATUS) |= (1<<1);

    // Enable End_of_sequence interrupt
    HWREG(ADC_TSC + ADC_TSC_IRQENABLE_SET) |= (1 << 1);

    // Lock step config register. ACD_CTRL register
    HWREG(ADC_TSC + ADC_TSC_CTRL) &= ~(1 << 2);

    // Clear FIFO0 by reading from it.
    unsigned int count = HWREG(ADC_TSC + ADC_TSC_FIFO0COUNT);
    unsigned int i;
    volatile unsigned int data;
    for(i=0; i<count; i++){
        data = HWREG(ADC_TSC + ADC_TSC_FIFO0DATA);
    }

    // Clear FIFO1 by reading from it.
    count = HWREG(ADC_TSC + ADC_TSC_FIFO1COUNT);
    for(i=0; i<count; i++){
        data = HWREG(ADC_TSC + ADC_TSC_FIFO1DATA);
    }

    // Enable ADC Module. ADC_CTRL register
    HWREG(ADC_TSC + ADC_TSC_CTRL) |= 1;
}

#endif
