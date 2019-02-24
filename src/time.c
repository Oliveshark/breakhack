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
#define m_gmtime gmtime_s
#else
#define m_gmtime gmtime_r
#endif

time_t
time_get_weekly_seed(void)
{
	time_t now = time(NULL);

	struct tm tm;
	m_gmtime(&now, &tm);

	// Zero out the hour and minutes to 00:00:01
	now -= tm.tm_hour * SECONDS_PER_HOUR;
	now -= tm.tm_min * SECONDS_PER_MINUTE;
	now -= tm.tm_sec;
	now += 60; // Set it to one minute past midnight

	// Reverse time back to last monday
	unsigned int dayOfWeek = tm.tm_wday;
	now -= (dayOfWeek == 0 ? 6 : dayOfWeek - 1) * SECONDS_PER_DAY;

	debug("Weekly seed: %u", now);

	return now;
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
