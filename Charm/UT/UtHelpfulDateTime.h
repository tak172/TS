#pragma once

#include <ostream>
#include <boost/date_time/posix_time/posix_time.hpp>
#include <cppunit/extensions/HelperMacros.h>

std::ostream& operator <<(std::ostream& stream, const std::wstring& data);
std::basic_ostream<char>& operator <<(std::basic_ostream<char>& stream, const boost::posix_time::time_duration duration);
void assertTimeOfDateEqual(unsigned hours, unsigned minutes, unsigned seconds, boost::posix_time::time_duration actual);
boost::posix_time::ptime ptimeForDate(unsigned short year, unsigned short month, unsigned short day);
time_t timeForDate(unsigned short year, unsigned short month, unsigned short day);
time_t timeForDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute, unsigned short seconds = 0);
time_t timeForLocalDateTime(unsigned short year, unsigned short month, unsigned short day, unsigned short hour, unsigned short minute);
boost::posix_time::time_period periodForTimes(time_t first, time_t last);