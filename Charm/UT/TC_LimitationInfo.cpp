#include "stdafx.h"
#include "TC_LimitationInfo.h"
#include "../helpful/Serialization.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include "../Augur/LimitationInfo.h"
#include "../helpful/RestrictSpeed.h"
#include "../helpful/ConjugateWaysLimit.h"
#include "../helpful/ExcerptLot.h"
#include "XmlEqual.h"

using namespace std;
using namespace boost::posix_time;
using namespace Excerpt;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LimitationInfo );

const string wnumAttr = "way";
const string pnumAttr = "park";
const string bdgAttr = "bdg";
const string startTimeAttr = "start";
const string endTimeAttr = "end";

const string testStr = "<LimitationInfos>"\
		"<LimitInfo start=\"20171215T072511Z\" end=\"20171215T094511Z\">"\
			"<location esr=\"22151:33662\" waynum=\"3\">"\
				"<picketing_start picketing1_val=\"1~\" picketing1_comm=\"mockaxis\" />"\
				"<picketing_stop picketing1_val=\"2~\" picketing1_comm=\"mockaxis\" />"\
			"</location>"\
			"<conjwayslimit rightdir=\"bidir_ab_semas\" wrongdir=\"electok\" />"\
		"</LimitInfo>"\
		"<LimitInfo start=\"20170915T220000Z\" end=\"20170915T221000Z\">"\
			"<location esr=\"17000:18000\" waynum=\"2\" FullSpan=\"Y\"/>"\
			"<conjwayslimit rightdir=\"\" wrongdir=\"phone\" />"\
		"</LimitInfo>"\
		"<LimitInfo start=\"20170915T024512Z\" end=\"20170915T040047Z\">"\
			"<location esr=\"11555\" waynum=\"4\" parknum=\"7\" />"\
			"<velocity SpeedAll=\"37\" SpeedGoods=\"49\" />" \
		"</LimitInfo>"\
	"</LimitationInfos>";

void TC_LimitationInfo::Serialize()
{
	vector<LimitationInfo> liminfoVec;

	//окно 1
	tm startMom1 = to_tm( from_iso_string( "20171215T072511" ) );
	tm endMom1 = to_tm( from_iso_string( "20171215T094511" ) );
	rwRuledInterval mockRuler( L"mockaxis", rwCoord( 1, 0 ), rwCoord( 2, 0 ) );
	SpanLot spanLot1( EsrKit( 22151, 33662 ), 3, mockRuler );
	auto conjWaysLimitPtr1 = make_shared<ConjugateWaysLimit>( ConjugateWaysLimit( MoveHelpSystem::ALSN, MoveHelpSystem::BLOCKSTAFF ) );
	liminfoVec.emplace_back( LimitationInfo( spanLot1, _mkgmtime( &startMom1 ), _mkgmtime( &endMom1 ), nullptr, conjWaysLimitPtr1 ) );

	//окно 2
	tm startMom2 = to_tm( from_iso_string( "20170915T220000" ) );
	tm endMom2 = to_tm( from_iso_string( "20170915T221000" ) );
	auto conjWaysLimitPtr2 = make_shared<ConjugateWaysLimit>( ConjugateWaysLimit( MoveHelpSystem::FORBIDMOVING, MoveHelpSystem::TELEPHONE ) );
	liminfoVec.emplace_back( LimitationInfo( SpanLot( EsrKit( 17000, 18000 ), 2 ), _mkgmtime( &startMom2 ), _mkgmtime( &endMom2 ), nullptr, conjWaysLimitPtr2 ) );

	//предупреждение
	tm startMom3 = to_tm( from_iso_string( "20170915T024512" ) );
	tm endMom3 = to_tm( from_iso_string( "20170915T040047" ) );
	auto speedPtr = make_shared<RestrictSpeed>( RestrictSpeed() );
	(*speedPtr)[RestrictSpeed::ALL] = 37;
	(*speedPtr)[RestrictSpeed::GOODS] = 49;
	TermWayLot termLot( EsrKit( 11555 ), ParkWayKit( 7, 4 ) );
	liminfoVec.emplace_back( LimitationInfo( termLot, _mkgmtime( &startMom3 ), _mkgmtime( &endMom3 ), speedPtr, nullptr ) );

	string serialStr = serialize( liminfoVec );
	auto mismRes = mismatch( serialStr.cbegin(), serialStr.cend(), testStr.cbegin(), testStr.cend() );
	CPPUNIT_ASSERT( xmlEqual( serialStr, testStr ) );
}

