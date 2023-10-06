#include "stdafx.h"

#include "TC_ExtId.h"
#include "../helpful/ExtId.h"
#include "../helpful/Attic.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ExtId );

void TC_ExtId::none()
{
    ExtId none;
    CPPUNIT_ASSERT( none.empty() );
    CPPUNIT_ASSERT( !none.is_external() );
    CPPUNIT_ASSERT( none.get_value().empty() );
    CPPUNIT_ASSERT( none.get_refcode().empty() );
}

void TC_ExtId::local()
{
    ExtId loc = ExtId::make_local( 111 );
    CPPUNIT_ASSERT( !loc.empty() );
    CPPUNIT_ASSERT( !loc.is_external() );
    CPPUNIT_ASSERT( loc.get_value() == ExtId::IdCode("i111", "") );
    CPPUNIT_ASSERT( loc.get_refcode() == "" );
    unsigned curr_top = 67;
    curr_top = ExtId::update_top( curr_top, loc );
    CPPUNIT_ASSERT( curr_top == 111 );

    ExtId loc2 = ExtId::make_local( 300 );
    CPPUNIT_ASSERT( loc2.get_value() == ExtId::IdCode("i300", "") );
    curr_top = ExtId::update_top( curr_top, loc2 );
    CPPUNIT_ASSERT( curr_top == 300 );
}

void TC_ExtId::exter()
{
    // простое создание
    ExtId ext( "777", "my777" );
    CPPUNIT_ASSERT( !ext.empty() );
    CPPUNIT_ASSERT( ext.is_external() );
    CPPUNIT_ASSERT( ext.get_value() == ExtId::IdCode("R777", "") );
    CPPUNIT_ASSERT( ext.get_refcode() == "my777" );
    // не влияет на top
    unsigned curr_top = 67;
    curr_top = ExtId::update_top( curr_top, ext );
    CPPUNIT_ASSERT( curr_top == 67 );
    // повторное использование
    ExtId ext2("R002", "xx");
    CPPUNIT_ASSERT( ext2.get_value() == ExtId::IdCode("R002", "") );
    CPPUNIT_ASSERT( ext2.get_refcode() == "xx" );
    // конверсия устаревшего
    ExtId ext7("G007", "");
    CPPUNIT_ASSERT( ext7.get_value() == ExtId::IdCode("R007", "") );
    CPPUNIT_ASSERT( ext7.get_refcode() == "007" );
}

void TC_ExtId::compatibility()
{
    ExtId old( "5", "" );
    CPPUNIT_ASSERT( !old.empty() );
    CPPUNIT_ASSERT( old.is_external() );
    CPPUNIT_ASSERT( old.get_value() == ExtId::IdCode("R5","") );
    CPPUNIT_ASSERT( old.get_refcode() == "" );

    ExtId more( "G33", "" );
    CPPUNIT_ASSERT( !more.empty() );
    CPPUNIT_ASSERT( more.is_external() );
    CPPUNIT_ASSERT( more.get_value() == ExtId::IdCode("R33", "") );
    CPPUNIT_ASSERT( more.get_refcode() == "33" );
}

void TC_ExtId::serializing()
{
    ExtId e1( "33.44", "9876543210" );
    ExtId g1( "G12345", "" );
    ExtId l1 = ExtId::make_local(  97 );
    attic::a_document doc("xxx");

    attic::a_node en = doc.document_element().append_child("E");
    attic::a_node gn = doc.document_element().append_child("G");
    attic::a_node ln = doc.document_element().append_child("L");

    e1.serialize( en, "EEid" );
    g1.serialize( gn, "GGid" );
    l1.serialize( ln, "LLid" );

    ExtId e2, g2, l2;
    e2.deserialize( en, "EEid" );
    g2.deserialize( gn, "GGid" );
    l2.deserialize( ln, "LLid" );
    CPPUNIT_ASSERT( e1.get_value() == e2.get_value() );
    CPPUNIT_ASSERT( g1.get_value() == g2.get_value() );
    CPPUNIT_ASSERT( l1.get_value() == l2.get_value() );
    CPPUNIT_ASSERT( e1.get_refcode() == e2.get_refcode() );
    CPPUNIT_ASSERT( g1.get_refcode() == g2.get_refcode() );
    CPPUNIT_ASSERT( l1.get_refcode() == l2.get_refcode() );
}

void TC_ExtId::hashing()
{
    ExtId id[] = { 
        ExtId( "123.456", "9876543210" ),
        ExtId( "123.456", "1111111111" ),
        ExtId( "765.000", "9876543210" ),
        ExtId( "765.000", "1111111111" )
    };

    for( auto i : id )
    {
        for( auto j : id )
        {
            bool direct = ( i == j );
            bool revert = ( i != j );
            bool hashed = ( std::hash<ExtId>()(i) == std::hash<ExtId>()(j) );
            CPPUNIT_ASSERT( direct != revert );
            CPPUNIT_ASSERT( direct == hashed );
        }
    }
}

void TC_ExtId::compare_id()
{
	ExtId id1("R488ff2ec-712e-4e03-8f77-72c2707a1646", "55965766-fc03-4c6a-b0e5-d6819638515f" );
	ExtId id2("R488ff2ec-712e-4e03-8f77-72c2707a1646", "00000000-0000-0000-0000-000000000000");
	ExtId id3("R488ff2ec-712e-4e03-8f77-72c2707a1646", "55965766-fc03-4c6a-b0e5-d6819638515f" );
	CPPUNIT_ASSERT( id1 == id2 );
	CPPUNIT_ASSERT( id2 == id1 );
	CPPUNIT_ASSERT( id2 == id3 );
	CPPUNIT_ASSERT( id1 == id3 );

	CPPUNIT_ASSERT( !(id1<id2) );
	CPPUNIT_ASSERT( !(id2<id1) );
}
