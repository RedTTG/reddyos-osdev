#pragma once

typedef struct date_time {
    i32 year;
    u32 month;
    u32 day;
    u32 hour;
    u32 minute;
    u32 second;
} date_time_t;

void rtc_init(void);