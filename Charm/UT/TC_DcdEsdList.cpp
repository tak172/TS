#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_DcdEsdList.h"

#include "../Actor/HemDraw/DcdEsdList.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DcdEsdList );

void TC_DcdEsdList::LoadSave()
{
//     const std::wstring dcdesd_str = 
//                  L"<DCD_ESD_List>"
//                  L"<District name='Ventspils-Tukums-2' code='11'>"
//                  L"<DCD fio='Petrov A.S.' type='0' gang='1' />"
//                  L"<DCD fio='Ivanov E.K.' type='0' gang='2' />"
//                  L"<DCD fio='Sidorov T.T.' type='0' gang='3' />"
//                  L"<DCD fio='Kozlov S.D.' type='0' gang='4' />"
//                  L"<DCD fio='Semenov A.M.' type='1' gang='0' />"
//                  L"<DCD fio='Frolov V.E.' type='2' gang='0' />"
//                  L"<Station ESR_code='09810' >"
//                  L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//                  L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//                  L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//                  L"<ESD fio='Semenov A.M.' type='3' gang='0' />"
//                  L"</Station>"
//                  L"<Station ESR_code='09780' >"
//                  L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//                  L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//                  L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//                  L"<ESD fio='Semenov A.M.' type='3' gang='0' />"
//                  L"</Station>"
//                  L"</District>"
//                  L"<District name='Jelgava-Krustpils' code='22'>"
//                  L"<DCD fio='Petrov A.S.' type='0' gang='1' />"
//                  L"<DCD fio='Ivanov E.K.' type='0' gang='2' />"
//                  L"<DCD fio='Sidorov T.T.' type='0' gang='3' />"
//                  L"<DCD fio='Kozlov S.D.' type='0' gang='1' />"
//                  L"<Station ESR_code='09211' >"
//                  L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//                  L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//                  L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//                  L"</Station>"
//                  L"</District>"
//         L"</DCD_ESD_List>";
// 
// 
//     attic::a_document x_doc;
//     x_doc.load_wide( dcdesd_str );
//     std::wstring str_src = FromUtf8(x_doc.document_element().to_str());
// 
//     DcdEsdList de_list;
//     x_doc.document_element() >> de_list;
// 
//     attic::a_document y_doc;
//     y_doc.set_document_element("DCD_ESD_List") << de_list;
//     std::wstring str_dst = FromUtf8(y_doc.document_element().to_str());
// 
//     auto mismres = std::mismatch( str_src.cbegin(), str_src.cend(), str_dst.cbegin(), str_dst.cend() );

//     CPPUNIT_ASSERT( str_src == str_dst );
}

