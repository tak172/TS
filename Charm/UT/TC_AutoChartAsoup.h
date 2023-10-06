#ifndef TC_AUTOCHARTASOUP_H
#define TC_AUTOCHARTASOUP_H

#include <set>
#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"
#include "../Hem/HemHelpful.h"
#include "../helpful/Attic.h"
#include "UtTearDown.h"

//Тестирование AutoChart.AsoupLayer

class AutoChartTest;
class TC_AutoChartAsoup : 
	protected TC_ChartImpl,
	public UtTearDowner
{
public:
	TC_AutoChartAsoup();
	~TC_AutoChartAsoup();

private:
	TrackerInfrastructure & TI;
	boost::scoped_ptr<AutoChartTest> autoChart;
	CPPUNIT_TEST_SUITE( TC_AutoChartAsoup );
	CPPUNIT_TEST( AsoupMessage );
	CPPUNIT_TEST( ArrivingAsoupBeforeLOS );
	CPPUNIT_TEST( ArrivingAsoupAfterLOS );
	CPPUNIT_TEST( ParseRawAsoupText );
	CPPUNIT_TEST( WithdrawalAsoupMessage );
	CPPUNIT_TEST( WithdrawalDuplicateMessages );
	CPPUNIT_TEST( ShrinkAsoup );
	//CPPUNIT_TEST( UralMessage );
	CPPUNIT_TEST( ArrivingAsoupAfterLOSRealBadges );
	CPPUNIT_TEST( ServedChanges );
	CPPUNIT_TEST( WithdrawalAsoupMessageUndoGlue );
	CPPUNIT_TEST( WithdrawalAsoupMessageUndoGlueWithSpanStopping );
	CPPUNIT_TEST_SUITE_END();
//
protected:
	void AsoupMessage(); //простая проверка для АСОУП события
	void ArrivingAsoupBeforeLOS(); //АСОУП-сообщение о прибытии подвижной единицы ДО сигнала занятости
	void ArrivingAsoupAfterLOS(); //АСОУП-сообщение о прибытии подвижной единицы ПОСЛЕ сигнала занятости
	void ParseRawAsoupText(); //разбор сообщения фонда о новом АСОУП сообщении, проверка формата выходного документа
	void WithdrawalAsoupMessage(); //разбор сообщения об отмене другого АСОУП сообщения (текущие ожидания)
	void WithdrawalDuplicateMessages(); //проверка поведения при дублировании сообщений АСОУП
	void ShrinkAsoup(); // проверка удаления старых сообщений
	//void UralMessage(); //проверка для ГИД-Урал события
	void ArrivingAsoupAfterLOSRealBadges();
	void ServedChanges();
	void WithdrawalAsoupMessageUndoGlue();
	void WithdrawalAsoupMessageUndoGlueWithSpanStopping();

private:
	bool PassAsoupMessageThrough(const std::wstring& messageText);
};
#endif // TC_AUTOCHARTASOUP_H