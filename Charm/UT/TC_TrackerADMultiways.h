#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ��������� ������� ��� ������� ������������� �/� �����

class TC_TrackerADMultiways : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerADMultiways );
	CPPUNIT_TEST( ArriveTransit );
	CPPUNIT_TEST( TransitTransit );
	CPPUNIT_TEST( TransitArrive );
	CPPUNIT_TEST( PassArrive );
	CPPUNIT_TEST( PassTransit );
	CPPUNIT_TEST( ArrivePass );
	CPPUNIT_TEST( TransitPass );
	CPPUNIT_TEST( ReversiveTransitTransit );
	CPPUNIT_TEST( ReversiveDepartTransit );
	CPPUNIT_TEST( MultiArrdepEventsOnUnstableSignal );
	CPPUNIT_TEST( MultiWayBornAndGoSpan );
	CPPUNIT_TEST( UpgradeOnTwoADWaysAndLeave );
	CPPUNIT_TEST( ComplexWayBornAndGoOut );
	CPPUNIT_TEST( OutFromADWayWithMidsema );
	CPPUNIT_TEST( GoingThroughADWayWithMidsema1 );
	CPPUNIT_TEST( GoingThroughADWayWithMidsema2 );
	CPPUNIT_TEST( ExcessiveDeparture );
	CPPUNIT_TEST_SUITE_END();

	void ArriveTransit(); //�������� �� ������ ����. ������������� ����������
	void TransitTransit(); //������������� ����� �����
	void ReversiveTransitTransit(); //������������� ����� ����� (� �������� �����������)
	void ReversiveDepartTransit(); //����������� � ������������� (� �������� �����������)
	void TransitArrive(); //������������� ������� ����. �������� �� ���������
	void PassArrive(); //������ �� �������� �������� ������� ����. �������� �� ���������
	void PassTransit(); //������ �� �������� �������� ������� ����. ������������� ����������
	void ArrivePass(); //�������� �� ������ ����. ������ �� �������� �������� ������� ����
	void TransitPass(); //������������� ������� ����. ������ �� �������� �������� ������� ����
	void MultiArrdepEventsOnUnstableSignal(); //������������� ������� ��������/����������� �� ������������ (��������) �������
	void MultiWayBornAndGoSpan(); //������������� �� ��������� �/� ����� � ����� �� �������
	void UpgradeOnTwoADWaysAndLeave(); //�������� ������ �� ��, ������������� �� ���� �/� �����, � ����� � ������ �� �/� �����
	void ComplexWayBornAndGoOut(); //�������� ������ �� ��������� �/� ���� � ����� � ����
	void OutFromADWayWithMidsema(); //����� � ���������� �/� ���� �� ���������� �����������
	void GoingThroughADWayWithMidsema1(); //������������� ���������� �/� ���� �� ���������� ����������� (������� 1)
	void GoingThroughADWayWithMidsema2(); //������������� ���������� �/� ���� �� ���������� ����������� (������� 2)
	void ExcessiveDeparture(); //���������� ����������� �� ������ ������ ��

	typedef TrackerInfrastructure::TIActivity TIActivity;
	//������ ����� ��� ���� (� ���������� ������������)
	std::vector <TIActivity> goodWaysPassingActs;
	void MakeActivities();
	void SetPreMoveActivity( TIActivity preMoveActivity ) { goodWaysPassingActs[0] = preMoveActivity; }
	void SetSemaActivities( TIActivity firstSemaActivity, TIActivity secondSemaActivity )
	{
		goodWaysPassingActs[2] = firstSemaActivity;
		goodWaysPassingActs[4] = secondSemaActivity;
	}
};