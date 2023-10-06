#ifndef TC_HEMASOUP_H
#define TC_HEMASOUP_H

#include <cppunit/extensions/HelperMacros.h>

//�������� ����������� ��������� �����-���������
//�� ��������������� ���� � ������ Hem (HappenLayer)

class TC_HemAsoup : public CPPUNIT_NS::TestFixture
{
public:
	virtual void tearDown();

private:
	CPPUNIT_TEST_SUITE( TC_HemAsoup );
	CPPUNIT_TEST( ArrivingAsoupBeforeLOS );
	CPPUNIT_TEST( ArrivingAsoupAfterLOS );
	CPPUNIT_TEST_SUITE_END();

protected:
	void ArrivingAsoupBeforeLOS(); //�����-��������� � �������� ��������� ������� �� ������� ���������
	void ArrivingAsoupAfterLOS(); //�����-��������� � �������� ��������� ������� ����� ������� ���������
};
#endif // TC_HEMASOUP_H