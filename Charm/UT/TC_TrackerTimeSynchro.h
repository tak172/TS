#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//������� ��������� �������� � �������������

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerTimeSynchro : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();
	virtual void tearDown();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTimeSynchro );
	CPPUNIT_TEST( BoundaryStripBlinking );
	CPPUNIT_TEST( TrainIdentification );
	CPPUNIT_TEST( DislokTimeoutBusy );
	CPPUNIT_TEST( DislokBusyTimeout );
	CPPUNIT_TEST( DislokBusy );
	CPPUNIT_TEST( BusyDislok );
	CPPUNIT_TEST( BusyDislok2 );
	CPPUNIT_TEST( BusyNoextDislok );
	CPPUNIT_TEST( BusyDislokFreeBusyTimeout );
	CPPUNIT_TEST( ExtDislokBusy );
    CPPUNIT_TEST( TestRepeatValue );
	CPPUNIT_TEST( HemDisformDelay );

	CPPUNIT_TEST( GlobalDelayVersusParkWayDelay );
	CPPUNIT_TEST( GlobalDelayVersusOnlyWayDelay );
	CPPUNIT_TEST( GlobalDelayVersusOnlyParkDelay );
	CPPUNIT_TEST( GlobalDelayVersusStationDelay );
	CPPUNIT_TEST( ParkWayDelayVersusOnlyParkDelay );
	CPPUNIT_TEST( ParkWayDelayVersusOnlyWayDelay );

	CPPUNIT_TEST_SUITE_END();

	void BoundaryStripBlinking(); //������� ������� �� ������� ������� � ��������
	void TrainIdentification(); //������������� ������ � ������ �������� ����� ���������� ��������

	//������������� ����� ��������� ��������� ������� ���������� ��������� �������: ������� ������������� (Dislok),
	//��������� �������� ������������� �� ��������� ������� (Timeout), ������� ��������� (Busy) � ������� ����������� (Free)
	void DislokTimeoutBusy();
	void DislokBusyTimeout();
	void DislokBusy();
	void BusyDislok();
	void BusyDislok2();
	void BusyNoextDislok();
	void BusyDislokFreeBusyTimeout();
	void ExtDislokBusy();
	void TestRepeatValue();     // ���������� ��������� ������ �� ������ ����������� ���������
	void HemDisformDelay(); //�������� ��������������� �� ������ ����������
	void GlobalDelayVersusParkWayDelay(); //����� ����� ���������� ��������� � ��������� �� ������������ ���� � ����
	void GlobalDelayVersusOnlyWayDelay(); //����� ����� ���������� ��������� � ��������� �� ������������ ���� (��� �������� �����)
	void GlobalDelayVersusOnlyParkDelay(); //����� ����� ���������� ��������� � ��������� �� ������������ ���� (��� �������� ����)
	void GlobalDelayVersusStationDelay(); //����� ����� ���������� ��������� � ��������� �� ������������ �������
	void ParkWayDelayVersusOnlyParkDelay(); //����� ����� ��������� �� ������������ ���� � ���� � ��������� �� ������������ ���� (��� �������� ����)
	void ParkWayDelayVersusOnlyWayDelay(); //����� ����� ��������� �� ������������ ���� � ���� � ��������� �� ������������ ���� (��� �������� �����)

	bool IdentifiedAsTrain( const BadgeE &, const TrainDescr & ) const;
};