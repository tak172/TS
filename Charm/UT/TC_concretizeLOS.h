#ifndef TC_LOS_CONCRETIZE_H
#define TC_LOS_CONCRETIZE_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrKit.h"
#include "../helpful/Badge.h"
/* 
 * Проверка подстройки LOS'ов для различных логических объектов
 */

class TC_concretizeLOS : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_concretizeLOS );
  CPPUNIT_TEST( fab_head );
//   CPPUNIT_TEST( fab_route );
  CPPUNIT_TEST( fab_strip );
  CPPUNIT_TEST( fab_landscape );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void fab_head();
//     void fab_route();
    void fab_strip();
    void fab_landscape();
private:
    void make_concrete_los();

    std::vector<CLogicElement*> elem;
    std::vector<LOS_base *> los;
};


#endif // TC_LOS_CONCRETIZE_H