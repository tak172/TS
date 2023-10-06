#include "stdafx.h"
#include "TC_UserChartAsoup.h"
#include "../Hem/UserChart.h"
#include "../Guess/Msg.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/EventTrio.h"
#include "../Hem/AsoupEvent.h"
#include "../Asoup/Message.h"
#include "../Hem/AsoupLayer.h"
#include "../helpful/LocoCharacteristics.h"
#include "../helpful/Serialization.h"
#include "../helpful/TopologyDecl.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_UserChartAsoup );

void TC_UserChartAsoup::setUp()
{
	m_userChart.reset(new UserChart());
}

void TC_UserChartAsoup::tearDown()
{
	m_userChart.reset(nullptr);
    LocoCharacteristics::Shutdowner();
}

void TC_UserChartAsoup::AsoupMessage()
{
    const wchar_t *c_1042 = L"(:1042 909/000+11310 2419 1131 039 0950 03 11420 27 01 08 30 00/00 0 0/00 00 0\n"
        L"Þ2 0 00 00 00 00 0000 0 0 042 02429 01647 136 56069321 52600855 000 000 00 028 006.20 001 000 000 60 004 000 000 90 023 006 000 95 012 000 000 96 011 006 000\n"
        L"Þ3 539 00012331 1 06 45 0000 00000 VOLKOVS     \n"
        L"Þ3 539 00012332 9:)";
    CPPUNIT_ASSERT(m_userChart->processNewAsoupText(c_1042, boost::gregorian::date(2015, 6, 6), nullptr, 0));

    attic::a_document userOutDoc("Test");
    attic::a_node userChangesNode = userOutDoc.document_element().append_child("userCh");
    attic::a_node userAnalysisNode = userOutDoc.document_element().append_child("analysis");
    HemHelpful::SpotDescrMap descrMap;
    CPPUNIT_ASSERT(m_userChart->takeChanges(userChangesNode, userAnalysisNode, 0));
 	attic::a_node expectedAsoupLayerNode = userChangesNode.child(AsoupLayer_xAttr);
 	CPPUNIT_ASSERT(expectedAsoupLayerNode);

    size_t count = 0u;
    for (attic::a_node asoupTrioNode : expectedAsoupLayerNode.children(Trio_xTag))
    {
        TrioAsoup expectedTrioAsoup;
        expectedTrioAsoup << asoupTrioNode;
        CPPUNIT_ASSERT(!expectedTrioAsoup.empty());
        ++count;
    }

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned)count);
    CPPUNIT_ASSERT_EQUAL(0u, (unsigned)descrMap.size());

    // ToDo: Âðåìåííàÿ ïðîâåðêà
    CPPUNIT_ASSERT(!userAnalysisNode.first_child());
}

void TC_UserChartAsoup::AsoupUsed()
{
    AsoupLayer layer;
    const BadgeE someBdg( L"xyz", EsrKit(567) );

    HemHelpful::AsoupEventPtr asoupPtr(new AsoupEvent(HCode::DEPARTURE, someBdg, ParkWayKit(), 0, TrainDescr()));
    HemHelpful::SpotEventPtr spotPtr(new SpotEvent(HCode::DEPARTURE, someBdg, 0));
    layer.AddAsoupEvent(asoupPtr);

    {
        attic::a_document doc("Changes");
        layer.TakeChanges(doc.document_element());
        std::wstring text = doc.pretty_wstr();
        CPPUNIT_ASSERT(!text.empty());
    }    

    layer.SetServed(asoupPtr, AsoupLayer::SpotEventAddress(someBdg, 0, HCode::DEPARTURE));

    {
        attic::a_document doc("ReverseChanges");
        layer.GetReverseChanges(doc.document_element());
        std::wstring text = doc.pretty_wstr();
        CPPUNIT_ASSERT(!text.empty());

        // GetFlatContent ïîëó÷àåò äàííûå èç ñëîÿ ÀÑÎÓÏ áåç óêàçàíèÿ çàíÿòîñòè ñîáûòèé ÀÑÎÓÏ
        // TODO: Èñïðàâèòü ýòó îøèáêó
        StringPtr flatContent = layer.GetFlatContent();
        CPPUNIT_ASSERT(!flatContent->empty());
    }

    {
        attic::a_document doc("Changes");
        layer.TakeChanges(doc.document_element());
        std::wstring text = doc.pretty_wstr();
        CPPUNIT_ASSERT(!text.empty());
    }
}


