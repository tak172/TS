#ifndef TC_BUZZER_H
#define TC_BUZZER_H

#include <cppunit/extensions/HelperMacros.h>

class TC_Buzzer : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Buzzer );
  CPPUNIT_TEST( permanent );
  CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;
protected:
    void permanent();
private:
    boost::filesystem::path pathWav;
    std::wstring            fileWav;
};


#endif // TC_BUZZER_H