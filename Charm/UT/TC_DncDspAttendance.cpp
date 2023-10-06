#include "stdafx.h"

#include "../helpful/Attic.h"
#include "../helpful/Interval_Time_t.h"
#include "../helpful/StrToTime.h"
#include "../Hem/DncDspAttendance.h"
#include "TC_DncDspAttendance.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DncDspAttendance );

void TC_DncDspAttendance::LoadSave()
{
    attic::a_document sourceDoc;
    sourceDoc.load_wide(
        L"<test>"
        L"  <DncDspAttendance>"
        L"    <District code='111-555'>"
        L"      <Handover time='20151218T130000Z' />"
        L"      <Handover time='20151218T190000Z' revert='20151218T193100Z' unlocker='192.168.1.11'/>"
        L"      <Dnc start='20151218T030000Z' persona='Иванов' />"
        L"      <Dnc start='20151218T150000Z' persona='Петров П.П.' />"
        L"      <List>"
        L"           <Dnc persona='Иванов'/>"
        L"           <Dnc persona='Петров П.П.'/>"
        L"           <Dnc persona='Сидоренко'/>"
        L"      </List>"
        L"    </District>"
        L"    <District code='222-777' >"
        L"      <Dnc start='20151218T030000Z' persona='Сергушов' />"
        L"      <Dnc start='20151218T150000Z' persona='Дятлов' />"
        L"      <List>"
        L"           <Dnc persona='Дятлов'/>"
        L"           <Dnc persona='Сергушов'/>"
        L"           <Dnc persona='Яковлев'/>"
        L"      </List>"
        L"    </District>"
        L"    <District code='333-999'/>"
        L"    <Station ESR_code='01111' >"
        L"      <Dsp start='20151218T030000Z' persona='Маша' />"
        L"      <Dsp start='20151218T090000Z' />"
        L"      <Dsp start='20151218T150000Z' persona='Зина' />"
        L"      <List>"
        L"           <Dsp persona='Кутузов М.И.'/>"
        L"           <Dsp persona='Суворов А.В.'/>"
        L"      </List>"
        L"    </Station>"
        L"    <Station ESR_code='02222' >"
        L"      <Dsp start='20151218T030000Z' persona='Зоя' />"
        L"      <Dsp start='20151218T150000Z' persona='Киса' />"
        L"    </Station>"
        L"    <Station ESR_code='04444' >"
        L"      <List>"
        L"           <Dsp persona='Кусто Ж.'/>"
        L"      </List>"
        L"    </Station>"
        L"  </DncDspAttendance>"
        L"</test>"
        );

    DncDsp::Attendance attendance;
    sourceDoc.document_element() >> attendance;
    std::wstring etalon = sourceDoc.pretty_wstr();

     attic::a_document targetDoc;
     targetDoc.set_document_element("test") << attendance;
     std::wstring result = targetDoc.pretty_wstr();
 
     auto mismres = std::mismatch( etalon.cbegin(), etalon.cend(), result.cbegin(), result.cend() );
     CPPUNIT_ASSERT( etalon == result );

    // внести изменение
    std::string d2 = "222-777";
    interval_time_t big( StrDataTime_To_UTC("2015-12-18 01:00"), StrDataTime_To_UTC("2015-12-18 23:00") );
    auto moments1 = attendance.GetChangeDnc( d2, big );
    CPPUNIT_ASSERT( moments1.size()>1 );
    CPPUNIT_ASSERT( attendance.PushDnc( d2, StrDataTime_To_UTC("2015-12-18 20:00"), L"Кротов" ) );
    auto moments2 = attendance.GetChangeDnc( d2, big );
    CPPUNIT_ASSERT( moments1.size()<moments2.size() );

    // прочитать измененный набор
    attic::a_document chDoc;
    chDoc.set_document_element("myCh");
    attendance.TakeChanges( chDoc.document_element() );
    DncDsp::Attendance attendance2;
    attendance2.loadFromChanges( chDoc.document_element() );

    CPPUNIT_ASSERT( attendance == attendance2 );
}

time_t ptime_to_time_t( boost::posix_time::ptime moment )
{
    static const boost::posix_time::ptime EPOCHA(boost::gregorian::date(1970, 1, 1));
    return (moment - EPOCHA).total_seconds();
}

void TC_DncDspAttendance::Push()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    const std::string u1("uch1");
    const std::string u2("uch2");
    const EsrKit e1("12345");
    const EsrKit e2("76543");
    time_t pt = ptime_to_time_t(ptime(date(2013,2,5)));

