#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerTrainDisappearance : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainDisappearance );
	CPPUNIT_TEST( TwinklingDisappearance );
	CPPUNIT_TEST( RealDisappearance );
	CPPUNIT_TEST( HeterogeneousDisappearance );
	CPPUNIT_TEST( DisapperanceAndTeleport );
	CPPUNIT_TEST( DisappearanceVsSplitting );
	CPPUNIT_TEST( LengthyDisapperance );
	CPPUNIT_TEST( DisappearanceAndRecoverNear );
	CPPUNIT_TEST( DeadEndDisappearance );
	CPPUNIT_TEST( DisappearanceVsTeleportToTrain );
	CPPUNIT_TEST( DisappearanceVsTeleportToNoTrain );
	CPPUNIT_TEST_SUITE_END();

	void TwinklingDisappearance(); //������������������ ������������ (� ������ ����������� ���������) � ��������������� �������� ��������
	void RealDisappearance(); //�������������� ������������ ������
	void HeterogeneousDisappearance(); //����������� ������������ �������
	void DisapperanceAndTeleport(); //������ ������ (�� ���������� �����) ����� ��� ������������
	void LengthyDisapperance(); //������ ������������ �� (�� ������������� ���������� ��������)
	void DisappearanceVsSplitting(); //������������ ��, ������� �� ���������
	void DisappearanceAndRecoverNear(); //������������ ������ � ���� �������� � ����������� ���������������
	void DeadEndDisappearance(); //������������ ����� �������
	void DisappearanceVsTeleportToTrain(); //��� ���������� ������ ����� � ����������� ����� ����� ������� ������ ������� �������� �� ����� ��������� ������ ��������
	void DisappearanceVsTeleportToNoTrain(); //��� ���������� ������ ����� � ����������� ����� ����� ������� ���������� �������� �� �� ����� ��������
};