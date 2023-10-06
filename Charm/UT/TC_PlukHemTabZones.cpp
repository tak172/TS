#include "stdafx.h"
#include "TC_PlukHemTabZones.h"
#include "TC_PortablePgSQL.h"
#include "../helpful/Time_Iso.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukHemTabZones );

// тестовая БД
class UT_PlukDataBase : public PlukDataBase
{
public:
    UT_PlukDataBase(std::string stringConnect)
        : PlukDataBase(stringConnect)
    {}
//     // создать тестовый разборщик изменений
//     std::shared_ptr<parse_change> buildParseChange(std::shared_ptr<db::pg_query> db_query) override
//     {
//         return std::make_shared<UT_parse_change>(db_query, cache);
//     }
};

static std::wstring raw_data()
{
    return
        L"<HemTabZone>"
        L"  <HemTab>"
        L"    <HemWin filename='DGR-&apos;-Vt' district_code='09180-09820' tabname='DGR-Vt'>"
        L"      <!--Daugavpils-Šķ.-->"
        L"      <Station ESR='11010'>"
        L"        <Park Id='1'>"
        L"          <Way Id='1' />"
        L"          <Way Id='2' />"
        L"          <Way Id='3' />"
        L"        </Park>"
        L"      </Station>"
        L"      <OddNum spanlist='09190:11420,09181:09251' />"
        L"      <EvenNum spanlist='09190:11420,09181:09251' />"
        L"    </HemWin>"
        L"  </HemTab>"
        L"  <DistrictList>"
        L"    <District code='09000-09640' name='Rigas .mezgls'>"
        L"      <Involve esr='09710' />"
        L"      <Involve esr='09580' border='Y' />"
        L"      <Involve esr='09006' border='Y' target='Y' />"
        L"    </District>"
        L"  </DistrictList>"
        L"</HemTabZone>";
}

void TC_PlukHemTabZones::WriteReadRemove()
{
    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );

    std::string t1 = "20201010T010000Z";
    std::string t2 = "20201111T000200Z";
    std::string t3 = "20201212T000003Z";
    CPPUNIT_ASSERT( time_from_iso(t1) < time_from_iso(t2) );
    CPPUNIT_ASSERT( time_from_iso(t2) < time_from_iso(t3) );

    // изначально никаких данных нет
    std::string m;
    m = plukDB->readHemTabZone( t3 );
    CPPUNIT_ASSERT( m.empty() );

    // записать нечто на момент t2
    attic::a_document src_;
    CPPUNIT_ASSERT( src_.load_wide( raw_data() ) );
    // подготовить образцы для T2 и T3
    src_.document_element().set_name("HTZ");
    src_.document_element().ensure_attribute("moment").set_value( t2 );
    const std::string srcT2 = src_.to_str();
    src_.document_element().ensure_attribute("moment").set_value( t3 );
    const std::string srcT3 = src_.to_str();
    CPPUNIT_ASSERT( srcT2 != srcT3 );
    // вернуть обратно
    src_.document_element().remove_attribute("moment");
    src_.document_element().set_name("HeeeeTaaaaaaZoooo");

    // запись T2 читается и в момент T2 и позже
    CPPUNIT_ASSERT( plukDB->writeHemTabZone( t2, src_.to_str() ) );
    m = plukDB->readHemTabZone( t1 );
    CPPUNIT_ASSERT( m == "" );
    m = plukDB->readHemTabZone( t2 );
    CPPUNIT_ASSERT( m == srcT2 );
    m = plukDB->readHemTabZone( t3 );
    CPPUNIT_ASSERT( m == srcT2 );

    // записать T3 и проверить чтение раздельное
    CPPUNIT_ASSERT( plukDB->writeHemTabZone( t3, src_.to_str() ) );
    m = plukDB->readHemTabZone( t1 );
    CPPUNIT_ASSERT( m == "" );
    m = plukDB->readHemTabZone( t2 );
    CPPUNIT_ASSERT( m == srcT2 );
    m = plukDB->readHemTabZone( t3 );
    CPPUNIT_ASSERT( m == srcT3 );

    // удалить T3 и проверить возврат к исходному поведению
    CPPUNIT_ASSERT( !plukDB->removeHemTabZone( t2 ) ); // удаляется только последний
    CPPUNIT_ASSERT( plukDB ->removeHemTabZone( t3 ) );
    CPPUNIT_ASSERT( !plukDB->removeHemTabZone( t3 ) );
    m = plukDB->readHemTabZone( t1 );
    CPPUNIT_ASSERT( m == "" );
    m = plukDB->readHemTabZone( t2 );
    CPPUNIT_ASSERT( m == srcT2 );
    m = plukDB->readHemTabZone( t3 );
    CPPUNIT_ASSERT( m == srcT2 );
}

std::shared_ptr<PlukDataBase> TC_PlukHemTabZones::GetPlukDataBase(const std::string& stringConnect)
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

void TC_PlukHemTabZones::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukHemTabZones::tearDown() 
{
	LocalPostgresInstance::Stop();
}
