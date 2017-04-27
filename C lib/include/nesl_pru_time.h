/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */

/*
 * Based on timecounter and clocksource from the Linux kernel v4.1
 * See ATTRIB for GPLv2 licensing information.
 */

#ifndef NESL_PRU_TIME_H
#define NESL_PRU_TIME_H

#include <stdint.h>

#define CLOCKSOURCE_MASK(bits) (cycle_t)((bits) < 64 ? ((1ULL<<(bits))-1) : (cycle_t) -1)

typedef int64_t s64;
typedef uint64_t u64;
typedef uint32_t u32;
typedef u64 cycle_t;

struct cyclecounter
{
    cycle_t (*read)(const struct cyclecounter *cc);
    cycle_t mask;
    u32 mult;
    u32 shift;
};

struct timecounter
{
    const struct cyclecounter *cc;
    cycle_t cycle_last;
    u64 nsec;
    u64 mask;
    u64 frac;
};

struct pru_time
{
    struct timecounter tc;
    struct cyclecounter cc;
    int (*slew_cc)(s64 delta);
};

void
timecounter_init(struct timecounter *tc,
        const struct cyclecounter *cc,
        u64 start_tstamp)
{
    tc->cc = cc;
    tc->cycle_last = cc->read(cc);
    tc->nsec = start_tstamp;
    tc->mask = (1ULL << cc->shift) - 1;
    tc->frac = 0;
}

static inline void
timecounter_adjtime(struct timecounter *tc, s64 delta)
{
    tc->nsec += delta;
}

static inline u64
cyclecounter_cyc2ns(const struct cyclecounter *cc,
        cycle_t cycles, u64 mask, u64 *frac)
{
    u64 ns = (u64) cycles;

    ns = (ns * cc->mult) + *frac;
    *frac = ns & mask;
    return ns >> cc->shift;
}

static u64
timecounter_read_delta(struct timecounter *tc)
{
    cycle_t cycle_now, cycle_delta;
    u64 ns_offset;

    /* read cycle counter: */
    cycle_now = tc->cc->read(tc->cc);

    /* calculate the delta since the last timecounter_read_delta(): */
    cycle_delta = (cycle_now - tc->cycle_last) & tc->cc->mask;

    /* convert to nanoseconds: */
    ns_offset = cyclecounter_cyc2ns(tc->cc, cycle_delta,
            tc->mask, &tc->frac);

    /* update time stamp of timecounter_read_delta() call: */
    tc->cycle_last = cycle_now;

    return ns_offset;
}

u64
timecounter_read(struct timecounter *tc)
{
    u64 nsec;

    /* increment time by nanoseconds since last call */
    nsec = timecounter_read_delta(tc);
    nsec += tc->nsec;
    tc->nsec = nsec;

    return nsec;
}

/**
 * Initialize pru_time with a clock source (e.g. IEP).
 * This tracks absolute time in nanoseconds.
 *
 * Params:
 * pru_time - pointer to uninitialized pru_time struct
 * mult, shift - for calculating nanoseconds per clock cycle
 * bits - bits in clock counter source
 * read_cc - function for reading the clock counter
 * slew_cc - Optional function for adjusting clock counter
 * Return 0 on success
 */
int
init_pru_time(struct pru_time *pru_time,
        u32 mult, u32 shift, u32 bits,
        cycle_t (*read_cc)(const struct cyclecounter *cc),
        int (*slew_cc)(s64 delta)
        )
{
    memset(pru_time, 0, sizeof(struct pru_time));

    pru_time->cc.read = read_cc;
    pru_time->cc.mask = CLOCKSOURCE_MASK(bits);
    pru_time->cc.mult = mult;
    pru_time->cc.shift = shift;

    pru_time->slew_cc = slew_cc;

    timecounter_init(&pru_time->tc, &pru_time->cc, IEP_CNT);

    return 0;
}

/**
 * Read the current pru_time and handle clock counter rollover.
 * You should call this periodically to maintain proper time.
 *
 * Return absolute time in nanoseconds
 */
u64
read_pru_time(struct pru_time *pru_time)
{
    return timecounter_read(&pru_time->tc);
}

/**
 * Adjust PRU time to align with host timestamp.
 *
 * Params:
 * ts_pru - nanosecond timestamp of pru
 * ts_host - nanosecond timestamp of host
 * Returns the signed offset in nanoseconds between pru and host
 */
s64
adj_pru_time(struct pru_time *pru_time, u64 ts_pru, u64 ts_host)
{
    s64 delta = 0;
    if (ts_host > ts_pru) {
        delta = ts_host - ts_pru;
    } else if (ts_host < ts_pru) {
        delta = ts_pru - ts_host;
        delta = -delta;
    }

    // slew_cc allows users to define a custom callback to slew the
    // source clock counter according to their own algorithm.
    // This can be useful in preventing time from going backward.
    // If slew_cc is not set or returns 0 then just jump the clock.
    if (pru_time->slew_cc == NULL
            || (pru_time->slew_cc != NULL && !pru_time->slew_cc(delta))) {
        timecounter_adjtime(&pru_time->tc, delta);
    }

    return delta;
}

#endif
