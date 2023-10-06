#ifndef TC_ATTIC_H
#define TC_ATTIC_H

#include <cppunit/extensions/HelperMacros.h>

/* 
* Проверка attic::a_attribute и attic::a_document
 */

class TC_Attic : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Attic );
  CPPUNIT_TEST( attr_time );
  CPPUNIT_TEST( attr_time_2038 );
  CPPUNIT_TEST( attr_char );
  CPPUNIT_TEST( attr_wchar );
  CPPUNIT_TEST( attr_int_zero );
  CPPUNIT_TEST( attr_ulong );
  CPPUNIT_TEST( attr_uuid );
  CPPUNIT_TEST( attr_string_ref );
  CPPUNIT_TEST( entity );
  CPPUNIT_TEST( pretty );
  CPPUNIT_TEST( declaration );
  CPPUNIT_TEST( doc_comment );
  CPPUNIT_TEST( trim_ws_around_pcdata );
  CPPUNIT_TEST( identical );
  CPPUNIT_TEST( isLess );
  CPPUNIT_TEST( encoding );
  CPPUNIT_TEST( xmllike );
  //CPPUNIT_TEST( memoryUsage );
  CPPUNIT_TEST( async_time_attribute_read );
  CPPUNIT_TEST( async_time_attribute_write );
  CPPUNIT_TEST( adjacentPCData );
  CPPUNIT_TEST( exact_text );

  CPPUNIT_TEST_SUITE_END();

protected:
    void attr_time();
    void attr_time_2038();
    void attr_char();
    void attr_wchar();
    void attr_int_zero();
    void attr_ulong();
    void attr_uuid();
    void attr_string_ref();
    void entity();
    void pretty();
    void declaration();
    void doc_comment();
    void trim_ws_around_pcdata();
    void identical();
    void isLess();
    void encoding();
    void xmllike();
    //void memoryUsage();
    void async_time_attribute_read();
    void async_time_attribute_write();
    void adjacentPCData();
    void exact_text();
private:
};


#endif // TC_ATTIC_H