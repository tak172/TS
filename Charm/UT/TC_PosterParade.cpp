#include "stdafx.h"

#include "TC_PosterParade.h"
#include "../helpful/Attic.h"
#include "../helpful/Log.h"
#include "../helpful/PicketingInfo.h"
#include "../helpful/Pilgrim.h"
#include "../helpful/PicketingInfo.h"
#include "../Actor/PosterParade.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PosterParade );

void TC_PosterParade::setUp()
{
    PicketingInfo pi_dummy;
    myEsrGuide.setNameAndAxis(EsrKit(11011),      L"st110",      pi_dummy);
    myEsrGuide.setNameAndAxis(EsrKit(22022),      L"st220",      pi_dummy);
    myEsrGuide.setNameAndAxis(EsrKit(11011,22022),L"per110-220", pi_dummy);
}

void TC_PosterParade::tearDown()
{
    Pilgrim::Shutdowner();
}

static vector<BadgeE>& operator << ( vector<BadgeE>& v, const BadgeE& b )
{
    v.push_back(b);
    return v;
}

static vector<BadgeE>& operator << ( vector<BadgeE>& lhs, const vector<BadgeE>& rhs )
{
    lhs.insert( lhs.end(), rhs.begin(), rhs.end() );
    return lhs;
}

static void init( PosterParade &pp )
{
    attic::a_document doc;
    doc.load_wide( 
        L"<Poster path='D:/myFolder' autotext='Станция %1%\\nЗажглась %2%'>"
        L"    <Place ESR_code='11011'>"
        L"        <It Badge='none' />"
        L"        <It Badge='one' sound='sound one' />"
        L"        <It Badge='four' sound='sound four' hide_text='Y'/>"
        L"    </Place>"
        L"    <Place ESR_code='22022' >"
        L"        <It Badge='two' text='text two' />"
        L"        <It Badge='kkaa7' text='text kkaa7(mix)'                  />"
        L"        <It Badge='kkBb7' text='text kkBb7(mix)' sound=''         />"
        L"        <It Badge='kkXx7' text=''                sound='sound.Xx' />"
        L"        <It Badge='kkYy7'                        sound='sound.Yy' />"
        L"    </Place>"
        L"    <Place ESR_code='33033' >"
        L"        <It Badge='three' text='text 3' sound='sound 3' />"
        L"        <It Badge='threX' text='text 3' sound='sound 3' ban='Y' />"
        L"    </Place>"
        L"    <Place ESR_code='44044' ban='Y' />"
        L" "
        L"    <Substitution>"
        L"      <It of='@@' to='kka#1'/>"
        L"      <It of='z7' to='kkzZ7'/>"
        L"    </Substitution>"
        L"    <Compound  before_sound='attention.wav' >"
        L"        <Place ESR_code='11011'       sound='st.one.wav'     />"
        L"        <Place ESR_code='22022'       sound='st.two.wav'     />"
        L"        <Place ESR_code='11011:22022' sound='st.one-two.wav' />"
        L"        <Part idx='0' str='kk'                                  />"
        L"        <Part idx='2' str='aa' sound='say_aa.wav'               />"
        L"        <Part idx='2' str='Bb' sound='say_Bb.wav' hide_text='Y' />"
        L"        <Part idx='2' str='zZ'                    hide_text='Y' />"
        L"        <Part idx='2' str='Xx'                                  />"
        L"        <Part idx='2' str='Yy'                                  />"
        L"        <Part idx='4' str='7'  sound='say_7.wav'                />"
        L"        <Part idx='4' str=''                                    />"
        L"    </Compound>"
        L"</Poster>"
        );
    pp.load(doc);
}

