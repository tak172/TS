#include "stdafx.h"
#include "TC_PlukSchedule.h"
#include "TC_PortablePgSQL.h"
#include "../helpful/Time_Iso.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukSchedule );

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

static std::string data( const time_t _startMom, size_t _count )
{
    std::string sm = time_to_iso(_startMom);
    std::string res =
        "  <RegulatorySchedule startTime='"+sm+"' creationDate='"+sm+"'>";
    if ( _count >=1 )
        res +=
        "    <Thread train='801'>"
        "      <Event esr='11000' type='departure' time='130700' odd='False' stagename='Daugavpils pas' stage='1' />"
		"      <Event esr='11443' type='arrival'   time='132000' odd='False' stagename='Liksna' stage='1' />"
		"      <Event esr='11443' type='departure' time='132100' odd='False' stagename='Liksna' stage='1' />"
		"      <Event esr='11442' type='arrival'   time='132600' odd='False' stagename='Vabole' stage='1' />"
		"      <Event esr='11442' type='departure' time='132700' odd='False' stagename='Vabole' stage='1' />"
        "      <Event esr='11446' type='arrival'   time='133700' odd='False' stagename='Nicgale' stage='1' />"
        "    </Thread>"
        ;
    if ( _count >=2 )
        res +=
        "    <Thread train='808'>"
        "      <Event esr='09010' type='departure'  time='130100' odd='False' stagename='Riga pas.' stage='2' />"
        "      <Event esr='09006' type='arrival'    time='130930' odd='False' stagename='Janavarti' stage='2' />"
		"      <Event esr='09006' type='departure'  time='131030' odd='False' stagename='Janavarti' stage='2' />"
        "      <Event esr='09420' type='arrival'    time='132100' odd='False' stagename='Salaspils' stage='2' />"
		"      <Event esr='09420' type='departure'  time='132200' odd='False' stagename='Salaspils' stage='2' />"
        "      <Event esr='09413' type='transition' time='132730' odd='False' stagename='Saulkalne' stage='2' />"
        "    </Thread>"
        ;
    res +=
        "  </RegulatorySchedule>"
        ;
    return res;
}

static std::string rev_ordered_event( const time_t _startMom )
{
	std::string sm = time_to_iso(_startMom);
	std::string res =
		"  <RegulatorySchedule startTime='"+sm+"' creationDate='"+sm+"'>"
		"    <Thread train='801'>"
		"      <Event esr='11111' type='departure'  time='130100' />"
        "      <Event esr='11111' type='arrival'    time='130100' />"
		"      <Event esr='22222' type='departure'  time='130200' />"
        "      <Event esr='22222' type='arrival'    time='130200' />"
		"      <Event esr='33333' type='departure'  time='130300' />"
        "      <Event esr='33333' type='arrival'    time='130300' />"
		"      <Event esr='44444' type='departure'  time='130400' />"
        "      <Event esr='44444' type='arrival'    time='130400' />"
		"      <Event esr='55555' type='departure'  time='130500' />"
		"      <Event esr='55555' type='arrival'    time='130500' />"
		"    </Thread>"
		"  </RegulatorySchedule>"
		;
	return res;
}

static std::string rev_ordered_train( const time_t _startMom )
{
    std::string sm = time_to_iso(_startMom);
    std::string res =
        "  <RegulatorySchedule startTime='"+sm+"' creationDate='"+sm+"'>"
        "    <Thread train='801'>"
        "      <Event esr='11111' type='departure'  time='130100' />"
        "      <Event esr='11111' type='arrival'    time='130100' />"
        "      <Event esr='22222' type='departure'  time='130200' />"
        "      <Event esr='22222' type='arrival'    time='130200' />"
        "    </Thread>"
        "    <Thread train='15'>"
        "      <Event esr='11111' type='departure'  time='150100' />"
        "      <Event esr='11111' type='arrival'    time='150100' />"
        "      <Event esr='22222' type='departure'  time='150200' />"
        "      <Event esr='22222' type='arrival'    time='150200' />"
        "    </Thread>"
        "    <Thread train='6801'>"
        "      <Event esr='11111' type='departure'  time='180100' />"
        "      <Event esr='11111' type='arrival'    time='180100' />"
        "      <Event esr='22222' type='departure'  time='180200' />"
        "      <Event esr='22222' type='arrival'    time='180200' />"
        "    </Thread>"
        "  </RegulatorySchedule>"
        ;
    return res;
}

