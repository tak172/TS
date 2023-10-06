#include "stdafx.h"
#include "TC_Rokit.h"
#include "cppunit\TestAssert.h"
#include "..\Rokit\RokitRegularString.h"
#include "..\Rokit\SensorActivity.h"
#include "..\Rokit\SensorConsumption.h"
#include "..\Rokit\SensorChangeValue.h"
#include "..\Rokit\SensorStaticValue.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Rokit );

void TC_Rokit::setUp()
{
	
}

void TC_Rokit::tearDown()
{
	
}

void TC_Rokit::FillSensors()
{
	std::string message01 = 
		"<application>"
		"<sensor key=\"Error\" type=\"Error\"/>"
		"<sensor key=\"Activity\" type=\"Activity\" interval=\"eeee\"/>"
		"</application>";

	std::string message02 = 
		"<application>"
		"<sensor key=\"Consumption\" type=\"Consumption\" normal=\"0\" warning=\"1024\" danger=\"2048\">eeee</sensor>"
		"<sensor key=\"ChangeValue\" type=\"ChangeValue\" equal=\"dec\">5005</sensor>"
		"</application>";

	std::string message03 = 
		"<application>"
		"<sensor key=\"StaticValue\" type=\"StaticValue\">1</sensor>"
		"</application>";
	
	std::string message04 = 
		"<application>"
		"<sensor key=\"Activity\" type=\"Activity\" interval=\"1000\"/>"
		"<sensor key=\"Consumption\" type=\"Consumption\" normal=\"256\" warning=\"1024\" danger=\"2048\">512</sensor>"
		"<sensor key=\"ChangeValue\" type=\"ChangeValue\" equal=\"inc\">5000</sensor>"
		"<sensor key=\"StaticValue\" type=\"StaticValue\">0</sensor>"
		"</application>";

	std::string message05 = 
		"<application>"
		"<sensor key=\"StaticValue\" type=\"Remove\"/sensor>"
		"</application>";

	RemoteViewer remoteViewer = RemoteViewer(L"TC_Rokit");
	PushMessageToRemoteViewer(message01, remoteViewer);
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 1);
	PushMessageToRemoteViewer(message02, remoteViewer);
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 3);
	PushMessageToRemoteViewer(message03, remoteViewer);
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 4);
	PushMessageToRemoteViewer(message04, remoteViewer);
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 4);
	PushMessageToRemoteViewer(message05, remoteViewer);
	CPPUNIT_ASSERT(remoteViewer.GetTrashSensors().size() == 1);
	remoteViewer.EraseTrash();
	CPPUNIT_ASSERT(remoteViewer.GetTrashSensors().size() == 0);
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 3);
	remoteViewer.EraseSensors();
	CPPUNIT_ASSERT(remoteViewer.GetSensors().size() == 0);
}

void TC_Rokit::Activity()
{
	const std::string incorrectMessage = "<sensor key=\"Activity\" type=\"Activity\"/>";
	std::function<std::string(int, std::wstring)> generateMessage = [](int interval, std::wstring value) {
		SensorActivitySerializable activity = SensorActivitySerializable(L"Activity", interval, value);
		return activity.Serialize();
	};
	boost::posix_time::ptime currentTimeStamp(boost::date_time::not_a_date_time);
	SensorActivity sensorActivity = SensorActivity([&currentTimeStamp]() { 
		return currentTimeStamp;
	} );
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::disabled);
	currentTimeStamp = boost::posix_time::ptime(boost::gregorian::date(2000, 1, 1), boost::posix_time::time_duration(0, 0, 0));
	PushMessageToSensor(generateMessage(1000, L"1000"), sensorActivity);
	currentTimeStamp += boost::posix_time::milliseconds(100);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::normal);
	currentTimeStamp += boost::posix_time::milliseconds(500);
	PushMessageToSensor(generateMessage(1000, L"1000"), sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::warning);
	currentTimeStamp += boost::posix_time::milliseconds(1000);
	PushMessageToSensor(generateMessage(1000, L"1000"), sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::danger);
	PushMessageToSensor(generateMessage(1000, L"1001"), sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::normal);
	currentTimeStamp = boost::posix_time::ptime(boost::gregorian::date(2000, 1, 1), boost::posix_time::time_duration(0, 0, 0));
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::danger);
	currentTimeStamp += boost::posix_time::milliseconds(100);
	PushMessageToSensor(generateMessage(1000, L"1002"), sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::normal);
	currentTimeStamp += boost::posix_time::milliseconds(100);
	PushMessageToSensor(generateMessage(1000, L"1003"), sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::normal);
	currentTimeStamp += boost::posix_time::milliseconds(800);
	PushMessageToSensor(incorrectMessage, sensorActivity);
	CPPUNIT_ASSERT(sensorActivity.GetLevel() == BaseSensor::State::disabled);
}

