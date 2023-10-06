#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class TC_GraphicValidator :
	public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_GraphicValidator );
	CPPUNIT_TEST( CorrectUnmatchedSwitchesBDef );
	CPPUNIT_TEST( CorrectUnmatchedSwitchesBUndef );
	CPPUNIT_TEST( NoCorrectLSwitchWOLinks );
	CPPUNIT_TEST( SemaWithLegNAlienHeadsLogic );
	CPPUNIT_TEST( SemaWOLegOneHeadLogic );
	CPPUNIT_TEST( SemaWOLegManyHeadLogic );
	CPPUNIT_TEST( SemaWOLegManyTrainHeadLogic );
	CPPUNIT_TEST_SUITE_END();

	void CorrectUnmatchedSwitchesBDef(); //на графической стрелке перепутаны плюс и минус (при определенном базовом участке)
	void CorrectUnmatchedSwitchesBUndef(); //на графической стрелке перепутаны плюс и минус (при неопределенном базовом участке)
	void NoCorrectLSwitchWOLinks(); //отсутствие коррекции для логической стрелки без связей
	void CorrectUnmatchedSwitches( bool base_defined );

	//на графическом светофоре размечена мачта, и одна или несколько голов размечена объектом, входящим в другой логический светофор (отностельно связи мачты):
	void SemaWithLegNAlienHeadsLogic();
	//на графическом светофоре размечена только одна голова:
	void SemaWOLegOneHeadLogic();
	//на графическом светофоре размечено несколько разнотипных голов от разных логических светофоров:
	void SemaWOLegManyHeadLogic();
	//на графическом светофоре размечено несколько поездных голов от разных логических светофоров:
	void SemaWOLegManyTrainHeadLogic();
};