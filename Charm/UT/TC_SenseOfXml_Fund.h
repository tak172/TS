#ifndef TC_SENSEOFXML_FUND_H
#define TC_SENSEOFXML_FUND_H

#include <cppunit/extensions/HelperMacros.h>

//Тестирование XML-команд для Hem

class TC_SenseOfXml_Fund : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_SenseOfXml_Fund );
    CPPUNIT_TEST( check_PostSlice_toOutlet );
    CPPUNIT_TEST( compatible_Backup );
    CPPUNIT_TEST_SUITE_END();

protected:
    void check_PostSlice_toOutlet();
    void compatible_Backup();
};

#endif // TC_SENSEOFXML_FUND_H