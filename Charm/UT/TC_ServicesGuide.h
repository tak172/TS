#pragma once

#include <cppunit/extensions/HelperMacros.h>

// ����� ��� �������������� �����
class TC_ServicesGuide : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_ServicesGuide );
    CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
};
