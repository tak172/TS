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
	CPPUNIT_TEST( ApplyGUInfo ); //сначала появление поезда, потом появление номера
	CPPUNIT_TEST( NumberRepet ); //наложение одинаковых номеров на разные поезда (индекс отсутствует)
	//CPPUNIT_TEST( NumberRepetIndexNoRepet ); //наложение одинаковых номеров и разных индексов на разные поезда (закомментировано, т.к. пока поезда различаются только по номеру)
	CPPUNIT_TEST( NumberRepetIndexRepet ); //наложение одинаковых номеров и индексов на разные поезда
	CPPUNIT_TEST_SUITE_END();

	void ApplyGUInfo();
	void NumberRepet();
	void NumberRepetIndexNoRepet();
	void NumberRepetIndexRepet();
	TrainDescr StoreTrainInfo( unsigned int tnumber, std::wstring tindex, std::wstring place, time_t moment );
	bool TrainAtPlace( const TrainDescr &, std::wstring ) const;
};