void TC_DcdEsdList::getDistrict()
{
//     const std::wstring dcdesd_str = 
//         L"<DCD_ESD_List>"
//         L"<District name='Ventspils-Tukums-2' code='11'>"
//         L"<DCD fio='Petrov A.S.' type='0' gang='1' />"
//         L"<DCD fio='Ivanov E.K.' type='0' gang='2' />"
//         L"<DCD fio='Sidorov T.T.' type='0' gang='3' />"
//         L"<DCD fio='Kozlov S.D.' type='0' gang='4' />"
//         L"<DCD fio='Semenov A.M.' type='1' gang='0' />"
//         L"<DCD fio='Frolov V.E.' type='2' gang='0' />"
//         L"<Station ESR_code='09810' >"
//         L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//         L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//         L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//         L"<ESD fio='Semenov A.M.' type='3' gang='0' />"
//         L"</Station>"
//         L"<Station ESR_code='09780' >"
//         L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//         L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//         L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//         L"<ESD fio='Semenov A.M.' type='3' gang='0' />"
//         L"</Station>"
//         L"</District>"
//         L"<District name='Jelgava-Krustpils' code='22'>"
//         L"<DCD fio='Petrov A.S.' type='0' gang='1' />"
//         L"<DCD fio='Ivanov E.K.' type='0' gang='2' />"
//         L"<DCD fio='Sidorov T.T.' type='0' gang='3' />"
//         L"<DCD fio='Kozlov S.D.' type='0' gang='1' />"
//         L"<Station ESR_code='09211' >"
//         L"<ESD fio='Petrov A.S.' type='0' gang='1' />"
//         L"<ESD fio='Ivanov E.K.' type='0' gang='2' />"
//         L"<ESD fio='Sidorov T.T.' type='0' gang='3' />"
//         L"</Station>"
//         L"</District>"
//         L"</DCD_ESD_List>";
// 
// 
//     attic::a_document x_doc;
//     x_doc.load_wide( dcdesd_str );
// 
//     DcdEsdList de_list;
//     x_doc.document_element() >> de_list;
// 
    DcdEsdDistrictPtr pDistrict;
// 
//     {
//         pDistrict = de_list.get_dcd_esd(L"11");
//         CPPUNIT_ASSERT ( pDistrict );
//         CPPUNIT_ASSERT ( pDistrict->get_name() == L"Ventspils-Tukums-2" );
//         const std::vector<CDcdEsd>& vDcd = pDistrict->get_dcd();
//         CPPUNIT_ASSERT ( vDcd.size()==6 );
// 
//         const CDcdEsd& dcd0 = vDcd[0];
//         CPPUNIT_ASSERT ( dcd0.fio==L"Petrov A.S." );
//         CPPUNIT_ASSERT ( dcd0.gang==1 );
//         CPPUNIT_ASSERT ( dcd0.state==Dcd_ready );
// 
//         const CDcdEsd& dcd5 = vDcd[5];
//         CPPUNIT_ASSERT ( dcd5.fio==L"Frolov V.E." );
//         CPPUNIT_ASSERT ( dcd5.gang==0 );
//         CPPUNIT_ASSERT ( dcd5.state==Dcd_holiday );
// 
//         const std::vector<CStationEsdPtr>& vEsd = pDistrict->get_esd();
// 
//         CPPUNIT_ASSERT ( vEsd.size()==2 );
// 
//         const CStationEsdPtr& stat0 = vEsd[0];
//         CPPUNIT_ASSERT ( stat0->get_esr()==EsrKit(9810) );
//         const std::vector<CDcdEsd>& vEsd0 = stat0->get_esd();
//         CPPUNIT_ASSERT ( vEsd0.size()==4 );
//         const CDcdEsd& dcd00 = vEsd0[0]; 
//         CPPUNIT_ASSERT ( dcd00.fio==L"Petrov A.S." );
//         CPPUNIT_ASSERT ( dcd00.gang==1 );
//         CPPUNIT_ASSERT ( dcd00.state==Dcd_ready );
//         const CDcdEsd& dcd02 = vEsd0[2]; 
//         CPPUNIT_ASSERT ( dcd02.fio==L"Sidorov T.T." );
//         CPPUNIT_ASSERT ( dcd02.gang==3 );
//         CPPUNIT_ASSERT ( dcd02.state==Dcd_ready );
// 
//         const CStationEsdPtr& stat1 = vEsd[1];
//         CPPUNIT_ASSERT ( stat1->get_esr()==EsrKit(9780) );
//         const std::vector<CDcdEsd>& vEsd1 = stat1->get_esd();
//         CPPUNIT_ASSERT ( vEsd1.size()==4 );
//         const CDcdEsd& dcd11 = vEsd1[1]; 
//         CPPUNIT_ASSERT ( dcd11.fio==L"Ivanov E.K." );
//         CPPUNIT_ASSERT ( dcd11.gang==2 );
//         CPPUNIT_ASSERT ( dcd11.state==Dcd_ready );
//         const CDcdEsd& dcd13 = vEsd1[3]; 
//         CPPUNIT_ASSERT ( dcd13.fio==L"Semenov A.M." );
//         CPPUNIT_ASSERT ( dcd13.gang==0 );
//         CPPUNIT_ASSERT ( dcd13.state==Dcd_rezerv );
//     }
// 
//     {
//         pDistrict = de_list.get_dcd_esd(L"22");
//         CPPUNIT_ASSERT ( pDistrict );
//         CPPUNIT_ASSERT ( pDistrict->get_name() == L"Jelgava-Krustpils" );
//         const std::vector<CDcdEsd>& vDcd = pDistrict->get_dcd();
//         CPPUNIT_ASSERT ( vDcd.size()==4 );
// 
//         const CDcdEsd& dcd1 = vDcd[1];
//         CPPUNIT_ASSERT ( dcd1.fio==L"Ivanov E.K." );
//         CPPUNIT_ASSERT ( dcd1.gang==2 );
//         CPPUNIT_ASSERT ( dcd1.state==Dcd_ready );
// 
//         const CDcdEsd& dcd5 = vDcd[3];
//         CPPUNIT_ASSERT ( dcd5.fio==L"Kozlov S.D." );
//         CPPUNIT_ASSERT ( dcd5.gang==1 );
//         CPPUNIT_ASSERT ( dcd5.state==Dcd_ready );
// 
//         const std::vector<CStationEsdPtr>& vEsd = pDistrict->get_esd();
// 
//         CPPUNIT_ASSERT ( vEsd.size()==1 );
// 
//         const CStationEsdPtr& stat0 = vEsd[0];
//         CPPUNIT_ASSERT ( stat0->get_esr()==EsrKit(9211) );
//         const std::vector<CDcdEsd>& vEsd0 = stat0->get_esd();
//         CPPUNIT_ASSERT ( vEsd0.size()==3 );
//         const CDcdEsd& dcd00 = vEsd0[0]; 
//         CPPUNIT_ASSERT ( dcd00.fio==L"Petrov A.S." );
//         CPPUNIT_ASSERT ( dcd00.gang==1 );
//         CPPUNIT_ASSERT ( dcd00.state==Dcd_ready );
//         const CDcdEsd& dcd02 = vEsd0[2]; 
//         CPPUNIT_ASSERT ( dcd02.fio==L"Sidorov T.T." );
//         CPPUNIT_ASSERT ( dcd02.gang==3 );
//         CPPUNIT_ASSERT ( dcd02.state==Dcd_ready );
//     }
// 
}