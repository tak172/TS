#include "stdafx.h"

#include "TC_DateTime.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/date_time/c_local_time_adjustor.hpp>
#include "../helpful/StrToTime.h"

using namespace std;
using namespace boost::posix_time;
using namespace boost::gregorian;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DateTime );

void TC_DateTime::setUp()
{
    // Для примера использовано Рижское летнее время, поскольку в России с 2014 года часы не переводятся
    // "EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00" - [ посл. вск марта; посл. вск октября]
    set_TimeZone("EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00");
    winter_hours = +2;
    summer_hours = +3;
}
void TC_DateTime::tearDown()
{
    winter_hours = 0;
    summer_hours = 0;
    set_TimeZone("");
}

void TC_DateTime::fromYMDhms()
{
    time_t s1 = StrDataTime_To_UTC( "2014-01-01 05:13:42", time_t(999) ); // зимнее
    time_t s2 = StrDataTime_To_UTC( "2014-07-01 05:13:42", time_t(888) ); // летнее
    time_t s3 = StrDataTime_To_UTC( "2014-12-01 05:13:42", time_t(777) ); // снова зимнее
    time_t s4 = StrDataTime_To_UTC( "2014-12-01 05:13",    time_t(666) ); // без секунд
    ptime pt1( date(2014, 1,1), hours(5)+minutes(13)+seconds(42)-hours(winter_hours) );
    ptime pt2( date(2014, 7,1), hours(5)+minutes(13)+seconds(42)-hours(summer_hours) );
    ptime pt3( date(2014,12,1), hours(5)+minutes(13)+seconds(42)-hours(winter_hours) );
    ptime pt4 = pt3 - time_duration( 0, 0, pt3.time_of_day().seconds() );
    CPPUNIT_ASSERT( from_time_t(s1) == pt1 );
    CPPUNIT_ASSERT( from_time_t(s2) == pt2 );
    CPPUNIT_ASSERT( from_time_t(s3) == pt3 );
    CPPUNIT_ASSERT( from_time_t(s4) == pt4 );
}

void TC_DateTime::fromYMDhms_bad()
{
    time_t x = StrDataTime_To_UTC( "???", time_t(777) );
    CPPUNIT_ASSERT( x==time_t(777) );
    time_t y = StrDataTime_To_UTC( "", time_t(888) );
    CPPUNIT_ASSERT( y==time_t(888) );
    time_t z = StrDataTime_To_UTC( "20120630051355", time_t(999) );
    CPPUNIT_ASSERT( z==time_t(999) );
}

void TC_DateTime::fromDMYhms()
{
    time_t sample1 = UTC_from_DMYhms( "26.08.2014 00:30:00", time_t(777) );
    time_t sample2 = StrDataTime_To_UTC( "2014-08-26 00:30:00", time_t(888) );
    CPPUNIT_ASSERT( sample1==sample2 );
    sample1 = UTC_from_DMYhms(    "26.10.2014 03:30", time_t(777) );
    sample2 = StrDataTime_To_UTC( "2014-10-26 03:30", time_t(888) );
    CPPUNIT_ASSERT( sample1==sample2 );
    sample1 = UTC_from_DMYhms(    "26.12.2014 01:30:00", time_t(777) );
    sample2 = StrDataTime_To_UTC( "2014-12-26 01:30:00", time_t(888) );
    CPPUNIT_ASSERT( sample1==sample2 );
}

void TC_DateTime::fromDMYhms_bad()
{
    time_t a = UTC_from_DMYhms( "!!!", time_t(333) );
    CPPUNIT_ASSERT( a==time_t(333) );
    time_t b = UTC_from_DMYhms( "", time_t(444) );
    CPPUNIT_ASSERT( b==time_t(444) );
}

void TC_DateTime::check_dst()
{
    // "EET+02EEST+01,M3.5.0/03:00,M10.5.0/04:00" - [ посл. вск марта; посл. вск октября]
    map< string, ptime > row;
    const time_duration WIN = -hours(winter_hours);
    const time_duration S = -hours(summer_hours);
    row["2014-01-01 07:30:00"] = ptime( date(2014, 1, 1), WIN + hours(7)+minutes(30) );
    row["2014-03-30 02:59:00"] = ptime( date(2014, 3,30), WIN + hours(2)+minutes(59) );
    row["2014-03-30 03:00:00"] = ptime( date(2014, 3,30), WIN + hours(3)+minutes(00) );
    row["2014-03-30 03:11:00"] = ptime( date(2014, 3,30), WIN + hours(3) ); // разрыв 3:00-4:00 и 03:11 не существует
    row["2014-03-30 04:00:00"] = ptime( date(2014, 3,30),   S + hours(4)+minutes(00) );
    row["2014-03-30 04:01:00"] = ptime( date(2014, 3,30),   S + hours(4)+minutes(01) );
    row["2014-10-26 02:30:00"] = ptime( date(2014,10,26),   S + hours(2)+minutes(30) );
    row["2014-10-26 03:59:00"] = ptime( date(2014,10,26),   S + hours(3)+minutes(59) );
    row["2014-10-26 04:00:00"] = ptime( date(2014,10,26), WIN + hours(4)+minutes(00) );
    row["2014-10-26 04:01:00"] = ptime( date(2014,10,26), WIN + hours(4)+minutes(01) );
    row["2014-12-31 23:55:00"] = ptime( date(2014,12,31), WIN + hours(23)+minutes(55));

    for( auto& temp : row )
    {
        const string& text = temp.first;
        time_t tim = StrDataTime_To_UTC( text, time_t(888) );
        CPPUNIT_ASSERT( from_time_t(tim) == temp.second );
    }
}

void TC_DateTime::to_string()
{
    time_t t;
    t = StrDataTime_To_UTC( "2014-12-01 05:13:42", time_t(999) ); // зимнее
    CPPUNIT_ASSERT( "2014-12-01 05:13"   == UTC_to_String(t) ); // default options DTO_YMD_HM
    CPPUNIT_ASSERT( "2014-12-01 05:13"   == UTC_to_String(t, DTO_YMD_HM) );
    CPPUNIT_ASSERT( "2014-12-01 05:13:42"== UTC_to_String(t, DTO_YMD_HMS) );
    CPPUNIT_ASSERT( "2014-12-01 05:14"   == UTC_to_String(t, DTO_YMD_HM | DTO_ROUND_M) );
    CPPUNIT_ASSERT( "01-12-14 05:13:42"  == UTC_to_String(t, DTO_DMY_HMS) );
    CPPUNIT_ASSERT(          "05:13:42"  == UTC_to_String(t, DTO_HMS) );

    t = StrDataTime_To_UTC( "2014-07-01 05:13:42", time_t(888) ); // летнее
    CPPUNIT_ASSERT( "2014-07-01 05:13"==UTC_to_String(t) );
}

void TC_DateTime::check_moscow_local()
{
    // зимнее время в Риге отстает на час от московского
    time_t winter_Riga   = UTC_From_Local(       ptime( date(2019,12,12), hours(17) ) );
    time_t winter_Moscow = UTC_From_MoscowLocal( ptime( date(2019,12,12), hours(18) ) );
    CPPUNIT_ASSERT( winter_Riga == winter_Moscow );
    // летнее время в Риге совпадает с московским
    time_t summer_Riga   = UTC_From_Local(       ptime( date(2020,6,6), hours(18) ) );
    time_t summer_Moscow = UTC_From_MoscowLocal( ptime( date(2020,6,6), hours(18) ) );
    CPPUNIT_ASSERT( summer_Riga == summer_Moscow );
}
