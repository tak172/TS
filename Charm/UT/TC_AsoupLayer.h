#pragma once 

#include <cppunit/extensions/HelperMacros.h>
class TC_AsoupLayer : public CPPUNIT_NS::TestFixture
{
private:
    CPPUNIT_TEST_SUITE( TC_AsoupLayer );
    CPPUNIT_TEST( Push );
    CPPUNIT_TEST( ParkWayKitUsage );
    CPPUNIT_TEST( Served );
    CPPUNIT_TEST( Serialization );
    CPPUNIT_TEST( DoubleSerialization );
    CPPUNIT_TEST( RealExample );
    CPPUNIT_TEST( Doubles );
    CPPUNIT_TEST( UnlinkEvents );
    CPPUNIT_TEST( StrangeTrindex );
    CPPUNIT_TEST( NAKDetection );
    CPPUNIT_TEST( RemoveEvent );
    CPPUNIT_TEST( AsoupUniqueness );
    // Функционал ещё не реализован
    CPPUNIT_TEST( WithdrawalSearch );
    CPPUNIT_TEST( TestCreateAsoup );
	CPPUNIT_TEST( CreateAsoupU3 );
    CPPUNIT_TEST_SUITE_END();

    void Push();
    void ParkWayKitUsage();
    void Served();
    void Serialization();
    void DoubleSerialization();
    void RealExample();
    void Doubles();
    void UnlinkEvents();
    void StrangeTrindex();
    void NAKDetection();
    void RemoveEvent();
    void AsoupUniqueness();
    void WithdrawalSearch();
    void TestCreateAsoup();
	void CreateAsoupU3();
};
