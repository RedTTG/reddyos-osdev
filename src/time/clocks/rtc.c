#include "common.h"

struct timespec* base_rtc;

void rtc_real(struct timespec *ts) {
    ts->tv_sec = base_rtc->tv_sec;
    ts->tv_nsec = base_rtc->tv_nsec;
    ts->tv_nsec += ns_since_boot();
    timespec_normalize(ts);
}

bool get_update_flag() {
    return (rcmos(0x0A) & 0x80) != 0;
}

struct timespec* rtc_to_timespec(
    u32 second,
    u32 minute,
    u32 hour,
    u32 day,
    u32 month,
    i32 year,
    i32 century
)
{
    struct timespec* ts = kmalloc(sizeof(struct timespec));

    int full_year = year + (century * 100);

    i64 days = days_since_epoch(full_year, month, day);

    i64 sec =
        days * 86400LL +
        hour * 3600LL +
        minute * 60LL +
        second;

    ts->tv_sec = sec;
    ts->tv_nsec = 0;

    return ts;
}

void rtc_init(void) {
    u8 century_reg = get_century_register();
    u32 second;
    u32 minute;
    u32 hour;
    u32 day;
    u32 month;
    i32 year;
    i32 century;

    while (1) {
        while (!get_update_flag()) {
            __asm__ volatile ("pause");
        }
        second = rcmos(0x00);
        minute = rcmos(0x02);
        hour = rcmos(0x04);
        day = rcmos(0x07);
        month = rcmos(0x08);
        year = rcmos(0x09);
        century = century_reg ? rcmos(century_reg) : 20;

        u32 new_second = rcmos(0x00);
        u32 new_minute = rcmos(0x02);
        u32 new_hour = rcmos(0x04);
        u32 new_day = rcmos(0x07);
        u32 new_month = rcmos(0x08);
        i32 new_year = rcmos(0x09);
        i32 new_century = century_reg ? rcmos(century_reg) : 20;

        if (second == new_second &&
            minute == new_minute &&
            hour == new_hour &&
            day == new_day &&
            month == new_month &&
            year == new_year &&
            century == new_century
        ) {
            break;
        }
    }

    if ((rcmos(0x0B) & 0x04) == 0) {
        second = (second & 0x0F) + ((second / 16) * 10);
        minute = (minute & 0x0F) + ((minute / 16) * 10);
        hour = ((hour & 0x0F) + (((hour & 0x70) / 16) * 10)) | (hour & 0x80);
        day = (day & 0x0F) + ((day / 16) * 10);
        month = (month & 0x0F) + ((month / 16) * 10);
        year = (year & 0x0F) + ((year / 16) * 10);
        if (century_reg) {
            century = (century & 0x0F) + ((century / 16) * 10);
        }
    }
    if ((rcmos(0x0B) & 0x02) == 0 && (hour & 0x80) != 0) {
        hour = ((hour & 0x7F) + 12) % 24;
    }

    base_rtc = rtc_to_timespec(second, minute, hour, day, month, year, century);

    register_time_source(CLOCK_RTC, rtc_real);
}
