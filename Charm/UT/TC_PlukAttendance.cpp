#include "stdafx.h"
#include "TC_PlukAttendance.h"
#include "TC_PortablePgSQL.h"
// #include "../helpful/Serialization.h"
// #include "../helpful/Log.h"
#include "../Pluk/parse_change.h"
#include "../Pluk/pg_query.h"
// #include "../Permission/PermissionDesignated.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukAttendance );

// тестовый разборщик изменений
class UT_parse_change : public parse_change
{
public:
    UT_parse_change(db_query_ptr _db_query, cache_ptr _cache)
        : parse_change(_db_query,_cache)
    {}
    // специальная реакция на ошибку (для обнаружения в тестах)
    void handle_error(const std::string&) override
    {
        CPPUNIT_FAIL("UT_parse_change: error occured in DB");
    }
};

// тестовая БД
class UT_PlukDataBase : public PlukDataBase
{
public:
    UT_PlukDataBase(std::string stringConnect)
        : PlukDataBase(stringConnect)
    {}
    // создать тестовый разборщик изменений
    std::shared_ptr<parse_change> buildParseChange(std::shared_ptr<db::pg_query> db_query) override
    {
        return std::make_shared<UT_parse_change>(db_query, cache);
    }
};

void TC_PlukAttendance::Write()
{
    //CPPUNIT_ASSERT( false );
}

void TC_PlukAttendance::Rewrite()
{
    CPPUNIT_ASSERT( false );
}

std::shared_ptr<PlukDataBase> TC_PlukAttendance::GetPlukDataBase(const std::string& stringConnect)
{
	auto plukDB = std::make_shared<UT_PlukDataBase>(stringConnect);
	const int max_wait = 5;
	int wait_count = 0;
	while (!plukDB->isConnect() && wait_count < max_wait) {
		++wait_count;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (wait_count == max_wait)
		throw std::runtime_error("Error connect to: " + stringConnect);
	else 
		return plukDB;
}

void TC_PlukAttendance::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukAttendance::tearDown() 
{
	LocalPostgresInstance::Stop();
}
