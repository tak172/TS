#ifndef TC_LEXICON_H
#define TC_LEXICON_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrKit.h"
#include "../helpful/Badge.h"
/* 
 * Проверка класса Lexicon
 */

class CLogicElement;
class Lexicon;
class TC_Lexicon : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Lexicon );
  CPPUNIT_TEST( LogEl_vs_Badge );
  CPPUNIT_TEST( change_all );
  CPPUNIT_TEST( change_single );
  CPPUNIT_TEST( append_repeatly );
  CPPUNIT_TEST( putAllXml );
  CPPUNIT_TEST( putChangesXml );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void LogEl_vs_Badge();
    void change_all();
    void change_single();
    void append_repeatly();
    void putAllXml();
    void putChangesXml();
private:
    EsrKit                          ESR;
    std::vector<CLogicElement*>     OBJS;
    std::vector<BadgeE>             BADGES;
    typedef boost::tuple<CLogicElement*, CLogicElement*, CLogicElement*> MIX;
    std::vector< MIX > MIXS;

    void put_test_objects_to( Lexicon &lex );
};


#endif // TC_LEXICON_H