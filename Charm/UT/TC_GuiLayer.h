#pragma once

#include <cppunit/extensions/HelperMacros.h>

namespace Gui { class Event; };
// проверка класса
class TC_GuiLayer : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_GuiLayer );
    CPPUNIT_TEST( Reload );
    CPPUNIT_TEST( MessageToHaron );
    CPPUNIT_TEST( UpdateEvent );
	CPPUNIT_TEST( CreateDelayMarkAndSoanStopping );
    CPPUNIT_TEST_SUITE_END();

private:
    void Reload();
    void MessageToHaron();
    void UpdateEvent();
	void CreateDelayMarkAndSoanStopping();
};
