#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/UncontrolledStations.h"
#include "UtHemHelper.h"

class HappenLayer;
class AsoupLayer;
class TopologyTest;
class EsrGuide;
class UtUserChart;

namespace Regulatory
{
class Layer;
}
namespace Hem
{
    class GuessTransciever;
};

class TC_AsoupToSpot : public CPPUNIT_NS::TestFixture
{
public:
    TC_AsoupToSpot(void);
    ~TC_AsoupToSpot(void);

    CPPUNIT_TEST_SUITE( TC_AsoupToSpot );
    CPPUNIT_TEST( identify );
    CPPUNIT_TEST( identifyChaining );
    CPPUNIT_TEST( identifyDeparture );
    CPPUNIT_TEST( identifyArrival );
    CPPUNIT_TEST( unlink );
    CPPUNIT_TEST( raiiUnlinker );
    // CPPUNIT_TEST( raiiUnlinkerDouble );
    CPPUNIT_TEST( gluingPathesOnSameStation );
    CPPUNIT_TEST( gluingPathesTerminalNearSpan );
    CPPUNIT_TEST( dontGluingPairedWithoutIndexOnSameStation );
    CPPUNIT_TEST( linkIdentifiedPathFromSpan );
    CPPUNIT_TEST( repeatlyIdentify );
    CPPUNIT_TEST( IdentifyWithPassagirArrivalMessage );
    CPPUNIT_TEST( identifyDepartureWithChangeFeature );
    CPPUNIT_TEST( IdentifyWithPassagirArrivalMessage2 );
    CPPUNIT_TEST( IdentifyWithSynonym );
    CPPUNIT_TEST( IdentifyByDeparture );
    CPPUNIT_TEST( IdentifyByDepartureIncludeInfoChanging );
    CPPUNIT_TEST( IdentifyByArrival );
    CPPUNIT_TEST( IdentifyWithPassagirArrivalMessage_5522 );
    CPPUNIT_TEST( AttachPathWithSingleForm );
    CPPUNIT_TEST( SkipGluingPathesOnSameStation );
    CPPUNIT_TEST( RelinkAsoupAfterGluingPathes );
    CPPUNIT_TEST( IdentifyByArrival_5823 );
    CPPUNIT_TEST( LinkAsoupByTime_6071 );
    CPPUNIT_TEST( IdentifyWithSaveCrewInfo_6139 );
    CPPUNIT_TEST( IdentifyLaterAsoup_6149 );
    CPPUNIT_TEST( IdentifyWithChangeLitersDTN_5208 );
	CPPUNIT_TEST( LinkInfoChangingAndDepartureDifNumber_7050 );

    CPPUNIT_TEST_SUITE_END();

private:
    void setUp() override;
    void tearDown() override;

    void identify(); // ������� ���� ������ identify
    void identifyChaining(); // ������������� ������ ��������� ����� ����� �������������
    void identifyDeparture(); // ������������� ���� �������� ����� ��� ����������� �� �������
    void identifyArrival(); // ������������� ���� �������� ����� �� �������� �� �������
    void unlink(); // ���� ����������� ����� ����� ����� � ������ �������
    void raiiUnlinker(); // ���� ���������������� ������, ������������� ����� �� ����� ��� �������� ��������������� ����������

	void LinkFreeAsoup(std::shared_ptr<const AsoupEvent> asoup, UtUserChart &m_chart, time_t currentTime);

	void raiiUnlinkerDouble();
    void gluingPathesOnSameStation(); // ������� ����������� ����� (������ � �������� ����� �������)
    void gluingPathesTerminalNearSpan(); // ������� ����������� ����� (������� � ���������� �������)
    void dontGluingPairedWithoutIndexOnSameStation();//��������� ������ ����� ��� ������� (������ �� ����� �������)
    void linkIdentifiedPathFromSpan();
    void repeatlyIdentify(); // ��������� ���������������� ������������� ����� �� ����� ��������� ����
    void IdentifyWithPassagirArrivalMessage(); //������������� ���������� � ������������ �� PV LDZ
    void identifyDepartureWithChangeFeature(); //������������� �� ������ ����� ��
    void IdentifyWithPassagirArrivalMessage2(); //������������� ���������� � ������������ �� PV LDZ
    void IdentifyWithSynonym();
    void IdentifyByDeparture();
    void IdentifyByDepartureIncludeInfoChanging();
    void IdentifyByArrival();
    void IdentifyWithPassagirArrivalMessage_5522();
    void AttachPathWithSingleForm();
    void SkipGluingPathesOnSameStation();
    void RelinkAsoupAfterGluingPathes();
    void IdentifyByArrival_5823();
    void LinkAsoupByTime_6071();
    void IdentifyWithSaveCrewInfo_6139();
    void IdentifyLaterAsoup_6149();
    void IdentifyWithChangeLitersDTN_5208();
	void LinkInfoChangingAndDepartureDifNumber_7050();


    std::unique_ptr<HappenLayer> m_happenLayer;
    std::unique_ptr<AsoupLayer> m_asoupLayer;
    std::shared_ptr<Regulatory::Layer> m_regular;
    std::shared_ptr<EsrGuide> esrGuide;
    std::shared_ptr<TopologyTest> topology;
    std::shared_ptr<Hem::GuessTransciever> guessTransciever;
	DistrictGuide dg;
    UncontrolledStations uncontrolledStations;
    UtIdentifyCategoryList identifyCategory;
};

