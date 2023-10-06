#include "stdafx.h"
#include "../helpful/DuePath.h"
#include "TC_DuePath.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_DuePath );

void TC_DuePath::actor_64_noext()
{
    auto res = DuePath( L"K:\\UU\\Charm\\_out_\\Actor64.Debug\\Actor64.exe", true, L"Fund" );
    CPPUNIT_ASSERT( 5 == res.size() );
    CPPUNIT_ASSERT( res[ 0 ] == L"Fund" ); // (�������� ������)
    CPPUNIT_ASSERT( res[ 1 ] == L"K:/UU/Charm/_out_/Fund64.Debug/Fund64.exe"  ); // (�� ������, ������)
    CPPUNIT_ASSERT( res[ 2 ] == L"K:/UU/Charm/_out_/Fund64.Debug/Fund.exe"    ); // (�� ������, ��������������)
    CPPUNIT_ASSERT( res[ 3 ] == L"K:/UU/Charm/_out_/Actor64.Debug/Fund64.exe" ); // (����� ������, ������)
    CPPUNIT_ASSERT( res[ 4 ] == L"K:/UU/Charm/_out_/Actor64.Debug/Fund.exe"   ); // (����� ������, ��������������)
}

void TC_DuePath::hem_64_dll()
{
    // �������������� ���� � �������������� ������
    auto res = DuePath( L"C:\\Retime\\Charm\\Fund.exe", true, L"Hem.dll" ); 
    CPPUNIT_ASSERT( 2 == res.size() );
    CPPUNIT_ASSERT( res[ 0 ] == L"Hem.dll" ); // (�������� ������)
    CPPUNIT_ASSERT( res[ 1 ] == L"C:/Retime/Charm/Hem64.dll" ); // (����� ������, ������)
}


void TC_DuePath::guess_32_dll()
{
    // ������������� ��� � �������� ������
    auto res = DuePath( L"K:\\UU\\Charm\\_out_\\Fund.Debug\\Fund.exe", false, L"Guess64.dll" );
    CPPUNIT_ASSERT( 3 == res.size() );
    CPPUNIT_ASSERT( res[ 0 ] == L"Guess64.dll" ); // (�������� ������)
    CPPUNIT_ASSERT( res[ 1 ] == L"K:/UU/Charm/_out_/Guess.Debug/Guess.dll" ); // (�� ������ �������)
    CPPUNIT_ASSERT( res[ 2 ] == L"K:/UU/Charm/_out_/Fund.Debug/Guess.dll" ); // (��������������� ��� � ����� source)
}
