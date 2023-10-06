#include "stdafx.h"

#include <boost/date_time/posix_time/posix_time.hpp>
#include "../helpful/LogCleaner.h"
#include "../helpful/Log.h"
#include "TC_LogCleaner.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LogCleaner );

using namespace boost::posix_time;
using namespace std;

const auto f_00 = L"20150723T000000Z_gu.gdb";
const auto f_15 = L"20150723T001500Z_r32.gdb";
const auto f_30 = L"20150723T003000Z_i.gdb";
const auto f_31 = L"20150723T003100Z_i4.gdb";
const auto f_60 = L"20150723T006000Z_i.gdb";

void TC_LogCleaner::test_size()
{
    LogCleaner::collection files;

    // набор пуст
    files.clear();
    CPPUNIT_ASSERT( !LogCleaner::detectArchaic( files, 1024, 0 ) );
    CPPUNIT_ASSERT( files.empty() );
    // весь набор влезает в интервал
    files.clear();
    files.emplace_back( f_00, 500 );
    files.emplace_back( f_15, 200 );
    CPPUNIT_ASSERT( !LogCleaner::detectArchaic( files, 1024, 0 ) );
    CPPUNIT_ASSERT( files.empty() );
    // удаление самых ранних, выходящих за диапазон
    files.clear();
    files.emplace_back( f_00, 400 );
    files.emplace_back( f_15, 10000 );
    files.emplace_back( f_30, 300 );
    files.emplace_back( f_31, 200 );
    files.emplace_back( f_60, 100 );
    CPPUNIT_ASSERT( LogCleaner::detectArchaic( files, 1024, 0 ) );
    CPPUNIT_ASSERT( files.size()==2 );
    CPPUNIT_ASSERT( files[0].second == 400 );
    CPPUNIT_ASSERT( files[1].second == 10000 );
}


void TC_LogCleaner::test_count()
{
	LogCleaner::collection files;

	// набор пуст
	files.clear();
	CPPUNIT_ASSERT( !LogCleaner::detectArchaic( files, 1024, 3 ) );
	CPPUNIT_ASSERT( files.empty() );
	// количество допустимо
	files.clear();
	files.emplace_back( f_00, 500 );
	files.emplace_back( f_15, 200 );
	CPPUNIT_ASSERT( !LogCleaner::detectArchaic( files, 1024, 3 ) );
	CPPUNIT_ASSERT( files.empty() );
	// удаление по количеству
	files.clear();
	files.emplace_back( f_00, 300 );
	files.emplace_back( f_30, 250 );
	files.emplace_back( f_31, 200 );
	files.emplace_back( f_60, 100 );
	CPPUNIT_ASSERT( LogCleaner::detectArchaic( files, 1024, 3 ) );
	CPPUNIT_ASSERT( files.size()==1 );
	CPPUNIT_ASSERT( files[0].second == 300 );
}

// макрос DebugLog должен
// - использоваться как обычная функция в т.ч. в любой ветке условного оператора
// - в чужом режиме не только отсекать вывод, но даже не вычислять аргументы
void TC_LogCleaner::test_DebugLog_else()
{
    const int OK = 777;
    const int BAD = 4;
    const bool yes = (0 != GetTickCount());

    // использование в условном операторе 
    int value = OK;
    if ( yes )
        if ( OK == value )
            DebugLog(L"Text %d and %d", 123, 456 );
        else
            value = BAD; // если ветка else продолжит раскрытый макрос
    else
        value = BAD; // никогда не попадаем т.к. yes==true всегда 
    CPPUNIT_ASSERT_EQUAL( OK, value );
}

void TC_LogCleaner::test_DebugLog_evaluate()
{
    // невычисление аргументов
    auto hookDebug = [](int i){
        std::string msg = "debug evaluate "+std::to_string( i );
        CPPUNIT_FAIL( msg );
        return msg;
    };
    auto hookRelease = [](int j){
        std::string msg = "release evaluate "+std::to_string( j );
        CPPUNIT_FAIL( msg );
        return msg;
    };

#ifdef _DEBUG
    ReleaseLog("Arguments %s", hookRelease(11)+hookRelease(22) );
#else
    DebugLog("Arguments %s", hookDebug(37)+hookDebug(28) );
#endif
}
