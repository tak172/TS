#ifndef TC_EMITTER_H_
#define TC_EMITTER_H_

#include <cppunit/extensions/HelperMacros.h>

namespace NsCanopy { class Filer; };
namespace Bbx { class Location; };
class Emitter_Base;

/* 
 * Класс для проверки построения эмиттеров входных данных
 */
class TC_Emitter : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Emitter );
  CPPUNIT_TEST( testLiveEmitter );
  CPPUNIT_TEST( testTimeMachineEmitter_noBbx );
  CPPUNIT_TEST( testTimeMachineEmitter );
  CPPUNIT_TEST( testPlainEmitter_noBbx );
  CPPUNIT_TEST( testPlainEmitter );
  CPPUNIT_TEST( testFreezeHefEmitter );
  CPPUNIT_TEST( testFreezeHefEmitter_noData );
//  CPPUNIT_TEST( testFreezePlukEmitter );
  CPPUNIT_TEST( testFreezePlukEmitter_noData );
  CPPUNIT_TEST( testRdcSyncEmitter );
  CPPUNIT_TEST( testRdcSyncEmitter_noData );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void testLiveEmitter();
    void testTimeMachineEmitter_noBbx();
    void testTimeMachineEmitter();
    void testPlainEmitter_noBbx();
    void testPlainEmitter();
    void testFreezeHefEmitter();
    void testFreezeHefEmitter_noData();
    void testFreezePlukEmitter();
    void testFreezePlukEmitter_noData();
    void testRdcSyncEmitter();
    void testRdcSyncEmitter_noData();

    void writeFileHef( time_t moment, std::string tzone  );
    void writeFileBbx( time_t momentFrom, time_t momentTo );
    void writePluk( time_t moment, std::string tzone  );

    std::wstring                     m_folder;
    std::unique_ptr<NsCanopy::Filer> m_filer;
    std::unique_ptr<Bbx::Location>   m_bbxLocation;
    std::unique_ptr<Emitter_Base>    m_emitter;
};


#endif //TC_EMITTER_H_
