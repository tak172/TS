#include "stdafx.h"
#include <boost/filesystem/operations.hpp>
#include "TC_Buzzer.h"
#include "../Actor/Vocal.h"
#include "../Actor/Buzzer.h"

namespace bfs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Buzzer );


void TC_Buzzer::setUp()
{
    boost::system::error_code ec;
    pathWav = bfs::temp_directory_path( ec ) / L"ooc.wav";
    fileWav = pathWav.generic_wstring();

    std::ofstream ofs( pathWav.c_str() );
    ofs << "some";
    ofs.close();
}

void TC_Buzzer::tearDown()
{
    boost::system::error_code ec;
    CPPUNIT_ASSERT( !bfs::exists( pathWav ) || bfs::remove( pathWav, ec ) );
}

void TC_Buzzer::permanent()
{
    attic::a_document doc;
    doc.load_wide(
        L"<Poster path='./' autotext='Станция %1%\\nЗажглась %2%'>"
        L"    <!-- A: звуки ответственных команд -->"
        L"    <Crucial Command='DOSTUP.WAV' KeyIn='KEY_IN.WAV' KeyOut='KEY_OUT.WAV' />"
        L"    <!-- звонки непрерывные (с включения лампы и до выключения) -->"
        L"    <Buzzer item='раз[11111]' sound='" + fileWav + "' />"
        L"    <Buzzer item='два[22222]' sound='" + fileWav + "' />"
        L"</Poster>"
    );

    Vocal vocal;
    Buzzer buzzer( vocal );
    buzzer.load( doc );
    // единственный сигнал
    const BadgeE xxx( L"ЖЖЖ", EsrKit( 12345 ) );
    const BadgeE one( L"раз", EsrKit( 11111 ) );
    const BadgeE two( L"два", EsrKit( 22222 ) );

    buzzer.set( xxx );
    CPPUNIT_ASSERT( !buzzer.is_active( xxx ) );
    CPPUNIT_ASSERT( !buzzer.is_active( fileWav ) );

    buzzer.set( one );
    CPPUNIT_ASSERT( buzzer.is_active( one ) );
    CPPUNIT_ASSERT( vocal.permanent_exist( fileWav ) );

    buzzer.set( two );
    CPPUNIT_ASSERT( buzzer.is_active( two ) );
    CPPUNIT_ASSERT( vocal.permanent_exist( fileWav ) );

    buzzer.reset( one );
    CPPUNIT_ASSERT( !buzzer.is_active( one ) );
    CPPUNIT_ASSERT( vocal.permanent_exist( fileWav ) );

    buzzer.reset( two );
    CPPUNIT_ASSERT( !buzzer.is_active( two ) );
    CPPUNIT_ASSERT( !vocal.permanent_exist( fileWav ) );
}
