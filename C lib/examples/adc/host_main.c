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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>
#include <prussdrv.h>
#include <pruss_intc_mapping.h>

int main (void)
{
    int n, ret;
    uint32_t *shared_mem;

    if(getuid()!=0){
        printf("You must run this program as root. Exiting.\n");
        exit(EXIT_FAILURE);
    }

    /* Initialize structure used by prussdrv_pruintc_intc   */
    /* PRUSS_INTC_INITDATA is found in pruss_intc_mapping.h */
    tpruss_intc_initdata pruss_intc_initdata = PRUSS_INTC_INITDATA;

    /* Allocate and initialize memory */
    prussdrv_init ();
    ret = prussdrv_open (PRU_EVTOUT_0);
    if(ret){
        printf("Failed to open the PRU-ICSS, have you loaded the overlay?");
        exit(EXIT_FAILURE);
    }
    /* Map PRU's INTC */
    prussdrv_pruintc_init(&pruss_intc_initdata);

    // Map PRU's shared memory into user-space
    if (prussdrv_map_prumem(PRUSS0_SHARED_DATARAM, (void **) &shared_mem)) {
        printf("map shared memory failed\n");
        exit(EXIT_FAILURE);
    }

    memset(shared_mem, 0, 0x2000);

    /* Load the memory data file */
    prussdrv_load_datafile(PRU_NUM, "./data.bin");

    /* Load and execute binary on PRU */
    prussdrv_exec_program (PRU_NUM, "./text.bin");

    /* Wait for event completion from PRU */
    n = prussdrv_pru_wait_event (PRU_EVTOUT_0);  // This assumes the PRU generates an interrupt
    //prussdrv_pru_clear_event(PRU_EVTOUT0, PRU0_ARM_INTERRUPT);
    // connected to event out 0 immediately before halting
    printf("PRU program completed, event number %d.\n", n);
    int i;
    for (i = 0; i < 7; i++) {
        printf("Channel %lu: %f Volts\n",
                shared_mem[i*2],
                (shared_mem[i*2+1] / 4096.0f) * 1.8f); // 12 bit ADC
    }

    /* Disable PRU and close memory mappings */
    prussdrv_pru_disable(PRU_NUM);
    prussdrv_exit ();
    return(EXIT_SUCCESS);
}
