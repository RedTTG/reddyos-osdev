#include <time.h>

#include "common.h"
#include "abi-bits/clockid_t.h"
#include "abi-bits/errno.h"

u64 do_sys_clock_gettime(clockid_t clock, struct timespec* ts) {
    switch (clock) {
        case CLOCK_REALTIME:
            clock_source_t* rtc = get_clock(CLOCK_REALTIME);
            rtc->real(ts);
            return 0;
        case CLOCK_MONOTONIC:
            ts->tv_nsec = clock_get_best_ns();
            timespec_normalize(ts);
            return 0;
        default:
            return -EINVAL;
    }
}
