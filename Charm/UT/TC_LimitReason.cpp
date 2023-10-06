#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_LimitReason.h"

#include "../Actor/HemDraw/LimitReason.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_LimitReason );

void TC_LimitReason::LoadSave()
{
    const std::wstring reason_txt = 
        L"<LimitReasonList>"
        L"<GroupReason full_name='Производство работ, ограждаемых сигналами остановки (скорость ограничена)'>"
        L"<LimitReason code='13459' full_name='Остановка для электросекций' short_name='Ост. для эл-секций' />"
        L"<LimitReason code='13480' full_name='Рихтовка бесстыкового пути 10-60 м' short_name='Рихтовка б/с пути' />"
        L"<LimitReason code='13499' full_name='Смена охранных брусьев' short_name='Смена охран. брусьев' />"
        L"</GroupReason>"
        L"<GroupReason full_name='Производство работ, ограждаемых сигналами остановки (скорость установленная)'>"
        L"<LimitReason code='13502' full_name='Смена крестовины' short_name='Смена крестовин' />"
        L"<LimitReason code='13516' full_name='Смена изоляции изолирующих стыков' short_name='Смена изостыков' />"
        L"<LimitReason code='13519' full_name='Монтаж арматуры пневматической обдувки' short_name='Монтаж пневмообдувки' />"
        L"<LimitReason code='13521' full_name='Монтаж электрообогрева стрелочного перевода' short_name='Монтаж обогрева стрелки' />"
        L"</GroupReason>"
        L"</LimitReasonList>";

    attic::a_document x_doc;
    x_doc.load_wide( reason_txt );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    LimitReasonList lrl;
    x_doc.document_element() >> lrl;

    CPPUNIT_ASSERT ( !lrl.empty() );
    CPPUNIT_ASSERT ( lrl.vReason.size()==2 );
    
    {
        //0-я группа
        const LimitReasonGroup& lrg = lrl.vReason.at(0);
        CPPUNIT_ASSERT ( lrg.group_name==L"Производство работ, ограждаемых сигналами остановки (скорость ограничена)" );
        CPPUNIT_ASSERT ( lrg.vReason.size()==3 && lrg.have_reason(13459) && 
                        lrg.have_reason(13480) && lrg.have_reason(13499));
    }
    {
        //1-я группа
        const LimitReasonGroup& lrg = lrl.vReason.at(1);
        CPPUNIT_ASSERT ( lrg.group_name==L"Производство работ, ограждаемых сигналами остановки (скорость установленная)" );
        CPPUNIT_ASSERT ( lrg.vReason.size()==4 && lrg.have_reason(13502) && 
            lrg.have_reason(13516) && lrg.have_reason(13519) && lrg.have_reason(13521));
    }

    attic::a_document y_doc;
    y_doc.set_document_element(lrl.tag()) << lrl;
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    //auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend() );
    CPPUNIT_ASSERT( str_src == str_dst );
}
