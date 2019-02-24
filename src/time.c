/*
 * BreakHack - A dungeone crawler RPG
 * Copyright (C) 2018  Linus Probert <linus.probert@gmail.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <time.h>
#include "time.h"
#include "util.h"

#define SECONDS_PER_DAY 86400
#define SECONDS_PER_HOUR 3600
#define SECONDS_PER_MINUTE 60

#ifdef _MSC_VER
#define m_gmtime(time, obj) gmtime_s(obj, time)
#else
#define m_gmtime gmtime_r
#endif

static time_t
get_last_monday(time_t time, struct tm *tm)
{
	// Zero out the hour and minutes to 00:01:00
	time -= tm->tm_hour * SECONDS_PER_HOUR;
	time -= tm->tm_min * SECONDS_PER_MINUTE;
	time -= tm->tm_sec;
	time += 60; // Set it to one minute past midnight

	// Reverse time back to last monday
	unsigned int dayOfWeek = tm->tm_wday;
	time -= (dayOfWeek == 0 ? 6 : dayOfWeek - 1) * SECONDS_PER_DAY;

	return time;
}

time_t
time_get_weekly_seed(void)
{
	time_t now = time(NULL);

#ifdef MINGW
	struct tm *tm;
	tm = gmtime(&now);
	time_t lastMonday = get_last_monday(now, tm);
#else
	struct tm tm;
	m_gmtime(&now, &tm);
	time_t lastMonday = get_last_monday(now, &tm);
#endif

	debug("Weekly seed: %u", lastMonday);
	return lastMonday;
}

// Example: 190225_weekly
char *
time_get_weekly_lb_name(void)
{
	time_t seed = time_get_weekly_seed();
	struct tm tm;
	m_gmtime(&seed, &tm);
	char *name = ec_malloc(sizeof(char) * 15);
	m_sprintf(name,
		  15,
		  "%u%.2u%.2u_weekly",
		  tm.tm_year % 100,
		  tm.tm_mon + 1,
		  tm.tm_mday
		 );
	debug("Weekly leaderboard: %s", name);
	return name;
}
