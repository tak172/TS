#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_ServicesGuide.h"

#include "../Actor/HemDraw/Service.h"
#include "../Actor/HemDraw/AliasNameGuide.h"
#include "../helpful/Dictum.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ServicesGuide );

void TC_ServicesGuide::LoadSave()
{
    const std::wstring services_txt = 
        L"<F2A_SERVICETYPE>"
        L"<Service inner_name='srvWagon' full_name='Служба вагонного хозяйства' short_name='В' />"
        L"<Service inner_name='srvMoving' full_name='Служба перевозок' short_name='Д' />"
        L"<Service inner_name='srvEnergy' full_name='Служба электрификации и энергоснабжения' short_name='Э' />"
        L"<Service inner_name='srvWay' full_name='Служба пути' short_name='П' />"
        L"<Service inner_name='srvOther' full_name='Прочие' short_name='...' />"
        L"</F2A_SERVICETYPE>";
 
    attic::a_document x_doc;
    x_doc.load_wide( services_txt );
    std::wstring str_src = FromUtf8(x_doc.document_element().to_str());

    ServicesGuide sg;
    x_doc.document_element() >> sg;

    const auto& vItems = sg.all_items();

    CPPUNIT_ASSERT ( !sg.empty() );
    CPPUNIT_ASSERT ( vItems.size()==5 );

    {
        //1-я группа
        auto cit = vItems.find(srvEnergy);
        CPPUNIT_ASSERT ( cit!=vItems.end() && cit->second.get_short_name()==L"Э" &&
                        cit->second.get_name()==L"Служба электрификации и энергоснабжения" );
    }
    {
        //5-я группа
        auto cit = vItems.find(srvMoving);
        CPPUNIT_ASSERT ( cit!=vItems.end() && cit->second.get_short_name()==L"Д" &&
            cit->second.get_name()==L"Служба перевозок" );
    }

    attic::a_document y_doc;
    y_doc.set_document_element(XML_F2A_SERVICETYPE) << sg;
    std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());

    //auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), src_dst.cend() );
    CPPUNIT_ASSERT( str_src == str_dst );
}
