#ifndef TC_PIPELINE_XMLQUEUE_H
#define TC_PIPELINE_XMLQUEUE_H

#include <cppunit/extensions/HelperMacros.h>

class TC_PipelineXmlQueue : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_PipelineXmlQueue );
  CPPUNIT_TEST( check );
  CPPUNIT_TEST( quick_check );
  CPPUNIT_TEST_SUITE_END();

protected:
    void check();
    void quick_check(); // проверка, что проверка действительно быстрая
};


#endif // TC_PIPELINE_XMLQUEUE_H