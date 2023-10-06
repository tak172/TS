#ifndef TC_NIIASXML_H
#define TC_NIIASXML_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../Fund/ExcerptTricolor.h"
#include "../Fund/ExcerptWarning.h"
#include "../Fund/ExcerptWindow.h"
#include "../Fund/GidUraler.h"
#include "../Fund/SupplyTrainInfo.h"
#include "../Fund/UralParser/StripShot.h"
#include "../StatBuilder/basic_defs.h"

/* 
 * Класс для проверки работы класса KmPk
 */

class TC_NiiasXml : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_NiiasXml );
  CPPUNIT_TEST( getStrip );
  CPPUNIT_TEST( test12Ktsm );
  CPPUNIT_TEST( test13warning );
  CPPUNIT_TEST( test08Window );
  CPPUNIT_TEST( getHtml );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void getStrip();
    void test08Window();
    void test12Ktsm();
    void test13warning();
    void getHtml();
private:
    void fill_giduraler(const attic::a_document& xdoc);
    bool checkObjState( const wchar_t * obj_name, int checked_state );
    bool checkWinPhase( const wchar_t * obj_name, WINDOW_PHASE checked_phase );

    enum FIG_STATE{
        NONE,
        BACKGROUND,
        LIGHT1,
        LIGHT2
    };

    attic::a_document doc1;
    attic::a_document doc2;

    GIDURALER    giduraler;
    SupplyTrainInfo out_SuppInfo;
    boost::posix_time::ptime moment;
};


#endif // TC_NIIASXML_H