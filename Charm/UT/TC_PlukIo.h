#pragma once
#include <string>
#include <memory>
#include <cppunit/extensions/HelperMacros.h>
#include "../Pluk/PlukDataBase.h"
#include "UtTearDown.h"

class TC_PlukIo :
	public UtTearDowner
{
	CPPUNIT_TEST_SUITE(TC_PlukIo);
#if _WIN64
    CPPUNIT_TEST( InOutEvent );
    CPPUNIT_TEST( InOutPvh);
    CPPUNIT_TEST( ChangeLoco);
    CPPUNIT_TEST( CheckLocomotiveForEventEqualTime_6192 );
    CPPUNIT_TEST( CheckWriteAsoupFeatAndLoco_6194 );
    CPPUNIT_TEST( CheckWriteAsoupSomeLocomotives_6194 );
    CPPUNIT_TEST( CheckLocomotiveForFirstEventEqualTime_6192 );
    CPPUNIT_TEST( CheckMarkWriteRead_6210 );
    CPPUNIT_TEST( CheckEditAsoup);
    CPPUNIT_TEST( CheckDeleteAsoup);
    CPPUNIT_TEST( CheckDeleteLiter_6192);
    CPPUNIT_TEST( Check_DD_Attendance );
    CPPUNIT_TEST( CheckTrainWarning );

#ifdef _REMOVE_ALL_OBJECT_PROPERTY_
    CPPUNIT_TEST( DelEventProperty );
#endif

#endif
    CPPUNIT_TEST_SUITE_END();

public:

	void InOutEvent();
    void InOutPvh();
    void ChangeLoco();
    void DelEventProperty();
    void CheckLocomotiveForEventEqualTime_6192();
    void CheckLocomotiveForFirstEventEqualTime_6192();
    void CheckWriteAsoupFeatAndLoco_6194();
    void CheckWriteAsoupSomeLocomotives_6194();
    void CheckMarkWriteRead_6210();
    void CheckEditAsoup();
    void CheckDeleteAsoup();
    void CheckDeleteLiter_6192();
    void Check_DD_Attendance();
    void CheckTrainWarning();

	std::shared_ptr<PlukDataBase> GetPlukDataBase(const std::string& stringConnect);

	void setUp() override;
};
