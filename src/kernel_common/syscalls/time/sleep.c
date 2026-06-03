#include "common.h"

u64 do_sys_nanosleep(struct timespec* duration, struct timespec* remaining) {
    u64 nanoseconds = timespec_to_nsec(duration);
    busy_sleep_best_ns(nanoseconds);
    return 0;
}