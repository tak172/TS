#include "stdafx.h"
#include "TC_PlukBroker.h"
#include <memory>
#include <time.h>
#include <chrono>
#include <future>
#include <fstream>
#include "../helpful/Amqp/Message.h"
#include "../helpful/Amqp/Connection.h"
#include "../helpful/Amqp/Factory.h"
#include "../helpful/Amqp/Customer.h"
#include "../Pluk/PlukCommandFactory.h"
#include "../Pluk/PlukMessage.h"
#include "../helpful/Serialization.h"
#include "../Pluk/PlukConst.h"
#include "../helpful/Utf8.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukBroker );

using namespace std;
using namespace Network;

void TC_PlukBroker::GetStatutorySchedule()
{

	//тест только для проверки работы запущенной службы Pluk
	cout << endl;
	time_t from(1507852800);
	time_t to(1507939200);
	PlukCommandFactory commandFactory;
	string message = serialize<PlukMessage>(commandFactory.GetStatutorySchedule(from, to));
	Factory factory;
	shared_ptr<Connection> connection = factory.createConnection("localhost:8672");
	int c = 1;
	for (int i = 0; i < c; i++)
	{
		auto timeout = std::chrono::seconds(30);
		Customer customer(connection, timeout);
		auto response = customer.request( PlukConst::PlukAddress, message);
		if (response)
		{
			auto msg = deserialize<PlukMessage>(response->getContent());
			ofstream file;
			file.open(to_string(i) + "response.log", ios_base::in | ios_base::out | ios_base::trunc);
			file <<  msg->getBody();
			file.close();
			cout << i << " response success" << std::endl;
		} else {
			cout << i << " response timeout" << std::endl;
		}
	}
}
