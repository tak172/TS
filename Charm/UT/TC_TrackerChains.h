#ifndef TC_TRACKERCHAINS_H
#define TC_TRACKERCHAINS_H

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� �������� ���������� ������� ��������� �� ������� Guess (Tracker) ��� �� ���������, ������� � ����������

class TestTracker;
class TrainField;
class TC_TrackerChains : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TrackerChains );
	CPPUNIT_TEST( AppearanceScenario );
	CPPUNIT_TEST( MergingScenario );
	CPPUNIT_TEST( LongMergingScenario );
	CPPUNIT_TEST( TripleSwitchScenario );
	CPPUNIT_TEST( SpanToStatScenario );
	CPPUNIT_TEST( SplittingScenario );
	CPPUNIT_TEST( SwitchToggling );
	CPPUNIT_TEST( IncorrectChainsMergingOnForcedLocation );
	CPPUNIT_TEST_SUITE_END();

public:
	TC_TrackerChains();
	virtual void setUp();
	virtual void tearDown();

protected:
	void AppearanceScenario(); //�������� ��������� ������� (��������� ����� �� ������)
	void MergingScenario(); //�������� ������� �������
	void LongMergingScenario(); //�������� ������� ������� (����� ������ �������) �������
	void TripleSwitchScenario(); //�������� ���������� ������� ����� ��� �������
	void SpanToStatScenario(); //�������� ������� ������� �� �������� � �������
	void SplittingScenario(); //�������� ��������� �������
	void SwitchToggling(); //������� ������� � �������� �������
	void IncorrectChainsMergingOnForcedLocation(); //��������� ������� �� ����� ������������� ��������� ��

private:
	//���������� ��� "����" ������� (TFReversed ��������� � �������� ������� ( ���������� ��������� � ��������� ������ � ����� ����� )
	TrainFieldPtr TFReverted;
	void CheckUniqueExistence( BadgeE bdg_array[], unsigned int array_size ) const;
};
#endif // TC_TRACKERCHAINS_H