void TC_UserChartAsoup::AsoupDir()
{
	const wchar_t *c_1042 = L"(:1042 909/000+11001 2738 0900 257 1800 01 11420 16 12 02 40 02/04 1 0/00 00 0\n"
		L"Þ2 0 00 00 00 00 0000 0 0 057 01353 00000 228 60023819 63847354 000 000 00 000 057.60 000 057 000\n"
		L"Þ4 11290 2 17 000 008.60 000 008 \n"
		L"Þ4 11290 2 63 000 018.60 000 018 \n"
		L"Þ4 11290 2 83 000 031.60 000 031 \n"
		L"Þ12 00 60023819 1 000 86240 22210 3126 300 00 00 00 00 00 0236 12 60 0600 04 100 18000 00000 11290 83 09000 03 00000 0000 020 1 1020 APEHÄA 128 000 00000000\n"
		L"Þ12 00 61501375 255239255255 0245\n"
		L"Þ12 00 63418974 255239255255 0238\n"
		L"Þ12 00 54152707 255237255247 0237560009901-\n"
		L"Þ12 00 60802626 255237255247 02320600APEHÄA\n"
		L"Þ12 00 63801278 255239255255 0237\n"
		L"Þ12 00 55075899 255237255247 0233560009901-\n"
		L"Þ12 00 56126428 255239255255 0234\n"
		L"Þ12 00 57914145 255239255255 0233\n"
		L"Þ12 00 52869070 255239255255 0240\n"
		L"Þ12 00 63833206 255237255247 02370600APEHÄA\n"
		L"Þ12 00 56305238 255237255247 0236560009901-\n"
		L"Þ12 00 52906971 255239255255 0232\n"
		L"Þ12 00 55348296 199239255255 862202221058600235\n"
		L"Þ12 00 53456661 255239255255 0243\n"
		L"Þ12 00 59079186 255239255255 0233\n"
		L"Þ12 00 52274305 195239247191 64890222105789000023863027\n"
		L"Þ12 00 53429726 195239247191 86220222105860300024183020\n"
		L"Þ12 00 55078885 199239255255 862402221031260231\n"
		L"Þ12 00 61689386 195237247247 648402221086834000235060063APEHÄA\n"
		L"Þ12 00 61687612 255255255255 \n"
		L"Þ12 00 52868684 195237247247 86240222103126300024056008309901-\n"
		L"Þ12 00 63766968 195237247247 648402221086834000238060063APEHÄA\n"
		L"Þ12 00 63766901 255239255255 0237\n"
		L"Þ12 00 60440617 255239255255 0240\n"
		L"Þ12 00 60942067 195255247255 8624022210312630083\n"
		L"Þ12 00 61584074 255239255255 0245\n"
		L"Þ12 00 63738116 195239247255 64840222108683400023863\n"
		L"Þ12 00 61352126 195239255191 648902221057895000235027\n"
		L"Þ12 00 55715502 195237247183 86240222103126300023356008302009901-\n"
		L"Þ12 00 52273257 195255255255 86220222105860000\n"
		L"Þ12 00 55047104 195239247255 20540222104907300024317\n"
		L"Þ12 00 56072127 255239255255 0244\n"
		L"Þ12 00 52998457 195239247255 64890222105789500023263\n"
		L"Þ12 00 63766273 195237255247 6484022210868340002370600APEHÄA\n"
		L"Þ12 00 54903133 255237255247 0235560009901-\n"
		L"Þ12 00 61816567 195237247247 862402221031265000245060083APEHÄA\n"
		L"Þ12 00 56250012 255237255247 0236560009901-\n"
		L"Þ12 00 55007983 255239255255 0235\n"
		L"Þ12 00 61726626 255237255247 02450600APEHÄA\n"
		L"Þ12 00 59520379 195237247247 64840222108683300024056006309901-\n"
		L"Þ12 00 58748054 255255255255 \n"
		L"Þ12 00 63766067 255237255247 02380600APEHÄA\n"
		L"Þ12 00 53756094 195237247247 86240222103126500023256008309901-\n"
		L"Þ12 00 63767602 195237247247 648402221086833000237060063APEHÄA\n"
		L"Þ12 00 61688347 255239255255 0235\n"
		L"Þ12 00 63767750 255239255255 0238\n"
		L"Þ12 00 55076673 195237247247 86240222103126500023256008309901-\n"
		L"Þ12 00 59403972 255239255255 0237\n"
		L"Þ12 00 60615770 195237247247 648402221086833000233060063APEHÄA\n"
		L"Þ12 00 52858339 195237247247 20540222104907000024256001709901-\n"
		L"Þ12 00 52989852 255239255255 0234\n"
		L"Þ12 00 58372657 199239247255 86220222105860023283\n"
		L"Þ12 00 54105747 195239245247 205401611249075000243170109439 \n"
		L"Þ12 00 58057639 255239255255 0241\n"
		L"Þ12 00 56121643 255239255255 0243\n"
		L"Þ12 00 63847354 255237255255 02390600:)";

	std::wostringstream logString;
	std::string serialization = Nsi::serializeTopology(L"C:\\ReTime\\Charm\\logics\\Topology.stn", L"C:\\ReTime\\Charm\\logics\\Direct_ASOUP.stn", L"C:\\ReTime\\Charm\\logics\\TopologyBlacklist.stn", logString );
	attic::a_document tempDoc;
	if (attic::a_parse_result parse = tempDoc.load_utf8(serialization))
	{
		auto topology = deserialize<Topology>(tempDoc.to_str());
		m_userChart->setupTopology(topology);
		CPPUNIT_ASSERT(m_userChart->processNewAsoupText(c_1042, boost::gregorian::date(2021, 12, 16), nullptr, 0));
	}

}
