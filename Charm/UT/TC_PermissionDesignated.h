#ifndef TC_PERMISSIONDESIGNATED_H
#define TC_PERMISSIONDESIGNATED_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Permission/PermissionDesignated.h"

/* 
 * Класс для проверки работы класса Permission::Designated
 */
class TC_PermissionDesignated : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PermissionDesignated );
  CPPUNIT_TEST( LoadSave );
  CPPUNIT_TEST( LoadSave_compat );
  CPPUNIT_TEST( LoadSave_sortComputers );
  CPPUNIT_TEST( LoadSaveJournal );
  CPPUNIT_TEST( GetSetPermit );
  CPPUNIT_TEST( GetPermitByIp );
  CPPUNIT_TEST( GetIpHostName );
  CPPUNIT_TEST( accessNormative );
  CPPUNIT_TEST( accessSupervis );
  CPPUNIT_TEST( GetSetPermit_Journal );
  CPPUNIT_TEST_SUITE_END();

protected:
    void LoadSave();
    void LoadSave_compat();
    void LoadSave_sortComputers();
    void LoadSaveJournal();
    void GetSetPermit();
    void GetPermitByIp();
    void GetIpHostName();
    void accessNormative();
    void accessSupervis();
    void GetSetPermit_Journal();
};


#endif // TC_PERMISSIONDESIGNATED_H