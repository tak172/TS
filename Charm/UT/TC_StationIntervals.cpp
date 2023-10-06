#include "stdafx.h"
#include "TC_StationIntervals.h"
#include "../helpful/Serialization.h"
#include "../helpful/StatIntervals.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StationIntervals );

const string testStr = "<StatIntervals>"\
	"<timekit esrCode=\"11030\" tsk=\"3\"> "\
		"<neck esrCode=\"11040\" tn=\"4\" tnp=\"1\" />"\
		"<neck esrCode=\"11048\" tnp=\"2\" />"\
		"<neck esrCode=\"11052\" tn=\"2\" tnp=\"5\" />"\
	"</timekit>"\
	"<timekit esrCode=\"11034:11044\" Ipr=\"9\" Iot=\"3\" Ipk=\"14\" tp=\"4\" tpbo=\"2\" />"\
	"<timekit esrCode=\"12010:12050\" Iot=\"31\" tpbo=\"27\" />"\
	"</StatIntervals>";

void TC_StationIntervals::Deserialize()
{
	string serialStr = testStr;
	auto statIntervalsPtr = deserialize<ACollStInt>( serialStr );
	CPPUNIT_ASSERT( statIntervalsPtr );
	const ACollStInt & statIvalCollection = *statIntervalsPtr;
	const auto & stations = statIvalCollection.getStations();
	CPPUNIT_ASSERT( stations.size() == 5 );
	CPPUNIT_ASSERT( find( stations.cbegin(), stations.cend(), 11030 ) != stations.cend() );
	CPPUNIT_ASSERT( find( stations.cbegin(), stations.cend(), 11034 ) != stations.cend() );
	CPPUNIT_ASSERT( find( stations.cbegin(), stations.cend(), 11044 ) != stations.cend() );
	CPPUNIT_ASSERT( find( stations.cbegin(), stations.cend(), 12010 ) != stations.cend() );
	CPPUNIT_ASSERT( find( stations.cbegin(), stations.cend(), 12050 ) != stations.cend() );
	const auto & undepIvals = statIvalCollection.getUndepInterval( StationEsr(11030) );
	CPPUNIT_ASSERT( undepIvals.tsk == chrono::minutes( 3 ) );
	const auto & statIvals1 = statIvalCollection.getInterval( StationEsr(11030), StationEsr(11040) );
	CPPUNIT_ASSERT( statIvals1.tn == chrono::minutes( 4 ) && statIvals1.tnp == chrono::minutes( 1 ) );
	const auto & statIvals2 = statIvalCollection.getInterval( StationEsr(11030), StationEsr(11048) );
	CPPUNIT_ASSERT( statIvals2.tn == chrono::minutes( 3 ) && statIvals2.tnp == chrono::minutes( 2 ) );
	const auto & statIvals3 = statIvalCollection.getInterval( StationEsr(11030), StationEsr(11052) );
	CPPUNIT_ASSERT( statIvals3.tn == chrono::minutes( 2 ) && statIvals3.tnp == chrono::minutes( 5 ) );

	const auto & statIvals4 = statIvalCollection.getInterval( StationEsr(11034), StationEsr(11044) );
	CPPUNIT_ASSERT( statIvals4.Ipr == chrono::minutes( 9 ) && statIvals4.Iot == chrono::minutes( 3 ) && statIvals4.Ipk == chrono::minutes( 14 ) &&
		statIvals4.tp == chrono::minutes( 4 ) && statIvals4.tpbo == chrono::minutes( 2 ) );
	const auto & statIvals5 = statIvalCollection.getInterval( StationEsr(11044), StationEsr(11034) );
	CPPUNIT_ASSERT( statIvals5.Ipr == chrono::minutes( 9 ) && statIvals5.Iot == chrono::minutes( 3 ) && statIvals5.Ipk == chrono::minutes( 14 ) &&
		statIvals5.tp == chrono::minutes( 4 ) && statIvals5.tpbo == chrono::minutes( 2 ) );
	const auto & statIvals6 = statIvalCollection.getInterval( StationEsr(12010), StationEsr(12050) );
	CPPUNIT_ASSERT( statIvals6.Ipr == chrono::minutes( 5 ) && statIvals6.Iot == chrono::minutes( 31 ) && statIvals6.Ipk == chrono::minutes( 9 ) &&
		statIvals6.tp == chrono::minutes( 2 ) && statIvals6.tpbo == chrono::minutes( 27 ) );
	const auto & statIvals7 = statIvalCollection.getInterval( StationEsr(12050), StationEsr(12010) );
	CPPUNIT_ASSERT( statIvals7.Ipr == chrono::minutes( 5 ) && statIvals7.Iot == chrono::minutes( 31 ) && statIvals7.Ipk == chrono::minutes( 9 ) &&
		statIvals7.tp == chrono::minutes( 2 ) && statIvals7.tpbo == chrono::minutes( 27 ) );

	//запрос интервалов, не описанных в файле
	const auto & undefIvals = statIvalCollection.getInterval( StationEsr(999), StationEsr(1234) );
	CPPUNIT_ASSERT( undefIvals.tn == chrono::minutes( 3 ) && undefIvals.tnp == chrono::minutes( 3 ) && undefIvals.tsk == chrono::minutes( 1 ) &&
		undefIvals.Ipr == chrono::minutes( 5 ) && undefIvals.Iot == chrono::minutes( 5 ) && undefIvals.Ipk == chrono::minutes( 9 ) &&
		undefIvals.tp == chrono::minutes( 2 ) && undefIvals.tpbo == chrono::minutes( 5 ) );
}