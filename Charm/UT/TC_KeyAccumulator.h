#ifndef _TC_KEYACCUMULATOR_H_
#define _TC_KEYACCUMULATOR_H_

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/Badge_fwd.h"

class CLogicCommand;

/// \brief �������� Mission::KeyAccumulator

class TC_KeyAccumulator : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_KeyAccumulator );
  CPPUNIT_TEST( accum1b );
  CPPUNIT_TEST( accum2b );
  CPPUNIT_TEST( curs );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void accum1b(); ///< ������� �� ����� ������
    void accum2b(); ///< ������� �� ���� �������
    void curs();    ///< ��������� �������
private:
    std::vector<CLogicCommand> cmds;
    static const EsrKit e;
    static const BadgeE x,Y,z,w;
};

#endif