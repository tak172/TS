#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_MarksGuide.h"

#include "../Actor/HemDraw/MarksType.h"
#include "../Actor/HemDraw/AliasNameGuide.h"
#include "../helpful/Dictum.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_MarksGuide );

void TC_MarksGuide::LoadSave()
{
    const std::wstring marks_txt = 
        L"<F2A_MARKTYPE>"
        L"<Mark inner_name='markWay' full_name='Пути' short_name='П' />"
        L"<Mark inner_name='markEnergy' full_name='Энергохозяйства' short_name='Э' />"
        L"<Mark inner_name='markMoving' full_name='Перевозок' short_name='Д' />"
        L"<Mark inner_name='markWagon'  full_name='Вагонная' short_name='В' />"
        L"<Mark inner_name='markPassag' full_name='Пассажирская' short_name='Л' />"
        L"<Mark inner_name='markGoods' full_name='Грузовая' short_name='М' />"
        L"<Mark inner_name='markMilitary' full_name='Военизированная охрана' short_name='ВО' />"
        L"<Mark inner_name='markLocom' full_name='Локомотивная' short_name='Т' />"
        L"<Mark inner_name='markSignal' full_name='Сигнализации и связи' short_name='Ш' />"
        L"<Mark inner_name='markCustom' full_name='Таможенная' short_name='Тм' />"
        L"<Mark inner_name='markABClose' full_name='Закрытие автоблокировки' short_name='АБ' />"
        L"<Mark inner_name='markDropped' full_name='Брошенные' short_name='БР' />"
        L"<Mark inner_name='markHook' full_name='Прицепка/отцепка' short_name='+/-' />"
        L"<Mark inner_name='markPushWay' full_name='\"Толчок\" в пути' short_name='ТК!' />"
        L"<Mark inner_name='markSlide' full_name='\"Ползун\"' short_name='ПН' />"
        L"<Mark inner_name='markEnergyFail' full_name='Отказ устр. энергоснабжения' short_name='Оэ' />"
        L"<Mark inner_name='markPONAB' full_name='Тревога ПОНАБ/\"Диск\"' short_name='ПНБ' />"
        L"<Mark inner_name='markFalsePONAB' full_name='Ложная тревога ПОНАБ/\"Диск\"' short_name='-ПНБ' />"
        L"<Mark inner_name='markDelayOnStat' full_name='Задержка поезда на станции' short_name='СТП' />"
        L"</F2A_MARKTYPE>";

    attic::a_document x_doc;
    x_doc.load_wide( marks_txt );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    MarksGuide mg;
    x_doc.document_element() >> mg;

    const auto& vItems = mg.all_items();

    CPPUNIT_ASSERT ( !mg.empty() );
    CPPUNIT_ASSERT ( vItems.size()==19 );

    {
        auto cit = vItems.find(markSignal);
        CPPUNIT_ASSERT ( cit!=vItems.end() && cit->second.get_short_name()==L"Ш" &&
            cit->second.get_name()==L"Сигнализации и связи" );
    }
    {
        auto cit = vItems.find(markFalsePONAB);
        CPPUNIT_ASSERT ( cit!=vItems.end() && cit->second.get_short_name()==L"-ПНБ" &&
            cit->second.get_name()==L"Ложная тревога ПОНАБ/\"Диск\"" );
    }

    attic::a_document y_doc;
    y_doc.set_document_element(XML_F2A_MARKTYPE) << mg;
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend() );
    CPPUNIT_ASSERT( str_src == str_dst );
}
