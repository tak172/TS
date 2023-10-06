#include "stdafx.h"

#include "TC_LOS_switch.h"
#include "../Actor/LOSSwitch.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_switch );

/*
проверить класс LOS_switch
*/

void TC_LOS_switch::signalTS()
{
    /* Result state
                      MinusState
    PlusState  | none | passive | active
    -----------+------+---------+--------
    none       |  ?   |    ?    |   ?
    -----------+------+---------+--------
    passive    |  ?   |   OOC   |  MINUS
    -----------+------+---------+--------
    active     |  ?   |   PLUS  |  OOC
    -----------+------+---------+--------
    */
    LOS_switch sw;
    // row 1
    sw.setPlus( StateLTS::UNDEF );
    sw.setMinus( StateLTS::UNDEF );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && !sw.isOOC() && !sw.isKnown() );

    sw.setPlus( StateLTS::UNDEF );
    sw.setMinus( StateLTS::PASSIVE );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && !sw.isOOC() && !sw.isKnown() );

    sw.setPlus(StateLTS::UNDEF );
    sw.setMinus(StateLTS::ACTIVE);
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && !sw.isOOC() && !sw.isKnown() );

    // row 2
    sw.setPlus( StateLTS::PASSIVE );
    sw.setMinus( StateLTS::UNDEF );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && !sw.isOOC() && !sw.isKnown() );

    sw.setPlus( StateLTS::PASSIVE );
    sw.setMinus( StateLTS::PASSIVE );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && sw.isOOC() && sw.isKnown() );

    sw.setPlus(StateLTS::PASSIVE);
    sw.setMinus(StateLTS::ACTIVE);
    CPPUNIT_ASSERT(!sw.isPlus() && sw.isMinus() && !sw.isOOC() && sw.isKnown() );

    // row 3
    sw.setPlus( StateLTS::ACTIVE );
    sw.setMinus( StateLTS::UNDEF );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && !sw.isOOC() && !sw.isKnown() );

    sw.setPlus( StateLTS::ACTIVE );
    sw.setMinus( StateLTS::PASSIVE );
    CPPUNIT_ASSERT( sw.isPlus() && !sw.isMinus() && !sw.isOOC() && sw.isKnown() );

    sw.setPlus( StateLTS::ACTIVE );
    sw.setMinus( StateLTS::ACTIVE );
    CPPUNIT_ASSERT( !sw.isPlus() && !sw.isMinus() && sw.isOOC() && sw.isKnown() );
}

void TC_LOS_switch::signalTSext()
{
    std::string sample[] = {
        // явный ВК отсутствует
        "???    ",
        "??A    ",
        "??p    ",
        "?A?    ",
        "?AA    kO",
        "?Ap    kP",
        "?p?    ",
        "?pA    kM",
        "?pp    kO",
        // явный ВК АКТИВЕН - всегда известно и всегда ВК
        "A??    kO",
        "A?A    kO",
        "A?p    kO",
        "AA?    kO" ,
        "AAA    kO",
        "AAp    kO",
        "Ap?    kO",
        "ApA    kO",
        "App    kO",
        // явный ВК пассивен
        "p?? ---",
        "p?A ---",
        "p?p ---",
        "pA? ---",
        "pAA kO",
        "pAp kP",
        "pp? ---",
        "ppA kM",
        "ppp kO",
    };
    LOS_switch sw;
    for( const std::string& sam : sample )
    {
        CPPUNIT_ASSERT( sam.size() >= 3 );
        sw.setOOC(   StateLTS( sam[ 0 ] ) );
        sw.setPlus ( StateLTS( sam[ 1 ] ) );
        sw.setMinus( StateLTS( sam[ 2 ] ) );

        auto letter = [sam](char ch) {
            return std::string::npos != sam.find( ch, 3 );
        };

        CPPUNIT_ASSERT( sw.isPlus() == letter( 'P' ) );
        CPPUNIT_ASSERT( sw.isMinus() == letter( 'M' ) );
        CPPUNIT_ASSERT( sw.isOOC() == letter( 'O' ) );
        CPPUNIT_ASSERT( sw.isKnown() == letter( 'k' ) );
    }
}
