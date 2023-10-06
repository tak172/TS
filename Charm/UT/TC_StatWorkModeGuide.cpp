#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_StatWorkModeGuide.h"
#include "../helpful/StatWorkModeGuide.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StatWorkModeGuide );

void TC_StatWorkModeGuide::LoadSave()
{
    const std::wstring workmode_str = 
        L"<StatWorkModeProperties>"
        L"<Station EsrCode='09010' DayNight='Y' Dsp='2'>"
        L"<timeChange  Time='08:00'/>"
        L"<timeChange  Time='14:00'/>"
        L"<timeChange  Time='20:00'/>"
        L"</Station>"
        L"</StatWorkModeProperties>";

    attic::a_document x_doc;
    x_doc.load_wide( workmode_str );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    CStatWokModeGuide swg;
    swg.deserialize( x_doc.document_element() );

    attic::a_document y_doc;
    swg.serialize(y_doc.set_document_element(CStatWokModeGuide::tag()));
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend() );

    CPPUNIT_ASSERT( str_src == str_dst );
}

void TC_StatWorkModeGuide::testGetTime()
{
    const std::wstring workmode_str = 
        L"<StatWorkModeProperties>"
        L"<Station EsrCode='09010' DayNight='Y' Worker='1'>"
        L"<timeChange  Time='08:00'/>"
        L"<timeChange  Time='14:00'/>"
        L"<timeChange  Time='20:00'/>"
        L"</Station>"
        L"</StatWorkModeProperties>";

    attic::a_document x_doc;
    x_doc.load_wide( workmode_str );

    CStatWokModeGuide swg;
    swg.deserialize( x_doc.document_element() );

    CStatWorkMode mode = swg.getStationMode(EsrKit(9010));

    CPPUNIT_ASSERT ( mode.isDayNight() && mode.getDsp()==1 );
    CPPUNIT_ASSERT ( mode.NearTimeByHoursAndMinute(14, 15).first==14*3600 );
    CPPUNIT_ASSERT ( mode.NearTimeByHoursAndMinute(9,45).first==8*3600 );
    CPPUNIT_ASSERT ( mode.NearTimeByHoursAndMinute(23,10).first==20*3600 );
}