void TC_PlukSchedule::getList()
{
	auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );

	// счетчик расписаний
	auto count_schedules = [ plukDB ]( time_t left, time_t right ){
		std::string m = plukDB->getScheduleList( time_to_iso( left ), time_to_iso( right ) );
		attic::a_document doc;
		CPPUNIT_ASSERT( doc.load_utf8( m ) );
		int count=0;
		for( auto ch : doc.document_element().children("schedule") )
			++count;
		return count;
	};

	time_t a = time_from_iso("20200101T000000Z");
	time_t b = time_from_iso("20210101T000000Z");

	// изначально никаких данных нет
    CPPUNIT_ASSERT_EQUAL( 0, count_schedules( a, b )  );
    // записать расписания с началом в точках A и B
    CPPUNIT_ASSERT( plukDB->writeSchedule( data( a, 1 ) ) );
	CPPUNIT_ASSERT( plukDB->writeSchedule( data( b, 2 ) ) );
	//
	CPPUNIT_ASSERT_EQUAL( 0, count_schedules( a-2, a-1 )  );
	CPPUNIT_ASSERT_EQUAL( 2, count_schedules( a,   b+1 )  );
	CPPUNIT_ASSERT_EQUAL( 2, count_schedules( b-1, b+1 )  );
	CPPUNIT_ASSERT_EQUAL( 1, count_schedules( b,   b+1 )  );
}

void TC_PlukSchedule::eventOrder()
{
	auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );

	time_t a = time_from_iso("20200101T000000Z");
	// записать расписание
	CPPUNIT_ASSERT( plukDB->writeSchedule( rev_ordered_event( a ) ) );
	std::string m = plukDB->getSchedule( time_to_iso( a ), time_to_iso( a ) );
	attic::a_document doc;
	CPPUNIT_ASSERT( doc.load_utf8( m ) );

	auto types_by_esr = [ &doc ]( EsrKit esr ){
		std::vector<std::string> v;
		std::string q = "//Event[@esr='" + esr.to_string() + "']";
		auto nodes = doc.select_nodes( q.c_str() );
		for ( auto it : nodes )
			v.push_back( it.node().attribute("type").as_string() );
		return v;
	};

	auto v1 = types_by_esr( EsrKit(11111) );
	CPPUNIT_ASSERT( 2 == v1.size() );
	CPPUNIT_ASSERT( v1[0] == "arrival" );
	CPPUNIT_ASSERT( v1[1] == "departure" );

	auto v2 = types_by_esr( EsrKit(22222) );
	CPPUNIT_ASSERT( 2 == v2.size() );
	CPPUNIT_ASSERT( v2[0] == "arrival" );
	CPPUNIT_ASSERT( v2[1] == "departure" );

	auto v3 = types_by_esr( EsrKit(33333) );
	CPPUNIT_ASSERT( 2 == v3.size() );
	CPPUNIT_ASSERT( v3[0] == "arrival" );
	CPPUNIT_ASSERT( v3[1] == "departure" );

	auto v4 = types_by_esr( EsrKit(44444) );
	CPPUNIT_ASSERT( 2 == v4.size() );
	CPPUNIT_ASSERT( v4[0] == "arrival" );
	CPPUNIT_ASSERT( v4[1] == "departure" );

	auto v5 = types_by_esr( EsrKit(55555) );
	CPPUNIT_ASSERT( 2 == v5.size() );
	CPPUNIT_ASSERT( v5[0] == "arrival" );
	CPPUNIT_ASSERT( v5[1] == "departure" );
}

void TC_PlukSchedule::trainOrder()
{
    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );

    time_t a = time_from_iso("20200101T000000Z");
    // записать расписание
    CPPUNIT_ASSERT( plukDB->writeSchedule( rev_ordered_train( a ) ) );
    std::string m = plukDB->getSchedule( time_to_iso( a ), time_to_iso( a ) );
    attic::a_document doc;
    CPPUNIT_ASSERT( doc.load_utf8( m ) );

    attic::a_node n1 = doc.document_element()
        .child("RegulatorySchedule")
        .child("Thread");
    CPPUNIT_ASSERT( n1.attribute("train").as_wstring() == L"15" );

    attic::a_node n2 = n1.next_sibling();
    CPPUNIT_ASSERT( n2.attribute("train").as_wstring() == L"801" );

    attic::a_node n3 = n2.next_sibling();
    CPPUNIT_ASSERT( n3.attribute("train").as_wstring() == L"6801" );
}

std::shared_ptr<PlukDataBase> TC_PlukSchedule::GetPlukDataBase(const std::string& stringConnect)
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

void TC_PlukSchedule::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukSchedule::tearDown() 
{
	LocalPostgresInstance::Stop();
}
