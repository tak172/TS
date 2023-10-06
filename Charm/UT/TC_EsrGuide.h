#ifndef TC_ESRGUIDE_H
#define TC_ESRGUIDE_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../helpful/PicketingInfo.h"

/* 
 * Класс для проверки работы класса EsrGuide
 */

class TC_EsrGuide : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_EsrGuide );
  CPPUNIT_TEST( GetSet );
  CPPUNIT_TEST( LoadSaveCompatible );
  CPPUNIT_TEST( LoadSaveSpecialization );
  CPPUNIT_TEST( GetSpanForTerminals );
  CPPUNIT_TEST_SUITE_END();

public:
  virtual void setUp() override;

protected:
	void GetSet();
    void LoadSaveCompatible();
    void LoadSaveSpecialization();
    void GetSpanForTerminals();

private:
    EsrKit k1;
    EsrKit k2;
    EsrKit k12;
    EsrKit k3;
    EsrKit k23;
    PicketingInfo p1;
    PicketingInfo p2;
    PicketingInfo p3;
};


#endif // TC_ESRGUIDE_H