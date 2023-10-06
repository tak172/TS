#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrackerGUral : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerGUral );
	CPPUNIT_TEST( ApplyGUInfo ); //������� ��������� ������, ����� ��������� ������
	CPPUNIT_TEST( NumberRepet ); //��������� ���������� ������� �� ������ ������ (������ �����������)
	//CPPUNIT_TEST( NumberRepetIndexNoRepet ); //��������� ���������� ������� � ������ �������� �� ������ ������ (����������������, �.�. ���� ������ ����������� ������ �� ������)
	CPPUNIT_TEST( NumberRepetIndexRepet ); //��������� ���������� ������� � �������� �� ������ ������
	CPPUNIT_TEST_SUITE_END();

	void ApplyGUInfo();
	void NumberRepet();
	void NumberRepetIndexNoRepet();
	void NumberRepetIndexRepet();
	TrainDescr StoreTrainInfo( unsigned int tnumber, std::wstring tindex, std::wstring place, time_t moment );
	bool TrainAtPlace( const TrainDescr &, std::wstring ) const;
};