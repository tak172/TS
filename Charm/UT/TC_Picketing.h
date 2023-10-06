#ifndef TC_PICKETING_H
#define TC_PICKETING_H
#include <cppunit/extensions/HelperMacros.h>

/* 
 * Класс для проверки работы класса PicketingInfo
 */

class TC_Picketing : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Picketing );
  CPPUNIT_TEST( XmlReadSave_normal );
  CPPUNIT_TEST( XmlReadSave_bigNumber );
  CPPUNIT_TEST( XmlReadSave_repeatDirection );
  CPPUNIT_TEST( XmlReadSave_inconsistence );
  CPPUNIT_TEST( Median );
  CPPUNIT_TEST( Intersect );
  CPPUNIT_TEST( GetCoord );
  CPPUNIT_TEST( FirstCoord );
  CPPUNIT_TEST( Wstring );
  CPPUNIT_TEST_SUITE_END();

protected:
    void XmlReadSave_normal();
    void XmlReadSave_bigNumber();
    void XmlReadSave_repeatDirection();
    void XmlReadSave_inconsistence();
    void Median();
    void Intersect();
    void GetCoord();
    void FirstCoord();
    void Wstring();
private:
    bool XmlReadSave( const std::wstring& in, const std::wstring& out );
};


#endif // TC_PICKETING_H