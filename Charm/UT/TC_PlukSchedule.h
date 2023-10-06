#pragma once
#include <cppunit/extensions/HelperMacros.h>
#include "../Pluk/PlukDataBase.h"

class TC_PlukSchedule :
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE(TC_PlukSchedule);
#if _WIN64
    CPPUNIT_TEST( getList );
	CPPUNIT_TEST(eventOrder);
    CPPUNIT_TEST(trainOrder);
#endif
    CPPUNIT_TEST_SUITE_END();

public:
	void getList();
    void eventOrder();
    void trainOrder();

	std::shared_ptr<PlukDataBase> GetPlukDataBase(const std::string& stringConnect);
	void setUp() override;
	void tearDown() override;
};
