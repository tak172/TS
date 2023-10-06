#include "stdafx.h"
#include "UtHelpfulDateTime.h"
#include "../helpful/StrToTime.h"

const boost::gregorian::date c_epoch(1970, 1, 1);

std::ostream& operator <<(std::ostream& stream, const std::wstring& data)
{
    stream << ToOtherCP(data, GetConsoleCP());
    return stream;
}

std::basic_ostream<char>& operator <<(std::basic_ostream<char>& stream, const boost::posix_time::time_duration duration)
{
	stream << duration.hours() << ":" << duration.minutes() << ":" << duration.seconds();
	return stream;
}

void assertTimeOfDateEqual(unsigned hours, unsigned minutes, unsigned seconds, boost::posix_time::time_duration actual)
{
	boost::posix_time::time_duration expected = 
		boost::posix_time::hours(hours)
		+ boost::posix_time::minutes(minutes)
		+ boost::posix_time::seconds(seconds);

	CPPUNIT_ASSERT_EQUAL(expected, actual);
}

boost::posix_time::ptime ptimeForDate(unsigned short year, unsigned short month, unsigned short day)
{
	return boost::posix_time::ptime(boost::gregorian::date(year, month, day));
}

time_t timeForDate(unsigned short year, unsigned short month, unsigned short day)
{
	return boost::posix_time::time_duration(
		boost::posix_time::ptime(boost::gregorian::date(year, month, day))
		- boost::posix_time::ptime(c_epoch))
		.total_seconds();
}

time_t timeForDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short second)
{
	boost::posix_time::ptime localPtime(boost::gregorian::date(year, month, day), boost::posix_time::hours(hour) + boost::posix_time::minutes(minute));
	return boost::posix_time::time_duration(localPtime - boost::posix_time::ptime(c_epoch))
		.total_seconds() + second;
}

time_t timeForLocalDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute)
{
	boost::posix_time::ptime localPtime(boost::gregorian::date(year, month, day), boost::posix_time::hours(hour) + boost::posix_time::minutes(minute));
	return UTC_From_Local(localPtime);
}

boost::posix_time::time_period periodForTimes(time_t first, time_t last)
{
	return boost::posix_time::time_period(boost::posix_time::from_time_t(first), boost::posix_time::from_time_t(last));
}