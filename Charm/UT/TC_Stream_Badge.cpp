#include "stdafx.h"

#include <sstream>
#include "TC_Stream_Badge.h"
#include "../helpful/stream_Badge.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Stream_Badge );

/*
проверить класс LOS_switch
*/

void TC_Stream_Badge::char_stream()
{
    BadgeE b1( L"one", EsrKit(345,12) );
    BadgeU b2( L"two", 765 );

    std::stringstream buffer;
    buffer << b1 << b2;
    std::string s = buffer.str();
    CPPUNIT_ASSERT(s=="one[00012:00345]two<765>");
}

void TC_Stream_Badge::wide_stream()
{
    BadgeU b1( L"xx", 111 );
    BadgeE b2( L"yy", EsrKit(3,4) );

    std::wstringstream buffer;
    buffer << b1 << b2;
    std::wstring w = buffer.str();
    CPPUNIT_ASSERT(w==L"xx<111>yy[00003:00004]");
}
