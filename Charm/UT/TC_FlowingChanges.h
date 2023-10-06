#ifndef TC_FLOWINGCHANGES_H
#define TC_FLOWINGCHANGES_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "../helpful/FlowingChanges.h"

/** \brief Проверка PosterBan
 */

class TC_FlowingChanges : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_FlowingChanges );
  CPPUNIT_TEST( changes );
  CPPUNIT_TEST( ref_out_in );
  CPPUNIT_TEST( inc_head );
  CPPUNIT_TEST( inc_tail );
  CPPUNIT_TEST( inc_swap );
  CPPUNIT_TEST_SUITE_END();

public:
    TC_FlowingChanges();
    virtual void setUp();
    virtual void tearDown();
protected:
    void changes();
	void ref_out_in();
    void inc_head();
    void inc_tail();
    void inc_swap();
private:
    const std::pair<time_t, BadgeE> data0;
    const std::pair<time_t, BadgeE> data1;
    const std::pair<time_t, BadgeE> data2;
    const std::pair<time_t, BadgeE> data3;
    attic::a_document xdoc;
    attic::a_node ref;
    attic::a_node before;
    attic::a_node after;

    typedef std::pair<time_t, BadgeE> COLL_TYPE;
    typedef std::deque< COLL_TYPE > COLLECTION;
    COLLECTION out;
};


#endif // TC_FLOWINGCHANGES_H