#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Hem_aeLimit : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeLimit );
    CPPUNIT_TEST( Create );
    CPPUNIT_TEST( Replace );
    CPPUNIT_TEST( Remove );
    CPPUNIT_TEST( Expire );
	CPPUNIT_TEST_SUITE_END();

private:
	void Create();  // �������� ��������������
    void Replace(); // ��������� ��������������
    void Remove();  // �������� ��������������
    void Expire();  // ��������� ��������������
};
