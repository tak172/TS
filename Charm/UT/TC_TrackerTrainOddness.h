#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ����������� �������� ��

class TC_TrackerTrainOddness : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTrainOddness );
	CPPUNIT_TEST( WoOddnessToEvenByTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByShuntSema );
	CPPUNIT_TEST( WoOddnessToOddByTrainSema );
	CPPUNIT_TEST( OddToEvenByTrainSema );
	CPPUNIT_TEST( EvenToOddByTrainSema );
	CPPUNIT_TEST( EvenToEvenByTrainSema );
	CPPUNIT_TEST( OddToOddByTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByOppTrainSema );
	CPPUNIT_TEST( WoOddnessToEvenByOppShuntSema );
	CPPUNIT_TEST( WoOddnessToOddByOppTrainSema );
	CPPUNIT_TEST( ChangeOddnessByStandardSpan );
	CPPUNIT_TEST( ChangeOddnessByNonStandardSpan );
	CPPUNIT_TEST( ChangeOddnessForHouseholdOnSpanToStation );
	CPPUNIT_TEST( UnidentifyForNoHouseholdOnSpanToStation );
	CPPUNIT_TEST_SUITE_END();

	void WoOddnessToEvenByTrainSema(); //����� "��� ��������->������" �� ��������� ���������
	void WoOddnessToEvenByShuntSema(); //����� "��� ��������->������" �� ����������� ���������
	void WoOddnessToOddByTrainSema(); //����� "��� ��������->��������" �� ��������� ���������
	void OddToEvenByTrainSema(); //����� "��������->������" �� ��������� ���������
	void EvenToOddByTrainSema(); //����� "������->��������" �� ��������� ���������
	void EvenToEvenByTrainSema(); //��� ����� �������� "������" �� ��������� ���������
	void OddToOddByTrainSema(); //��� ����� �������� "��������" �� ��������� ���������
	void WoOddnessToEvenByOppTrainSema(); //����� "��� ��������->������" �� ���������� ��������� ���������
	void WoOddnessToEvenByOppShuntSema(); //����� "��� ��������->������" �� ���������� ����������� ���������
	void WoOddnessToOddByOppTrainSema(); //����� "��� ��������->��������" �� ���������� ��������� ���������
	void ChangeOddnessByStandardSpan(); //����� �������� ��� ������ �� ������� �������
	void SpanChangingOddnessCheck( std::wstring fromPlace, const TrainDescr & initDescr, Oddness initOddness, std::wstring toPlace, const TrainDescr & resDescr, 
		Oddness resOddness );
	void ChangeOddnessByNonStandardSpan(); //����� �������� ��� ������ �� ������� �� ����������� � �������������� ���������
	void ThroughEvenTrainSema( Oddness initOddnes, Oddness newOddness, bool withNumber ); //������ ����� ������ ��������
	void ThroughOddTrainSema( Oddness initOddness, Oddness newOddness, bool withNumber ); //������ ����� �������� ��������
	void ChangeOddnessForHouseholdOnSpanToStation(); //����� �������� �������������� ������ ��� ������� ����� ����� �������� � �������� �� �������
	void UnidentifyForNoHouseholdOnSpanToStation(); //����������� ������������� �� �������������� ������ ��� ������� ����� ����� �������� � �������� �� �������
};