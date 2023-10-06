#include "stdafx.h"
#include "TC_PlukPermission.h"
#include "TC_PortablePgSQL.h"
#include "../helpful/Serialization.h"
#include "../helpful/Log.h"
#include "../Pluk/parse_change.h"
#include "../Pluk/pg_query.h"
#include "../Permission/PermissionDesignated.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukPermission );

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

void TC_PlukPermission::Write()
{
    std::wstring srcText =
        L"<DesignatedPermission>"
        L"  <Access district='11111-22222' default='view'>"
        L"    <Rule ip='192.168.46.1' level='edit'/>"
        L"    <Rule ip='192.168.46.2' level='edit'/>"
        L"  </Access>"
        L"  <Access district='11111-33333' default='view'>"
        L"    <Rule ip='192.168.46.1' level='edit'/>"
        L"    <Rule ip='192.168.46.3' level='edit'/>"
        L"  </Access>"
        L"  <Access district='22222-33333' default='view'>"
        L"    <Rule ip='192.168.46.2' level='edit'/>"
        L"    <Rule ip='192.168.46.3' level='edit'/>"
        L"  </Access>"
        L"  <Normative>"
        L"    <Rule ip='192.168.46.4' level='edit'/>"
        L"  </Normative>"
        L"  <Supervisor>"
        L"    <Rule ip='192.168.46.4' level='edit'/>"
        L"    <Rule ip='192.168.46.7' level='edit'/>"
        L"  </Supervisor>"
        L"</DesignatedPermission>";

    // записать
    attic::a_document srcPerm;
    CPPUNIT_ASSERT( srcPerm.load_wide(srcText) );
    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );
    CPPUNIT_ASSERT( plukDB->writePermission( srcPerm.to_str() ) );

    // прочитать из базы обратно
    std::string trgText = plukDB->selectPermission();
    attic::a_document trgDoc;
    CPPUNIT_ASSERT( trgDoc.load_utf8( trgText ) );

    // проверить совпадение
    std::string srcT = srcPerm.to_str();
    auto mism = std::mismatch(srcT.begin(), srcT.end(), trgText.cbegin(), trgText.cend() );
    CPPUNIT_ASSERT( srcT == trgText );
}

void TC_PlukPermission::Rewrite()
{
    using namespace Permission;
    namespace bt = boost::posix_time;

    // Создать большой список разрешений
    const std::wstring distr[] = {
        L"11111-22222",
        L"22222-33333",
        L"33333-44444",
        L"44444-55555",
        L"55555-66666",
        L"66666-77777",
        L"77777-88888",
        L"88888-99999"
    };
    const std::string ip_pfx = "192.168.46.";
    const int IP_FROM = 1;
    const int IP_TO = 250;

    Designated perm1;
    for( auto& d : distr ) {
        perm1.addDistrict( d, Level::VIEW() );
        for( int j = IP_FROM; j <= IP_TO; ++j ) {
            perm1.setPermit( d, Level::EDIT(), ip_pfx+std::to_string( j ) );
        }
    }
    
    // записать
    const auto strPerm1 = serialize(perm1);
    auto plukDB = GetPlukDataBase( LocalPostgresInstance::GetStringConnect() );
    boost::posix_time::ptime w1 = bt::microsec_clock::universal_time();
    CPPUNIT_ASSERT( plukDB->writePermission( strPerm1 ) );
    boost::posix_time::ptime w2 = bt::microsec_clock::universal_time();

    // записать малое изменение
    Designated perm2 = perm1;
    bool ok = perm2.setPermit( distr[0], Level::VIEW(), ip_pfx+std::to_string(IP_FROM) );
    CPPUNIT_ASSERT( ok );
    perm2.delPermit( ip_pfx+std::to_string(IP_FROM+1) );
    perm2.setNormativePermit( Level::EDIT(), IpAdr("192.168.55.55") );
    const auto strPerm2 = serialize(perm2);

    boost::posix_time::ptime r1 = bt::microsec_clock::universal_time();
    CPPUNIT_ASSERT( plukDB->writePermission( strPerm2 ) );
    boost::posix_time::ptime r2 = bt::microsec_clock::universal_time();

    // проверить совпадение
    bt::time_duration dw = w2-w1;
    bt::time_duration dr = r2-r1;
    OutputDebugString( (L"  write "+ bt::to_simple_wstring(dw) + L"\n").c_str() );
    OutputDebugString( (L"rewrite "+ bt::to_simple_wstring(dr) + L"\n").c_str() );
    WriteLog(L"  write "+ bt::to_simple_wstring(dw) );
    WriteLog(L"rewrite "+ bt::to_simple_wstring(dr) );
    CPPUNIT_ASSERT( dr*3 < dw );

    // убедиться, что в базе последний вариант
    std::string rewritedText;
    {
        if ( auto pperm3 = deserialize<Designated>( plukDB->selectPermission() ) )
            rewritedText = serialize( *pperm3 );
    }

    // проверить совпадение
    auto mism = std::mismatch(strPerm2.begin(), strPerm2.end(), rewritedText.begin(), rewritedText.end() );
    CPPUNIT_ASSERT( strPerm2 == rewritedText );
}

std::shared_ptr<PlukDataBase> TC_PlukPermission::GetPlukDataBase(const std::string& stringConnect)
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

void TC_PlukPermission::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukPermission::tearDown() 
{
	LocalPostgresInstance::Stop();
}
