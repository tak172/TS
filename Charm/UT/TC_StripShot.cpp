#include "stdafx.h"

#include "TC_StripShot.h"
#include "../Fund/UralParser/StripShot.h"
#include "BuildTrainDescr.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StripShot );

void TC_StripShot::BadData()
{
    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_utf8( "..." );
    attic::a_node info = x_doc.child("Info");

    std::vector<StripShot> vStrip = StripShot::readTags( info, "Strip" );
    CPPUNIT_ASSERT( vStrip.empty()==true );
}

void TC_StripShot::parsing_StripShot()
{
    const wstring p=
        L"<Info>                             "
        L"   <Strip />                       "
        L"   <Strip Id='65432 5п        ' /> " // коректный
        L"   <Strip Id='65432 5п        ' /> " // дубль
        L"   <Strip Id='00000 нулев_еср ' /> "
        L"   <Strip Id='      отсут_еср ' /> "
        L"   <Strip Id='31330 а б в г д ' /> " // с пробелами
        L"   <Strip Id='31330 длинноеимя' /> " // без пробелов в конце
        L"</Info>                            ";

    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide( p );
    CPPUNIT_ASSERT( res );

    std::vector<std::wstring> v;
    StripShot Strip;
    for( attic::a_node it=x_doc.child("Info").child("Strip"); 
        it; 
        it=it.next_sibling("Strip") 
        )
    {
        Strip.getNord(it);
        v.push_back( Strip.get_guId() );
    }
    CPPUNIT_ASSERT(v.size()==7);
    CPPUNIT_ASSERT(v[0]==L"");
    CPPUNIT_ASSERT(v[1]==L"65432 5п");
    CPPUNIT_ASSERT(v[2]==L"65432 5п");
    CPPUNIT_ASSERT(v[3]==L"00000 нулев_еср");
    CPPUNIT_ASSERT(v[4]==L"      отсут_еср");
    CPPUNIT_ASSERT(v[5]==L"31330 а б в г д");
    CPPUNIT_ASSERT(v[6]==L"31330 длинноеимя");
}

void TC_StripShot::parsing_Train()
{
    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide(
        L"<Info>                                                                               "
        L"  <Strip Id='64104 НУ1      '>                                                       "
        L"    <Train Num='111' Idx='1212-121-1212' Ulen='21' Brut='2211' Delay='+2:11' />      "
        L"    <Train Num='3333' Idx='3331-332-3333' OH='Т' Ulen='60' Brut='373' Delay='+0:35'/>"
        L"    <Train Num='4444' Idx='4441-443-4444' />"
        L"  </Strip>                                                                           "
        L"</Info>                                                                              "
        );
    CPPUNIT_ASSERT( res );

    StripShot Strip;
    Strip.getNord( x_doc.child("Info").child("Strip") );

    TrainDescr tr0( BuildTrain( 111, L"1212-121-1212", L"", 21, 2211, 0, L"+2:11" ) );
    TrainDescr tr1( BuildTrain( 3333, L"3331-332-3333", L"Т", 60, 373, 0, L"+0:35" ) );
    TrainDescr tr2( BuildTrain( 4444, L"4441-443-4444", L"", 0, 0, 0, L"" ) );
    CPPUNIT_ASSERT( Strip.get_trains().size()==3 );
    CPPUNIT_ASSERT( Strip.get_trains()[0]==tr0 );
    CPPUNIT_ASSERT( Strip.get_trains()[1]==tr1 );
    CPPUNIT_ASSERT( Strip.get_trains()[2]==tr2 );
}
