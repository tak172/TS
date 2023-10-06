#ifndef TC_POSTERBAN_H
#define TC_POSTERBAN_H

#include <cppunit/extensions/HelperMacros.h>

/** \brief Проверка PosterBan
 */

class TC_PosterBan : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PosterBan );
  CPPUNIT_TEST( direct_and_group );
  CPPUNIT_TEST_SUITE_END();

protected:
	void direct_and_group();
private:
};


#endif // TC_POSTERBAN_H