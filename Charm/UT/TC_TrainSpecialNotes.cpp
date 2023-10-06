#include "stdafx.h"

#include "TC_TrainSpecialNotes.h"
#include "../helpful/TrainSpecialNotes.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainSpecialNotes );

void TC_TrainSpecialNotes::createSeparated()
{
    TrainSpecialNotes none;
    CPPUNIT_ASSERT( none.to_wstr() == L"" );
    CPPUNIT_ASSERT( !none.HaveHeavy() );
    CPPUNIT_ASSERT( !none.HaveDanger() );
    CPPUNIT_ASSERT( !none.HaveLong() );
    CPPUNIT_ASSERT( !none.HaveLongUp() );
    CPPUNIT_ASSERT( !none.HaveOversize() );
    CPPUNIT_ASSERT( !none.OnePersonMode() );

    for( int i=1; i < 0x1F; i<<=1 )
    {
        bool parUpLong = i&0x1 ? true : false;
        bool parLong   = i&0x2 ? true : false;
        bool parHeavy  = i&0x4 ? true : false;
        bool parDanger = i&0x8 ? true : false;
        bool parOversz = i&0x10? true : false;
        TrainSpecialNotes temp( parUpLong, parLong, parHeavy, parDanger, 0, parOversz );
        CPPUNIT_ASSERT( temp.to_wstr() != L"" );
        CPPUNIT_ASSERT( parHeavy == temp.HaveHeavy() );
        CPPUNIT_ASSERT( parDanger == temp.HaveDanger() );
        CPPUNIT_ASSERT( parLong == temp.HaveLong() );
        CPPUNIT_ASSERT( parUpLong == temp.HaveLongUp() );
        CPPUNIT_ASSERT( parOversz == temp.HaveOversize() );
        CPPUNIT_ASSERT( !(temp == none) );
    }
}

void TC_TrainSpecialNotes::createFromString()
{
    wstring source[] = { L"", L"НД", L"Н2508", L"ТНД", L"ТН2508ВМ", L"Т2508Д", L"ВМН25ПД", L"Н", };
    wstring target[] = { L"", L"ДН", L"Н2508", L"ДТН", L"ТВМН2508", L"ДТ",     L"ПДВМН25", L"Н", };
    CPPUNIT_ASSERT( size_array(source) == size_array(target) );

    for( size_t i=0; i < size_array(source); ++i )
    {
        TrainSpecialNotes temp( source[i] );
        CPPUNIT_ASSERT( temp.to_wstr() == target[i] );
    }
}

void TC_TrainSpecialNotes::custom()
{
    TrainSpecialNotes none;
    CPPUNIT_ASSERT( none.GetOversizeDown() == L"" );
    CPPUNIT_ASSERT( none.GetOversizeSide() == L"" );
    CPPUNIT_ASSERT( none.GetOversizeUp()   == L"" );
    CPPUNIT_ASSERT( none.GetOversizeMaxVertical() == L"" );

    TrainSpecialNotes more(L"Н1234ПДТ");
    CPPUNIT_ASSERT( more.GetOversizeDown() == L"1" );
    CPPUNIT_ASSERT( more.GetOversizeSide() == L"2" );
    CPPUNIT_ASSERT( more.GetOversizeUp()   == L"3" );
    CPPUNIT_ASSERT( more.GetOversizeMaxVertical() == L"4" );

    TrainSpecialNotes part(L"Н56");
    CPPUNIT_ASSERT( part.GetOversizeDown() == L"5" );
    CPPUNIT_ASSERT( part.GetOversizeSide() == L"6" );
    CPPUNIT_ASSERT( part.GetOversizeUp()   == L"" );
    CPPUNIT_ASSERT( part.GetOversizeMaxVertical() == L"" );
}
