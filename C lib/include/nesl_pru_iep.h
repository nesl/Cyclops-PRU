/*
 * Copyright (c) Regents of the University of California, 2017. All rights reserved.
 * See LICENSE and ATTRIB in the repository root.
 */
#ifndef NESL_PRU_IEP_H
#define NESL_PRU_IEP_H

#include <stdint.h>

static uint32_t *IEP_REG = (uint32_t*) 0x2E000;
static uint32_t *IEP_TMR_CNT = (uint32_t*) 0x2E00C;
static uint32_t *IEP_TMR_COMPEN = (uint32_t*) 0x2E008;

#define ENABLE_IEP_TMR() (*IEP_REG |= 0x011)
#define DISABLE_IEP_TMR() (*IEP_REG &= 0)
#define IEP_CNT (*IEP_TMR_CNT)
#define IEP_COMPEN (*IEP_TMR_COMPEN)
#define SET_IEP_COMP_INC(val) (*IEP_REG |= (val << 8))
#define IEP_CNT_TO_NS(count) ((count) * 5);

struct iep_time {
    uint64_t ts_ns;
    uint32_t last_iep_cnt;
    int64_t offset;
};

inline void init_iep_time(struct iep_time *time)
{
    DISABLE_IEP_TMR();
    ENABLE_IEP_TMR();
    IEP_CNT = 0;

    time->last_iep_cnt = 0;
    time->ts_ns = 0;
    time->offset = 0;
}

inline uint64_t iep_get_time(struct iep_time *time)
{
    uint32_t iep_cnt = IEP_CNT;
    uint32_t last_iep_cnt = time->last_iep_cnt;
    uint64_t ts_ns = time->ts_ns;

    if (iep_cnt < last_iep_cnt) {
        // roll over happened
        ts_ns +=
            IEP_CNT_TO_NS((UINT32_MAX - last_iep_cnt)
                    + iep_cnt);
    } else {
        ts_ns += IEP_CNT_TO_NS(iep_cnt - last_iep_cnt);
    }

    time->last_iep_cnt = iep_cnt;
    time->ts_ns = ts_ns;

    return ts_ns + time->offset;
}

inline void iep_adj_time(struct iep_time *time,
        uint64_t ts_ns, uint64_t external_ts_ns)
{
    time->offset += external_ts_ns - ts_ns;
}

#endif
