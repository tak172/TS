#include "stdafx.h"

#include "TC_Excerpt_Lot.h"
#include "../helpful/ExcerptLot.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Excerpt_Lot );

void TC_Excerpt_Lot::Span_Full()
{
    attic::a_document x_doc;
    x_doc.load_wide( 
        L"<Test>"
        L"  <ForbidEvent Id='1' Sudden='Y' esr='09750:09772' FullSpan='Y' Reason='13505' inner_name='srvWay'>"
        L"    <picketing_start picketing1_val='109~100' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop picketing1_val='124~900' picketing1_comm='Вентспилс' />"
        L"  </ForbidEvent>"
        L"</Test>"
        );
    rewrite_lot(x_doc.document_element());
    Excerpt::Lot lot = Excerpt::Lot::readFrom( x_doc.document_element().first_child() ); 
    auto spl = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( nullptr != spl );
    CPPUNIT_ASSERT( spl->getRInterval().ruler() == L"Вентспилс" );
    CPPUNIT_ASSERT( spl->getRInterval().interval().begin() == rwCoord("109~100") );
    CPPUNIT_ASSERT( spl->getRInterval().interval().end() == rwCoord("124~900") );
    CPPUNIT_ASSERT( lot.getEsr() == EsrKit("09750:09772") );
    CPPUNIT_ASSERT( spl->getParkWay().empty() );
    CPPUNIT_ASSERT( spl->isFullSpan() );
}

void TC_Excerpt_Lot::Span_SingleWay_Segment()
{
    attic::a_document x_doc;
    x_doc.load_wide( 
        L"<Test>"
        L"  <ForbidEvent Id='1' Sudden='N' esr='09750:09772' FullSpan='n' waynum='2' Reason='13505' inner_name='srvWay'>"
        L"    <picketing_start picketing1_val='109~500' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop picketing1_val='109~700' picketing1_comm='Вентспилс' />"
        L"  </ForbidEvent>"
        L"</Test>"
        );

    rewrite_lot(x_doc.document_element());
    Excerpt::Lot lot = Excerpt::Lot::readFrom( x_doc.document_element().first_child() ); 
    auto spl = lot.get<Excerpt::SpanLot>();
    CPPUNIT_ASSERT( nullptr != spl );
    CPPUNIT_ASSERT( spl->getRInterval().ruler() == L"Вентспилс" );
    CPPUNIT_ASSERT( spl->getRInterval().interval().begin() == rwCoord("109~500") );
    CPPUNIT_ASSERT( spl->getRInterval().interval().end() == rwCoord("109~700") );
    CPPUNIT_ASSERT( lot.getEsr() == EsrKit("09750:09772") );
    CPPUNIT_ASSERT( spl->getParkWay() == ParkWayKit(2) );
    CPPUNIT_ASSERT( !spl->isFullSpan() );
}

void TC_Excerpt_Lot::Terminal_IoWay()
{
    attic::a_document x_doc;
    x_doc.load_wide( 
        L"<Test>"
        L"<ForbidEvent Id='1' esr='01234' parknum='3' waynum='7' StartTime='20160616T050020Z' StopTime='20160617T150020Z' Reason='13505' inner_name='srvWay'>"
        L"  <picketing_start picketing1_val='109~500' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='109~700' picketing1_comm='Вентспилс' />"
        L"</ForbidEvent>"
        L"</Test>"
        );
    rewrite_lot(x_doc.document_element());
    Excerpt::Lot lot = Excerpt::Lot::readFrom( x_doc.document_element().first_child() ); 
    auto twl = lot.get<Excerpt::TermWayLot>();
    CPPUNIT_ASSERT( nullptr != twl );
    CPPUNIT_ASSERT( lot.getEsr() == EsrKit("01234") );
    CPPUNIT_ASSERT( twl->getParkWay() == ParkWayKit(3,7) );
}

void TC_Excerpt_Lot::Terminal_SeparateObject()
{
    attic::a_document x_doc;
    x_doc.load_wide( 
        L"<Test>"
        L"<ForbidEvent Id='1' esr='04321' FullSpan='Y' JustObject='12SP[04321]' Reason='13505' inner_name='srvWay'>"
        L"  <picketing_start picketing1_val='109~100' picketing1_comm='Вентспилс' />"
        L"  <picketing_stop picketing1_val='124~900' picketing1_comm='Вентспилс' />"
        L"</ForbidEvent>"
        L"</Test>"
        );
    rewrite_lot(x_doc.document_element());
    Excerpt::Lot lot = Excerpt::Lot::readFrom( x_doc.document_element().first_child() ); 
    auto spar = lot.get<Excerpt::SparseLot>();
    CPPUNIT_ASSERT( nullptr != spar );
    CPPUNIT_ASSERT( lot.getEsr() == EsrKit("04321") );
    CPPUNIT_ASSERT( spar->getObject() == BadgeE( L"12SP", lot.getEsr() ) );
}

void TC_Excerpt_Lot::rewrite_lot( attic::a_node parent )
{
    Excerpt::Lot temp = Excerpt::Lot::readFrom( parent.first_child() );
    attic::a_node rewrite_node = parent.append_child("Rewrited");
    temp.writeTo( rewrite_node );
    parent.remove_child( parent.first_child() );
}
