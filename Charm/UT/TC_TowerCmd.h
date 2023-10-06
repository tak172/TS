#ifndef TC_TOWERCMD_H
#define TC_TOWERCMD_H

#include <cppunit/extensions/HelperMacros.h>

/* 
* Проверка TowerCmd
 */

class TC_TowerCmd : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TowerCmd );
  CPPUNIT_TEST( split );
  CPPUNIT_TEST( timeout );
  CPPUNIT_TEST( decode_LOS );
  CPPUNIT_TEST( build );
  CPPUNIT_TEST( parse_setObject );
  CPPUNIT_TEST( parse_listTrain );
  CPPUNIT_TEST( parse_listVan );
  CPPUNIT_TEST( parse_start );
  CPPUNIT_TEST( parse_time );
  CPPUNIT_TEST( parse_pult_split );
//  CPPUNIT_TEST( parse_pult_set );
  CPPUNIT_TEST_SUITE_END();

protected:
    void split();
    void timeout();
    void decode_LOS();
    void build();

    void parse_setObject();
    void parse_listTrain();
    void parse_listVan();
    void parse_start();
    void parse_time();
    void parse_pult_split();
    void parse_pult_set();
};


#endif // TC_TOWERCMD_H