#define HOUR_IN_SEC(x) x*3600
#define MINUTE_IN_SEC(x) x*60

    DncDsp::Attendance attendance;
    CPPUNIT_ASSERT( attendance.PushDnc( u1, pt+HOUR_IN_SEC(6),  L"1-6" ) );
    CPPUNIT_ASSERT( attendance.PushDnc( u2, pt+HOUR_IN_SEC(5),  L"2-5" ) );
    CPPUNIT_ASSERT( attendance.PushDnc( u1, pt+HOUR_IN_SEC(18), L"1-18" ) );
    CPPUNIT_ASSERT( attendance.PushDnc( u2, pt+HOUR_IN_SEC(17), L"2-17" ) );
    CPPUNIT_ASSERT( !attendance.PushDnc( u1, pt+HOUR_IN_SEC(9), L"1-9" ) );
    CPPUNIT_ASSERT( !attendance.PushDnc( u2, pt+HOUR_IN_SEC(9), L"2-9" ) );

    std::pair<time_t, std::wstring> person_info = attendance.GetDnc( u1, pt+HOUR_IN_SEC(1) );
    CPPUNIT_ASSERT( 0==person_info.first && L""    == person_info.second );
    person_info = attendance.GetDnc( u1, pt+HOUR_IN_SEC(6) );
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(6)==person_info.first && L"1-6" == person_info.second );
    person_info = attendance.GetDnc( u1, pt+HOUR_IN_SEC(18)+MINUTE_IN_SEC(2)); 
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(18)==person_info.first && L"1-18"== person_info.second );

    person_info = attendance.GetDnc( u2, pt+HOUR_IN_SEC(1) ); 
    CPPUNIT_ASSERT( 0==person_info.first && L""    == person_info.second );
    person_info = attendance.GetDnc( u2, pt+HOUR_IN_SEC(6) );
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(5)==person_info.first && L"2-5" == person_info.second );
    person_info = attendance.GetDnc( u2, pt+HOUR_IN_SEC(18) );
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(17)==person_info.first && L"2-17"== person_info.second );

    CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(6),  L"1e6" ) ) ;
    CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(5),  L"2e5" ) );
    CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(18), L"1e18") );
    CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(17), L"2e17" ) );
    CPPUNIT_ASSERT( !attendance.PushWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(9), L"1e9" ) );
    CPPUNIT_ASSERT( !attendance.PushWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(9), L"2e9" ) );

    person_info = attendance.GetWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(1), 1 ); 
    CPPUNIT_ASSERT( 0==person_info.first && L""    == person_info.second );
    person_info = attendance.GetWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(6), 1 ); 
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(6)==person_info.first && L"1e6" == person_info.second );
    person_info = attendance.GetWorker( DncDsp::DSP, e1, pt+HOUR_IN_SEC(18)+MINUTE_IN_SEC(2), 1); 
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(18)==person_info.first && L"1e18"== person_info.second );

    person_info = attendance.GetWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(1), 1); 
    CPPUNIT_ASSERT( 0==person_info.first && L""    == person_info.second );
    person_info = attendance.GetWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(6), 1); 
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(5)==person_info.first && L"2e5" == person_info.second );
    person_info = attendance.GetWorker( DncDsp::DSP, e2, pt+HOUR_IN_SEC(18)+MINUTE_IN_SEC(2), 1); 
    CPPUNIT_ASSERT( pt+HOUR_IN_SEC(17)==person_info.first && L"2e17"== person_info.second );
}

