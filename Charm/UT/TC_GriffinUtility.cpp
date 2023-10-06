#include "stdafx.h"

#include "TC_GriffinUtility.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/GriffinUtility.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/ForbidEvent.h"
#include "UtHemHelper.h"
#include "../helpful/Time_Iso.h"
#include "../helpful/GriffinMessage.h"
#include "../helpful/Serialization.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_GriffinUtility );

void TC_GriffinUtility::single()
{
    SpotEvent event = createEvent<SpotEvent>(
        L"<SpotEvent name='Departure' Bdg='x[111]' create_time='20161111T000000Z' />" );
    auto td = std::make_shared<TrainDescr>(L"X777", L"0111-066-0222");
    td->SetFeature( TrainCharacteristics::TrainFeature::FinalDestination, EsrKit(911).to_wstring() );
    const auto td_view = griffin::Utility::to_wstring( *td );

    griffin::Utility gu( L"direct", L"revers" );
    gu.set_begin( event );
    gu.add_description( td );

    CPPUNIT_ASSERT( L"direct" == gu.runOperation );
    CPPUNIT_ASSERT( L"revers" == gu.invOperation );
    CPPUNIT_ASSERT( EsrKit(111) == gu.beginKit);
    CPPUNIT_ASSERT( time_from_iso("20161111T000000Z") == gu.beginMoment);
    CPPUNIT_ASSERT( gu.endKit.empty() );
    CPPUNIT_ASSERT( 0 == gu.endMoment );
    CPPUNIT_ASSERT( std::wstring::npos != gu.description.find( td_view ) );
}

void TC_GriffinUtility::both()
{
    SpotEvent one = createEvent<SpotEvent>(
        L"<SpotEvent name='Departure' Bdg='xx[222]' create_time='20161111T000000Z' />" );
    SpotEvent two = createEvent<SpotEvent>(
        L"<SpotEvent name='Arrival'   Bdg='yy[333]' create_time='20161111T002222Z' />" );
    auto td1 = std::make_shared<TrainDescr>(L"X257", L"0111-066-0222");
    td1->SetFeature( TrainCharacteristics::TrainFeature::FinalDestination, EsrKit(71717).to_wstring() );
    const auto td1_view = griffin::Utility::to_wstring( *td1 );
    auto td2 = std::make_shared<TrainDescr>(L"Y742", L"0222-044-0111");
    const auto td2_view = griffin::Utility::to_wstring( *td2 );

    griffin::Utility gu( L"aa", L"bb" );
    gu.set_begin( one );
    gu.add_description( td1 );
    gu.set_end(   two );
    gu.add_description( td2 );

    CPPUNIT_ASSERT( L"aa" == gu.runOperation );
    CPPUNIT_ASSERT( L"bb" == gu.invOperation );
    CPPUNIT_ASSERT( EsrKit(222) == gu.beginKit);
    CPPUNIT_ASSERT( time_from_iso("20161111T000000Z") == gu.beginMoment);
    CPPUNIT_ASSERT( EsrKit(333) == gu.endKit);
    CPPUNIT_ASSERT( time_from_iso("20161111T002222Z") == gu.endMoment );
    CPPUNIT_ASSERT( std::wstring::npos != gu.description.find( td1_view ) );
    CPPUNIT_ASSERT( std::wstring::npos != gu.description.find( td2_view ) );
}

void TC_GriffinUtility::forbid()
{
    Forbid::Event event = createEvent<Forbid::Event>(
        L"  <Data Id='G25' StartTime='20151019T220000Z' StopTime='20151019T233000Z' esr='11050:11051' Reason='1111' UntilStop='' SpeedPass='60' SpeedGoods='62' SpeedSuburban='61'>"
        L"    <picketing_start picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop picketing1_val='429~' picketing1_comm='Вентспилс' />"
        L"  </Data>"
        );

    griffin::Utility gu( L"win", L"lost" );
    gu.set_begin_end( event );

    CPPUNIT_ASSERT( gu.runOperation == L"win" );
    CPPUNIT_ASSERT( gu.invOperation == L"lost" );
    CPPUNIT_ASSERT( gu.beginKit == EsrKit(11050,11051) );
    CPPUNIT_ASSERT( gu.endKit.empty() );
    CPPUNIT_ASSERT( time_from_iso("20151019T220000Z") == gu.beginMoment );
    CPPUNIT_ASSERT( time_from_iso("20151019T233000Z") == gu.endMoment );
}

void TC_GriffinUtility::forbidUntilStop()
{
    Forbid::Event event = createEvent<Forbid::Event>(
        L"  <Data Id='G27' esr='09280' waynum='3' parknum='1' StartTime='20151019T220000Z' "
        L"        Reason='1601' UntilStop='Y' RightAB='Y' ZeroVOltage='Y' />"
        );

    griffin::Utility gu( L"zv", L"unZv" );
    gu.set_begin_end( event );
    const griffin::Utility* pgi = &gu; 

    {
        griffin::UndoUtility    grifUndo;
        grifUndo.userIp = "1.1.1.1";
        grifUndo.invOperation = ToUtf8(pgi->invOperation);
        auto msg = griffin::GriffinMessage( grifUndo.userIp, ToUtf8(pgi->runOperation) );
        if ( pgi->begin() )
        {
            grifUndo.beginMoment = pgi->beginMoment;
            if ( !pgi->beginKit.empty() )
                grifUndo.beginPoint = '['+pgi->beginKit.to_string()+']';
            msg.set_begin( grifUndo.beginMoment, grifUndo.beginPoint );
        }
        if ( pgi->end() )
        {
            grifUndo.endMoment = pgi->endMoment;
            if ( !pgi->endKit.empty() )
                grifUndo.endPoint = '['+pgi->endKit.to_string()+']';
            msg.set_end( grifUndo.endMoment, grifUndo.endPoint );
        }
        grifUndo.dcode = "DistrTest";
        msg.set_region( grifUndo.dcode );
        grifUndo.description = ToUtf8( pgi->description );
        msg.set_description( grifUndo.description );

        std::string ss = serialize<griffin::GriffinMessage>(msg);
        CPPUNIT_ASSERT( !ss.empty() );
    }

    CPPUNIT_ASSERT( gu.runOperation == L"zv" );
    CPPUNIT_ASSERT( gu.invOperation == L"unZv" );
    CPPUNIT_ASSERT( gu.beginKit == EsrKit(9280) );
    CPPUNIT_ASSERT( gu.endKit.empty() );
    CPPUNIT_ASSERT( time_from_iso("20151019T220000Z") == gu.beginMoment );
    CPPUNIT_ASSERT( !gu.endMoment );
}
