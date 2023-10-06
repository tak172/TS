#ifndef TC_TERRACHART_H
#define TC_TERRACHART_H

#include <cppunit/extensions/HelperMacros.h>

//���� TerraChart ���������� � �������� XML-�����
class TC_TerraChart : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_TerraChart );
	CPPUNIT_TEST( LoadSave );
    CPPUNIT_TEST( ReduceAmountDncDspAttendance );
	CPPUNIT_TEST_SUITE_END();

protected:
	void LoadSave(); // ���� ���������� � �������� �� xml
    void ReduceAmountDncDspAttendance(); // ���� �������� ������ ������� DncDspAttendance
};

#endif // TC_TERRACHART_H