#include "stdafx.h"
#include "TC_TrackerSpanStopping.h"
#include "../Guess/TrainContainer.h"

using namespace std;

static const std::string intervalSec_xAttr = "intervalSec";

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerSpanStopping );

void TC_TrackerSpanStopping::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.disappTrainsHolding = std::chrono::hours( 2 );
	timConstr.bufferLimitMs = 10 * 3600 * 1000; //десять часов
	timConstr.entranceStoppingLimitSec = 18 * 60; //18 минут
}

void TC_TrackerSpanStopping::CasualCaseWithLengthWithPicketage()
{
	//стоянка на участке W (длина 11000 метров, расстояние между стыками 5000 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 5000 метров за 50 минут (3000 секунд).

	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.IncreaseTime( 2900 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 200 * 1000 ); //генерация стоянки на перегоне

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN ); 
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - WEntryMom );

	//окончание стоянки на перегоне
	TI.DetermineStripBusy( L"H", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWithLengthWOPicketage()
{
	//стоянка на участке H (длина 7000 метров, расстояние между стыками неизвестно).
	//длина определяется по непосредственной длине участка.
	//поезд должен проехать 7000 метров за 70 минут (4200 секунд).

	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	time_t HEntryMom = TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	TI.IncreaseTime( 4100 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 200 * 1000 ); //генерация стоянки на перегоне

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - HEntryMom );

	//окончание стоянки на перегоне
	time_t KEntryMom = TI.DetermineStripBusy( L"K", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == KEntryMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWOLengthWithPicketage()
{
	//стоянка на участке Q (длина не определена, расстояние между стыками 1550 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 1550 метров за 15.5 минут (930 секунд).

	TI.DetermineStripBusy( L"A", *tracker );
	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );

	TI.IncreaseTime( 910 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 50 * 1000 ); //генерация стоянки на перегоне

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - QEntryMom );

	//окончание стоянки на перегоне
	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == WEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWOLengthWOPicketage()
{
	//стоянка на участке X (длина не определена, расстояние между стыками неизвестно).
	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 1150 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 100 * 1000 ); //генерация стоянки на перегоне

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - XEntryMom );

	//окончание стоянки на перегоне
	time_t IEntryMom = TI.DetermineStripBusy( L"I", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == IEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterReborn()
{
	//продолжительное нахождение на участке X, пропадание и восстановление поезда, приводящее к 
	//генерации начала стоянки на перегоне, фиксируемой от момента до пропадания поезда

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //поезд стоит на X некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 800 * 1000 ); //поезд отсутствует значительное время, превышающее интервал стоянки на перегоне
	time_t rebornMom = TI.DetermineStripBusy( L"X", *tracker ); //восстановление там же

	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //событие стоянки должно быть после формирования
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 8 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	auto spotDetPtr = ( *tevIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == rebornMom - XEntryMom );

	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterReborn2()
{
	//продолжительное нахождение на участке X, пропадание и восстановление поезда, приводящее к 
	//генерации начала стоянки на перегоне, фиксируемой от момента до пропадания поезда

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //поезд стоит на X некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 500 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //стоянки нет
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	TI.IncreaseTime( 200 * 1000 );

	trackerEvents = tracker->GetTrainEvents(); //стоянка есть
	CPPUNIT_ASSERT( any_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterWideReborn()
{
	//продолжительное нахождение на участке X, пропадание и восстановление на нескольких участках, приводящее к 
	//генерации начала стоянки на перегоне

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //поезд стоит некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"I", *tracker );
	TI.IncreaseTime( 800 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.FlushData( *tracker, false ); //стоянка есть

	tunities = tcont.GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->Head() == TI.Get( L"I" ).bdg );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( any_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
			return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
		} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterWideReborn()
{
	//продолжительное нахождение на участке X, пропадание и восстановление на нескольких участках, не приводящее к 
	//генерации начала стоянки на перегоне

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //поезд стоит некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 800 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.FlushData( *tracker, false ); //стоянки нет

	tunities = tcont.GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->Head() == TI.Get( L"I" ).bdg );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRebornNear()
{
	//продолжительное нахождение на участке X, пропадание и восстановление поезда на соседнем участке

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //поезд стоит на X некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 800 * 1000 );

	auto rebornMom = TI.DetermineStripBusy( L"I", *tracker ); //восстановление рядом

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterRebornAndWaiting()
{
	//событие стоянки генерируется в процессе ожидания после продолжительного нахождения в "небытии" и восстановлении.

	//пропадание на участке X и генерация начала стоянки на перегоне, фиксируемой в момент пропадания поезда
	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 800 * 1000 ); //поезд отсутствует продолжительное время (стоянки на перегоне все еще нет)
	TI.DetermineStripBusy( L"X", *tracker ); //восстановление там же
	time_t currentMom = TI.IncreaseTime( 600 * 1000 ); //поезд стоит на X некоторое время, необходимое для генерации стоянки на перегоне

	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //событие стоянки должно быть после формирования
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 8 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	auto spotDetPtr = ( *tevIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - XEntryMom );

	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRelight()
{
	//продолжительное нахождение на участке X, длительное пропадание и невосстановление поезда, не приводящее к 
	//генерации начала стоянки на перегоне

	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).
	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();

	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //поезд стоит на X некоторое время

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 3 * 3600 * 1000 ); //поезд отсутствует значительное время, превышающее интервал стоянки на перегоне и интервал для восстановления
	TI.DetermineStripBusy( L"X", *tracker ); //восстановление там же
	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //событие стоянки должно быть после формирования
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() != trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() != trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRelightAndWaiting()
{
	//событие стоянки генерируется в процессе ожидания после продолжительного нахождения в "небытии" и невосстановлении из-за превышения лимита восстановления

	//пропадание на участке X. после восстановления и ожидания стоянка на перегоне отсутствует.
	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).
	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();

	TI.DetermineStripFree( L"G", *tracker );

	//происходит моргание участка
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 3 * 3600 * 1000 ); //поезд отсутствует продолжительное время
	TI.DetermineStripBusy( L"X", *tracker ); //восстановление там же
	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //событие стоянки должно быть после формирования
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() != trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() != trainId );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::ParallelActivity()
{
	//исчезновение на участке X, возникновение ПЕ на другом перегоне, восстановление поезда на участке X

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 1400 * 1000 ); //поезд находится на X значительное время, превышающее интервал стоянки на перегоне
	TI.DetermineStripBusy( L"W", *tracker ); //появление другого поезда на другом перегоне
	TI.DetermineStripBusy( L"I", *tracker ); //окончание стоянки на перегоне исходного поезда

	auto trackerEvents = tracker->GetTrainEvents(); //событие стоянки должно быть после формирования
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingAndPocketEntry()
{
	//стоянка на участке PK3 (длина не определена, расстояние между стыками 440 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 440 метров за 4,4 минуты (264 секунды).

	TI.DetermineStripBusy( L"PK2", *tracker );
	time_t PK2EntryMom = TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK2", *tracker );

	TI.IncreaseTime( 260 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 10 * 1000 ); //генерация стоянки на перегоне
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() && trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - PK2EntryMom );

	//окончание стоянки на перегоне и уход в карман
	time_t PocketEntryMom = TI.DetermineStripFree( L"PK3", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() && ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == PocketEntryMom );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() && ( *eventIt )->GetCode() == HCode::POCKET_ENTRY && ( *eventIt )->GetTime() == PocketEntryMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingIntoPocket()
{
	//стоянка в кармане 2 (длина кармана = 420 (PK9) + 1250 (PK10) + 2000 (PK11) = 3670 метров ).
	//длина определяется по сумме длин всех участков, входящих в карман.
	//поезд должен проехать 3670 метров за 36,7 минуты (2202 секунды).

	TI.DetermineStripBusy( L"PK7", *tracker );
	TI.DetermineStripBusy( L"PK8", *tracker );
	TI.DetermineStripFree( L"PK7", *tracker );
	time_t PocketEntryMom = TI.DetermineStripFree( L"PK8", *tracker );

	TI.IncreaseTime( 2180 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 30 * 1000 ); //генерация стоянки на перегоне
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );

	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() && ( *spanStopIt )->GetBadge() == TI.Get( L"PK10" ).bdg );
	auto spotDetPtr = ( *spanStopIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - PocketEntryMom );

	//окончание стоянки на перегоне и выход из кармана
	time_t PocketExitMom = TI.DetermineStripBusy( L"PK12", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK10" ).bdg );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::POCKET_EXIT && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK12" ).bdg );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_MOVE && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK12" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SeveralStops()
{
	//стоянка на участке H (4200 секунд) и участке W (3000 секунд)

	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"K", *tracker );

	//генерация первой стоянки
	TI.IncreaseTime( 4220 * 1000 );

	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );

	//генерация второй стоянки
	TI.IncreaseTime( 3020 * 1000 );

	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	auto startStopStartCount = count_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( startStopStartCount == 2 );
	auto startStopEndCount = count_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( startStopEndCount == 2 );
	auto spanStopStartIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopStartIt != trackerEvents.cend() && ( *spanStopStartIt )->GetBadge() == TI.Get( L"H" ).bdg );
	auto spanStopEndIt = find_if( spanStopStartIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt != trackerEvents.cend() && ( *spanStopEndIt )->GetTime() == WEntryMom && ( *spanStopEndIt )->GetBadge() == TI.Get( L"H" ).bdg );
	spanStopStartIt = find_if( spanStopEndIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopStartIt != trackerEvents.cend() && ( *spanStopStartIt )->GetBadge() == TI.Get( L"W" ).bdg );
	spanStopEndIt = find_if( spanStopStartIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt != trackerEvents.cend() && ( *spanStopEndIt )->GetTime() == QEntryMom && ( *spanStopEndIt )->GetBadge() == TI.Get( L"W" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::StopBeforeStation()
{
	//стоянка на участке Q (длина не определена, расстояние между стыками 1550 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 1550 метров за 15.5 минут (930 секунд).
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	TI.IncreaseTime( 910 * 1000 ); //стоянки на перегоне все еще нет
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	TI.IncreaseTime( 50 * 1000 ); //генерация стоянки на перегоне
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() && trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );

	//окончание стоянки на перегоне
	time_t AEntryMom = TI.DetermineStripBusy( L"A", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );

	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == AEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingEarlierThanEntraStopping()
{
	//стоянка на участке Q (длина не определена, расстояние между стыками 1550 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 1550 метров за 15.5 минут (930 секунд).

	//допустимый интервал стоянки перед входным имеет БОЛЬШЕЕ значение (18 минут по умолчанию)

	//въезд на Q
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	//увеличение времени за два шага, для имитации реального хода времени (чтобы события созадавались в естественной последовательности)
	TI.IncreaseTime( 16 * 60 * 1000 ); //стоянка 16 минут
	TI.IncreaseTime( 4 * 60 * 1000 ); //стоянка 4 минуты

	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() );
	trackerEvents = tracker->GetTrainEvents();
	auto entraStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( entraStopIt == trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::EntraStoppingEarlierThanSpanStopping()
{
	//стоянка на участке I (длина не определена, расстояние между стыками неизвестно).
	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	//допустимый интервал стоянки перед входным имеет МЕНЬШЕЕ значение (18 минут по умолчанию)

	//въезд на I
	TI.DetermineHeadClose( L"Head108", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	time_t IEntryMom = TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );

	//увеличение времени за два шага, для имитации реального хода времени (чтобы события созадавались в естественной последовательности)
	TI.IncreaseTime( 19 * 60 * 1000 ); //стоянка 19 минут
	TI.IncreaseTime( 2 * 60 * 1000 ); //стоянка 2 минуты

	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );
	trackerEvents = tracker->GetTrainEvents();
	auto entraStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( entraStopIt != trackerEvents.cend() );

	//въезд на станцию
	TI.DetermineStripBusy( L"J", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );
	auto spanStopEndIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt == trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::EntraStoppingEarlierThanSpanStopping2()
{
	//стоянка на участке I (длина не определена, расстояние между стыками неизвестно).
	//длина определяется умолчательным значением (2000 метров).
	//поезд должен проехать 2000 метров за 20 минут (1200 секунд).

	//допустимый интервал стоянки перед входным имеет МЕНЬШЕЕ значение (18 минут по умолчанию)

	//въезд на I
	TI.DetermineHeadClose( L"Head108", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );

	TI.IncreaseTime( 19 * 60 * 1000 ); //стоянка 19 минут (генерация стоянки перед входным)

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto backEvent = trackerEvents.back();
	CPPUNIT_ASSERT( backEvent->GetCode() == HCode::ENTRANCE_STOPPING );
	TI.DetermineHeadOpen( L"Head108", *tracker );

	TI.IncreaseTime( 5 * 60 * 1000 ); //продолжение стоянки длительностью, достаточной для генерации стоянки на перегоне
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"X", *tracker ); //освобождение хвоста, ошибочно приводящее к генерации стоянки на перегоне

	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NoTrainSpanStopping()
{
	//стоянка на участке W (длина 11000 метров, расстояние между стыками 5000 метров).
	//длина определяется по расстоянию между стыками.
	//поезд должен проехать 5000 метров за 50 минут (3000 секунд).
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	time_t bornMom = TI.FlushData( *tracker, false );
	TI.IncreaseTime( 3100 * 1000 ); //стоянки на перегоне быть не должно, т.к. это не поезд
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	//ПЕ смещается и становится поездом
	//стоянка на участке H (длина 7000 метров, расстояние между стыками неизвестно).
	//длина определяется по непосредственной длине участка.
	//поезд должен проехать 7000 метров за 70 минут (4200 секунд).
	TI.DetermineStripBusy( L"H", *tracker );
	TI.IncreaseTime( 4250 * 1000 ); //есть стоянка на перегоне
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}