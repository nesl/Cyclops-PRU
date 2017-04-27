#ifndef SHARED_CONF_H
#define SHARED_CONF_H

#define SHARED_MEM_BASE 0x10000

// Location of ring buffer needed for getting
// host timestamp to PRU.
// SHARED_MEM_BASE + RBUG_ADDR
#define RBUF_ADDR 0

// Location of 64 bit PRU timestamp.
// SHARED_MEM_BASE + TS_ADDR
#define TS_ADDR 0x1400

#endif
