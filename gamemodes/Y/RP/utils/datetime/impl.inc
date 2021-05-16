#if defined _inc_impl
	#undef _inc_impl
#endif

#include <YSI_Coding\y_hooks>

stock FixWorldTime(&hours, timezone_offset = TIME_ZONE_OFFSET) 
{
    hours += timezone_offset;
    hours = hours < 0 ? (hours + 24) : hours;
    hours = hours > 23 ? (hours - 24) : hours;
}

stock GetDayOrdinal(number)
{
	static const ordinal[4][3] = { "st", "nd", "rd", "th" };
	number = number < 0 ? -number : number;
	return (((10 < (number % 100) < 14)) ? ordinal[3] : (0 < (number % 10) < 4) ? ordinal[((number % 10) - 1)] : ordinal[3]);
}

stock GetDateTime(bool:isGameTime = false)
{
	new
	    year,
		month,
		day,
		hour,
		minute,
		second
	;

	getdate(year, month, day); //ymd
	gettime(hour, minute, second); //hms

	if (isGameTime)
	{
		hour += TIME_ZONE_OFFSET;

		if (hour > 23)
		{
			hour -= 24;
			day ++;
		}	
		if (day > MonthTimes[month-1][IsLeapYear(year)])
		{
			day = 1;
			month ++;
		}
		if (month > 12)
		{
			month = 1;
			year ++;
		}

	}

	new
        string[15];

	static const DateString[] = "%02d/%02d/%d, %02d:%02d";

	format(
        string,
        sizeof(string),
        DateString,
        year,
        month,
		day,
		hour,
		minute
    );

	return string;
}

stock GetTime12Hours(bool:isGameTime = false)
{
	new
		hour,
		minute,
		second,
		meridien[3]
	;

	gettime(hour, minute, second); //hms

	if (isGameTime)
	{
		hour += TIME_ZONE_OFFSET;
		if (hour > 23)
		{
			hour -= 24;
		}
	}

	if (hour < 12) 
	{
		meridien = "AM";
	}
	else
	{
		meridien = "PM";
		hour %= 12;
	}

	new
        string[9];

	static const TimeString[] = "%02d:%02d %s";

	format(
        string,
        sizeof(string),
        TimeString,
        hour,
        minute,
		meridien
    );

	return string;
}