#include "stdafx.h"

#include "TC_LOS_Board.h"
#include "../Actor/LOSBoard.h"
#include "BuildTrainDescr.h"
#include "../helpful/Oddness.h"

using namespace std;
using namespace boost::posix_time;


CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_Board );

void TC_LOS_Board::setUp()
{
    testData xx;
    xx.traits = BuildTrain( 1234, L"1-1-1", L"бл", 60, 1100, 110, L"1:0:0" );
    xx.evenCapacity = L"76";
	xx.oddCapacity = L"77";
    xx.used = true;
    xx.lock = false;
    xx.passenger = true;
    d.push_back(xx);

    testData yy;
    yy.traits = BuildTrain( 7654, L"1-1-1", L"др", 60, 1100, 110, L"0" );
    yy.evenCapacity = L"2";
	yy.oddCapacity = L"5";
    yy.used = false;
    yy.lock = true;
    yy.passenger = false;
    d.push_back(yy);

    for( size_t u=0; u<d.size(); u++ )
    {
        LOS_base* los = LOS_base::fabric_from_object(STRIP);
        los->setWayPassenger(d[u].passenger);
        los->setCapacity(Oddness::EVEN, d[u].evenCapacity);
		los->setCapacity(Oddness::ODD, d[u].oddCapacity);
        losToTDescr[los] = d[u].traits;
        los->setUsed( d[u].used? StateLTS::ACTIVE : StateLTS::PASSIVE );
        los->setLock( d[u].lock? StateLTS::ACTIVE : StateLTS::PASSIVE );
		vector <const LOS_base *> lvec( 1, los );
        ways.push_back( lvec );
    }
}
void TC_LOS_Board::tearDown()
{
    d.clear();
    while( !ways.empty() )
    {
        delete ways.front().front();
        ways.erase(ways.begin());
    }
}

void TC_LOS_Board::item()
{
    boost::scoped_ptr<LOS_base> board( LOS_base::fabric_from_object(LOGIC_BOARD) );

    board->setWayPassenger(false);
    CPPUNIT_ASSERT( board->isWayPassenger()==false );
    board->reset_signals();
    CPPUNIT_ASSERT( board->get_Subitems<Info_Number>( false ).empty() );
    board->setWayPassenger(true);
    CPPUNIT_ASSERT( board->isWayPassenger()==true );

	unsigned int pos = 0;
	for ( const vector<const LOS_base*> & wvec : ways )
	{
		const LOS_base * los_strip = wvec.front();
		board->insert_Subitem<Info_Number>( Info_Number( losToTDescr[los_strip], los_strip->getCapacity( Oddness::EVEN ), los_strip->getCapacity( Oddness::ODD ),
			los_strip->isStableUsed(), los_strip->isLock(), false, pos++ ) );
	}

    INFO_NUMV pes = board->get_Subitems<Info_Number>( false );
    CPPUNIT_ASSERT( pes.size()==d.size() );
    for( size_t u=0; u<d.size(); u++ )
    {
        CPPUNIT_ASSERT(pes[u].traitsPE == d[u].traits );
        CPPUNIT_ASSERT(pes[u].evenCapacity==d[u].evenCapacity);
		CPPUNIT_ASSERT(pes[u].oddCapacity==d[u].oddCapacity);
        CPPUNIT_ASSERT(pes[u].used_mark==d[u].used);
        CPPUNIT_ASSERT(pes[u].lock_mark==d[u].lock);
        CPPUNIT_ASSERT(pes[u].m_uPos == u);
    }
}
