#include "stdafx.h"

#include "TC_RestrictIp.h"
#include "../Haron/RestrictIp.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_RestrictIp );

using namespace boost::asio::ip;

void TC_RestrictIp::single()
{
    u_long use_ip = address_v4::from_string("11.22.33.44").to_ulong();
    u_long bad_ip = address_v4::from_string("77.66.55.44").to_ulong();
    RestrictIpUnit unit;
    unit.setIP( use_ip );

    CPPUNIT_ASSERT(  unit.include(use_ip) );
    CPPUNIT_ASSERT( !unit.include(bad_ip) );
    CPPUNIT_ASSERT( !unit.include(0) );
}

void TC_RestrictIp::group()
{
    DWORD use_ip = address_v4::from_string("11.22.33.44").to_ulong();
    DWORD ok__ip = address_v4::from_string("11.22.33.111").to_ulong();
    DWORD bad_ip = address_v4::from_string("88.88.33.44").to_ulong();
    DWORD sub_ip = address_v4::from_string("255.255.255.0").to_ulong();
    RestrictIpUnit unit;
    unit.setIP( use_ip );
    unit.setMask( sub_ip );

    CPPUNIT_ASSERT( unit.include(use_ip) );
    CPPUNIT_ASSERT( unit.include(ok__ip) );
    CPPUNIT_ASSERT( !unit.include(bad_ip) );
}

void TC_RestrictIp::getters()
{
    DWORD use_ip = address_v4::from_string("11.22.33.44").to_ulong();
    DWORD sub_ip = address_v4::from_string("255.255.255.0").to_ulong();

    RestrictIpUnit unit;
    CPPUNIT_ASSERT( unit.strIP()==L"0.0.0.0" );
    CPPUNIT_ASSERT( unit.strMask()==L"255.255.255.255" );

    unit.setIP( use_ip );
    CPPUNIT_ASSERT( unit.strIP()==L"11.22.33.44" );
    CPPUNIT_ASSERT( unit.strMask()==L"255.255.255.255" );

    unit.setMask( sub_ip );
    CPPUNIT_ASSERT( unit.strIP()==L"11.22.33.44" );
    CPPUNIT_ASSERT( unit.strMask()==L"255.255.255.0" );
}

void TC_RestrictIp::from_template()
{
    RestrictIpUnit unit;

    unit = RestrictIpGroup::fromTemplate(L"123.124.125.126/24");
    CPPUNIT_ASSERT( unit.strIP()==L"123.124.125.126" );
    CPPUNIT_ASSERT( unit.strMask()==L"255.255.255.0" );

    unit = RestrictIpGroup::fromTemplate(L"0.0.0.0/0");
    CPPUNIT_ASSERT( unit.strIP()==L"0.0.0.0" );
    CPPUNIT_ASSERT( unit.strMask()==L"0.0.0.0" );

    unit = RestrictIpGroup::fromTemplate(L"1.2.3.4/255.255.0.0");
    CPPUNIT_ASSERT( unit.strIP()==L"1.2.3.4" );
    CPPUNIT_ASSERT( unit.strMask()==L"255.255.0.0" );
}
