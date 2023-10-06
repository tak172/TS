#ifndef TC_GUESSTRANS_H
#define TC_GUESSTRANS_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/GuessTransciever.h"
#include "TrackerInfrastructure.h"
#include "../Hem/HemHelpful.h"

//�������� ������ GuessTransciever (�������� ��������������� �� + ���������� ������������� ������� ������������)

class GuessTranscieverTest : public Hem::GuessTransciever
{
public:
};

class TC_GuessTransciever : public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_GuessTransciever );
    CPPUNIT_TEST( TestReduce );
    CPPUNIT_TEST( TestPathChange );
	CPPUNIT_TEST_SUITE_END();

	GuessTranscieverTest guessTransciever;

    void TestReduce();  //������� ���������� ���������
    void TestPathChange(); //����� ����� ��� ID
};
#endif // TC_GUESSTRANS_H