#include "stdafx.h"

#include "TC_Locomotive.h"
#include "../helpful/Attic.h"
#include "../helpful/RT_Macros.h"
#include "../helpful/Crew.h"
#include "../helpful/Locomotive.h"
#include "../helpful/LocoCharacteristics.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Locomotive );

void TC_Locomotive::tearDown()
{
    LocoCharacteristics::Shutdowner();
}

void TC_Locomotive::read_write()
{
    attic::a_document doc("test");

    Locomotive locA( LocoId(111, 512), 1, 110011 );
    Locomotive locB;

    attic::a_node n = doc.document_element().append_child( Locomotive::noun_node() );
    locA.serialize( n );
    locB.deserialize( n );
    std::wstring w = doc.pretty_wstr();
    CPPUNIT_ASSERT( !w.empty() );
    // ������ ��������
    CPPUNIT_ASSERT( locA == locB );
    CPPUNIT_ASSERT( locB.getId() == LocoId(111,512) );
    CPPUNIT_ASSERT( !locB.exist_crew() );
    CPPUNIT_ASSERT( !locB.empty() );
    CPPUNIT_ASSERT( locB.getConsecution() == 110011 );
    CPPUNIT_ASSERT( locB.getCarrier() == 0 );

    // ��������� ����������� � ����� ���������
    CPPUNIT_ASSERT( locA.getCarrier() == 0 );
    locA.setCarrier( 255 );
    CPPUNIT_ASSERT( locA.getCarrier() == 255 );
    CPPUNIT_ASSERT( locA != locB );

    locA.serialize( n );
    locB.deserialize( n );
    CPPUNIT_ASSERT( locA == locB );
    CPPUNIT_ASSERT( locB.getId() == LocoId(111,512) );
    CPPUNIT_ASSERT( !locB.exist_crew() );
    CPPUNIT_ASSERT( !locB.empty() );
    CPPUNIT_ASSERT( locB.getConsecution() == 110011 );
    CPPUNIT_ASSERT( locB.getCarrier() == 255 );
}

// ������������� � ������������ ������� 
void TC_Locomotive::compatibility()
{
    attic::a_document doc;
    doc.load_wide(
        L"<List>"
        L"  <Locomotive Series='2��10�' NumLoc='3423' Depo='0' Consec='3'>"
        L"    <Crew EngineDriver='BIZNJA' Tim_Beg='2018-02-08 03:10' />"
        L"  </Locomotive>"
        L"  <Locomotive Series='2��116' NumLoc='933' Depo='0' Consec='2'>"
        L"    <Crew />"
        L"  </Locomotive>"
        L"  <Locomotive Series='���70' NumLoc='230' Depo='0' Consec='1'>"
        L"    <Crew EngineDriver='M�HAEB' TabNum='17664' Tim_Beg='2018-02-13 19:23' />"
        L"  </Locomotive>"
        L"</List>"
        );

    std::vector<Locomotive> vl;
    for( attic::a_node node : doc.document_element().children( Locomotive::noun_node() ) )
    {
        Locomotive loc;
        loc.deserialize( node );
        vl.push_back( loc );
    }
    
    // checking
    CPPUNIT_ASSERT( vl.size() == 3 );
    CPPUNIT_ASSERT( vl[0].getId().number() == 3423 );
    CPPUNIT_ASSERT( vl[0].getId().seriesName() == L"2��10�" );
    CPPUNIT_ASSERT( vl[0].getConsecution() == 3 );
    CPPUNIT_ASSERT( vl[0].exist_crew() );
    CPPUNIT_ASSERT( vl[0].get_crew().get_engineDriver() == L"BIZNJA" );

    CPPUNIT_ASSERT( vl[1].getId().number() == 933 );
    CPPUNIT_ASSERT( vl[1].getId().seriesName() == L"2��116" );
    CPPUNIT_ASSERT( vl[1].getConsecution() == 2 );
    CPPUNIT_ASSERT( !vl[1].exist_crew() );

    CPPUNIT_ASSERT( vl[2].getId().number() == 230 );
    CPPUNIT_ASSERT( vl[2].getId().seriesName() == L"���70" );
    CPPUNIT_ASSERT( vl[2].getConsecution() == 1 );
    CPPUNIT_ASSERT( vl[2].exist_crew() );
    CPPUNIT_ASSERT( vl[2].get_crew().get_engineDriver() == L"M�HAEB" );
}

// ������������� � ������� �� 1.3 build 71
void TC_Locomotive::compat1_3_71()
{
    std::wstring prev[] = { L"2TE10MK", L"2M62UM" };
    std::wstring next[] = { L"2��10��", L"2�62��" };
    for( size_t i=0; i<size_array(prev); ++i  )
    {
        auto code = LocoCharacteristics::instance()->GetSeriesCode( prev[i] );
        CPPUNIT_ASSERT( 0 != code );
        auto curr = LocoCharacteristics::instance()->GetSeriesName( code );
        CPPUNIT_ASSERT( curr != prev[i] );
        CPPUNIT_ASSERT( curr == next[i] );
    }
}

void TC_Locomotive::changeKIG_572()
{
    auto code = LocoCharacteristics::instance()->GetSeriesCode( L"��1��" );
    CPPUNIT_ASSERT( 834 == code );
    auto curr = LocoCharacteristics::instance()->GetSeriesName( 834 );
    CPPUNIT_ASSERT( curr == L"��1��" );
}

// ������������ ���������� �������� ��������� � �������� � �������� �����
void TC_Locomotive::mixRusLat()
{
    std::wstring cyrName = L"��1�"; // ������ ���������
    std::wstring mixName = L"�P1A"; // ��������� + ��������� 'P,A'
    CPPUNIT_ASSERT( cyrName != mixName );
    auto cyrCode = LocoCharacteristics::instance()->GetSeriesCode( cyrName );
    auto mixCode = LocoCharacteristics::instance()->GetSeriesCode( mixName );
    CPPUNIT_ASSERT( cyrCode == mixCode );
    CPPUNIT_ASSERT( cyrCode );
}

// ��������� ������������ �������� ����� ����������
void TC_Locomotive::saveUnknown()
{
    std::wstring missName = L"���������_��_���������";
    auto missCode = LocoCharacteristics::instance()->GetSeriesCode( missName );
    CPPUNIT_ASSERT( 0 != missCode );
    std::wstring revName = LocoCharacteristics::instance()->GetSeriesName( missCode );
    CPPUNIT_ASSERT( revName == missName );
}