/// \test явно указанные объекты попадают в список возможного отображения
void TC_PosterParade::append_direct()
{
    PosterParade pp(myEsrGuide);
    init( pp );

    CPPUNIT_ASSERT(  pp.append(MOMENT,BadgeE( L"none", EsrKit(11011) )) );
    CPPUNIT_ASSERT(  pp.append(MOMENT,BadgeE( L"one",  EsrKit(11011) )) );
    CPPUNIT_ASSERT(  pp.append(MOMENT,BadgeE( L"four", EsrKit(11011) )) );
    CPPUNIT_ASSERT(  pp.append(MOMENT,BadgeE( L"two",  EsrKit(22022) )) );
    CPPUNIT_ASSERT(  pp.append(MOMENT,BadgeE( L"three",EsrKit(33033) )) );
    CPPUNIT_ASSERT( !pp.append(MOMENT,BadgeE( L"what", EsrKit(22022) )) );

    CPPUNIT_ASSERT( !pp.append(MOMENT,BadgeE( L"threX",EsrKit(33033) )) );
    CPPUNIT_ASSERT( !pp.append(MOMENT,BadgeE( L"four", EsrKit(44044) )) );
}

/// \test подходящие под правило объекты попадают в список возможного отображения
void TC_PosterParade::append_compound()
{
    PosterParade pp(myEsrGuide);
    init( pp );

    // учет ЕСР
    CPPUNIT_ASSERT(  pp.append( MOMENT, BadgeE(L"kkaa7",EsrKit(11011)) ) );
    CPPUNIT_ASSERT(  pp.append( MOMENT, BadgeE(L"kkBb", EsrKit(22022)) ) );
    CPPUNIT_ASSERT(  pp.append( MOMENT, BadgeE(L"z7",   EsrKit(11011,22022)) ) );
    CPPUNIT_ASSERT( !pp.append( MOMENT, BadgeE(L"@@",   EsrKit(77777)) ) );
    CPPUNIT_ASSERT( !pp.append( MOMENT, BadgeE(L"four", EsrKit(44044) )) );
}

/// \test генерация отображаемого текста для явно указанных объектов
void TC_PosterParade::text_direct()
{
    PosterParade pp(myEsrGuide);
    init( pp );

    wstring a;
    a = pp.getText( BadgeE( L"none", EsrKit(11011) ) );
    CPPUNIT_ASSERT( a==L"Станция st110\nЗажглась none" );
    a = pp.getText( BadgeE( L"one",  EsrKit(11011) ) );
    CPPUNIT_ASSERT( a==L"Станция st110\nЗажглась one" );
    a = pp.getText( BadgeE( L"four", EsrKit( 11011 ) ) );
    CPPUNIT_ASSERT( a.empty() );
    a = pp.getText( BadgeE( L"two",  EsrKit(22022) ) );
    CPPUNIT_ASSERT( a==L"text two" );
    a = pp.getText( BadgeE( L"three",EsrKit(33033) ) );
    CPPUNIT_ASSERT( a==L"text 3" );
    a = pp.getText( BadgeE( L"what", EsrKit(22022) ) );
    CPPUNIT_ASSERT( a==L"" );
}

/// \test генерация отображаемого текста для попавших под правило объектов
void TC_PosterParade::text_compound()
{
    PosterParade pp(myEsrGuide);
    init( pp );
    wstring a,b;
    a = pp.getText( BadgeE(L"kkaa7",EsrKit(11011)) );
    b = L"Станция st110\nЗажглась kkaa7";
    CPPUNIT_ASSERT( a==b );
    a = pp.getText( BadgeE(L"kkBb", EsrKit(22022)) );
    b = L"";
    CPPUNIT_ASSERT( a==b );
    a = pp.getText( BadgeE(L"z7",   EsrKit(11011,22022)) );
    b = L"";
    CPPUNIT_ASSERT( a==b );
    a = pp.getText( BadgeE(L"@@",   EsrKit(77777)) );
    b = L"";
    CPPUNIT_ASSERT( a==b );
}

