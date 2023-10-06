#pragma once
#include <cppunit/extensions/HelperMacros.h>

// На основе файла TC_UartThread из тестов для RDC
 
class TC_UartThread : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_UartThread );
    // работа каналов в целом
    CPPUNIT_TEST( Bisk_WholeFile );
    CPPUNIT_TEST( GidUral_WholeFile );
    CPPUNIT_TEST( Asoup_WholeFile );
#ifndef LINUX
	CPPUNIT_TEST(CommandTraktTu);
	CPPUNIT_TEST(Dialog_Udp);
#endif // !LINUX
    CPPUNIT_TEST_SUITE_END();

public:
    void setUp() override;
    void tearDown() override;
    bool wipeFiles();

    void Bisk_WholeFile();
    void GidUral_WholeFile();
    void Asoup_WholeFile();
#ifndef LINUX
	void CommandTraktTu();
	void Dialog_Udp();
#endif // !LINUX
    std::shared_ptr<Network::Docker> make_link( unsigned short portH, unsigned short portF ) const;

    std::wstring m_tempDir;
    std::wstring m_extIn;
    std::wstring m_extOut;
};
