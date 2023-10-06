#ifndef TC_AUTOCHARTHAPPEN_H
#define TC_AUTOCHARTHAPPEN_H

#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"

//������������ AutoChart.HappenLayer

class TestTracker;
class HemEvent;
class Chart;
class AutoChartTest;
class TC_AutoChartHappen : 
	private TC_ChartImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	TC_AutoChartHappen();
	~TC_AutoChartHappen();

private:
	TrackerInfrastructure & TI;
	boost::scoped_ptr<AutoChartTest> autoChart;
	attic::a_document xml_doc;
	CPPUNIT_TEST_SUITE( TC_AutoChartHappen );
	CPPUNIT_TEST( StationEntry );
	CPPUNIT_TEST( StationExit );
	CPPUNIT_TEST( SpanMove );
	CPPUNIT_TEST( StartValidation );
	CPPUNIT_TEST( PocketTransition );
	CPPUNIT_TEST( PocketExit );
	CPPUNIT_TEST( InvalidForm );
	CPPUNIT_TEST( Recovery );
    //CPPUNIT_TEST( InfoChanging );
    CPPUNIT_TEST( EntranceStopping );
    CPPUNIT_TEST( SpanToSpanTransit );
    CPPUNIT_TEST( ReduceAmount );
	CPPUNIT_TEST( StatToStatWOArrdep );
	CPPUNIT_TEST( TwoEventsValidness );
    CPPUNIT_TEST( SafelyDiscard );
	//CPPUNIT_TEST( MultiInfoChanges );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StationEntry(); //�������� ����� �� �������
	void StationExit(); //�������� ������ �� �������
	void SpanMove(); //�������� �� ��������
	void StartValidation(); //�������� ������ ���� � �������� �������
	void PocketTransition(); //������ "�������"
	void PocketExit(); //����� �� "�������" ������ ������
	void InvalidForm(); //������������ ������������ ������
	void Recovery(); //�������������� ������ ����� ����������
    void InfoChanging(); //��������� ���������� �� ����
    void EntranceStopping(); //������� ����� ������� ����������
    void SpanToSpanTransit(); //������������� �� �������� �� ��������
    void ReduceAmount(); // �������� ������ �����
	void StatToStatWOArrdep(); //������������� �� ������� �� ������� ��� ������� �/� �����
	void TwoEventsValidness(); //���������� ������������������ ���� �������
	void MultiInfoChanges(); //������������� ��������� �������� ���������� � ����� ����� � ���� �����
    void SafelyDiscard();// ������� �������������� ����
	//��������� �� ������� ��� ���������� ���� �� ������:
	bool PermittedAsSequence( const std::pair <HCode, BadgeE> &,
		const std::pair <HCode, BadgeE> &,
		time_t momtime );
};
#endif // TC_AUTOCHARTHAPPEN_H