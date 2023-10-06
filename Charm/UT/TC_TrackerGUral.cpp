#include "stdafx.h"
#include "TC_TrackerGUral.h"
#include "../UT/TestTracker.h"
#include "../Guess/Msg.h"
#include "../Guess/TrainChanges.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainInfoPackage.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerGUral );
void TC_TrackerGUral::setUp()
{
	TI.Reset();
}

void TC_TrackerGUral::ApplyGUInfo()
{
	//����� �� Y3
	auto reftime = TI.DetermineStripBusy( L"Y3", *tracker );
	ChangesUnion changes = tracker->TakeTrainChanges();

	//����������� �����
	TrainDescr refdescr( to_wstring( 1549 ) );
	tracker->SetTrainsInfo( CreateInfoPackage( false, 0, refdescr, TI.Get( L"Y3" ).bdg, TrainCharacteristics::Source::Disposition, &reftime ) );

	//�������� ������
	TI.DetermineStripFree( L"Y3", *tracker, false );
	TI.DetermineStripBusy( L"X3", *tracker, false );
	time_t next_time = TI.FlushData( *tracker, false );

	changes = tracker->TakeTrainChanges();
	for ( const auto & change : changes.placeViewChanges )
	{
		ConstTrainDescrPtr tdescr = change.tdescrPtr;
		CPPUNIT_ASSERT( tdescr && *tdescr == refdescr );
		if ( change.place == TI.Get( L"X3" ).bdg )
			CPPUNIT_ASSERT( change.appeared );
		else
		{
			CPPUNIT_ASSERT( change.place == TI.Get( L"Y3" ).bdg );
			CPPUNIT_ASSERT( !change.appeared );
		}
	}
}

void TC_TrackerGUral::NumberRepet()
{
	//������ �� M2 � �� L2
	auto reftime = TI.DetermineStripBusy( L"M2", *tracker );

	//����������� ����� �� M2
	const TrainDescr & refdescr = StoreTrainInfo( 1549, L"", L"M2", reftime );

	//�� �2 - ����� 1549
	CPPUNIT_ASSERT( TrainAtPlace( refdescr, L"M2" ) );

	//��� �� ����� �� L2
	reftime = TI.DetermineStripBusy( L"L2", *tracker );
	const TrainDescr & refdescr2 = StoreTrainInfo( 1549, L"", L"L2", reftime );

	//�� L2 - ����� 1549. �� M2 - ��� ��� ���� �� �����
	CPPUNIT_ASSERT( TrainAtPlace( refdescr2, L"L2" ) );
	CPPUNIT_ASSERT( !TrainAtPlace( refdescr2, L"M2" ) );
}

void TC_TrackerGUral::NumberRepetIndexNoRepet()
{
	//������ �� M2 � �� L2
	auto reftime = TI.DetermineStripBusy( L"M2", *tracker );

	//����������� ����� �� M2
	const TrainDescr & refdescr = StoreTrainInfo( 1549, L"111-33-44", L"M2", reftime );

	//�� �2 - ����� 1549
	CPPUNIT_ASSERT( TrainAtPlace( refdescr, L"M2" ) );

	//��� �� ����� � ������ �������� �� L2
	reftime = TI.DetermineStripBusy( L"L2", *tracker );
	const TrainDescr & refdescr2 = StoreTrainInfo( 1549, L"99-444-22", L"L2", reftime );

	//�� M2 � L2 - ������ ������ � ���������� ������� 1549
	CPPUNIT_ASSERT( TrainAtPlace( refdescr, L"M2" ) );
	CPPUNIT_ASSERT( TrainAtPlace( refdescr2, L"L2" ) );
}

void TC_TrackerGUral::NumberRepetIndexRepet()
{
	//������ �� M2 � �� L2
	auto reftime = TI.DetermineStripBusy( L"M2", *tracker );

	//����������� ����� �� M2
	const TrainDescr & refdescr = StoreTrainInfo( 1549, L"33-666-7", L"M2", reftime );

	//�� �2 - ����� 1549
	CPPUNIT_ASSERT( TrainAtPlace( refdescr, L"M2" ) );

	//��� �� ����� �� L2
	reftime = TI.DetermineStripBusy( L"L2", *tracker );
	const TrainDescr & refdescr2 = StoreTrainInfo( 1549, L"33-666-7", L"L2", reftime );

	//�� L2 - ����� 1549. �� M2 - ��� ��� ���� �� �����
	CPPUNIT_ASSERT( TrainAtPlace( refdescr2, L"L2" ) );
	CPPUNIT_ASSERT( !TrainAtPlace( refdescr2, L"M2" ) );
}


TrainDescr TC_TrackerGUral::StoreTrainInfo( unsigned int tnumber, wstring tindex, wstring place, time_t moment )
{
	//����������� ����� �� M2
	TrainDescr refdescr( to_wstring( tnumber ), tindex );
	tracker->SetTrainsInfo( CreateInfoPackage( false, 0, refdescr, TI.Get( place ).bdg, TrainCharacteristics::Source::Disposition, &moment ) );
	//tracker->SetTrainsInfo( CreateInfoPackage( refdescr, 0, moment, TI.Get( place ).bdg, TrainCharacteristics::Source::Disposition ) );
	return refdescr;
}

bool TC_TrackerGUral::TrainAtPlace( const TrainDescr & refdescr, wstring objname ) const
{
	bool found = false;
	const auto & objbadge = TI.Get( objname ).bdg;
	auto tlist = trainCont->GetUnities();
	for ( const auto train : tlist )
	{
		auto tdescrPtr = train->GetDescrPtr();
		if ( tdescrPtr && *tdescrPtr == refdescr )
		{
			const auto & tplace = train->GetPlace();
			auto tpIt = find( tplace.cbegin(), tplace.cend(), objbadge );
			found = ( tpIt != tplace.cend() );
			if ( found )
				break;
		}
	}
	return found;
}