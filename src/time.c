#include "time.h"
#include "util.h"

#define SECONDS_PER_DAY 86400
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60

time_t
time_get_weekly_seed(void)
{
	time_t now = time(NULL);

	struct tm *tm;
	tm = localtime(&now);

	// Zero out the hour and minutes to 00:00:01
	now -= tm->tm_hour * SECONDS_PER_HOUR;
	now -= tm->tm_min * SECONDS_PER_MINUTE;
	now -= tm->tm_sec;
	now += 1;

	// Reverse time back to last monday
	unsigned int dayOfWeek = tm->tm_wday;
	now -= (dayOfWeek == 0 ? 6 : dayOfWeek - 1) * SECONDS_PER_DAY;

	info("Monday: %s", ctime(&now));

	return now;
}
