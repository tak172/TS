#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//ѕоведение поездов на тех.узлах

class TC_TrackerTechnodes : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerTechnodes );
	CPPUNIT_TEST( CargoIntoTechnode );
	CPPUNIT_TEST( CargoIntoTechnode2 );
	CPPUNIT_TEST( CargoOnTechnodeInput );
	CPPUNIT_TEST( CargoOnTechnodeOutput );
	CPPUNIT_TEST( CargoOnTechnodeOutput2 );
	CPPUNIT_TEST( CargoRightdirRoutedDeparture );
	CPPUNIT_TEST( CargoRightdirRoutedDeparture2 );
	CPPUNIT_TEST( CargoWrongdirRoutedDeparture );
	CPPUNIT_TEST( CargoRightdirUnroutedDeparture );
	CPPUNIT_TEST( CargoWrongdirUnroutedDeparture );
	CPPUNIT_TEST( DisformOnTechnodeWOOddnessIgnore );
	CPPUNIT_TEST( HouseholdIntoTechnode );
	CPPUNIT_TEST( HouseholdIntoTechnode2 );
	CPPUNIT_TEST( HouseholdOnTechnodeInput );
	CPPUNIT_TEST( HouseholdOnTechnodeOutput );
	CPPUNIT_TEST( HouseholdOnTechnodeOutput2 );
	CPPUNIT_TEST( HouseholdRightdirRoutedDeparture );
	CPPUNIT_TEST( HouseholdWrongdirRoutedDeparture );
	CPPUNIT_TEST( HouseholdRightdirUnroutedDeparture );
	CPPUNIT_TEST( HouseholdWrongdirUnroutedDeparture );
	CPPUNIT_TEST( HouseholdADInputOnTechnode );
	CPPUNIT_TEST( DeparturingWhileTailInput );
	CPPUNIT_TEST_SUITE_END();

	//грузовые
	void CargoIntoTechnode(); //проезд через чужой светофор на тех.узле с признаком игнорировани€ четности
	void CargoIntoTechnode2();
	void CargoOnTechnodeInput(); //въезд на тех.узел
	void CargoOnTechnodeOutput(); //выезд с тех.узла
	void CargoOnTechnodeOutput2();
	void CargoRightdirRoutedDeparture(); //отправление с п/о пути по заданному маршруту в правильном направлении
	void CargoRightdirRoutedDeparture2();
	void CargoWrongdirRoutedDeparture(); //отправление с п/о пути по заданному маршруту в неправильном направлении
	void CargoRightdirUnroutedDeparture(); //отправление с п/о пути без маршрута в правильном направлении
	void CargoWrongdirUnroutedDeparture(); //отправление с п/о пути без маршрута в неправильном направлении
	void DisformOnTechnodeWOOddnessIgnore(); //проезд через чужой светофор на тех.узле без признака игнорировани€ четности

	//хоз€йственные
	void HouseholdIntoTechnode();
	void HouseholdIntoTechnode2();
	void HouseholdOnTechnodeInput();
	void HouseholdOnTechnodeOutput();
	void HouseholdOnTechnodeOutput2();
	void HouseholdRightdirRoutedDeparture();
	void HouseholdWrongdirRoutedDeparture();
	void HouseholdRightdirUnroutedDeparture();
	void HouseholdWrongdirUnroutedDeparture();
	void HouseholdADInputOnTechnode(); //обычный въезд хоз€йственного на путь

	void DeparturingWhileTailInput(); //стандартное отправление поезда с одновременным въездом в хвост маневровой
};