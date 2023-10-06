#include "stdafx.h"

#include "TC_Monitor.h"
#include "../helpful/StateReporter.h"
#include "../helpful/StateCollector.h"
#include "../helpful/RSensorsSerializable.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Monitor );

using namespace Monitor;

const unsigned short c_listenPort = 1379u;

void TC_Monitor::tearDown()
{
    KeyValueStateCollector::Shutdowner();
}

void TC_Monitor::KeyValueCollector()
{
	std::function<std::string(std::string, std::string)> generateMessage = [](std::string appName, std::string value) {
		return "<application name=\"" + appName + "\">\n  " + value + "</application>\n";
	};

	std::string appName = "UnitTest";
	std::string dlm = "  ";
    auto& collector = *KeyValueStateCollector::instance();
	collector.setAppName(appName);
	collector.updateActivity(L"Key1", 1000, L"1000");
	SensorActivitySerializable activity(L"Key1", 1000, L"1000");
	CPPUNIT_ASSERT_EQUAL(generateMessage(appName, activity.Serialize()), collector.getStateString());

	collector.updateConsumption(L"Key2", 0, 256, 512, 255);
	SensorConsumptionSerializable consumption(L"Key2", 0, 256, 512, 255);

	CPPUNIT_ASSERT_EQUAL(generateMessage(appName, activity.Serialize() + dlm + consumption.Serialize()), collector.getStateString());

	collector.updateChangeValue(L"Key3", (int)SensorChangeValueSerializable::Equal::cnst, 5000);
	SensorChangeValueSerializable changeValue(L"Key3", SensorChangeValueSerializable::Equal::cnst, 5000);

	CPPUNIT_ASSERT_EQUAL(generateMessage(appName, activity.Serialize() + dlm + consumption.Serialize() + dlm + changeValue.Serialize()), collector.getStateString());

	collector.updateStaticValue(L"Key4", L"Значение1");
	SensorStaticValueSerializable staticValue(L"Key4", L"Значение1");

	CPPUNIT_ASSERT_EQUAL(generateMessage(appName, activity.Serialize() + dlm + consumption.Serialize() + dlm + changeValue.Serialize() + dlm + staticValue.Serialize()), collector.getStateString());

}

void TC_Monitor::Reporter()
{
    auto& collector = *KeyValueStateCollector::instance();
    collector.setAppName("UnitTest");
    StateReporter reporter(collector, c_listenPort);
    reporter.setMessageInterval(std::chrono::milliseconds(10));
    reporter.asyncShutdown();
}

void TC_Monitor::NetworkFails()
{
    auto& collector = *KeyValueStateCollector::instance();
    collector.setAppName("UnitTest");
    StateReporter reporter1(collector, c_listenPort);

    CPPUNIT_ASSERT_THROW_MESSAGE("Should not start two reporters from the same thread with same ports",
        StateReporter(collector, c_listenPort),
        std::runtime_error);
}

class TCReceiver 
{
public:
    explicit TCReceiver(unsigned short listenPort, unsigned short connectPort)
        : m_network(Network::Identifier("TCReceiver"), listenPort) 
    {
        m_network.add_addressee(std::make_pair("localhost", connectPort));
        m_network.start();
    }

    bool isConnected() const
    {
        return !m_network.get_channels().empty();
    }

    unsigned countIncomingMessages() 
    {
        unsigned count = 0u;

        while (!m_network.empty())
        {
            Network::bufferPtr msg = m_network.receive();
            ++count;
        }

        return count;
    }

private:
    Network::Docker m_network;
};

void TC_Monitor::ReportsCount()
{
    const unsigned intervalMs = 10u;
    const unsigned connectionTimeMs = 500u;
    const unsigned messagesExpected = 20u;

    auto& collector = *KeyValueStateCollector::instance();
    collector.setAppName("UnitTest");
    collector.updateStaticValue(L"Key1", L"Value1");
    collector.updateStaticValue(L"Key2", L"Value2");

    StateReporter reporter(collector, c_listenPort);
    reporter.setMessageInterval(std::chrono::milliseconds(intervalMs));
    TCReceiver receiver(c_listenPort + 1u, c_listenPort);

    std::this_thread::sleep_for(std::chrono::milliseconds(connectionTimeMs + 2 * intervalMs * messagesExpected));
    CPPUNIT_ASSERT(receiver.isConnected());

    reporter.asyncShutdown();
    unsigned reportsCount = receiver.countIncomingMessages();

    CPPUNIT_ASSERT(reportsCount >= messagesExpected);
}

void TC_Monitor::ActorReportingIn()
{
    const unsigned short actorPort = 13791u;
    TCReceiver receiver(actorPort + 1u, actorPort);

    std::this_thread::sleep_for(std::chrono::seconds(2u));
    CPPUNIT_ASSERT(receiver.isConnected());

    std::this_thread::sleep_for(std::chrono::seconds(14u));

    unsigned reportsCount = receiver.countIncomingMessages();

    CPPUNIT_ASSERT(reportsCount >= 1u);
}