#pragma once

#define IS_LEAP(y) ((y % 4 == 0 && y % 100 != 0) || (y % 400 == 0))

#define days_in_month ((int[]){31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31})

static inline i64 days_since_epoch(int year, int month, int day)
{
    i64 days = 0;

    // years
    for (int y = 1970; y < year; y++)
        days += IS_LEAP(y) ? 366 : 365;

    // months
    for (int m = 1; m < month; m++) {
        if (m == 2 && IS_LEAP(year))
            days += 29;
        else
            days += days_in_month[m - 1];
    }

    // days
    days += (day - 1);

    return days;
}

static inline void timespec_normalize(struct timespec *ts)
{
    ts->tv_sec += ts->tv_nsec / 1000000000LL;
    ts->tv_nsec %= 1000000000LL;

    if (ts->tv_nsec < 0) {
        ts->tv_sec--;
        ts->tv_nsec += 1000000000LL;
    }
}

static inline u64 timespec_to_nsec(struct timespec* ts) {
    return ts->tv_sec * 1000000000LL + ts->tv_nsec;
}