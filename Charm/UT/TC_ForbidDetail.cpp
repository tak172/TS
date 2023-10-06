#include "stdafx.h"

#include "TC_ForbidDetail.h"
#include "../helpful/Attic.h"
#include "../helpful/ForbidDetail.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_ForbidDetail );

void TC_ForbidDetail::serialize()
{
    ForbidDetail a,b;
    a.setPlan( interval_time_t( 123000,144000 ) );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setFact( interval_time_t( 155000,177000 ) );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setComment( L"Комментарий" );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setReason( 777 );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setService( "служба Ц" );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setShowOvertime( true );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
    a.setSlName( L"Служба Ы" );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );

    MoveHelpSystem codes[] = {
        // т.к. исходное состояние UNKNOWN, то такое значение должно быть НЕ первым
        MoveHelpSystem::AUTOBLOCK,
        MoveHelpSystem::UNKNOWN,
        MoveHelpSystem::SEMIAUTOMATICBLOCK,
        MoveHelpSystem::TELEPHONE,
        MoveHelpSystem::BLOCKSTAFF,
        MoveHelpSystem::FORBIDMOVING,
        MoveHelpSystem::ALSN
    };
    for( auto ic : codes )
    {
        a.setRightMoveMode( ic );
        CPPUNIT_ASSERT( check_and_reassign(a, b) );
        a.setWrongMoveMode( ic );
        CPPUNIT_ASSERT( check_and_reassign(a, b) );
    }

    a.setZeroVoltage( true );
    CPPUNIT_ASSERT( check_and_reassign(a, b) );
}


bool TC_ForbidDetail::check_and_reassign( const ForbidDetail &a, ForbidDetail &b )
{
    if ( !isEqual(a, b) )
    {
        attic::a_document doc("test");
        attic::a_node xn = doc.document_element().append_child("xx");
        a.writeTo( xn );
        ForbidDetail temp = ForbidDetail::readFrom(xn);
        if ( isEqual(a, temp) )
        {
            b = temp;
            return true;
        }
    }
    return false;
}

bool TC_ForbidDetail::isEqual( const ForbidDetail& a, const ForbidDetail& b)
{
    return
        a.getPlan() == b.getPlan() &&
        a.getFact() == b.getFact() &&
        a.getComment() == b.getComment() &&
        a.getReason() == b.getReason() &&
        a.getService() == b.getService() &&
        a.getShowOvertime() == b.getShowOvertime() &&
        a.getSlName() == b.getSlName() &&
        a.getSlLetter() == b.getSlLetter() &&
        a.getRightMoveMode() == b.getRightMoveMode() &&
        a.getWrongMoveMode() == b.getWrongMoveMode() &&
        a.getZeroVoltage() == b.getZeroVoltage();
}
