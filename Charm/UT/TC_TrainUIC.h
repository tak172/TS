#ifndef TC_TRAINUIC_H
#define TC_TRAINUIC_H

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Attic.h"
#include "../Fund/TrainUIC/EbiLock_Types.h"
#include "../Fund/TrainUIC/EbiLock_ObjMap.h"
#include "../Fund/TrainUIC/Atn_Tool.h"

class TC_TrainUIC : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_TrainUIC );

  CPPUNIT_TEST( ToolFromAtn );
  CPPUNIT_TEST( ToolToAtn );
  CPPUNIT_TEST( ToolServant );
  CPPUNIT_TEST( ToolTruncateNumber );
  CPPUNIT_TEST( DialogFromAtn );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void ToolFromAtn();
    void ToolToAtn();
    void ToolServant();
    void ToolTruncateNumber();
    void DialogFromAtn();

private:
    EbiLock::FuncLE funcLE;
    EbiLock::ObjMap ebiObjMap;
    TrainUIC::Tool tool;

    static BadgeE B(const std::string& text);
};


#endif // TC_TRAINUIC_H