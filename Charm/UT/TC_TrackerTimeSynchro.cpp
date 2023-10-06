#include "stdafx.h"
#include "TC_TrackerTimeSynchro.h"
#include <boost/date_time/posix_time/time_parsers.hpp>
#include "../Guess/TrainContainer.h"

using namespace std;

typedef TrainCharacteristics::Source Source;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerTimeSynchro );

void TC_TrackerTimeSynchro::setUp()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 50;
	timConstr.setDislocationDelays( DislocationDelays( chrono::seconds( 70 ), DislocationDelays::Detailization() ) );
	TI.Reset();
}

void TC_TrackerTimeSynchro::tearDown()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 0;
	timConstr.setDislocationDelays( DislocationDelays( chrono::seconds( 0 ), DislocationDelays::Detailization() ) );
	TI.Reset();
}

void TC_TrackerTimeSynchro::BoundaryStripBlinking()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 0;

	TI.DetermineStripBusy( L"C", *tracker );

	const auto & trainCont = tracker->GetTrainContainer();
	auto tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );

	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto tevents2 = tracker->GetTrainEvents();

	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	//дребезг:
	for ( unsigned int k = 0; k < 10; k++ )
	{
		TI.DetermineStripBusy( L"X", *tracker, false );
        TI.DetermineStripBusy( L"X", *tracker, false ); // повторная занятость в ту же секунду
        TI.DetermineStripFree( L"X", *tracker, false );
        TI.DetermineStripFree( L"X", *tracker, false ); // повторное освобождение в ту же секунду
		TI.FlushData( *tracker, false );
	}
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 2 );
	auto teIt = tevents.cbegin();
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::FORM && ( *teIt )->GetBadge() == TI.Get( L"C" ).bdg );
	++teIt;
	CPPUNIT_ASSERT( ( *teIt )->GetCode() == HCode::DEPARTURE && ( *teIt )->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::TrainIdentification()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.LPFThresholdSec = 5;
	timConstr.setDislocationDelays( DislocationDelays( chrono::seconds( 0 ), DislocationDelays::Detailization() ) );
	TI.Reset();

	TI.DetermineStripBusy( L"C", *tracker );
	const auto & tdescr = RandomTrainDescr();
	SetInfo( TI.Get( L"C" ).bdg, tdescr );
	const auto & thresholdMs = tracker->GetTimeConstraints().LPFThresholdSec * 1000;
	TI.IncreaseTime( thresholdMs );
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunity = tunities.back();
	CPPUNIT_ASSERT( tunity->IsTrain() );
	auto tunityDescr = *tunity->GetDescrPtr();
	CPPUNIT_ASSERT( tunityDescr == tdescr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}


void TC_TrackerTimeSynchro::TestRepeatValue()
{
    // исходное положение
    const auto & threshold = tracker->GetTimeConstraints().LPFThresholdSec;
    CPPUNIT_ASSERT( threshold >=2 );
    auto unities = tracker->GetTrainContainer().GetUnities();
    CPPUNIT_ASSERT_EQUAL( size_t(0), unities.size() );
    // дважды и вперемешку
    TI.DetermineStripBusy( L"Q", *tracker );
    TI.DetermineStripBusy( L"T", *tracker );
    TI.DetermineStripBusy( L"Q", *tracker );
    auto mom1 = TI.DetermineStripBusy( L"T", *tracker );
    TI.DetermineSwitchOutOfControl( L"15", *tracker );
    TI.DetermineSwitchOutOfControl( L"16", *tracker );

    unities = tracker->GetTrainContainer().GetUnities();
    CPPUNIT_ASSERT_EQUAL( size_t(0), unities.size() );

    // добавим пару посторонних (уже в следующий момент)
    auto mom2 = TI.DetermineStripBusy( L"M", *tracker );
    auto mom3 = TI.DetermineStripBusy( L"N", *tracker );
	unities = tracker->GetTrainContainer().GetUnities();
	CPPUNIT_ASSERT_EQUAL( size_t(0), unities.size() );
	TI.IncreaseTime( static_cast<unsigned int>( time_t( mom1 + threshold ) - mom2 ) * 1000 ); //в этот момент должны срабатывать начальные сигналы

    // проверим, что после threshold обработаны только Q и T
    unities = tracker->GetTrainContainer().GetUnities();
    CPPUNIT_ASSERT_EQUAL( size_t(2), unities.size() );
    // а в следующий момент обработаны все: Q T M N 
	TI.IncreaseTime( static_cast<unsigned int>( time_t( mom2 - mom1 - 1 ) * 1000 ) );
    unities = tracker->GetTrainContainer().GetUnities();
    CPPUNIT_ASSERT_EQUAL( size_t(4), unities.size() );

    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::DislokTimeoutBusy()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	const auto & dislokDelaySec = static_cast<unsigned int>( dislokDelays.getDelay( EsrKit(), ParkWayKit() ).count() );
	const auto & tdescr = RandomTrainDescr();
	SetInfo( TI.Get( L"C" ).bdg, tdescr, Source::Disposition );
	TI.IncreaseTime( ( thresholdSec + dislokDelaySec + 1 ) * 1000 );
	const auto & trainCont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"C", *tracker ); //приходит занятость, но слишком поздно
	TI.IncreaseTime( thresholdSec * 1000 );
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( !tunities.front()->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::DislokBusyTimeout()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	const auto & dislokDelaySec = static_cast<unsigned int>( dislokDelays.getDelay( EsrKit(), ParkWayKit() ).count() );
	const auto & tinfoDelayMs = ( thresholdSec + dislokDelaySec ) * 1000;
	const auto & dislocdescr = RandomTrainDescr();
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::Disposition );
	TI.IncreaseTime( tinfoDelayMs / 2 );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.IncreaseTime( tinfoDelayMs / 2 ); //сброс дислокации
	TI.IncreaseTime( thresholdSec * 1000 ); //приход занятости
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tdescrPtr = tunities.front()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == dislocdescr ); //успешная идентификация
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::DislokBusy()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	const auto & dislokDelaySec = static_cast<unsigned int>( dislokDelays.getDelay( EsrKit(), ParkWayKit() ).count() );
	const auto & tinfoDelayMs = ( thresholdSec + dislokDelaySec ) * 1000;
	const auto & dislocdescr = RandomTrainDescr();
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::Disposition );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.IncreaseTime( thresholdSec * 1000 ); //приход сигналов
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tdescrPtr = tunities.front()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == dislocdescr ); //успешная идентификация
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::BusyDislok()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	const auto & dislokDelaySec = static_cast<unsigned int>( dislokDelays.getDelay( EsrKit(), ParkWayKit() ).count() );
	const auto & dislocdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::Disposition );
	TI.IncreaseTime( thresholdSec * 1000 );
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tdescrPtr = tunities.front()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == dislocdescr ); //успешная идентификация
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::BusyDislok2()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislocdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	TI.IncreaseTime( thresholdSec * 1000 );
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::Disposition );
	TI.IncreaseTime( thresholdSec * 1000 ); //здесь должна произойти идентификация
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tdescrPtr = tunities.front()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == dislocdescr ); //успешная идентификация
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::BusyDislokFreeBusyTimeout()
{
	//проверка того, что поездной дескриптор не будет использован более одного раза

	const auto & trainCont = tracker->GetTrainContainer();
	TI.DetermineSwitchPlus( L"13", *tracker );
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	const auto & dislokDelaySec = static_cast<unsigned int>( dislokDelays.getDelay( EsrKit(), ParkWayKit() ).count() );
	const auto & tinfoDelayMs = ( thresholdSec + dislokDelaySec ) * 1000;
	const auto & dislocdescr = RandomTrainDescr( false );
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::Disposition );

	auto tunities = trainCont.GetUnities();

	TI.IncreaseTime( thresholdSec * 1000 );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curUnity = tunities.back();
	CPPUNIT_ASSERT( curUnity->IsTrain() && *curUnity->GetDescrPtr() == dislocdescr );

	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker ); //поезд не исчезает, а именно отъезжает
	TI.IncreaseTime( thresholdSec * 1000 );
	TI.DetermineStripBusy( L"C", *tracker ); //следующая ПЕ

	//приход нового сигнала занятости. к этому моменту поездная информация из буфера еще не должна удаляться по таймауту.
	//но она должна удаляться после ее назначения предыдущему поезду
	TI.IncreaseTime( thresholdSec * 1000 );
	
	tunities = trainCont.GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto gtrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 1 && tplace.front() == TI.Get( L"G" ).bdg;
	} );
	CPPUNIT_ASSERT( gtrainIt != tunities.cend() );
	auto gtrain = *gtrainIt;
	auto ctrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 1 && tplace.front() == TI.Get( L"C" ).bdg;
	} );
	CPPUNIT_ASSERT( ctrainIt != tunities.cend() );
	auto ctrain = *ctrainIt;
	auto tdescrPtr = gtrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == dislocdescr ); //успешная идентификация
	CPPUNIT_ASSERT( !ctrain->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::BusyNoextDislok()
{
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislocdescr = RandomTrainDescr();
	TI.DetermineStripBusy( L"C", *tracker );
	TI.IncreaseTime( thresholdSec * 1000 );
	SetInfo( TI.Get( L"C" ).bdg, dislocdescr, Source::User );
	TI.IncreaseTime( thresholdSec * 1000 );
	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tunities = trainCont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	CPPUNIT_ASSERT( train->IsTrain() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::ExtDislokBusy()
{
	const auto & disappDelay = tracker->GetTimeConstraints().disappTrainsHoldingMs();
	const auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	const auto & dislocdescr = RandomTrainDescr();

	const auto & trainCont = tracker->GetTrainContainer();
	const auto & tcharacts = TrainCharacteristics::instanceCRef();
	TrainCharacteristics::Source sources[] = { Source::Disposition, Source::Guess, Source::User };
	auto srcCount = size_array( sources );
	for( unsigned int i = 0; i < srcCount; ++i )
	{
		const auto & source = sources[i];
		SetInfo( TI.Get( L"C" ).bdg, dislocdescr, source );
		TI.IncreaseTime( thresholdSec * 1000 ); //если это информация не от внешней системы, она удаляется
		TI.DetermineStripBusy( L"C", *tracker );
		TI.IncreaseTime( thresholdSec * 1000 );
		const auto & tunities = trainCont.GetUnities();
		CPPUNIT_ASSERT( tunities.size() == 1 );
		auto train = tunities.front();
		const auto & srcInfo = tcharacts.GetSourceInfo( source );
		srcInfo.source == TrainCharacteristics::Source::Disposition ? CPPUNIT_ASSERT( train->IsTrain() ) : CPPUNIT_ASSERT( !train->IsTrain() );
		TI.DetermineStripFree( L"C", *tracker );
		TI.IncreaseTime( thresholdSec * 1000 );
		TI.IncreaseTime( disappDelay );
	}
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerTimeSynchro::HemDisformDelay()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	SetInfo( TI.Get( L"H" ).bdg, TrainDescr( L"100" ), Source::User );

	auto & thresholdSec = tracker->GetTimeConstraints().LPFThresholdSec;
	TI.IncreaseTime( thresholdSec * 1000 );

	const auto & trainCont = tracker->GetTrainContainer();
	auto trains = trainCont.GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 && trains.back()->IsTrain() );

	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"H", *tracker );
	TI.DetermineStripFree( L"W", *tracker );
	SetInfo( TI.Get( L"Q" ).bdg, TrainDescr( L"100" ), Source::Hem );
	DisformTrain( trains.back()->GetId(), Source::Hem );

	TI.IncreaseTime( thresholdSec * 1000 );
	trains = trainCont.GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	CPPUNIT_ASSERT( !trains.back()->IsTrain() );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 3 ); //FORM + SPAN_MOVE + DISFORM
	CPPUNIT_ASSERT( tevents.back()->GetCode() == HCode::DISFORM && tevents.back()->GetSource() == TrainCharacteristics::Source::Hem );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

bool TC_TrackerTimeSynchro::IdentifiedAsTrain( const BadgeE & bdg, const TrainDescr & checkDescr ) const
{
	const auto & trainCont = tracker->GetTrainContainer();
	auto tunityPtr = trainCont.GetUnity( bdg );
	bool identifiedAsTrain = false;
	if( tunityPtr && tunityPtr->IsTrain() )
	{
		if ( auto descrPtr = tunityPtr->GetDescrPtr() )
			identifiedAsTrain = ( *descrPtr == checkDescr );
	}
	return identifiedAsTrain;
}

void TC_TrackerTimeSynchro::GlobalDelayVersusParkWayDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool globalDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto globalDelay = ( globalDelayGreater ? bigDelay : smallDelay );
		auto cwayDelay = ( globalDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		auto ckey = make_pair( EsrKit( TI.Get( L"C" ).bdg.num() ), ParkWayKit( 3, 1 ) );
		waysDetalization.insert( make_pair( ckey, cwayDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( globalDelay, waysDetalization ) );
		TI.Reset();

		const auto & cwayDislocdescr = RandomTrainDescr(); //тестируемый детализированный путь
		const auto & dwayDislocdescr = RandomTrainDescr(); //одинаковый ЕСР-код, парк-путь - нет
		const auto & mwayDislocdescr = RandomTrainDescr(); //не совпадает ничего

		//пути C и D - разные пути но на одной и той же станции
		SetInfo( TI.Get( L"C" ).bdg, cwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"D" ).bdg, dwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"M" ).bdg, mwayDislocdescr, Source::Disposition );
		
		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );

		TI.DetermineStripBusy( L"C", *tracker );
		TI.DetermineStripBusy( L"D", *tracker );
		TI.DetermineStripBusy( L"M", *tracker );

		if ( globalDelayGreater )
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"D" ).bdg, dwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"D" ).bdg, dwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerTimeSynchro::GlobalDelayVersusOnlyWayDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool globalDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto globalDelay = ( globalDelayGreater ? bigDelay : smallDelay );
		auto wayDelay = ( globalDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		auto ckey = make_pair( EsrKit( TI.Get( L"C" ).bdg.num() ), ParkWayKit( 1 ) );
		waysDetalization.insert( make_pair( ckey, wayDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( globalDelay, waysDetalization ) );
		TI.Reset();

		const auto & cwayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и путь
		const auto & dwayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код, парк-путь - нет
		const auto & ewayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и путь
		const auto & mwayDislocdescr = RandomTrainDescr(); //ЕСР-код не совпадает

		//пути C и D - разные пути но на одной и той же станции
		SetInfo( TI.Get( L"C" ).bdg, cwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"D" ).bdg, dwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"E" ).bdg, ewayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"M" ).bdg, mwayDislocdescr, Source::Disposition );
		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );
		TI.DetermineStripBusy( L"C", *tracker );
		TI.DetermineStripBusy( L"D", *tracker );
		TI.DetermineStripBusy( L"E", *tracker );
		TI.DetermineStripBusy( L"M", *tracker );
		if ( globalDelayGreater )
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"D" ).bdg, dwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"E" ).bdg, ewayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"D" ).bdg, dwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"E" ).bdg, ewayDislocdescr ) );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerTimeSynchro::GlobalDelayVersusOnlyParkDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool globalDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto globalDelay = ( globalDelayGreater ? bigDelay : smallDelay );
		auto parkDelay = ( globalDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		auto r4key = make_pair( EsrKit( TI.Get( L"R4" ).bdg.num() ), ParkWayKit( 4, 0 ) );
		waysDetalization.insert( make_pair( r4key, parkDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( globalDelay, waysDetalization ) );
		TI.Reset();

		const auto & r4wayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и парк
		const auto & mwayDislocdescr = RandomTrainDescr(); //ЕСР-код не совпадает

		SetInfo( TI.Get( L"R4" ).bdg, r4wayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"M" ).bdg, mwayDislocdescr, Source::Disposition );
		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );
		TI.DetermineStripBusy( L"R4", *tracker );
		TI.DetermineStripBusy( L"M", *tracker );

		if ( globalDelayGreater )
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"R4" ).bdg, r4wayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"R4" ).bdg, r4wayDislocdescr ) );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerTimeSynchro::GlobalDelayVersusStationDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool globalDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto globalDelay = ( globalDelayGreater ? bigDelay : smallDelay );
		auto stationDelay = ( globalDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		auto r4key = make_pair( EsrKit( TI.Get( L"R4" ).bdg.num() ), ParkWayKit() );
		waysDetalization.insert( make_pair( r4key, stationDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( globalDelay, waysDetalization ) );
		TI.Reset();

		const auto & s4wayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код
		const auto & mwayDislocdescr = RandomTrainDescr(); //ЕСР-код не совпадает

		SetInfo( TI.Get( L"S4" ).bdg, s4wayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"M" ).bdg, mwayDislocdescr, Source::Disposition );
		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );
		TI.DetermineStripBusy( L"D", *tracker );
		TI.DetermineStripBusy( L"S4", *tracker );
		TI.DetermineStripBusy( L"M", *tracker );
		if ( globalDelayGreater )
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"S4" ).bdg, s4wayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"S4" ).bdg, s4wayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"M" ).bdg, mwayDislocdescr ) );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerTimeSynchro::ParkWayDelayVersusOnlyParkDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool parkwayDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto parkwayDelay = ( parkwayDelayGreater ? bigDelay : smallDelay );
		auto parkDelay = ( parkwayDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		auto ackey = make_pair( EsrKit( TI.Get( L"AC" ).bdg.num() ), ParkWayKit( 2, 1 ) );
		waysDetalization.insert( make_pair( ackey, parkwayDelay ) );
		auto parkKey = make_pair( EsrKit( TI.Get( L"AH" ).bdg.num() ), ParkWayKit( 2, 0 ) );
		waysDetalization.insert( make_pair( parkKey, parkDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( chrono::seconds( 0 ), waysDetalization ) );
		TI.Reset();

		const auto & acwayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и парк/путь
		const auto & ahwayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и парк

		SetInfo( TI.Get( L"AC" ).bdg, acwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"AH" ).bdg, ahwayDislocdescr, Source::Disposition );
		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );
		TI.DetermineStripBusy( L"AC", *tracker );
		TI.DetermineStripBusy( L"AH", *tracker );
		if ( parkwayDelayGreater )
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"AC" ).bdg, acwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"AH" ).bdg, ahwayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"AC" ).bdg, acwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"AH" ).bdg, ahwayDislocdescr ) );
		}
		CPPUNIT_ASSERT( !TI.TakeExceptions() );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
	}
}

