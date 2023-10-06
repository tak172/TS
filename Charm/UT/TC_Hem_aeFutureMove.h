#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeFutureMove : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeFutureMove );
    CPPUNIT_TEST( duplicate );
    CPPUNIT_TEST( reverse );
    CPPUNIT_TEST( rename );
	CPPUNIT_TEST_SUITE_END();

private:
    void duplicate(); // ����������� ���� ������� �������� ������ ������� (����� ��������� ������) #5116
    void reverse(); // ����������� ���� ������� �������� �������� ����������� ���� �� ������� #5121
    void rename(); // �������� ����� ����� ������� ������
};
