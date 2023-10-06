#pragma once


#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/RSensorsSerializable.h"
#include "../Rokit/RemoteViewer.h"
#include "../Rokit/BaseSensor.h"


class TC_Rokit : public CPPUNIT_NS::TestFixture
{

	CPPUNIT_TEST_SUITE(TC_Rokit);
	CPPUNIT_TEST(Activity);
	CPPUNIT_TEST(Consumption);
	CPPUNIT_TEST(ChangeValue);
	CPPUNIT_TEST(StaticValue);
	CPPUNIT_TEST(FillSensors);
	CPPUNIT_TEST_SUITE_END();

public:

	virtual void setUp();
	virtual void tearDown();

public:

	void Activity();
	void Consumption();
	void ChangeValue();
	void StaticValue();
	void FillSensors();
	void PushMessageToRemoteViewer(const std::string& message, RemoteViewer& remoteViewer);
	void PushMessageToSensor(const std::string& message, BaseSensor& sensor);
};