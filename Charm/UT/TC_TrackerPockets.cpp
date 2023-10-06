#include "stdafx.h"
#include "TC_TrackerPockets.h"
#include "../UT/TestTracker.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainUnity.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerPockets );
void TC_TrackerPockets::setUp()
{
	TI.Reset();
}

void TC_TrackerPockets::BusyInsensibility()
{
	TI.DetermineStripBusy( L"PK4", *tracker );
	TI.DetermineStripBusy( L"PK5", *tracker );
	TI.DetermineStripBusy( L"PK6", *tracker );
	TI.DetermineStripBusy( L"PK9", *tracker );
	TI.DetermineStripBusy( L"PK10", *tracker );
	TI.DetermineStripBusy( L"PK11", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.empty() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::FreeInsensibility()
{
	//простой въезд
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	changes = tracker->TakeTrainChanges();

	//зачистка "кармана" целиком
	TI.DetermineStripFree( L"PK4", *tracker );
	TI.DetermineStripFree( L"PK5", *tracker );
	TI.DetermineStripFree( L"PK6", *tracker );
	list <TrainUnityCPtr> trainList2 = trainCont->GetUnities();
	//поезд все еще в "кармане"
	CPPUNIT_ASSERT( trainList2.size() == 1 );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::TrainKeeping()
{
	//атомарно смещаем ПЕ по всем участкам перегона
	//появление:
	TI.DetermineStripBusy( L"PK2", *tracker );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr train = trainList.front();
	SetRandomOddInfo( train );

	//сдвиг:
	TI.DetermineStripFree( L"PK2", *tracker, false );
	TI.DetermineStripBusy( L"PK3", *tracker, false );
	TI.FlushData( *tracker, false );
	//уход в "карман":
	TI.DetermineStripFree( L"PK3", *tracker );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	//выход из "кармана":
	TI.DetermineStripBusy( L"PK7", *tracker );
	//сдвиг:
	TI.DetermineStripFree( L"PK7", *tracker, false );
	TI.DetermineStripBusy( L"PK8", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	//уход в "карман":
	TI.DetermineStripFree( L"PK8", *tracker );
	//выход из "кармана":
	TI.DetermineStripBusy( L"PK12", *tracker );
	//сдвиг:
	TI.DetermineStripFree( L"PK12", *tracker, false );
	TI.DetermineStripBusy( L"PK13", *tracker, false );
	TI.FlushData( *tracker, false );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 8 );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK2").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK3").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK5").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK7").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK8").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK10").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK12").bdg, false ) );
	CPPUNIT_ASSERT( FindIn( changes.placeViewChanges, TI.Get(L"PK13").bdg, true ) );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	TrainUnityCPtr now_train = trainList.front();
	CPPUNIT_ASSERT( train->GetId() == now_train->GetId() );
	auto place = now_train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 1 );
	CPPUNIT_ASSERT( place.front() == TI.Get(L"PK13").bdg );
	const auto & events = tracker->GetTrainEvents();
	VerifyHemPath( events );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::VerifyHemPath( const list <TrackerEventPtr> & events )
{
	CPPUNIT_ASSERT( events.size() == 10 );
	auto evIt = events.begin();
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK2").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK3").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::POCKET_ENTRY );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK3").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::POCKET_EXIT );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK7").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK7").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK8").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::POCKET_ENTRY );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK8").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::POCKET_EXIT );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK12").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK12").bdg );
	CPPUNIT_ASSERT( (*evIt)->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( (*evIt++)->GetBadge() == TI.Get(L"PK13").bdg );
}

void TC_TrackerPockets::MultiTrains()
{
	//помещение двух ПЕ в один "карман" и одной ПЕ в другой "карман"
	auto train1 = MoveThrow( L"PK2", L"PK3" );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train2 = MoveThrow( L"PK13", L"PK12" );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	auto train3 = MoveThrow( L"PK1", L"PK2", L"PK3" );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 3 );

	//перемещение ПЕ из "кармана" в "карман"
	auto train = MoveThrow( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train1 );
	train = MoveThrow( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train3 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );

	//выезд трех ПЕ из "кармана"
	MoveThrow( L"PK12", L"PK13" );
	train = MoveThrow( L"PK15", L"PK16" );
	CPPUNIT_ASSERT( train == train2 );
	MoveThrow( L"PK12", L"PK13" );
	train = MoveThrow( L"PK15", L"PK16" );
	CPPUNIT_ASSERT( train == train1 );
	MoveThrow( L"PK12", L"PK13" );
	train = MoveThrow( L"PK15", L"PK16" );
	CPPUNIT_ASSERT( train == train3 );
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

