#include "stdafx.h"
#include <sstream>
#include <boost/filesystem/operations.hpp>
#include "TC_Log.h"
#include "../helpful/Log.h"
#include "../helpful/Pilgrim.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Log );

/*
проверить Log
*/

void TC_Log::flush()
{
    namespace bt = boost::posix_time;

    // записываемые данные - достаточно большой объем (несколько кластеров на диске)
    std::vector<std::wstring> data;
    const size_t SZ = 10*1024;
    data.emplace_back( std::wstring( SZ, char('-') ) );
    for( char ch='0'; ch<='z'; ++ch )
        data.emplace_back( std::wstring( SZ, ch ) );

    // запись буферизированная
    bt::ptime t[4];
    t[0] = bt::microsec_clock::universal_time();
    for( auto& d : data )
        WriteLog( d );
    // а теперь сброс на диск
    t[1] = bt::microsec_clock::universal_time();
    WriteLog.flush();
    // запись снова буферизированная
    t[2] = bt::microsec_clock::universal_time();
    for( auto& d : data )
        WriteLog( d );
    t[3] = bt::microsec_clock::universal_time();
    // вычисление интервалов
    bt::time_duration d[3];
    d[0] = t[1] - t[0];
    d[1] = t[2] - t[1];
    d[2] = t[3] - t[2];
    // сравнение интервалов если время записи ненулевое (защита от SSD)
    if ( bt::time_duration() != d[0] )
    {
        CPPUNIT_ASSERT( d[0] * 2 < d[1] );
        CPPUNIT_ASSERT(            d[1] > d[2] * 2 );
        
        std::wstring s[3] = {
        to_simple_wstring( d[0] ),
        to_simple_wstring( d[1] ),
        to_simple_wstring( d[2] ),
        };
    }
}

void TC_Log::daily()
{
	namespace bt = boost::posix_time;
	namespace bg = boost::gregorian;

	bt::ptime etalon( bg::date(2077,5,17), bt::hours(23) ); // фиксированный момент в далеком будущем
	CPPUNIT_ASSERT( bt::microsec_clock::local_time() < etalon );

	auto mock_clock = [&etalon](){
		return etalon;
	};

	WriteLog.set_clock( mock_clock );
	WriteLog.set_dayly_split();
	WriteLog(L"\nСтартовая запись в 23:00\n ");
	etalon += bt::hours(24);
	WriteLog(L"\nЗавтрашняя запись в 23:00\n ");
	etalon += bt::hours(24);
	WriteLog(L"\nПослезавтрашняя запись в 23:00\n ");
	WriteLog.flush();
	std::vector<std::wstring> v = WriteLog.file_names();
	CPPUNIT_ASSERT( v.size() >=3 );
	CPPUNIT_ASSERT( std::wstring::npos != ( v.end() -3 )->find( L"770517-" ) ); // для дня 2077-05-17
	CPPUNIT_ASSERT( std::wstring::npos != ( v.end() -2 )->find( L"770518-" ) ); // для дня 2077-05-18
	CPPUNIT_ASSERT( std::wstring::npos != ( v.end() -1 )->find( L"770519-" ) ); // для дня 2077-05-19
}

static void ClearFutureLogFiles()
{
	WriteLog.set_clock();
	WriteLog.flush();

    std::vector<std::wstring> v = WriteLog.file_names();
	auto clean77 = []( const std::wstring& fn ){
		if ( fn.npos != fn.find(L"77051") )
        {
            boost::system::error_code ec;
            bool succ = boost::filesystem::remove( WriteLog.get_path() + fn, ec );
            CPPUNIT_ASSERT( succ && !ec );
        }
	};

	std::for_each( v.begin(), v.end(), clean77 );
}

void TC_Log::setUp()
{
	ClearFutureLogFiles();
}

void TC_Log::tearDown()
{
	ClearFutureLogFiles();
}
