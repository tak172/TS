#include "stdafx.h"

#include <sstream>
#include "TC_xml_LOS.h"
#include "../Actor/LOSBase.h"
#include "../Actor/LOSBoard.h"
#include "../Actor/LOSLandscape.h"
#include "../Actor/Commutator_Arrow.h"
#include "TC_LOS_landscape.h"
#include "BuildTrainDescr.h"
#include "../helpful/Oddness.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_xml_LOS );

using namespace std;
using namespace boost::posix_time;

const StateLTS sampleState[3]={StateLTS::ACTIVE, StateLTS::PASSIVE, StateLTS::UNDEF};

void TC_xml_LOS::signals_Commutator()
{
    // пустой тест т.к. в COMMUTATOR нет сигналов 
}

void TC_xml_LOS::fields_Commutator()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(COMMUTATOR) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->insert_Subitem( Commutator_Arrow(11, 22, true, false, false, +1.2) );
    CPPUNIT_ASSERT(chk(a,b));
    a->insert_Subitem( Commutator_Arrow(22, 33, false, true, false, -2.3) );
    CPPUNIT_ASSERT(chk(a,b));
    a->removeArrow( 11, 22 );
    CPPUNIT_ASSERT(chk(a,b));
    a->removeArrow( 22, 33 );
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Strip()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(STRIP) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->reset_signals();
    CPPUNIT_ASSERT( StateLTS::UNDEF!=sampleState[0] ); // для успешного первого прохода
    for( int i=0; i<size_array(sampleState); i++ )
    {
        StateLTS lts = sampleState[i];
        a->setUsed(lts); 
        CPPUNIT_ASSERT(chk(a,b));
        a->setLock(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setArtUsed(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setArtLock(lts);
        CPPUNIT_ASSERT(chk(a,b));
    }
}

void TC_xml_LOS::fields_Strip()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(STRIP) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    size_t pos=0;
	a->setCapacity(Oddness::EVEN, L"64");
	a->setCapacity(Oddness::ODD, L"65");
    CPPUNIT_ASSERT(chk(a,b));
    a->setMaxSpeed(123);
    CPPUNIT_ASSERT(chk(a,b));
    a->setWayPassenger(true);
    CPPUNIT_ASSERT(chk(a,b));
    a->setUnderWindow(true);
    CPPUNIT_ASSERT(chk(a,b));
    a->bindRoute(true);
    CPPUNIT_ASSERT(chk(a,b));
    a->setInLaunchedRoute(true);
    CPPUNIT_ASSERT(chk(a,b));
    a->setPartNeck(true);
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Switch()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(SWITCH) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->reset_signals();
    CPPUNIT_ASSERT( StateLTS::UNDEF!=sampleState[0] ); // для успешного первого прохода
    for( int i=0; i<size_array(sampleState); i++ )
    {
        StateLTS lts = sampleState[i];
        a->setPlus(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setMinus(lts);
        CPPUNIT_ASSERT(chk(a,b));
    }
}

void TC_xml_LOS::fields_Switch()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(SWITCH) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    size_t pos=0;
    a->setUnderWindow(true);
    CPPUNIT_ASSERT(chk(a,b));
    a->setPartNeck(true);
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Head()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(HEAD) );
    boost::scoped_ptr<LOS_base> b( a->clone() );
    a->reset_signals();
    CPPUNIT_ASSERT( StateLTS::UNDEF!=sampleState[0] ); // для успешного первого прохода
    for( int i=0; i<size_array(sampleState); i++ )
    {
        StateLTS lts = sampleState[i];
        a->setOpenSignal(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setCloseSignal(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setBreakSignal(lts);
        CPPUNIT_ASSERT(chk(a,b));
    }
}

void TC_xml_LOS::fields_Head()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(HEAD) );
    boost::scoped_ptr<LOS_base> b( a->clone() );
//     a->setTypeOfHead(TRAIN);
//     b->setTypeOfHead(SHUNTING);
//     CPPUNIT_ASSERT(chk(a,b));
//     a->setTypeOfHead(SHUNTING);
//     CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_HtmlText()
{
    // пустой тест т.к. в HtmlText нет сигналов 
}

