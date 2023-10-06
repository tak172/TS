#include "stdafx.h"

#include "TC_ExcerptWarn.h"
#include "../Fund/ExcerptWarning.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ExcerptWarn );

static wstring av( const char * aname, const char* avalue )
{
   wstring res;
   if ( avalue )
   {
       res += L" ";
       res += FromUtf8(aname);
       res += L"=\"";
       res += FromUtf8(avalue);
       res += L"\"";
   }
   return res;
}

void TC_ExcerptWarn::BadData()
{
    attic::a_document x_doc;
    /*attic::a_parse_result res = */x_doc.load_utf8( "..." );
    attic::a_node info = x_doc.child("Info");

    auto vLim = WarningExcerpt::readTags( info, "Warn" );
    CPPUNIT_ASSERT( vLim.empty() );
}

void TC_ExcerptWarn::Warn()
{
    const char * warn_text1251 = 
        "<Info>"
        " <Warn Id=\"2682484\" Stan=\"287102-287006\" KmPk_Beg=\"1661.1\" KmPk_End=\"1661.10\" Tim_Beg=\"2010-12-30 22:24\" >"
        "  <Object Put=\"1\" Napr=\"1\" />"
        "  <Speed Gruz=\"80\" Pass=\"100\" />"
        "  <Comment>ИЗМЕНЕНИЕ/ПР.108/Н</Comment>"
        " </Warn>"
        "</Info>";
    attic::a_document x_doc;
    attic::a_parse_result res = x_doc.load_wide( From1251( warn_text1251 ) );
    attic::a_node x_war = x_doc.child("Info").child("Warn");
    auto lim = WarningExcerpt::getNord(x_war);
    CPPUNIT_ASSERT( !lim.empty() );
}