void TC_DncDspAttendance::Changes()
{
    using namespace boost::posix_time;
    using namespace boost::gregorian;

    time_t pt = ptime_to_time_t(ptime(date(2013,2,5)));

#define HOUR_IN_SEC(x) x*3600
#define MINUTE_IN_SEC(x) x*60
    
    interval_time_t shift( pt+HOUR_IN_SEC(5), pt+HOUR_IN_SEC(17) );

    DncDsp::Attendance attendance;

    {
        const std::string uch("uch1");
        auto changes = attendance.GetChangeDnc( uch, shift );
        CPPUNIT_ASSERT( changes.empty() );

        CPPUNIT_ASSERT( attendance.PushDnc( uch, pt+HOUR_IN_SEC(2),  L"предыдущий" ) ); // до смены
        changes = attendance.GetChangeDnc( uch, shift );
        CPPUNIT_ASSERT( changes.size()==1 );
        CPPUNIT_ASSERT( changes.front()==shift.lower() );

        CPPUNIT_ASSERT( attendance.PushDnc( uch, pt+HOUR_IN_SEC(8),  L"сменщик" ) ); // середина смены
        changes = attendance.GetChangeDnc( uch, shift );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==shift.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(8) );

        CPPUNIT_ASSERT( attendance.PushDnc( uch, pt+HOUR_IN_SEC(20),  L"последующий" ) ); // после смены
        changes = attendance.GetChangeDnc( uch, shift );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==shift.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(8) );
        auto dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(8) );
        CPPUNIT_ASSERT( dncInfo.second==L"сменщик" );

        attendance.RemoveDnc( uch, pt+HOUR_IN_SEC(7) ); // убираем в середине смены
        changes = attendance.GetChangeDnc( uch, shift );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==shift.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(8) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(8) );
        CPPUNIT_ASSERT( dncInfo.second.empty() );
    }

    {
        const std::string uch("test");

        interval_time_t tIntervalAll(pt, pt+HOUR_IN_SEC(30));
        CPPUNIT_ASSERT( attendance.PushDnc( uch, pt+HOUR_IN_SEC(2),  L"предыдущий" ) ); // до смены
        auto changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );

        attendance.ChangeDnc( uch, pt+HOUR_IN_SEC(2), L"предыдущий",  pt+HOUR_IN_SEC(2), L"подмена" ); // меняем фамилию
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );
        auto dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( dncInfo.second == L"подмена" );
        attendance.ChangeDnc( uch, pt+HOUR_IN_SEC(2), L"подмена",  pt+HOUR_IN_SEC(5), L"подмена" ); // меняем время
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(5) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( dncInfo.second.empty() );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(5) );
        CPPUNIT_ASSERT( dncInfo.second == L"подмена" );
        attendance.ChangeDnc( uch, pt+HOUR_IN_SEC(5), L"подмена",  pt+HOUR_IN_SEC(2), L"предыдущий" ); // меняем фамилию и время
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==2 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( dncInfo.second == L"предыдущий" );

        CPPUNIT_ASSERT( attendance.PushDnc( uch, pt+HOUR_IN_SEC(20),  L"последующий" ) ); // после смены
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==3 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( changes[2]==pt+HOUR_IN_SEC(20) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(20) );
        CPPUNIT_ASSERT( dncInfo.second==L"последующий" );

        attendance.ChangeDnc( uch, pt+HOUR_IN_SEC(20), L"последующий",  pt+HOUR_IN_SEC(20), L"подмена" ); // меняем последнего
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==3 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( changes[2]==pt+HOUR_IN_SEC(20) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(20) );
        CPPUNIT_ASSERT( dncInfo.second == L"подмена" );
        attendance.ChangeDnc( uch, pt+HOUR_IN_SEC(20), L"подмена",  pt+HOUR_IN_SEC(30), L"подмена" ); // меняем последнего
        changes = attendance.GetChangeDnc( uch, tIntervalAll );
        CPPUNIT_ASSERT( changes.size()==3 );
        CPPUNIT_ASSERT( changes[0]==tIntervalAll.lower() );
        CPPUNIT_ASSERT( changes[1]==pt+HOUR_IN_SEC(2) );
        CPPUNIT_ASSERT( changes[2]==pt+HOUR_IN_SEC(30) );
        dncInfo = attendance.GetDnc(uch, pt+HOUR_IN_SEC(30) );
        CPPUNIT_ASSERT( dncInfo.second == L"подмена" );
    }

    {
        const EsrKit esr("12345");
        auto changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.empty() );

        CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, esr, pt+HOUR_IN_SEC(8),  L"впервые с середины смены" ) ); // середина смены
        changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.size()==1 && changesAll.begin()->second.size()==2 );
        CPPUNIT_ASSERT( changesAll.begin()->second[0]==shift.lower() );
        CPPUNIT_ASSERT( changesAll.begin()->second[1]==pt+HOUR_IN_SEC(8) );

        attendance.RemoveWorker( DncDsp::DSP, esr, pt+HOUR_IN_SEC(8), L"впервые с середины смены", 1  ); // убираем одного
        changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.empty() );

        //добавляем еще раз
        CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, esr, pt+HOUR_IN_SEC(8),  L"впервые с середины смены" ) ); // середина смены
        changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.size()==1 && changesAll.begin()->second.size()==2 );
        CPPUNIT_ASSERT( changesAll.begin()->second[0]==shift.lower() );
        CPPUNIT_ASSERT( changesAll.begin()->second[1]==pt+HOUR_IN_SEC(8) );

        CPPUNIT_ASSERT( attendance.PushWorker( DncDsp::DSP, esr, pt+HOUR_IN_SEC(20),  L"будет потом" ) ); // после смены
        changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.size()==1 && changesAll.begin()->second.size()==2 );
        CPPUNIT_ASSERT( changesAll.begin()->second[0]==shift.lower() );
        CPPUNIT_ASSERT( changesAll.begin()->second[1]==pt+HOUR_IN_SEC(8) );


        attendance.RemoveWorker( DncDsp::DSP, esr, pt+HOUR_IN_SEC(7), 1 ); // убираем всех
        changesAll = attendance.GetChangeWorker( DncDsp::DSP, esr, shift );
        CPPUNIT_ASSERT( changesAll.empty() );
    }

}
