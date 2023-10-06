#include "stdafx.h"

#include "TC_CrewLocomotive.h"
#include "../helpful/Attic.h"
#include "../helpful/Crew.h"
#include "../helpful/Locomotive.h"
#include "../helpful/LocoCharacteristics.h"
#include "UtHelpfulDateTime.h"

using namespace std;
using namespace boost::posix_time;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_CrewAndLocomotive );

string a = "<Test>";
string b = "   <Locomotive Series='ВЛ11' NumLoc='97' Depo='17' Type='2' Consec='2' />";
string c  = "   <Locomotive Series='ВЛ23' NumLoc='197' Depo='17' Type='2' Consec='2' />"
    /**/   "   <Crew EngineDriver='Михалков' TabNum='404' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />";
string d = "   <Locomotive Series='ВЛ15' NumLoc='97' Depo='17' Type='2' Consec='2' />";
string e = "   <Crew EngineDriver='Ёжиков' TabNum='202' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />";
string f  = "   <Crew EngineDriver='Белкин' TabNum='303' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />"
    /**/   "   <Crew EngineDriver='Суслик' TabNum='505' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />";
string g  = "</Test>";

string c_intervalFormat = 
"<Test>"
"   <Locomotive Series='ВЛ11' NumLoc='97' Depo='17' Consec='2' />"
"   <Locomotive Series='ВЛ23' NumLoc='197' Depo='17' Consec='2'>"
"      <Crew EngineDriver='Михалков' TabNum='404' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />"
"   </Locomotive>"
"   <Locomotive Series='ВЛ15' NumLoc='97' Depo='17' Consec='2'>"
"      <Crew EngineDriver='Ёжиков' TabNum='202' Dor='76' Depo='17' Tim_Beg='2015-05-30 10:54' />"
"   </Locomotive>"
"</Test>";

void TC_CrewAndLocomotive::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_CrewAndLocomotive::read_write()
{
    // эталон для проверки
    wstring ETALON;
    attic::a_document et_doc;
    CPPUNIT_ASSERT( et_doc.load_wide( From1251( c_intervalFormat ) ) );
    ETALON = et_doc.pretty_wstr();
    // документ для загрузки
    attic::a_document in_doc;
    CPPUNIT_ASSERT( in_doc.load_wide( From1251( a +b +c +d +e +f +g ) ) );
    // загрузка
    vector< Locomotive > vl;
    for( attic::a_node tn : in_doc.child("Test").children( Locomotive::noun_node() ) )
    {
        Locomotive loc;
        loc.readNovosibSupplimentary(tn);
        vl.push_back( loc );
    }
    // грубая проверка
    CPPUNIT_ASSERT( vl.size() == 3 );
    CPPUNIT_ASSERT( !vl[0].empty() );
    CPPUNIT_ASSERT( !vl[1].empty() );
    CPPUNIT_ASSERT( !vl[2].empty() );

    // полный вывод в текст
    attic::a_document xout( "Test" );
    for( auto& item : vl )
    {
        attic::a_node locoNode = xout.document_element().append_child(Locomotive::noun_node());
        item.serialize(locoNode);
    }

    wstring result = xout.pretty_wstr();
    auto mismresult = mismatch( ETALON.cbegin(), ETALON.cend(), result.cbegin(), result.cbegin() );

    CPPUNIT_ASSERT_EQUAL(ETALON, result);
}

void TC_CrewAndLocomotive::crew_only()
{
    // эталон для проверки и загрузки
    wstring ETALON;
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_wide( From1251( a + string() +e +f +g  ) ) );
    ETALON = doc.pretty_wstr();

    // загрузка
    vector< Crew > v;
    for( attic::a_node n : doc.child("Test").children( Crew::noun_node() ) )
    {
        Crew cr;
        cr.readSupplimentary(n);
        v.push_back( cr );
    }
    // грубая проверка
    CPPUNIT_ASSERT( v.size() == 3 );
    CPPUNIT_ASSERT( !v[0].empty() );
    CPPUNIT_ASSERT( !v[1].empty() );
    CPPUNIT_ASSERT( !v[2].empty() );

    // полный вывод в текст
    attic::a_document xout( "Test" );
    for( auto& cr : v )
    {
        attic::a_node locoNode = xout.document_element().append_child(Crew::noun_node());
        cr.serialize(locoNode);
    }

    wstring result = xout.pretty_wstr();
    auto mismresult = mismatch( ETALON.cbegin(), ETALON.cend(), result.cbegin(), result.cend() );

    CPPUNIT_ASSERT_EQUAL(ETALON, result);
}