void TC_LimitationInfo::Deserialize()
{
	string serialStr = testStr;
	auto liminfoVecPtr = deserialize<vector<LimitationInfo>>( serialStr );
	CPPUNIT_ASSERT( liminfoVecPtr );
	const auto & limInfoVec = *liminfoVecPtr;
	CPPUNIT_ASSERT( limInfoVec.size() == 3 );

	const auto & wndLimitation1 = limInfoVec[0];
	CPPUNIT_ASSERT( wndLimitation1.location.getParkWay().iWay() == 3 );
	CPPUNIT_ASSERT( wndLimitation1.location.getEsr() == EsrKit( 22151, 33662 ) );
	auto startMom1 = to_tm( from_iso_string("20171215T072511") );
	CPPUNIT_ASSERT( wndLimitation1.startMoment == _mkgmtime( &startMom1 ) );
	auto endMom1 = to_tm( from_iso_string("20171215T094511") );
	CPPUNIT_ASSERT( wndLimitation1.endMoment == _mkgmtime( &endMom1 ) );
	CPPUNIT_ASSERT( !wndLimitation1.speedRestrictPtr );
	auto conjwaylimitPtr1 = wndLimitation1.conjWaysLimitPtr;
	CPPUNIT_ASSERT( conjwaylimitPtr1 );
	auto rightdirLockPtr1 = conjwaylimitPtr1->getPermittedRightdirLock();
	CPPUNIT_ASSERT( rightdirLockPtr1 );
	CPPUNIT_ASSERT( *rightdirLockPtr1 == SpanLockType::BIDIR_AUTOBLOCK_WITH_LOCOSEMAS );
	auto wrongdirLockPtr1 = conjwaylimitPtr1->getPermittedWrongdirLock();
	CPPUNIT_ASSERT( wrongdirLockPtr1 );
	CPPUNIT_ASSERT( *wrongdirLockPtr1 == SpanLockType::ELECTRIC_TOKEN );

	const auto & wndLimitation2 = limInfoVec[1];
	CPPUNIT_ASSERT( wndLimitation2.location.getParkWay().iWay() == 2 );
	CPPUNIT_ASSERT( wndLimitation2.location.getEsr() == EsrKit( 17000, 18000 ) );
	auto startMom12 = to_tm( from_iso_string("20170915T220000") );
	CPPUNIT_ASSERT( wndLimitation2.startMoment == _mkgmtime( &startMom12 ) );
	auto endMom2 = to_tm( from_iso_string("20170915T221000") );
	CPPUNIT_ASSERT( wndLimitation2.endMoment == _mkgmtime( &endMom2 ) );
	CPPUNIT_ASSERT( !wndLimitation1.speedRestrictPtr );
	auto conjwaylimitPtr2 = wndLimitation2.conjWaysLimitPtr;
	CPPUNIT_ASSERT( conjwaylimitPtr2 );
	auto rightdirLockPtr2 = conjwaylimitPtr2->getPermittedRightdirLock();
	CPPUNIT_ASSERT( !rightdirLockPtr2 );
	auto wrongdirLockPtr2 = conjwaylimitPtr2->getPermittedWrongdirLock();
	CPPUNIT_ASSERT( *wrongdirLockPtr2 == SpanLockType::TELEPHONE_CONNECTION );

	const auto & warnLimitation = limInfoVec[2];
	CPPUNIT_ASSERT( warnLimitation.location.getParkWay().iWay() == 4 );
	CPPUNIT_ASSERT( warnLimitation.location.getParkWay().iPark() == 7 );
	CPPUNIT_ASSERT( warnLimitation.location.getEsr() == EsrKit( 11555 ) );
	auto startMom3 = to_tm( from_iso_string("20170915T024512") );
	CPPUNIT_ASSERT( warnLimitation.startMoment == _mkgmtime( &startMom3 ) );
	auto endMom3 = to_tm( from_iso_string("20170915T040047") );
	CPPUNIT_ASSERT( warnLimitation.endMoment == _mkgmtime( &endMom3 ) );
	auto warnSpeedPtr = warnLimitation.speedRestrictPtr;
	CPPUNIT_ASSERT( warnSpeedPtr );
	const auto & warnSpeed = *warnSpeedPtr;
	CPPUNIT_ASSERT( warnSpeed[RestrictSpeed::ALL] == 37 );
	CPPUNIT_ASSERT( warnSpeed[RestrictSpeed::PASSENGER] == RestrictSpeed::UNLIMITED );
	CPPUNIT_ASSERT( warnSpeed[RestrictSpeed::GOODS] == 49 );
	CPPUNIT_ASSERT( warnSpeed[RestrictSpeed::SUBURBAN] == RestrictSpeed::UNLIMITED );
	CPPUNIT_ASSERT( warnSpeed[RestrictSpeed::OVER_SWITCH] == RestrictSpeed::UNLIMITED );
}

void TC_LimitationInfo::SpanLimitLength()
{
	rwRuledInterval mockRuler( L"mockaxis", rwCoord( 146, 245 ), rwCoord( 714, 674 ) );
	SpanLot spanLot( EsrKit( 22151, 33662 ), 3, mockRuler );
	tm startMom = to_tm( from_iso_string("20171215T072511") );
	tm endMom = to_tm( from_iso_string("20171215T094511") );
	LimitationInfo limitInfo( spanLot, _mkgmtime( &startMom ), _mkgmtime( &endMom ), nullptr, nullptr );
	double retlength = 0;
	CPPUNIT_ASSERT( limitInfo.kmLength( retlength ) );
	CPPUNIT_ASSERT( retlength == 568.429 );
}