void TC_TrackerTimeSynchro::ParkWayDelayVersusOnlyWayDelay()
{
	auto bigDelay = chrono::seconds( 22 );
	auto smallDelay = chrono::seconds( 6 );
	const auto & smallDelaySec = static_cast<unsigned int>( smallDelay.count() );

	for ( auto i = 0; i < 2; ++i )
	{
		bool parkwayDelayGreater = ( i == 0 );

		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		timConstr.LPFThresholdSec = 0;

		auto parkwayDelay = ( parkwayDelayGreater ? bigDelay : smallDelay );
		auto wayDelay = ( parkwayDelay == bigDelay ? smallDelay : bigDelay );

		DislocationDelays::Detailization waysDetalization;
		const auto & ecode = EsrKit( TI.Get( L"C" ).bdg.num() );
		auto ckey = make_pair( ecode, ParkWayKit( 3, 1 ) );
		waysDetalization.insert( make_pair( ckey, parkwayDelay ) );
		auto wayKey = make_pair( ecode, ParkWayKit( 1 ) );
		waysDetalization.insert( make_pair( wayKey, wayDelay ) );
		timConstr.setDislocationDelays( DislocationDelays( chrono::seconds( 0 ), waysDetalization ) );
		TI.Reset();

		const auto & cwayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и парк/путь
		const auto & ewayDislocdescr = RandomTrainDescr(); //совпадает ЕСР-код и путь

		SetInfo( TI.Get( L"C" ).bdg, cwayDislocdescr, Source::Disposition );
		SetInfo( TI.Get( L"E" ).bdg, ewayDislocdescr, Source::Disposition );

		TI.IncreaseTime( ( smallDelaySec + 1 ) * 1000 );
		TI.DetermineStripBusy( L"C", *tracker );
		TI.DetermineStripBusy( L"E", *tracker );
		if ( parkwayDelayGreater )
		{
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"E" ).bdg, ewayDislocdescr ) );
		}
		else
		{
			CPPUNIT_ASSERT( !IdentifiedAsTrain( TI.Get( L"C" ).bdg, cwayDislocdescr ) );
			CPPUNIT_ASSERT( IdentifiedAsTrain( TI.Get( L"E" ).bdg, ewayDislocdescr ) );
		}
		tracker->Reset();
	}
}