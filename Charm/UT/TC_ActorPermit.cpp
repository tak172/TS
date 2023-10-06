#include "stdafx.h"

#include <sstream>
#include "TC_ActorPermit.h"
#include <boost/asio/ip/tcp.hpp>
#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "../helpful/Log.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ActorPermit );

using namespace std;
using namespace boost::posix_time;


void TC_ActorPermit::setUp()
{
    const wchar_t * cfg = 
        L"<Test>"
        L"   <Restrict>"
        L"       <Full IP='1.1.1.1' />"
        L"       <Full IP='2.2.2.2' include='72' />"
        L"       <Full IP='3.3.3.3' include='73,733,7333' />"
        L"       <Full IP='4.4.4.4' exclude='74' />"
        L"       <Full IP='5.5.5.5' exclude='75,755,7555' />"
        L"       <Full IP='6.6.6.6' include='7666' exclude='7666' />"
        L"   </Restrict>"
        L"</Test>";

    attic::a_document doc;
    attic::a_parse_result res = doc.load_wide( cfg );
    CPPUNIT_ASSERT( res==true );

    ap.initialize(doc.child("Test"));
}

void TC_ActorPermit::tearDown()
{
}

void TC_ActorPermit::check_none()
{
    CPPUNIT_ASSERT( ap.allowClick( ip4("1.1.1.1"), EsrKit(555) ) );
    CPPUNIT_ASSERT( ap.allowClick( ip4("1.1.1.1"), EsrKit(999) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("9.9.9.9"), EsrKit(555) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("9.9.9.9"), EsrKit(999) ) );
}

void TC_ActorPermit::check_include()
{
    CPPUNIT_ASSERT( ap.allowClick( ip4("2.2.2.2"), EsrKit(72) ) );
    CPPUNIT_ASSERT( ap.allowClick( ip4("3.3.3.3"), EsrKit(73) ) );
    CPPUNIT_ASSERT( ap.allowClick( ip4("3.3.3.3"), EsrKit(733) ) );
    CPPUNIT_ASSERT( ap.allowClick( ip4("3.3.3.3"), EsrKit(7333) ) );

    CPPUNIT_ASSERT( !ap.allowClick( ip4("3.3.3.3"), EsrKit(72) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("2.2.2.2"), EsrKit(733) ) );
}

void TC_ActorPermit::check_exclude()
{
    CPPUNIT_ASSERT( !ap.allowClick( ip4("4.4.4.4"), EsrKit(74) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("5.5.5.5"), EsrKit(75) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("5.5.5.5"), EsrKit(755) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("5.5.5.5"), EsrKit(7555) ) );

    CPPUNIT_ASSERT( ap.allowClick( ip4("4.4.4.4"), EsrKit(7444) ) );
    CPPUNIT_ASSERT( ap.allowClick( ip4("5.5.5.5"), EsrKit(7777) ) );
}

void TC_ActorPermit::check_include_exclude()
{
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit(75) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit(76) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit(766) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit(7666) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit(76666) ) );
    CPPUNIT_ASSERT( !ap.allowClick( ip4("6.6.6.6"), EsrKit() ) );
}

unsigned long TC_ActorPermit::ip4(const char * str)
{
    return boost::asio::ip::address::from_string(str).to_v4().to_ulong();
}
