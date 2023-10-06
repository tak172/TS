#ifndef TC_CANOPY_FILER_H_
#define TC_CANOPY_FILER_H_

#include <cppunit/extensions/HelperMacros.h>

class TC_CanopyFiler: public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_CanopyFiler );
  CPPUNIT_TEST( test_write_split );
  CPPUNIT_TEST( test_write_overwrite );
  CPPUNIT_TEST( test_write_wipeold );
  CPPUNIT_TEST( test_catalog );
  CPPUNIT_TEST( test_read );
  CPPUNIT_TEST( test_lock );
  CPPUNIT_TEST( test_error_write );
  CPPUNIT_TEST( test_write_trash );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void test_write_split();
    void test_write_overwrite();
    void test_write_wipeold();
    void test_catalog();
    void test_read();
    void test_lock();
    void test_error_write(); // ошибка при записи файла
    void test_write_trash();    // запись мусора
private:
    void unlink_all_files( const std::wstring& mask );

    std::wstring fold;
    boost::posix_time::time_duration m_old_FilerTimeout;
};


#endif // TC_CANOPY_FILER_H_