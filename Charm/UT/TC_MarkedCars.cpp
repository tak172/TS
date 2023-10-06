#include "stdafx.h"

#include "TC_MarkedCars.h"
#include "../Tower/DB_ADDR.h"
#include "../Tower/CarNumber.h"
#include "../Tower/MarkedCar.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_MarkedCars );

void TC_MarkedCars::sampling( MarkedCars& markedCars, std::wstring sample[], size_t count )
{
    std::vector<RollingStock::Car> vcar;
    for( size_t i=0; i<count; ++i ) {
        RollingStock::Car temp;
        temp.dba(      DB_ADDR( long( 50332021 + i ) ) );
        temp.number( CarNumber( long( 42000316 + i ) ) );
        temp.special_marks( sample[i] );
        vcar.push_back( temp );
    }
    markedCars.append_back( vcar );
}

void TC_MarkedCars::single()
{
    RollingStock::Model model;
    ChainInfo  chainInfo(model);
    MarkedCars markedCars(chainInfo);

    std::wstring sample[] = {
        L"",
        L"5Р ВМ 4Г НГ",
        L"4Ф ДБ"
    };
    sampling( markedCars, sample, size_array(sample) );

    std::wstring single0 = markedCars.group_special_marks( 0, 1 );
    CPPUNIT_ASSERT( single0 == sample[0] );
    std::wstring single1 = markedCars.group_special_marks( 1, 2 );
    CPPUNIT_ASSERT( single1 == sample[1] );
    std::wstring single2 = markedCars.group_special_marks( 2, 3 );
    CPPUNIT_ASSERT( single2 == sample[2] );
}

void TC_MarkedCars::repeat()
{
    RollingStock::Model model;
    ChainInfo  chainInfo(model);
    MarkedCars markedCars(chainInfo);

    std::wstring sample[] = {
        L"",
        L"",
        L"5Р ВМ 4Г НГ",
        L"5Р ВМ 4Г НГ",
        L"4Ф ДБ",
        L"4Ф ДБ",
        L"4Ф ДБ",
    };
    sampling( markedCars, sample, size_array(sample) );

    std::wstring a = markedCars.group_special_marks( 0, 2 ); // все пустые
    CPPUNIT_ASSERT( a == sample[0] );
    std::wstring b = markedCars.group_special_marks( 0, 4 ); // пустышки и одинаковые
    CPPUNIT_ASSERT( b == sample[2] );
    std::wstring d = markedCars.group_special_marks( 4, 7 ); // одинаковые
    CPPUNIT_ASSERT( d == sample[4] );
}

void TC_MarkedCars::select()
{
    RollingStock::Model model;
    ChainInfo  chainInfo(model);
    MarkedCars markedCars(chainInfo);

    std::wstring sample[] = {
        L"ОХР",
        L"5Р ВМ 4Г НГ",
        L"4Ф ДБ"
    };
    sampling( markedCars, sample, size_array(sample) );

    std::wstring a = markedCars.group_special_marks( 0, size_array(sample) );
    CPPUNIT_ASSERT( a == L"5Р ВМ 4Г НГ" );
}

void TC_MarkedCars::ticket6866()
{
    { // case 1
        RollingStock::Model model;
        ChainInfo  chainInfo(model);
        MarkedCars markedCars(chainInfo);

        std::wstring sample[] = {
            L"4Г НГ",
            L"3Е СЖП",
            L"3Е СЖ"
        };
        sampling( markedCars, sample, size_array(sample) );

        std::wstring a = markedCars.group_special_marks( 0, size_array(sample) );
        CPPUNIT_ASSERT( a == L"3Е СЖ 4Г НГ" );
    }
    { // case 2
        RollingStock::Model model;
        ChainInfo  chainInfo(model);
        MarkedCars markedCars(chainInfo);

        std::wstring sample[] = {
            L"8О ОП 6У ОХР",
            L"4З ЗМЧ",
        };
        sampling( markedCars, sample, size_array(sample) );

        std::wstring a = markedCars.group_special_marks( 0, size_array(sample) );
        CPPUNIT_ASSERT( a == L"8О ОП 4З ЗМЧ" );
    }
    { // case 3
        RollingStock::Model model;
        ChainInfo  chainInfo(model);
        MarkedCars markedCars(chainInfo);

        std::wstring sample[] = {
            L"7Х ЛВ",
            L"ОС",
            L"5В 8О ОС"
        };
        sampling( markedCars, sample, size_array(sample) );

        std::wstring a = markedCars.group_special_marks( 0, size_array(sample) );
        CPPUNIT_ASSERT( a == L"7Х ЛВ 8О ОС" );
    }
}
