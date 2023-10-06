#include "stdafx.h"
#include "TC_Picketing.h"
#include "../helpful/PicketingInfo.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Picketing );

void TC_Picketing::XmlReadSave_normal()
{
    bool succ = XmlReadSave(
        L"<Std  picketing1_val='111~111' picketing1_comm='������' />" // �����
        ,
        L"<Std  picketing1_val='111~111' picketing1_comm='������' />"
        );
    CPPUNIT_ASSERT( succ );
}

void TC_Picketing::XmlReadSave_bigNumber()
{
    bool succ = XmlReadSave(
        L"<Up  picketing7_val='777~777' picketing7_comm='������' "    // �����, ��������� ��������
        L"     picketing5_val='555~555' picketing5_comm='���-��' />"
        ,
        L"<XX  picketing1_val='555~555' picketing1_comm='���-��' "
        L"     picketing2_val='777~777' picketing2_comm='������' />"
        );
    CPPUNIT_ASSERT( succ );
}

void TC_Picketing::XmlReadSave_repeatDirection()
{
    bool succ1 = XmlReadSave(
        L"<Up  picketing7_val='777~777' picketing7_comm='������' "    // ��������� �����������, ��������� ���������
        L"     picketing5_val='555~555' picketing5_comm='������' />"
        ,
        L"<YY  picketing1_val='777~777' picketing1_comm='������' />"
        );
    bool succ2 = XmlReadSave(
        L"<Up  picketing7_val='777~777' picketing7_comm='������' "    // ��������� �����������, ��������� ���������
        L"     picketing5_val='555~555' picketing5_comm='������' />"
        ,
        L"<YY  picketing1_val='555~555' picketing1_comm='������' />"
        );
    CPPUNIT_ASSERT( succ1 || succ2 );
}

void TC_Picketing::XmlReadSave_inconsistence()
{
    bool succ = XmlReadSave(
        L"<Up  picketing5_val='777~777' picketing7_comm='������' "    // �������� ��������
        L"     picketing1_val='555~555' picketing3_comm='������' />"
        ,
        L"<ZZ />"
        );
    CPPUNIT_ASSERT( succ );
}


void TC_Picketing::Median()
{
    PicketingInfo pi1, pi2;
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("500~25") ) );
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("777~77") ) );
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("987~97") ) );

    pi2.SetCoord( rwRuledCoord(L"����", rwCoord("778~77") ) );
    pi2.SetCoord( rwRuledCoord(L"���!", rwCoord("501~25") ) );
    pi2.SetCoord( rwRuledCoord(L"����", rwCoord("988~97") ) );

    PicketingInfo med = PicketingInfo::getMedian( pi1, pi2 );
    CPPUNIT_ASSERT( !med.empty() );
    rwRuledCoord rc;
    CPPUNIT_ASSERT( med.GetFirstCoord( rc ) );
    CPPUNIT_ASSERT( rc.ruler() == L"����" );
    CPPUNIT_ASSERT( rc.coord() == rwCoord("987~597") );
}


void TC_Picketing::Intersect()
{
    PicketingInfo pi1, pi2;
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("111~11") ) );
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("111~22") ) );
    pi1.SetCoord( rwRuledCoord(L"����", rwCoord("111~33") ) );
    PicketingInfo self = pi1.intersect( pi1 );
    CPPUNIT_ASSERT( self == pi1 );

    pi2.SetCoord( rwRuledCoord(L"����", rwCoord("222~44") ) );
    pi2.SetCoord( rwRuledCoord(L"����", rwCoord("222~55") ) );
    pi2.SetCoord( rwRuledCoord(L"����", rwCoord("222~66") ) );

    PicketingInfo res1 = pi1.intersect( pi2 );
    CPPUNIT_ASSERT( res1.GetCoord( L"����" ) == rwCoord("111~22") );
    CPPUNIT_ASSERT( res1.GetCoord( L"����" ) == rwCoord("111~11") );
    PicketingInfo res2 = pi2.intersect( pi1 );
    CPPUNIT_ASSERT( res2.GetCoord( L"����" ) == rwCoord("222~55") );
    CPPUNIT_ASSERT( res2.GetCoord( L"����" ) == rwCoord("222~44") );
}


void TC_Picketing::GetCoord()
{
    PicketingInfo pk;
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("500~25") ) );
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("777~77") ) );
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("987~97") ) );
    CPPUNIT_ASSERT( pk.GetCoord( L"����" ) == rwCoord("777~77") );
    CPPUNIT_ASSERT( pk.GetCoord( L"����" ) == rwCoord("500~25") );
    CPPUNIT_ASSERT( pk.GetCoord( L"����" ) == rwCoord("987~97") );
    CPPUNIT_ASSERT( pk.GetCoord( L"--��" ) == rwCoord() );
}


void TC_Picketing::FirstCoord()
{
    PicketingInfo pk;
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("500~25") ) );
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("777~77") ) );
    rwRuledCoord temp(L"����", rwCoord("987~97") );
    pk.SetCoord( temp );

    rwRuledCoord rwc;
    CPPUNIT_ASSERT( pk.GetFirstCoord( rwc ) );
    CPPUNIT_ASSERT( rwc == temp );
}


void TC_Picketing::Wstring()
{
    PicketingInfo pk;
    pk.SetCoord( rwRuledCoord(L"�� �", rwCoord("123~45") ) );
    pk.SetCoord( rwRuledCoord(L"����", rwCoord("678~90") ) );

    std::wstring ws = pk.to_wstring();
    CPPUNIT_ASSERT( ws == L"678~90<����>,123~45<�� �>" );

    ws = PicketingInfo().to_wstring();
    CPPUNIT_ASSERT( ws == L"" );
}

bool TC_Picketing::XmlReadSave( const std::wstring& intext, const std::wstring& outtext )
{
    attic::a_document indoc;
    indoc.load_wide( intext );
    attic::a_document outdoc;
    outdoc.load_wide( outtext );
    std::wstring etalon = outdoc.pretty_wstr();

    PicketingInfo pi;
    pi.readFromXml( indoc.document_element() );

    attic::a_document temp( outdoc.document_element().name() );
    //������ � ������ ��������
    pi.saveToXml( temp.document_element() );
    std::wstring result1 = temp.pretty_wstr();

    //������ � �������� ��������
    pi.saveToXml( temp.document_element() );
    std::wstring result2 = temp.pretty_wstr();
    auto mismresult = std::mismatch( etalon.cbegin(), etalon.cend(), result1.cbegin(), result1.cend() );

    return etalon == result1 && etalon==result2;
}
