#ifndef TC_HEMASOUP_H
#define TC_HEMASOUP_H

#include <cppunit/extensions/HelperMacros.h>

//Проверка правильного наложения АСОУП-сообщения
//на соответствующую нить в модуле Hem (HappenLayer)

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
	void ArrivingAsoupBeforeLOS(); //АСОУП-сообщение о прибытии подвижной единицы ДО сигнала занятости
	void ArrivingAsoupAfterLOS(); //АСОУП-сообщение о прибытии подвижной единицы ПОСЛЕ сигнала занятости
};
#endif // TC_HEMASOUP_H