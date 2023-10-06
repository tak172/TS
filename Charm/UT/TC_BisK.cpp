#include "stdafx.h"

#include "TC_BisK.h"
#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "../Fund/BisK.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_BisK );

using namespace std;


void TC_BisK::isId()
{
    CPPUNIT_ASSERT( BisK::isID( "2509201609251639294100" ) );
    CPPUNIT_ASSERT( !BisK::isID("25092016xxxx1639294100" ) );
    CPPUNIT_ASSERT( !BisK::isID("12345" ) );
    CPPUNIT_ASSERT( !BisK::isID("25092016092516392941002509201609251639294100" ) );
}

void TC_BisK::makeId()
{
    BisK::ID from = "2509201609251639294100";
    BisK::ID to = BisK::makeID( from ); // создать идентификатор на основе родительского ид.

    CPPUNIT_ASSERT( BisK::isID( from ) );
    CPPUNIT_ASSERT( BisK::isID( to ) );
    CPPUNIT_ASSERT( from != to );
}

// проверка построения идентификатора при ровной секунде (нулевое количество микросекунд)
void TC_BisK::makeId_0_microsec()
{
    using namespace boost;

    BisK::ID from = "2509201609251639294100";
    posix_time::ptime strangeMoment( gregorian::date(2018,12,19), posix_time::time_duration(18,7,0) );
    BisK::ID to = BisK::makeID( from, strangeMoment ); // создать идентификатор на основе родительского ид.

    CPPUNIT_ASSERT( BisK::isID( from ) );
    CPPUNIT_ASSERT( BisK::isID( to ) );
    CPPUNIT_ASSERT( from != to );
}

void TC_BisK::makeAckSucc()
{
    const BisK::ID parId = "2509201609251111111111";
    const std::wstring fn = L"aaa.xml";
    attic::a_document doc;

    // квитанция нормальная
    CPPUNIT_ASSERT( doc.load_utf8( BisK::confirmSuccess( parId, fn ) ) );
    attic::a_node r = doc.document_element();
    CPPUNIT_ASSERT( r.name_is("zka") );
    CPPUNIT_ASSERT( r.attribute("file").as_string() == std::string("aaa_conf.xml") );
    CPPUNIT_ASSERT( r.child("To" ).text_as_string() == std::to_string( BisK::TO::CONFIRM ) );
    CPPUNIT_ASSERT( r.child("Ip" ).text_as_string() != std::string( parId ) );
    CPPUNIT_ASSERT( r.child("Ipz").text_as_string() == std::string( parId ) );
    CPPUNIT_ASSERT( r.child("Kp" ).text_as_string() == std::to_string( BisK::KP::SUCCESS ) );
    CPPUNIT_ASSERT( !r.child("Ko") );
}

void TC_BisK::makeAckFail()
{
    const BisK::ID parId = "2509201609251111111111";
    const std::wstring fn = L"aaa.xml";
    const std::wstring errtext = L"404 some bad";
    attic::a_document doc;

    // квитанция при ошибке
    CPPUNIT_ASSERT( doc.load_utf8( BisK::confirmFail( parId, fn, errtext ) ) );
    attic::a_node r = doc.document_element();
    CPPUNIT_ASSERT( r.name_is("zka") );
    CPPUNIT_ASSERT( r.attribute("file").as_string() == std::string("aaa_conf.xml") );
    CPPUNIT_ASSERT( r.child("To" ).text_as_string() == std::to_string( BisK::TO::CONFIRM ) );
    CPPUNIT_ASSERT( r.child("Ip" ).text_as_string() != std::string( parId ) );
    CPPUNIT_ASSERT( r.child("Ipz").text_as_string() == std::string( parId ) );
    CPPUNIT_ASSERT( r.child("Kp" ).text_as_string() == std::to_string( BisK::KP::FAIL ) );
    CPPUNIT_ASSERT( FromUtf8((r.child("Ko" ).text_as_string())) == errtext );
}

void TC_BisK::makeRequest()
{
    const BisK::ID myId = BisK::makeID( "2509000000000000000000" );
    attic::a_document doc;

    // запрос на получение всего
    const auto reqtxt = BisK::makeRequest(myId);
    std::string decl = "<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?>";
    CPPUNIT_ASSERT( 0 == reqtxt.find(decl) );
    CPPUNIT_ASSERT( doc.load_utf8( reqtxt ) );
    attic::a_node r = doc.document_element();
    CPPUNIT_ASSERT( r.name_is("zka") );
    CPPUNIT_ASSERT( r.attribute("file").as_string() == std::string( myId + "_req.xml" ) );
    CPPUNIT_ASSERT( r.child("To" ).text_as_string() == std::to_string( BisK::TO::REQUEST ) );
    CPPUNIT_ASSERT( r.child("Iu" ).text_as_string() == std::string( myId ) );
}
