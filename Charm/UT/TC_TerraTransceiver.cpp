#include "stdafx.h"

#include "TC_TerraTransceiver.h"
#include "../Hem/TerraTransceiver.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TerraTransceiver );

void TC_TerraTransceiver::checkBusy()
{
    // подготовка
    std::string etalon;
    {
        attic::a_document doc;
        doc.load_wide( 
            L"<FundEvents>"
            L"    <Trio layer='busy'>"
            L"        <Body create_time='20131003T120111Z' name='WayOccupy' Bdg='1Ap[33333]' />"
            L"    </Trio>"
            L"    <Trio layer='busy'>"
            L"        <Pre  create_time='20131003T120111Z' name='WayOccupy'  Bdg='1Ap[33333]' />"
            L"        <Body create_time='20131003T120444Z' name='WayEmpty'  Bdg='1xx[33333]' />"
            L"    </Trio>"
            L"</FundEvents>" );
        etalon = doc.to_str();
    }
    attic::a_document src;
    {
        src.load_wide( 
            L"<FundEvents>"
            L"    <BusyEvent create_time='20131003T120111Z' name='WayOccupy' Bdg='1Ap[33333]' />"
            L"    <BusyEvent create_time='20131003T120222Z' name='WayOccupy' Bdg='1xx[33333]' />"
            L"    <BusyEvent create_time='20131003T120333Z' name='WayEmpty'  Bdg='1Ap[33333]' />"
            L"    <BusyEvent create_time='20131003T120444Z' name='WayEmpty'  Bdg='1xx[33333]' />"
            L"</FundEvents>" );
    }

    // проверка
    attic::a_document out_doc;
    TerraTransceiver trans;
    trans.PreChart( src.document_element(), out_doc );

	// сравниваем результаты
	auto result = out_doc.to_str();
    auto mismresult = mismatch( etalon.cbegin(), etalon.cend(), result.cbegin(), result.cend() );

	CPPUNIT_ASSERT( etalon == result );
}

void TC_TerraTransceiver::checkSilence()
{
    // подготовка
    std::string etalon;
    {
        attic::a_document doc;
        doc.load_wide( 
            L"<FundEvents>"
            L"    <Trio layer='silence'>"
            L"        <Body create_time='20131003T120111Z' name='Cutoff' Bdg='aa[33333]' />"
            L"    </Trio>"
            L"    <Trio layer='silence'>"
            L"        <Pre  create_time='20131003T120111Z' name='Cutoff' Bdg='aa[33333]' />"
            L"        <Body create_time='20131003T120222Z' name='Cutin'  Bdg='bb[33333]' />"
            L"    </Trio>"
            L"    <Trio layer='silence'>"
            L"        <Body create_time='20131003T120333Z' name='Cutoff' Bdg='cc[44444]' />"
            L"    </Trio>"
            L"</FundEvents>" );
        etalon = doc.to_str();
    }
    attic::a_document src;
    {
        src.load_wide( 
            L"<FundEvents>"
            L"    <SilenceEvent create_time='20131003T120111Z' name='Cutoff' Bdg='aa[33333]' />"
            L"    <SilenceEvent create_time='20131003T120222Z' name='Cutin'  Bdg='bb[33333]' />"
            L"    <SilenceEvent create_time='20131003T120333Z' name='Cutoff' Bdg='cc[44444]' />"
            L"</FundEvents>" );
    }

    // проверка
    attic::a_document out_doc;
    TerraTransceiver trans;
    trans.PreChart( src.document_element(), out_doc );

    // сравниваем результаты
    auto result = out_doc.to_str();
    auto mismresult = mismatch( etalon.cbegin(), etalon.cend(), result.cbegin(), result.cend() );

    CPPUNIT_ASSERT( etalon == result );
}