TrainUnityCPtr TC_TrackerPockets::MoveThrow( wstring apprstrip1, wstring apprstrip2 )
{
	TI.DetermineStripBusy( apprstrip1, *tracker );
	auto tunities = trainCont->GetUnities();
	TrainUnityCPtr train = trainCont->GetUnity( TI.Get( apprstrip1 ).bdg );
	TI.DetermineStripFree( apprstrip1, *tracker, false );
	TI.DetermineStripBusy( apprstrip2, *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( apprstrip2, *tracker );
	return train;
}

TrainUnityCPtr TC_TrackerPockets::MoveThrow( wstring apprstrip1, wstring apprstrip2, wstring apprstrip3 )
{
	TI.DetermineStripBusy( apprstrip1, *tracker );
	TrainUnityCPtr train = trainCont->GetUnity( TI.Get( apprstrip1 ).bdg );
	TI.DetermineStripFree( apprstrip1, *tracker, false );
	TI.DetermineStripBusy( apprstrip2, *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( apprstrip2, *tracker, false );
	TI.DetermineStripBusy( apprstrip3, *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( apprstrip3, *tracker );
	return train;
}

void TC_TrackerPockets::ResetInPockets()
{
	//помещение ПЕ в "карман"
	auto train1 = MoveThrow( L"PK2", L"PK3" );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.front();
	auto place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 1 && place.front() == TI.Get( L"PK5" ).bdg );
	tracker->Reset();
	trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.empty() );
	CPPUNIT_ASSERT( !trainCont->IntoPocket( train ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::LongTrainMoveToPocket()
{
	//возникновение ПЕ
	TI.DetermineStripBusy( L"PK7", *tracker, false );
	TI.DetermineStripBusy( L"PK8", *tracker, false );
	TI.FlushData( *tracker, false );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.back();
	auto place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 2 );

	//уход в "карман"
	TI.DetermineStripFree( L"PK7", *tracker, false );
	TI.DetermineStripFree( L"PK8", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
    CPPUNIT_ASSERT( !trainList.empty() );
	train = trainList.back();
	place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 1 );
	CPPUNIT_ASSERT( place.front() == TI.Get( L"PK5" ).bdg || place.front() == TI.Get( L"PK10" ).bdg );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::LongTrainMoveOutPocket()
{
	//помещение ПЕ "карман"
	auto train1 = MoveThrow( L"PK2", L"PK3" );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.back();
	auto place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 1 && place.front() == TI.Get( L"PK5" ).bdg );

	//выход из "кармана"
	TI.DetermineStripBusy( L"PK7", *tracker, false );
	TI.DetermineStripBusy( L"PK8", *tracker, false );
	TI.FlushData( *tracker, false );
	trainList = trainCont->GetUnities();
	train = trainList.back();
	place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 2 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::SetInfo()
{
	//помещение ПЕ "карман"
	auto train1 = MoveThrow( L"PK2", L"PK3" );
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.back();
	auto tdescr1 = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescr1 != nullptr );
	auto place = train->GetPlace();
	CPPUNIT_ASSERT( place.size() == 1 && place.front() == TI.Get( L"PK5" ).bdg );
	SetRandomOddInfo( train );
	auto tdescr2 = train->GetDescrPtr();
	CPPUNIT_ASSERT( tdescr2 != nullptr );
	CPPUNIT_ASSERT( tdescr1 != tdescr2 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::BidirectOut()
{
	//помещение ПЕ "карман"
	auto train = MoveThrow( L"PK2", L"PK3" );

	//"выезд в обе стороны"
	TI.DetermineStripBusy( L"PK7", *tracker, false );
	TI.DetermineStripBusy( L"PK8", *tracker, false );
	TI.DetermineStripBusy( L"PK3", *tracker, false );
	TI.FlushData( *tracker, false );

	const auto & trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 2 );
	auto train1 = trainList.front();
	auto train2 = trainList.back();
	//проверить выезд в нужную сторону
	if ( train1->GetPlace().size() == 2 )
	{
		CPPUNIT_ASSERT( train == train1 );
		CPPUNIT_ASSERT( train != train2 );
	}
	else
	{
		CPPUNIT_ASSERT( train2->GetPlace().size() == 2 );
		CPPUNIT_ASSERT( train == train2 );
		CPPUNIT_ASSERT( train != train1 );
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::BecomeTrainOnEntry()
{
	//простой въезд
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker );

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	CPPUNIT_ASSERT( trainList.back()->IsTrain() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::DeleteTrain()
{
	//простой въезд
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker );

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 1 );
	auto train = trainList.back();
	TrainContainer & nconstTCont = tracker->GetTrainContainer();
	nconstTCont.DeleteUnity( *train, list<BadgeE>() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::HeterogeneousMoving()
{
	//простые, но разнородные движения (карманные и обычные)
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripBusy( L"PK8", *tracker );
	TI.DetermineStripBusy( L"PK13", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker, false );
	TI.DetermineStripFree( L"PK8", *tracker, false );
	TI.DetermineStripFree( L"PK13", *tracker, false );
	TI.DetermineStripBusy( L"PK12", *tracker, false );
	TI.FlushData( *tracker, false );

	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( trainList.size() == 3 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::DeleteAmongMany()
{
	//простой въезд в "карман" трех поездов
	TrainUnityCPtr midtrain;
	for ( unsigned int k = 0; k < 3; ++k )
	{
		auto train = MoveThrow( L"PK1", L"PK2", L"PK3" );
		if ( k == 1 ) 
			midtrain = train;
	}
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	CPPUNIT_ASSERT( midtrain );
	CPPUNIT_ASSERT( trains.size() == 3 );

	//удаление среднего из въехавших
	TrainContainer & nconstTCont = tracker->GetTrainContainer();
	nconstTCont.DeleteUnity( *midtrain, list<BadgeE>() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::MultiTrainsPositioning()
{
	// Бейджи участков карманов
	const BadgeE pk3 = TI.Get( L"PK4" ).bdg;
	const BadgeE pk4 = TI.Get( L"PK5" ).bdg;
	const BadgeE pk5 = TI.Get( L"PK6" ).bdg;
	const BadgeE pk8 = TI.Get( L"PK9" ).bdg;
	const BadgeE pk9 = TI.Get( L"PK10" ).bdg;
	const BadgeE pk10 = TI.Get( L"PK11" ).bdg;
	
	// Заводим в первый карман с тремя участками подряд четыре ПЕ

	auto train1 = MoveThrow( L"PK2", L"PK3" );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане одна, должна располагаться в середине", pk4, train1->GetPlace().front());

	//проверка произведенных изменений
	changes = tracker->TakeTrainChanges();
	auto chng1 = FindIn( changes.placeViewChanges, TI.Get( L"PK5" ).bdg );
	CPPUNIT_ASSERT( chng1 && chng1->appeared && train1->GetId() == chng1->trainId );

	auto train2 = MoveThrow( L"PK2", L"PK3" );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk4, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk3, train2->GetPlace().front());

	//проверка произведенных изменений
	changes = tracker->TakeTrainChanges();
	auto chng2 = FindIn( changes.placeViewChanges, TI.Get( L"PK4" ).bdg );
	CPPUNIT_ASSERT( chng1 && chng2->appeared && train2->GetId() == chng2->trainId );

	auto train3 = MoveThrow( L"PK2", L"PK3" );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk5, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk4, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk3, train3->GetPlace().front());

	//проверка произведенных изменений
	changes = tracker->TakeTrainChanges();
	auto chng31 = FindIn( changes.placeViewChanges, TI.Get( L"PK4" ).bdg );
	auto chng32 = FindIn( changes.placeViewChanges, TI.Get( L"PK5" ).bdg );
	auto chng33 = FindIn( changes.placeViewChanges, TI.Get( L"PK6" ).bdg );
	CPPUNIT_ASSERT( chng31 && chng31->appeared && train3->GetId() == chng31->trainId );
	CPPUNIT_ASSERT( chng32 && chng32->appeared && train2->GetId() == chng32->trainId );
	CPPUNIT_ASSERT( chng33 && chng33->appeared && train1->GetId() == chng33->trainId );

	auto train4 = MoveThrow( L"PK2", L"PK3" );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, крайние ПЕ остаются в крайнем участке", pk5, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, крайние ПЕ остаются в крайнем участке", pk5, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, расположены последовательно", pk4, train3->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, расположены последовательно", pk3, train4->GetPlace().front());

	// Выводим все ПЕ из первого кармана во второй

	auto train = MoveThrow ( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train1 );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане одна, расположена по центру кармана", pk9, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk5, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk4, train3->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk3, train4->GetPlace().front());

	train = MoveThrow ( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train2 );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk10, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk9, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk4, train3->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане две, расположены последовательно", pk3, train4->GetPlace().front());

	train = MoveThrow ( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train3 );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk10, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk9, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане три, расположены последовательно", pk8, train3->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане одна, должна располагаться в середине", pk4, train4->GetPlace().front());

	train = MoveThrow ( L"PK7", L"PK8" );
	CPPUNIT_ASSERT( train == train4 );
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, крайние ПЕ остаются в крайнем участке", pk10, train1->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, крайние ПЕ остаются в крайнем участке", pk9, train2->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, расположены последовательно", pk8, train3->GetPlace().front());
	CPPUNIT_ASSERT_EQUAL_MESSAGE("ПЕ в кармане четыре, расположены последовательно", pk8, train4->GetPlace().front());

	// Выводим все ПЕ из второго кармана

	train = MoveThrow( L"PK12", L"PK13" );
	CPPUNIT_ASSERT( train == train1 );
	train = MoveThrow( L"PK12", L"PK13" );
	CPPUNIT_ASSERT( train == train2 );
	train = MoveThrow( L"PK12", L"PK13" );
	CPPUNIT_ASSERT( train == train3 );
	train = MoveThrow( L"PK12", L"PK13" );
	CPPUNIT_ASSERT( train == train4 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::LongPocketMoving()
{
	TI.DetermineStripBusy( L"LPK2", *tracker );
	auto trains1 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains1.size() == 1 );
	auto inittrain = trains1.front();
	TI.DetermineStripFree( L"LPK2", *tracker ); //уход в "карман"
	TI.DetermineStripBusy( L"LPK14", *tracker ); //выход из "кармана"
	auto trains2 = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains2.size() == 1 );
	auto restrain = trains2.front();
	CPPUNIT_ASSERT( inittrain->GetId() == restrain->GetId() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::JointPicketage()
{
    MoveThrow( L"PK2", L"PK3" );
    MoveThrow( L"PK7", L"PK8" );
    const auto & evlist = tracker->GetTrainEvents();
	auto pentryPtr = find_if( evlist.cbegin(), evlist.cend(), []( TrackerEventPtr tePtr ){
        return tePtr->GetCode() == HCode::POCKET_ENTRY;
	} );
	auto pexitPtr = find_if( evlist.cbegin(), evlist.cend(), []( TrackerEventPtr tePtr ){
		return tePtr->GetCode() == HCode::POCKET_EXIT;
	} );
    CPPUNIT_ASSERT( pentryPtr != evlist.cend() && pexitPtr != evlist.cend() );
    auto pentryCoords = (*pentryPtr)->Coords();
    CPPUNIT_ASSERT( pentryCoords.first.empty() ^ pentryCoords.second.empty() );
    auto pentryPInfo = pentryCoords.first.empty() ? pentryCoords.second : pentryCoords.first;
    rwCoord pentryCoord;
    pentryPInfo.GetCoord( TI.axis_name, pentryCoord );
    CPPUNIT_ASSERT( pentryCoord == rwCoord( 10, 720 ) );

    auto pexitCoords = (*pexitPtr)->Coords();
    CPPUNIT_ASSERT( pexitCoords.first.empty() ^ pexitCoords.second.empty() );
    auto pexitPInfo = pexitCoords.first.empty() ? pexitCoords.second : pexitCoords.first;
	rwCoord pexitCoord;
	pexitPInfo.GetCoord( TI.axis_name, pexitCoord );
    CPPUNIT_ASSERT( pexitCoord == rwCoord( 12, 650 ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
    CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::FalsePocket()
{
	TI.DetermineStripBusy( L"FST1", *tracker );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto inittrain = trains.front();
	SetRandomOddInfo( inittrain );
	TI.DetermineStripFree( L"FST1", *tracker );
	TI.DetermineStripBusy( L"FST2", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 1 );
	auto nowtrain = trains.front();
	CPPUNIT_ASSERT( !nowtrain->IsTrain() && inittrain->GetId() != nowtrain->GetId() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::ForbidPocketJumping()
{
	TI.DetermineStripBusy( L"PK12", *tracker );
	TI.DetermineStripBusy( L"PK13", *tracker );
	TI.DetermineStripFree( L"PK12", *tracker );
	TI.DetermineStripBusy( L"PK15", *tracker );

	auto trains = trainCont->GetUnities();
	unsigned int goingTrainId = 0, stayingTrainId = 0;
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"PK13" ).bdg )
			goingTrainId = train->GetId();
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"PK15" ).bdg );
			stayingTrainId = train->GetId();
		}
	}
	TI.DetermineStripFree( L"PK13", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );
	for ( auto train : trains )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"PK14" ).bdg )
			CPPUNIT_ASSERT( train->GetId() == goingTrainId );
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"PK15" ).bdg );
			CPPUNIT_ASSERT( stayingTrainId = train->GetId() );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::OverflowShort()
{
	TI.DetermineStripBusy( L"PK13", *tracker, false );
	TI.DetermineStripBusy( L"PK16", *tracker, false );
	TI.FlushData( *tracker, false );
	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );

	//заезд в карман первого поезда
	TI.DetermineStripFree( L"PK13", *tracker );
	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 2 );

	//заезд в карман поезда, проиводящего к переполнению кармана
	MoveThrow( L"PK16", L"PK15" );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::OverflowLong()
{
	//заезд в карман трех поездов
	for ( unsigned int k = 0; k < 3; ++k )
		MoveThrow( L"PK1", L"PK2", L"PK3" );

	auto trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.size() == 3 );

	//заезд в карман поезда, проиводящего к переполнению кармана
	MoveThrow( L"PK1", L"PK2", L"PK3" );

	trains = trainCont->GetUnities();
	CPPUNIT_ASSERT( trains.empty() );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::PocketEntryVsDisform()
{
	TI.DetermineHeadClose( L"PKHead1", *tracker );
	TI.DetermineStripBusy( L"LPK2", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto spantrain = tunities.front();
	SetRandomEvenInfo( spantrain );
	auto trainIden = spantrain->GetId();

	//постановка маневровой и поезда вплотную
	TI.DetermineStripBusy( L"STPK1", *tracker );
	SetRandomOddInfo( TI.Get( L"STPK1" ).bdg ); //временно делаем поездом
	TI.DetermineStripBusy( L"STPK2", *tracker, false );
	TI.DetermineStripFree( L"STPK1", *tracker, false );
	TI.FlushData( *tracker, false );
	tunities = trainCont->GetUnities();
	auto shuntIden = 0;
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.back() == TI.Get( L"STPK2" ).bdg )
		{
			DisformTrain( tunity->GetId() );
			shuntIden = tunity->GetId();
		}
	}
	CPPUNIT_ASSERT( shuntIden != 0 && shuntIden != trainIden );

	TI.DetermineStripFree( L"LPK2", *tracker );

	//поезд 
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( tunities.front()->IsTrain() ^ tunities.back()->IsTrain() ); //один поезд, другая маневровая
	for ( auto tunity : tunities )
	{
		const auto & tplace = tunity->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tunity->IsTrain() )
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"LPK8" ).bdg );
			CPPUNIT_ASSERT( tunity->GetId() == trainIden );
		}
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"STPK2" ).bdg );
			CPPUNIT_ASSERT( tunity->GetId() == shuntIden );
		}
	}
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::JumpOutOfPocket()
{
	TI.DetermineStripBusy( L"PK2", *tracker );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainId = tunities.front()->GetId();

	//заезд в карман
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK2", *tracker );
	TI.DetermineStripFree( L"PK3", *tracker );

	//выпрыгивание из кармана через участок
	TI.DetermineStripBusy( L"PK8", *tracker );
	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	CPPUNIT_ASSERT( tunities.front()->GetId() == trainId );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::JumpOutVsExit()
{
	//одновременный въезд двух поездов в соседние карманы
	TI.DetermineStripBusy( L"PK2", *tracker );
	TI.DetermineStripBusy( L"PK7", *tracker );
	TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripBusy( L"PK8", *tracker );
	TI.DetermineStripFree( L"PK2", *tracker );
	TI.DetermineStripFree( L"PK7", *tracker );

	auto tunities2 = trainCont->GetUnities();
	TI.DetermineStripFree( L"PK3", *tracker, false );
	TI.DetermineStripFree( L"PK8", *tracker, false );
	TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	unsigned int trainId1 = 0, trainId2 = 0;
	for ( auto train : tunities )
	{
		const auto & tplace = train->GetPlace();
		CPPUNIT_ASSERT( tplace.size() == 1 );
		if ( tplace.front() == TI.Get( L"PK5" ).bdg )
			trainId1 = train->GetId();
		else
		{
			CPPUNIT_ASSERT( tplace.front() == TI.Get( L"PK10" ).bdg );
			trainId2 = train->GetId();
		}
	}
	CPPUNIT_ASSERT( trainId1 != 0 && trainId2 != 0 && trainId1 != trainId2 );

	//в спорной ситуации между "выпрыгиванием" через участок и выездом на смежный участок предпочтение отдается второму
	TI.DetermineStripBusy( L"PK8", *tracker );
	tunities = trainCont->GetUnities();
	auto pk7It = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"PK8" ).bdg );
	} );
	CPPUNIT_ASSERT( pk7It != tunities.cend() );
	CPPUNIT_ASSERT( ( *pk7It )->GetId() == trainId2 );

	//проверяем симметричный участок
	TI.DetermineStripFree( L"PK8", *tracker ); //загоняем поезд обратно в карман
	TI.DetermineStripBusy( L"PK7", *tracker );
	tunities = trainCont->GetUnities();
	auto pk6It = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return ( tplace.size() == 1 && tplace.front() == TI.Get( L"PK7" ).bdg );
	} );
	CPPUNIT_ASSERT( pk6It != tunities.cend() );
	CPPUNIT_ASSERT( ( *pk6It )->GetId() == trainId1 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerPockets::PocketExitAsSoonAsPossible()
{
	TI.DetermineStripBusy( L"PK13", *tracker );
	TI.DetermineStripBusy( L"PK15", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto initPk13Ptr = trainCont->GetUnity( TI.Get( L"PK13" ).bdg );
	auto initPk15Ptr = trainCont->GetUnity( TI.Get( L"PK15" ).bdg );
	CPPUNIT_ASSERT( initPk13Ptr && initPk15Ptr );
	auto initPk13Iden = initPk13Ptr->GetId();
	auto initPk15Iden = initPk15Ptr->GetId();

	TI.DetermineStripFree( L"PK15", *tracker ); //заезд в карман
	TI.DetermineStripBusy( L"PK12", *tracker ); //поезд должен выйти из кармана

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto pk12Ptr = trainCont->GetUnity( TI.Get( L"PK12" ).bdg );
	CPPUNIT_ASSERT( pk12Ptr && pk12Ptr->GetId() == initPk13Iden );
	auto pk13Ptr = trainCont->GetUnity( TI.Get( L"PK13" ).bdg );
	CPPUNIT_ASSERT( pk13Ptr && pk13Ptr->GetId() == initPk15Iden );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}