void TC_xml_LOS::fields_HtmlText()
{
	boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(FIGURE, HTML_AREA) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->setContent(L"HTML-ный текст");
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Figure()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(FIGURE) );
    boost::scoped_ptr<LOS_base> b( a->clone() );
    a->reset_signals();
    CPPUNIT_ASSERT( StateLTS::UNDEF!=sampleState[0] ); // для успешного первого прохода
    for( int i=0; i<size_array(sampleState); i++ )
    {
        StateLTS lts = sampleState[i];
        a->setMark(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setWink(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setMark2(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setWink2(lts);
        CPPUNIT_ASSERT(chk(a,b));
    }
}

void TC_xml_LOS::fields_Figure()
{
    // пустой тест т.к. в Figure полей нет
}


void TC_xml_LOS::signals_Board()
{
    // пустой тест т.к. в BOARD нет сигналов 
}

void TC_xml_LOS::fields_Board()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(LOGIC_BOARD) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->setWayPassenger(true);
    CPPUNIT_ASSERT(chk(a,b));
    LOS_board* ty_a = static_cast<LOS_board*>(a.get());
	ty_a->insertInfoNum( Info_Number( BuildTrain( 1234, L"1-1-1", L"y", 60, 1100, 550, L"time" ), L"22", L"23", true, false, false, 3, false ) );
    CPPUNIT_ASSERT(chk(a,b));
	ty_a->insertInfoNum( Info_Number( BuildTrain( 5678, L"1-1-1", L"b", 60, 1100, 550, L"time" ), L"68", L"69", false, true, false, 1, true ) );
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Landscape()
{
    // пустой тест т.к. в LANDSCAPE нет сигналов 
}

void TC_xml_LOS::fields_Landscape()
{
    using namespace Landscape;

    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object(LANDSCAPE) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    const vector<Path>   exP = TC_LOS_landscape::generate_Path();
    const vector<PeMark> exM = TC_LOS_landscape::generate_PeMark();

    for( const Path& p : exP )
    {
        CPPUNIT_ASSERT( a->insert_Subitem<Path>( p ) );
        CPPUNIT_ASSERT(chk(a,b));
    }
    unsigned int pathNum = 1;
    for( const PeMark& m : exM )
    {
        CPPUNIT_ASSERT( static_cast<LOS_landscape&>(*a).insert_PeMark( m, pathNum++ ) );
        CPPUNIT_ASSERT(chk(a,b));
    }

    wstring ruler;
    rwCoord left,right;
    TC_LOS_landscape::generate_Bounds( &ruler, &left, &right );
    static_cast<LOS_landscape&>(*a).setBound( ruler, left, right );
    CPPUNIT_ASSERT(chk(a,b));

    a->reset_signals();
    CPPUNIT_ASSERT(chk(a,b));
}


void TC_xml_LOS::signals_Text()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object( FIGURE, FIGURE ) );
    boost::scoped_ptr<LOS_base> b( a->clone() );
    a->reset_signals();
    CPPUNIT_ASSERT( StateLTS::UNDEF!=sampleState[0] ); // для успешного первого прохода
    for( int i=0; i<size_array(sampleState); i++ )
    {
        StateLTS lts = sampleState[i];
        a->setMark(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setWink(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setMark2(lts);
        CPPUNIT_ASSERT(chk(a,b));
        a->setWink2(lts);
        CPPUNIT_ASSERT(chk(a,b));
    }
}

void TC_xml_LOS::fields_Text()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object( FIGURE, GRAPHIC_TEXT ) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->setContent(L"новость");
    CPPUNIT_ASSERT(chk(a,b));
}

void TC_xml_LOS::signals_Void()
{
    // пустой тест т.к. в Void нет сигналов 
}

void TC_xml_LOS::fields_Void()
{
    // пустой тест т.к. в Void и полей никаких нет
}

void TC_xml_LOS::signals_WinText()
{
    // пустой тест т.к. в WINTEXT нет сигналов 
}

void TC_xml_LOS::fields_WinText()
{
    boost::scoped_ptr<LOS_base> a( LOS_base::fabric_from_object( FIGURE, GRAPHIC_TEXT ) );
    boost::scoped_ptr<LOS_base> b( a->clone() );

    a->setContent(L"новость");
    CPPUNIT_ASSERT(chk(a,b));
    a->setPhase(WPH_OVERRUN);
    CPPUNIT_ASSERT(chk(a,b));
	a->setPhase(WPH_NOTHING);
	CPPUNIT_ASSERT(chk(a,b));
}

//
// Общая часть
//
void TC_xml_LOS::setUp()
{
    doc.load_wide( L"<test />" );
    node = doc.document_element();
}

void TC_xml_LOS::tearDown()
{
    doc.reset();
    node = doc.document_element();
}

bool TC_xml_LOS::chk( boost::scoped_ptr<LOS_base>& one, boost::scoped_ptr<LOS_base>& two )
{
    bool before = two->isEqual(one.get());
    one->write_to_xml(node);
    two->read_from_xml(node);
    bool after = two->isEqual(one.get());
    return (after && !before);
}