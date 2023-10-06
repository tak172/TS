#ifndef TC_FUNDKERNEL_H
#define TC_FUNDKERNEL_H

#include <cppunit/extensions/HelperMacros.h>
#include "../Fund/FundKernel.h"

class TC_FundKernel : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_FundKernel );
  CPPUNIT_TEST( start_stop );
  CPPUNIT_TEST( get_lts );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp();
    void tearDown();
    void start_stop();
    void get_lts();

private:
    void create_fcfg_oec();
    void remove_fcfg_oec();

    // ожидание нужного состояния 
    typedef bool ( FundKernel::* MEMFUNC )( ) const;
    bool wait_for( MEMFUNC func, bool required );

    std::unique_ptr< FundKernel > kernel;
};


#endif // TC_FUNDKERNEL_H