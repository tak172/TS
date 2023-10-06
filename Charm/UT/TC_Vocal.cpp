#include "stdafx.h"

#include "TC_Vocal.h"
#include "../helpful/Attic.h"
#include "../helpful/Pilgrim.h"
#include "../Actor/Vocal.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Vocal );

void TC_Vocal::setUp()
{
}

void TC_Vocal::tearDown()
{
    Pilgrim::Shutdowner();
}

static void init( Vocal &pp )
{
    attic::a_document doc;
    doc.load_wide( 
        L"<Poster path='D:/myFolder' autotext='Станция %1%\\nЗажглась %2%'>"
        L"    <!-- A: звуки ответственных команд -->"
        L"    <Crucial Command='DOSTUP.WAV' KeyIn='KEY_IN.WAV' KeyOut='KEY_OUT.WAV' />"
        L"    <Place ESR_code='11011'>"
        L"        <It Badge='none' />"
        L"        <It Badge='one' sound='sound one' />"
        L"    </Place>"
        L"    <Place ESR_code='44044' ban='Y' />"
        L"</Poster>"
        );
    pp.load_special(doc);
}

/// \test установка пути к звуковым файлам
void TC_Vocal::sound_path()
{
    Vocal vocal;
    init( vocal );

    wstring folder = Pilgrim::instance()->AddSoundPath(L"test.wav");
    CPPUNIT_ASSERT( L"D:/myFolder/test.wav"==folder );
}
