#ifndef TC_LTS_LOGELEMENT_H
#define TC_LTS_LOGELEMENT_H

#include <cppunit/extensions/HelperMacros.h>


/// Тесты работы акцепторов ЛТС для класса \ref CLogicElement
class TC_Lts_LogElement : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Lts_LogElement );
  CPPUNIT_TEST( load_save_strip );
  CPPUNIT_TEST( load_save_switch );
  CPPUNIT_TEST( load_save_figure );
  CPPUNIT_TEST( load_save_head );
  CPPUNIT_TEST( accept );
  CPPUNIT_TEST( omittedTs_strip );
  CPPUNIT_TEST( omittedTs_switch );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void load_save_strip ();
    void load_save_switch ();
    void load_save_figure ();
    void load_save_head ();
    void accept();
    void omittedTs_strip();
    void omittedTs_switch();
private:
    std::string to_raw( const wchar_t * data );
    std::string storing_ts_byName( const std::string& u8 );
    void loading_ts_byName( const std::string& u8 );
};


#endif // TC_LTS_LOGELEMENT_H