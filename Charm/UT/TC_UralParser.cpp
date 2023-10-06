#include "stdafx.h"

#include "TC_UralParser.h"
#include "../helpful/Attic.h"
#include "../helpful/StrToTime.h"
#include "../StatBuilder/additional_func.h"

using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_UralParser );



void TC_UralParser::ConvTime()
{
    const char * dt[]={
        "2011-01-23 21:11",
        "2008-02-23 22:22",
        "2005-03-23 23:33",
        "2002-04-23 14:44",
        "1999-05-13 15:55",
        "1996-06-13 16:16",
        "1993-07-13 17:17",
        "1990-08-13 18:18",
        "1987-09-03 19:19",
        "1984-10-03 00:00",
        "1981-11-03 01:01",
        "1978-12-03 02:02",
    };
    for( int i=0; i<size_array(dt); i++ )
    {
        time_t t = StrDataTime_To_UTC( dt[i], i );

        struct tm temp_tm;
        localtime_s(&temp_tm, &t);
        char buff[200];
        strftime(buff,sizeof(buff),"%Y-%m-%d %H:%M",&temp_tm);
        if ( strcmp(dt[i],buff)!=0 )
            CPPUNIT_ASSERT_MESSAGE("преобразование даты",dt[i]);
    }

}

