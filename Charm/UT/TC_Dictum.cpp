#include "stdafx.h"

#include "TC_Dictum.h"
#include "../helpful/Dictum.h"
#include "../BlackBox/bbx_BlackBox.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Dictum );

void TC_Dictum::setUp()
{
    char lts_etalon[] = { 
        DICTUM_KIND::H2F_LTS,         // DICTUM_KIND
        0x78,0,0x56,0,0x34,0, 0x12,0, // moment
        true,                         // support
        'x','y','z',0,                // "xyz"
        0x44, 0x33, 0x22, 0x11,       // 0x11223344
        StateLTS::ACTIVE,
        'q',0,                        // "q"
        0x4, 0x3, 0x2, 0x1,       // 0x01020304
        StateLTS::PASSIVE
    };
    etalon1.assign( lts_etalon, lts_etalon+size_array(lts_etalon) );

    char lts2_etalon[] = { 
        DICTUM_KIND::H2F_LTS,         // DICTUM_KIND
        0x77,0,0x56,0,0x34,0, 0x12,0, // moment
        false,                        // support
        'z',0,                        // "q"
        0x4, 0x3, 0x2, 0x5,           // 0x05020304
        StateLTS::ACTIVE,
    };
    etalon2.assign( lts2_etalon, lts2_etalon+size_array(lts2_etalon) );
}

void TC_Dictum::compatible_output()
{
    DICTUM_LTS out_header;
    out_header.moment = 0x12003400560078;
    out_header.support = true;
    LOS_AREA_TYPE area = out_header.vectorize();
    BadgeU( L"xyz", 0x11223344 ).to_area(area);
    area.push_back( StateLTS(StateLTS::ACTIVE).to_char() );
    BadgeU( L"q",0x1020304 ).to_area(area);
    area.push_back( StateLTS(StateLTS::PASSIVE).to_char() );
    CPPUNIT_ASSERT( etalon1 == area );
}

void TC_Dictum::compatible_input()
{
    // re-input and check data
    DICTUM_LTS in_header;
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == Dictums::detect( etalon1.front() ) );
    CPPUNIT_ASSERT( in_header.peek( etalon1.begin(), etalon1.end() ) );
    CPPUNIT_ASSERT( 0x12003400560078 == in_header.moment );
    CPPUNIT_ASSERT( true == in_header.support );
    BadgeU my_lts;
    StateLTS my_val;
    auto it=etalon1.cbegin() + in_header.sizing();

    it = my_lts.from_area( it, etalon1.end() );
    my_val=StateLTS( *it );
    ++it;
    CPPUNIT_ASSERT( BadgeU( L"xyz", 0x11223344 ) == my_lts );
    CPPUNIT_ASSERT( StateLTS::ACTIVE == my_val );

    it = my_lts.from_area( it, etalon1.end() );
    my_val=StateLTS( *it );
    ++it;
    CPPUNIT_ASSERT( BadgeU( L"q",0x1020304 ) == my_lts );
    CPPUNIT_ASSERT( StateLTS::PASSIVE == my_val );
    
    CPPUNIT_ASSERT( etalon1.cend() == it );
}

void TC_Dictum::read_from_rich()
{
    LOS_AREA_TYPE temp;
    // подготовка данных
    using namespace std;
    string prefix = "<RichDictum>"
                    "    <Op Len='26' BbxId='1073741824' />"
                    "    <Op Len='17' BbxId='536870912' />"
                    "</RichDictum>";
    LOS_AREA_TYPE v;
    v.insert( v.end(), prefix.begin(), prefix.end() );
    v.push_back( 0 );
    v.insert( v.end(), etalon1.begin(), etalon1.end() );
    v.insert( v.end(), etalon2.begin(), etalon2.end() );

    // тестируемый экземпл€р
    RichDictum rd( Bbx::Identifier(), std::move(v), time_t(0) );
    // перва€ посылка
    CPPUNIT_ASSERT( !rd.empty() );
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == rd.kind() );
    CPPUNIT_ASSERT( Bbx::Identifier(Bbx::Identifier::FundOutput) == rd.BbxId() );
    temp.assign( rd.dataBegin(), rd.dataEnd() );
    CPPUNIT_ASSERT( temp == etalon1 );
    rd.pop();
    // втора€ посылка
    CPPUNIT_ASSERT( !rd.empty() );
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == rd.kind() );
    CPPUNIT_ASSERT( Bbx::Identifier(Bbx::Identifier::FundInput) == rd.BbxId() );
    temp.assign( rd.dataBegin(), rd.dataEnd() );
    CPPUNIT_ASSERT( temp == etalon2 );
    rd.pop();
    // больше ничего нет
    CPPUNIT_ASSERT( rd.empty() );
}

void TC_Dictum::write_to_rich()
{
    LOS_AREA_TYPE flat;
    {
        RichDictum rd;
        rd.push_back(Bbx::Identifier(Bbx::Identifier::FundOutput), etalon1.begin(), etalon1.end() );
        rd.push_back(Bbx::Identifier(Bbx::Identifier::FundInput ), etalon2.begin(), etalon2.end() );
        flat = rd.extract();
    }

    LOS_AREA_TYPE temp;
    RichDictum re( Bbx::Identifier(), std::move(flat), time_t(0) );
    // перва€ посылка
    CPPUNIT_ASSERT( !re.empty() );
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == re.kind() );
    CPPUNIT_ASSERT( Bbx::Identifier(Bbx::Identifier::FundOutput) == re.BbxId() );
    temp.assign( re.dataBegin(), re.dataEnd() );
    CPPUNIT_ASSERT( temp == etalon1 );
    re.pop();
    // втора€ посылка
    CPPUNIT_ASSERT( !re.empty() );
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == re.kind() );
    CPPUNIT_ASSERT( Bbx::Identifier(Bbx::Identifier::FundInput) == re.BbxId() );
    temp.assign( re.dataBegin(), re.dataEnd() );
    CPPUNIT_ASSERT( temp == etalon2 );
    re.pop();
    // больше ничего нет
    CPPUNIT_ASSERT( re.empty() );
}

void TC_Dictum::rich_and_legacy()
{
    LOS_AREA_TYPE myCopy = etalon1;
    RichDictum rd( Bbx::Identifier(), std::move(myCopy), time_t(0) );
    // перва€ посылка
    CPPUNIT_ASSERT( !rd.empty() );
    CPPUNIT_ASSERT( DICTUM_KIND::H2F_LTS == rd.kind() );
    CPPUNIT_ASSERT( Bbx::Identifier() == rd.BbxId() );
    LOS_AREA_TYPE temp;
    temp.assign( rd.dataBegin(), rd.dataEnd() );
    CPPUNIT_ASSERT( temp == etalon1 );
    rd.pop();
    // больше ничего нет
    CPPUNIT_ASSERT( rd.empty() );
}