/// \test генерация звуковых ссылок для явно указанных объектов
void TC_PosterParade::sound_direct()
{
    PosterParade pp(myEsrGuide);
    init( pp );

    typedef vector<wstring> VW;
    typedef wstring AW[];
    {
        VW a = pp.getSound( BadgeE( L"none", EsrKit(11011) ) );
        CPPUNIT_ASSERT( a.empty() );
    }
    {
        VW a = pp.getSound( BadgeE( L"one",  EsrKit(11011) ) );
        AW b = {L"sound one"};
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        VW a = pp.getSound( BadgeE( L"four", EsrKit( 11011 ) ) );
        AW b = { L"sound four" };
        CPPUNIT_ASSERT( a.size() == size_array( b ) );
        CPPUNIT_ASSERT( equal( b, b + size_array( b ), a.begin() ) );
    }
    {
        VW a = pp.getSound( BadgeE( L"two",  EsrKit(22022) ) );
        CPPUNIT_ASSERT( a.empty() );
    }
    {
        VW a = pp.getSound( BadgeE( L"three",EsrKit(33033) ) );
        AW b = {L"sound 3"};
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        VW a = pp.getSound( BadgeE( L"what", EsrKit(22022) ) );
        CPPUNIT_ASSERT( a.empty() );
    }
}

/// \test генерация звуковых ссылок для попавших под правило объектов
void TC_PosterParade::sound_compound()
{
    PosterParade pp(myEsrGuide);
    init( pp );
    typedef vector<wstring> VW;
    typedef wstring AW[];
    {
        VW a = pp.getSound( BadgeE(L"kkaa7",EsrKit(11011)) );
        AW b = {
            L"attention.wav",
            L"st.one.wav",
            L"say_aa.wav",
            L"say_7.wav"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        VW a = pp.getSound( BadgeE(L"kkBb", EsrKit(22022)) );
        AW b = {
            L"attention.wav",
            L"st.two.wav",
            L"say_Bb.wav"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        VW a = pp.getSound( BadgeE(L"z7",   EsrKit(11011,22022)) );
        AW b = {
            L"attention.wav",
            L"st.one-two.wav",
            L"say_7.wav"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        VW a = pp.getSound( BadgeE(L"@@",   EsrKit(77777)) );
        CPPUNIT_ASSERT( a.empty() );
    }
}

/// \test смешанная генерация - часть от прямого описания, часть из составного правила
void TC_PosterParade::mix_direct_compound()
{
    PosterParade pp(myEsrGuide);
    init( pp );
    typedef vector<wstring> VW;
    typedef wstring AW[];
    wstring t;
    {
        BadgeE obj(L"kkaa7",EsrKit(22022));
        t = pp.getText( obj );
        CPPUNIT_ASSERT( t==L"text kkaa7(mix)" );

        VW a = pp.getSound( obj );
        AW b = {
            L"attention.wav",
            L"st.two.wav",
            L"say_aa.wav",
            L"say_7.wav"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        BadgeE obj(L"kkBb7",EsrKit(22022));
        t = pp.getText( obj );
        CPPUNIT_ASSERT( t==L"text kkBb7(mix)" );

        VW a = pp.getSound( obj );
        CPPUNIT_ASSERT( a.empty() );
    }
    {
        BadgeE obj(L"kkXx7",EsrKit(22022));
        t = pp.getText( obj );
        CPPUNIT_ASSERT( t==L"" );

        VW a = pp.getSound( obj );
        AW b = {
            L"sound.Xx"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
    {
        BadgeE obj(L"kkYy7",EsrKit(22022));
        t = pp.getText( obj );
        CPPUNIT_ASSERT( t==L"Станция st220\nЗажглась kkYy7" );

        VW a = pp.getSound( obj );
        AW b = {
            L"sound.Yy"
        };
        CPPUNIT_ASSERT( a.size()==size_array(b) );
        CPPUNIT_ASSERT( equal(b,b+size_array(b),a.begin()) );
    }
}
