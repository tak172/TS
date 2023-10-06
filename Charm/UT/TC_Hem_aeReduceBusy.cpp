#include "stdafx.h"
#include <boost/date_time/posix_time/time_parsers.hpp>
#include "TC_Hem_aeReduceBusy.h"
#include "UtHemHelper.h"
#include "../Hem/Appliers/aeReduceBusy.h"
#include "../helpful/Time_Iso.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Hem_aeReduceBusy );

void TC_Hem_aeReduceBusy::onlyDeleteClosed()
{
    attic::a_document doc;
    doc.load_wide(
        L"<Test>"
        L"  <BusyLayer>"
        L"    <BusyPath>" // нить из единственного события нельзя удалять - она же не закрыта
        L"      <BusyEvent create_time='20160906T070000Z' name='WayOccupy' Bdg='10CA[11420]' waynum='10' parknum='1' />"
        L"    </BusyPath>"
        L"    <BusyPath>"
        L"      <BusyEvent create_time='20160906T073030Z' name='WayOccupy' Bdg='11C[09010]' waynum='20' parknum='2' />"
        L"      <BusyEvent create_time='20160906T074040Z' name='WayEmpty'  Bdg='11C[09010]' waynum='20' parknum='2' />"
        L"    </BusyPath>"
        L"    <BusyPath>"
        L"      <BusyEvent create_time='20160906T071111Z' name='WayOccupy' Bdg='10C[09010]' waynum='30' parknum='3' />"
        L"      <BusyEvent create_time='20160911T072222Z' name='WayEmpty'  Bdg='10C[09010]' waynum='30' parknum='3' />"
        L"    </BusyPath>"
        L"  </BusyLayer>"
        L"</Test>"
        );
    UtLayer<Busy::Layer> busy_layer;
    doc.document_element() >> (Busy::Layer&)busy_layer;

    CPPUNIT_ASSERT( busy_layer.path_count() == 3 );
    CPPUNIT_ASSERT( busy_layer.exist_event("@waynum=10") );
    CPPUNIT_ASSERT( busy_layer.exist_event("@waynum=20") );
    CPPUNIT_ASSERT( busy_layer.exist_event("@waynum=30") );

    time_t bound = time_from_iso("20160910T100000Z");
    Hem::aeReduceBusy reducer( busy_layer, bound );
    reducer.Action();

    CPPUNIT_ASSERT( busy_layer.path_count() == 2 );
    CPPUNIT_ASSERT( busy_layer.exist_event("@waynum=10") );
    CPPUNIT_ASSERT( !busy_layer.exist_event("@waynum=20") );
    CPPUNIT_ASSERT( busy_layer.exist_event("@waynum=30") );
}
