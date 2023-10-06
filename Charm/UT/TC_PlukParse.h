#pragma once
#include <string>
#include <memory>
#include <cppunit/extensions/HelperMacros.h>
#include "../Pluk/PlukDataBase.h"

class TC_PlukParse :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukParse);
#if _WIN64
    CPPUNIT_TEST( CreateService );
 	CPPUNIT_TEST(ParseHem);
 	CPPUNIT_TEST(ParseAsoup);
 	CPPUNIT_TEST(ParseBusy);
 	CPPUNIT_TEST(ParseSpeedLimit);
 	CPPUNIT_TEST(ParseForbid);
 	CPPUNIT_TEST(ParseMark);
    //CPPUNIT_TEST(ReplayLog);
#endif
    CPPUNIT_TEST_SUITE_END();

public:

	void CreateService();
	void ParseHem();
	void ParseAsoup();
	void ParseBusy();
	void ParseSpeedLimit();
	void ParseForbid();
	void ParseMark();
    void ReplayLog(); // прокрутка лога Pluk

	std::shared_ptr<PlukDataBase> GetPlukDataBase(const std::string& stringConnect);

	void setUp() override;
	void tearDown() override;
};
