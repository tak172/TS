#include "stdafx.h"

#include <memory>
#include <string>

#include "TC_PlukConnect.h"
#include "TC_PortablePgSQL.h"
#include "../Pluk/pg_factory.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukConnect );

using namespace std;
using namespace connection_pool;

const int min_connect = 2;
const int max_connect = 10;

typedef std::shared_ptr<connection_pool::pool<pg_connection>> pg_pool;

pg_pool GetNewPool() 
{
	shared_ptr<pg_factory> factory = shared_ptr<pg_factory>(new pg_factory(LocalPostgresInstance::GetStringConnect()));
	pg_pool new_pool = pg_pool(new pool<pg_connection>(factory, min_connect, max_connect));
	return new_pool;
}

void TC_PlukConnect::OpenCloseConnect()
{
	pg_pool pool = GetNewPool();
	CPPUNIT_ASSERT(pool->has_connect());
	CPPUNIT_ASSERT_NO_THROW(pool->pop_connect());
}

void TC_PlukConnect::OpenClosePoolConnect()
{
	pg_pool pool = GetNewPool();
	for (int i = 0; i < max_connect + min_connect; ++i)
	{
		CPPUNIT_ASSERT_NO_THROW(pool->pop_connect());
	}
}

void TC_PlukConnect::OverheadPoolConnect()
{
	pg_pool pool = GetNewPool();
	vector<shared_ptr<pg_connection>> connections;
	for (int i = 0; i < max_connect + min_connect; ++i)
	{
		if (i < max_connect) 
			CPPUNIT_ASSERT_NO_THROW(connections.push_back(pool->pop_connect()));
		else
			CPPUNIT_ASSERT_THROW(pool->pop_connect(), std::runtime_error);
	}
	connections.clear();	
}

void TC_PlukConnect::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukConnect::tearDown()
{
	LocalPostgresInstance::Stop();
}
