# Cyclops-PRU

The Beaglebone Black single-board computer is well-suited for real-time embedded applications because its SoC contains two Programmable Real-time Units" (PRUs): 200-MHz microcontrollers that run concurrently with the main 1-GHz CPU that runs Linux. 

This repo introduces "Cyclops": a web-browser-based IDE that facilitates the development of embedded applications on the Beaglebone Black's PRU. Users write PRU code in a simple JavaScript-inspired language, which Cyclops converts to PRU assembly code and deploys to the PRU. Cyclops automatically configures the Beaglebone's pinmux controller to use common I/O peripherals like ADC and PWM. Additionally, Cyclops includes a PRU library and Linux kernel module for synchronizing the PRU with the CPU clock, enabling the PRU to time-stamp sensor measurements using the Linux processor time within sub-microsecond accuracy. 

This repo consists of the C library and Cyclops IDE.