void TC_Rokit::Consumption()
{
	std::function<std::string(float, float, float, float)> generateMessage = [](float normal, float warning, float danger, float value) {
		SensorConsumptionSerializable consumption = SensorConsumptionSerializable(L"Consumption", normal, warning, danger, value);
		return consumption.Serialize();
	};

	std::string incorrectMessage = "<sensor key=\"Consumption\" type=\"Consumption\">eee</sensor>";

	SensorConsumption sensorConsumption = SensorConsumption();
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::disabled);
	PushMessageToSensor(generateMessage(0, 512, 1024, 0), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(0, 512, 1024, 513), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::warning);
	PushMessageToSensor(generateMessage(0, 512, 1024, 1025), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::danger);
	PushMessageToSensor(generateMessage(0, 512, 1024, -1), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::danger);
	PushMessageToSensor(generateMessage(-10, 512, 1024, -1), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(512, 1024, 2048, 1024), sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::warning);
	PushMessageToSensor(incorrectMessage, sensorConsumption);
	CPPUNIT_ASSERT(sensorConsumption.GetLevel() == BaseSensor::State::disabled);
}

void TC_Rokit::ChangeValue()
{
	typedef SensorChangeValueSerializable::Equal Equal;
	std::function<std::string(Equal, float)> generateMessage = [](Equal equal, float value) {
		SensorChangeValueSerializable changeValue = SensorChangeValueSerializable(L"ChangeValue", equal, value);
		return changeValue.Serialize();
	};

	std::string incorrectMessage = "<sensor key=\"ChangeValue\" type=\"ChangeValue\">eeee</sensor>";
	SensorChangeValue sensorChangeValue = SensorChangeValue();
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::disabled);
	PushMessageToSensor(generateMessage(Equal::inc, 5000), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::inc, 5001), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::inc, 5000), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::warning);
	PushMessageToSensor(generateMessage(Equal::inc, 5001), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::warning);
	sensorChangeValue.DisableWarning();
	PushMessageToSensor(generateMessage(Equal::inc, 5002), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::dec, 5001), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::cnst, 5001), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::cnst, 5001), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::normal);
	PushMessageToSensor(generateMessage(Equal::cnst, 5002), sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::warning);
	sensorChangeValue.DisableWarning();
	PushMessageToSensor(incorrectMessage, sensorChangeValue);
	CPPUNIT_ASSERT(sensorChangeValue.GetLevel() == BaseSensor::State::disabled);
}

void TC_Rokit::StaticValue()
{
	std::function<std::string(std::wstring)> generateMessage = [](std::wstring value) {
		SensorStaticValueSerializable staticValue = SensorStaticValueSerializable(L"StaticValue", value);
		return staticValue.Serialize();
	};
	std::wstring v1 = L"value1";
	std::wstring v2 = L"значение2";

	SensorStaticValue sensorStaticValue = SensorStaticValue();
	CPPUNIT_ASSERT(sensorStaticValue.GetLevel() == BaseSensor::State::disabled);
	PushMessageToSensor(generateMessage(v1), sensorStaticValue);
	CPPUNIT_ASSERT(sensorStaticValue.GetLevel() == BaseSensor::State::normal);
	CPPUNIT_ASSERT(sensorStaticValue.GetValue() == v1);
	PushMessageToSensor(generateMessage(v2), sensorStaticValue);
	CPPUNIT_ASSERT(sensorStaticValue.GetLevel() == BaseSensor::State::normal);
	CPPUNIT_ASSERT(sensorStaticValue.GetValue() == v2);
	PushMessageToSensor("", sensorStaticValue);
	CPPUNIT_ASSERT(sensorStaticValue.GetLevel() == BaseSensor::State::disabled);
}

void TC_Rokit::PushMessageToRemoteViewer(const std::string& message, RemoteViewer& remoteViewer)
{
	attic::a_document xmlDoc;
	xmlDoc.load_utf8(message);
	attic::a_node sensorsNode = xmlDoc.child(RokitSensorsXml::APPLICATION_XML_NODE);
	for (attic::a_node sensorNode : sensorsNode)
	{
		remoteViewer.SetSensorValue(sensorNode);
	}
}

void TC_Rokit::PushMessageToSensor(const std::string& message, BaseSensor& sensor)
{
	attic::a_document xmlDoc;
	xmlDoc.load_utf8(message);
	attic::a_node value = xmlDoc.child(RokitSensorsXml::SENSOR_XML_NODE);
	sensor.SetValue(value);
}
