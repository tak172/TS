#include "stdafx.h"
#include "TC_TrackerInfoSetting.h"
#include "../Guess/TrainContainer.h"
#include "../Guess/TrainInfoPackage.h"
#include "../Guess/ProcessInfoResult.h"
#include "../Guess/UnityConversion.h"

typedef TrainCharacteristics::Source Source;

using namespace std;

//локальные типы. только для использования внутри модуля трансляции

//факт нахождения на ПО-пути (прибыл или сформировался, но не отправился):
enum struct IntoADWayFact
{
	IntoWay = 0,
	NotIntoWay
};

string ToStr( const IntoADWayFact & intoWayFact )
{
	auto strVal = ( intoWayFact == IntoADWayFact::IntoWay ? "into way" : "not into way" );
	return string( "IntoWayExistance = " ) + strVal;
}

//тип поезда для приложения информации:
enum struct TrainType
{
	NoIdent = 0, //не идентифицированный поезд
	Cargo, //грузовой поезд
	Other //любой другой поезд
};

string ToStr( const TrainType & trainType )
{
	string strVal = "error";
	switch ( trainType )
	{
	case TrainType::Cargo:
		strVal = "cargo";
		break;
	case TrainType::NoIdent:
		strVal = "noident";
		break;
	case TrainType::Other:
		strVal = "other";
		break;
	}
	return string( "Init unity = " ) + strVal;
}

//тип прикладываемого номера:
enum struct NumberType
{
	Cargo = 0, //грузовой
	Other //любой другой
};

string ToStr( const NumberType & trainNumType )
{
	auto strVal = ( trainNumType == NumberType::Cargo ? "cargo" : "other" );
	return string( "New info = " ) + strVal;
}

//место приложения информации:
enum struct ApplyingPlace
{
	ADWay = 0, //ПО-путь
	NoADWay, //не ПО-путь (внутри станции)
	Span //перегонный путь
};

string ToStr( const ApplyingPlace & applPlace )
{
	string strVal = "error";
	switch ( applPlace )
	{
	case ApplyingPlace::ADWay:
		strVal = "AD";
		break;
	case ApplyingPlace::NoADWay:
		strVal = "noAD";
		break;
	case ApplyingPlace::Span:
		strVal = "span";
		break;
	}
	return string( "Applying place = " ) + strVal;
}

string ToStr( const Source & infoSource )
{
	string sourceStr;
	if ( infoSource == Source::Disposition ) 
		sourceStr += "disposition";
	else if ( infoSource == Source::Hem )
		sourceStr += "hem";
	else
		sourceStr += "other";
	return string( "Source = " ) + sourceStr;
}

//совпадение четности
enum struct OddnessMatch
{
	Matched = 0, //совпадают
	NoMatched //не совпадают
};

string ToStr( const OddnessMatch & oddnessMatch )
{
	auto strVal = ( oddnessMatch == OddnessMatch::Matched ? "matched" : "no matched" );
	return string( "Oddness match = " ) + strVal;
}

struct TrainSituation
{
	IntoADWayFact intoWayFact;
	TrainType trainType;
	NumberType numberType;
	ApplyingPlace applyingPlace;
	OddnessMatch oddnessMatch;
	TrainCharacteristics::Source infoSource;
	string ToStr() const 
	{
		return string( "Train situation: " ) + ::ToStr( intoWayFact ) + " | " + ::ToStr( trainType ) + " | " + ::ToStr( numberType ) + " | " + ::ToStr( applyingPlace ) + " | " + 
			::ToStr( oddnessMatch ) + " | " + ::ToStr( infoSource );
	}
};

enum struct NoTrainBehavior
{
	NoAction = 0,
	Compacted, //втягивание на путь
	WaitedOut //продолжительное неподвижное ожидание
};

string ToStr( const NoTrainBehavior & behavior )
{
	string behavStr;
	if ( behavior == NoTrainBehavior::NoAction ) 
		behavStr += "no action";
	else if ( behavior == NoTrainBehavior::Compacted )
		behavStr += "compacted";
	else if ( behavior == NoTrainBehavior::WaitedOut )
		behavStr += "waited out";
	return string( "Behavior = " ) + behavStr;
}

struct NoTrainSituation
{
	IntoADWayFact intoWayFact;
	ApplyingPlace applyingPlace;
	Source infoSource;
	NoTrainBehavior behavior;
	string ToStr() const 
	{
		return string( "NoTrain situation: " ) + ::ToStr( intoWayFact ) + " | " + ::ToStr( applyingPlace ) + " | " + ::ToStr( behavior ) + " | " + ::ToStr( infoSource );
	}
};

enum struct ApplyingResult
{
	Permitted = 0,
	Forbidden,
	ForbiddenThenPermittedAfterArrive,
	ForbiddenThenRecreationAfterArrive,
	NeedTrainCreation,
	HouseHoldPairRecreateOtherBid //для парного номер хозяйственного поезда - пересоздание, для остальных - запрет
};

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerInfoSetting );

void TC_TrackerInfoSetting::setUp()
{
	TI.Reset();
	tracker->Reset();
}

void TC_TrackerInfoSetting::PlaceChangings()
{
	//пусть модуль отслеживания ведет одну ПЕ
	TI.DetermineStripBusy( L"C", *tracker );

	SetInfo( TI.Get( L"C" ).bdg, TrainDescr() );

	changes = tracker->TakeTrainChanges(); //обнуляем изменения
	list <TrainUnityCPtr> trainList = trainCont->GetUnities();
	CPPUNIT_ASSERT( !trainList.empty() );
	TrainUnityCPtr trainPtr = *trainList.begin();
	TrainDescr testTrainDescr = RandomTrainDescr();
	SetInfo( TI.Get( L"C" ).bdg, testTrainDescr, Source::Hem );
	changes = tracker->TakeTrainChanges();
	CPPUNIT_ASSERT( changes.placeViewChanges.size() == 1 );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->appeared );
	CPPUNIT_ASSERT( changes.placeViewChanges.begin()->place == TI.Get( L"C" ).bdg );
	ConstTrainDescrPtr tdescr = changes.placeViewChanges.begin()->tdescrPtr;
	CPPUNIT_ASSERT( tdescr && *tdescr == testTrainDescr );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

template<typename SituationType, typename ArrayType>
void expand( vector<SituationType> & situations, const ArrayType & unityTypes,
						 function<void( typename const ArrayType::value_type & from, typename vector<SituationType>::value_type & to )> grabFunc )
{
	vector<SituationType> situationsInp( situations );
	situations.clear();
	for ( auto utIt = begin( unityTypes ); utIt != end( unityTypes ); ++utIt )
	{
		const auto & unType = *utIt;
		for_each( situationsInp.cbegin(), situationsInp.cend(), [&situations, &unType, &grabFunc]( const SituationType & sit ){
			SituationType newSit( sit );
			grabFunc( unType, newSit );
			situations.push_back( newSit );
		} );
	}
}

void cleanIncompatible( vector<NoTrainSituation> & situations )
{
	situations.erase( remove_if( situations.begin(), situations.end(), []( const NoTrainSituation & sit ){
			return sit.intoWayFact == IntoADWayFact::IntoWay && sit.applyingPlace == ApplyingPlace::Span && sit.infoSource == Source::Hem;
		} ), situations.end() );
}

TrainDescrPtr makeInitInfo( const TrainType & untype )
{
	shared_ptr<TrainDescr> tdescrPtr;
	wstring numstr;
	if ( untype == TrainType::NoIdent )
		numstr = L""; //нет номера
	if ( untype == TrainType::Cargo )
		numstr = L"1596";
	else if ( untype == TrainType::Other )
		numstr = L"6202";
	tdescrPtr.reset( new TrainDescr( numstr ) );
	return tdescrPtr;
}

TrainDescr makeNewInfo( TrainUnityCPtr tunityPtr, NumberType numberType, OddnessMatch oddnessMatch )
{
	TrainDescr newDescr;
	unsigned int oddSeed = ( numberType == NumberType::Cargo ? 9335 : 5335 );
	wstring numstr;
	Oddness curOddness = tunityPtr->GetOddness();
	unsigned int newNumber;
	if ( oddnessMatch == OddnessMatch::Matched && curOddness == Oddness::EVEN ||
		oddnessMatch == OddnessMatch::NoMatched && curOddness == Oddness::ODD )
		newNumber = oddSeed + 1;
	else
		newNumber = oddSeed;
	numstr = to_wstring( newNumber );
	newDescr.SetNumber( numstr );
	return newDescr;
}

wstring getApplyingName( const ApplyingPlace & applPlace )
{
	wstring applName;
	switch (applPlace)
	{
	case ApplyingPlace::ADWay:
		applName = L"C";
		break;
	case ApplyingPlace::NoADWay:
		applName = L"B";
		break;
	case ApplyingPlace::Span:
		applName = L"Q";
		break;
	}

	return applName;
}

ApplyingResult getExpectedResult( const TrainSituation & situation )
{
	ApplyingResult applResult = ApplyingResult::Permitted;
	if ( situation.infoSource == Source::Disposition ) //источник - внешняя система
	{
		if ( situation.trainType == TrainType::NoIdent )
		{
			if ( situation.oddnessMatch == OddnessMatch::Matched )
				applResult = ApplyingResult::Permitted;
			else if ( situation.intoWayFact == IntoADWayFact::IntoWay )
				applResult = ( situation.applyingPlace == ApplyingPlace::ADWay ? ApplyingResult::NeedTrainCreation : ApplyingResult::Forbidden );
			else
				applResult = ( situation.applyingPlace == ApplyingPlace::ADWay ? ApplyingResult::ForbiddenThenRecreationAfterArrive : ApplyingResult::Forbidden );
		}
		else if ( situation.trainType == TrainType::Cargo )
		{
			if ( situation.intoWayFact == IntoADWayFact::IntoWay && situation.numberType == NumberType::Cargo && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::Permitted;
			else if ( situation.intoWayFact == IntoADWayFact::IntoWay && situation.numberType == NumberType::Other && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::NeedTrainCreation;
			else if ( situation.intoWayFact == IntoADWayFact::NotIntoWay && situation.numberType == NumberType::Cargo && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::ForbiddenThenPermittedAfterArrive;
			else if ( situation.intoWayFact == IntoADWayFact::NotIntoWay && situation.numberType == NumberType::Other && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::ForbiddenThenRecreationAfterArrive;
			else
				applResult = ApplyingResult::Forbidden;
		}
		else if ( situation.trainType == TrainType::Other )
		{
			if ( situation.intoWayFact == IntoADWayFact::IntoWay && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::NeedTrainCreation;
			else if ( situation.intoWayFact == IntoADWayFact::NotIntoWay && situation.applyingPlace == ApplyingPlace::ADWay )
				applResult = ApplyingResult::ForbiddenThenRecreationAfterArrive;
			else if ( situation.intoWayFact == IntoADWayFact::NotIntoWay && situation.numberType == NumberType::Other && situation.applyingPlace == ApplyingPlace::Span && 
				situation.oddnessMatch == OddnessMatch::NoMatched )
				applResult = ApplyingResult::HouseHoldPairRecreateOtherBid;
			else
				applResult = ApplyingResult::Forbidden;
		}
	}
	return applResult;
}

ApplyingResult getExpectedResult( const NoTrainSituation & situation )
{
	ApplyingResult applResult = ApplyingResult::Forbidden;
	if ( situation.infoSource == Source::Disposition ) //источник - внешняя система
	{
		if ( situation.applyingPlace == ApplyingPlace::ADWay && situation.intoWayFact == IntoADWayFact::NotIntoWay && situation.behavior == NoTrainBehavior::NoAction ||
			//промах приложения информации по месту (маневровая считается прибывшей, если целиком в пределах ПО-пути)
			situation.intoWayFact == IntoADWayFact::IntoWay && situation.applyingPlace != ApplyingPlace::ADWay )
			applResult = ApplyingResult::Forbidden;
		else
			applResult = ApplyingResult::Permitted;
	}
	return applResult;
}

bool TC_TrackerInfoSetting::CheckPermitted( unsigned int prevIden, const TrainDescr & newDescr )
{
	bool permitted = false;
	auto tunities = trainCont->GetUnities();
	if ( tunities.size() == 1 )
	{
		auto tunity = tunities.back();
		if ( tunity->GetId() == prevIden )
		{
			auto tdescrPtr = tunity->GetDescrPtr();
			permitted = ( tdescrPtr && *tdescrPtr == newDescr );
		}
	}
	return permitted;
}

bool TC_TrackerInfoSetting::CheckForbidden( unsigned int prevIden, ConstTrainDescrPtr prevDescrPtr, const TrainDescr & newDescr )
{
	bool forbidden = false;
	auto tunities = trainCont->GetUnities();
	if ( tunities.size() == 1 )
	{
		auto tunity = tunities.back();
		if ( tunity->GetId() == prevIden )
		{
			auto tdescrPtr = tunity->GetDescrPtr();
			if ( !prevDescrPtr )
				forbidden = !tdescrPtr;
			else
				forbidden = ( *prevDescrPtr == *tdescrPtr && *tdescrPtr != newDescr );
		}
	}
	return forbidden;
}

bool TC_TrackerInfoSetting::CheckRecreated( unsigned int prevIden, ConstTrainDescrPtr prevDescrPtr, const TrainDescr & newDescr )
{
	bool recreated = false;
	auto tunities = trainCont->GetUnities();
	if ( tunities.size() == 1 )
	{
		auto tunity = tunities.back();
		if ( !prevDescrPtr || tunity->GetId() != prevIden )
		{
			auto tdescrPtr = tunity->GetDescrPtr();
			recreated = ( tdescrPtr && *tdescrPtr == newDescr );
		}
	}
	return recreated;
}

bool TC_TrackerInfoSetting::CheckForbiddenThenPermittedAfterArrive( unsigned int prevIden, ConstTrainDescrPtr prevDescrPtr, const TrainDescr & newDescr )
{
	bool forbiddenAtFirst = CheckForbidden( prevIden, prevDescrPtr, newDescr );
	bool arrivedCorrectly = false;
	if ( !prevDescrPtr )
	{
		//для маневровой - прибытие трактуется как полное нахождение на ПО-пути
		TI.DetermineStripFree( L"W", *tracker );
		TI.DetermineStripFree( L"Q", *tracker );
		TI.DetermineStripFree( L"A", *tracker );
		TI.DetermineStripFree( L"B", *tracker );
	}
	else
	{
		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
		TI.IncreaseTime( ensureArriveMs + 1000 );
		const auto & tevents = tracker->GetTrainEvents( prevIden );
		//сначала прибытие, потом смена информации:
		arrivedCorrectly = ( tevents.size() > 1 && ( *++tevents.crbegin() )->GetCode() == HCode::ARRIVAL && tevents.back()->GetCode() == HCode::INFO_CHANGING );
	}
	bool processedSuccessfully = ( !prevDescrPtr || arrivedCorrectly );
	return forbiddenAtFirst && CheckPermitted( prevIden, newDescr ) && processedSuccessfully;
}

bool TC_TrackerInfoSetting::CheckForbiddenThenRecreationAfterArrive( unsigned int prevIden, ConstTrainDescrPtr prevDescrPtr, const TrainDescr & newDescr )
{
	bool forbiddenAtFirst = CheckForbidden( prevIden, prevDescrPtr, newDescr );
	bool arrivedCorrectly = false;
	if ( !prevDescrPtr )
	{
		//для маневровой - прибытие трактуется как полное нахождение на ПО-пути
		TI.DetermineStripFree( L"W", *tracker );
		TI.DetermineStripFree( L"Q", *tracker );
		TI.DetermineStripFree( L"A", *tracker );
		TI.DetermineStripFree( L"B", *tracker );
	}
	else
	{
		TimeConstraints & timConstr = tracker->GetTimeConstraints();
		auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
		TI.IncreaseTime( ensureArriveMs + 1000 );
		const auto & tevents = tracker->GetTrainEvents( prevIden );
		//сначала прибытие, потом расформирование:
		arrivedCorrectly = ( tevents.size() > 1 && ( *++tevents.crbegin() )->GetCode() == HCode::ARRIVAL && tevents.back()->GetCode() == HCode::DISFORM );
	}
	bool processedSuccessfully = ( !prevDescrPtr || arrivedCorrectly );
	return forbiddenAtFirst && CheckRecreated( prevIden, prevDescrPtr, newDescr ) && processedSuccessfully;
}

bool TC_TrackerInfoSetting::CheckHouseHold( unsigned int prevIden, ConstTrainDescrPtr prevDescrPtr, const TrainDescr & newDescr )
{
	bool houseHoldProcessed = false;
	if ( prevDescrPtr )
	{
		unsigned int number = prevDescrPtr->GetNumber().getNumber();
		const auto & tcharatcts = TrainCharacteristics::instanceCRef();
		const auto & purpFeature = tcharatcts.GetPurposeFeature( number );
		if ( tcharatcts.IsHouseHoldTrain( purpFeature ) && prevDescrPtr->GetNumber().isPairedWith( newDescr.GetNumber() ) )
			//парный хозяйственный номер
			houseHoldProcessed = CheckRecreated( prevIden, prevDescrPtr, newDescr );
		else
			//любой другой номер
			houseHoldProcessed = CheckForbidden( prevIden, prevDescrPtr, newDescr );
	}
	return houseHoldProcessed;
}

void TC_TrackerInfoSetting::NoTrainInfoSetting()
{
	//подготовка входных данных
	array<IntoADWayFact, 2> wayFacts = { IntoADWayFact::IntoWay, IntoADWayFact::NotIntoWay };
	array<ApplyingPlace, 3> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay, ApplyingPlace::Span };
	array<NoTrainBehavior, 3> behaviorVariants = { NoTrainBehavior::NoAction, NoTrainBehavior::Compacted, NoTrainBehavior::WaitedOut };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	NoTrainSituation seedVal;
	vector<NoTrainSituation> situations( 1, seedVal );

	expand( situations, wayFacts, []( const IntoADWayFact & iwFact, NoTrainSituation & to ){ to.intoWayFact = iwFact; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, NoTrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, behaviorVariants, []( const NoTrainBehavior & behavior, NoTrainSituation & to ){ to.behavior = behavior; } );
	expand( situations, sources, []( const Source & source, NoTrainSituation & to ){ to.infoSource = source; } );
	cleanIncompatible( situations );

	for ( const auto & sit : situations )
	{
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		if ( sit.intoWayFact == IntoADWayFact::IntoWay ) //ПЕ изначально целиком на ПО-пути
		{
			TI.DetermineStripFree( L"Q", *tracker );
			TI.DetermineStripFree( L"A", *tracker );
			TI.DetermineStripFree( L"B", *tracker );
		}
		const auto & applyingBdg = TI.Get( getApplyingName( sit.applyingPlace ) ).bdg;
		const auto & newTrainDescr = TrainDescr( L"5584" ); //любой четный дескриптор
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, sit.infoSource ); //центральный вызов

		//предварительные проверки на большинство случаев
		if ( sit.intoWayFact == IntoADWayFact::IntoWay && sit.applyingPlace == ApplyingPlace::ADWay && sit.infoSource != Source::Hem )
			CPPUNIT_ASSERT( tunityPtr->IsTrain() ); //точно должно применяться сразу
		else if ( sit.intoWayFact == IntoADWayFact::NotIntoWay && sit.applyingPlace == ApplyingPlace::ADWay )
			CPPUNIT_ASSERT( !tunityPtr->IsTrain() ); //точно НЕ должно применяться сразу

		if ( sit.behavior == NoTrainBehavior::Compacted ) //втягивание хвоста (если не втянут)
		{
			//если ПЕ уже целиком в границах ПО-пути, данные действия не помешают
			TI.DetermineStripFree( L"Q", *tracker );
			TI.DetermineStripFree( L"A", *tracker );
			TI.DetermineStripFree( L"B", *tracker );
		}
		else if ( sit.behavior == NoTrainBehavior::WaitedOut ) //ожидание интервала
		{
			TimeConstraints & timConstr = tracker->GetTimeConstraints();
			auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
			TI.IncreaseTime( ensureArriveMs + 1000 );
		}

		auto expectedResult = getExpectedResult( sit );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( sit.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( sit.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::ArrivedNoDepartured()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 3> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay, ApplyingPlace::Span };
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::IntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & trainType, TrainSituation & to ){ to.trainType = trainType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );

	//собственно тестирование всех вариаций входных ситуаций
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
	for ( const auto & sit : situations )
	{
		TI.DetermineHeadClose( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"W", *tracker ); //это необходимо для генерации StationEntry
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		if ( auto initDescrPtr = makeInitInfo( sit.trainType ) )
		{
			SetInfo( TI.Get( L"C" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}

		TI.IncreaseTime( ensureArriveMs + 1000 );

		//убедимся, что событие прибытие сгенерировано
		const auto & tevents = tracker->GetTrainEvents( tunityPtr->GetId() );
		CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::ARRIVAL; } ) );

		const auto & applyingBdg = TI.Get( getApplyingName( sit.applyingPlace ) ).bdg;
		const auto & newTrainDescr = makeNewInfo( tunityPtr, sit.numberType, sit.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, sit.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( sit );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( sit.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( sit.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::NeedTrainCreation:
			CPPUNIT_ASSERT_MESSAGE( sit.ToStr(), CheckRecreated( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::ArrivedThenDepartured()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 3> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay, ApplyingPlace::Span };
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::NotIntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & trainType, TrainSituation & to ){ to.trainType = trainType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );

	//собственно тестирование всех вариаций входных ситуаций
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
	for ( const auto & situation : situations )
	{
		TI.DetermineHeadClose( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"W", *tracker ); //это необходимо для генерации StationEntry
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		if ( auto initDescrPtr = makeInitInfo( situation.trainType ) )
		{
			SetInfo( TI.Get( L"C" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}
		TI.IncreaseTime( ensureArriveMs + 1000 );

		//отправление
		TI.DetermineRouteSet( L"Head102->G", *tracker );
		TI.DetermineStripBusy( L"F", *tracker );

		//убедимся, что сгенерированы события прибытия и отправления
		const auto & tevents = tracker->GetTrainEvents( tunityPtr->GetId() );
		CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::ARRIVAL; } ) );
		CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::DEPARTURE; } ) );

		const auto & applyingBdg = TI.Get( getApplyingName( situation.applyingPlace ) ).bdg;
		const auto & newTrainDescr = makeNewInfo( tunityPtr, situation.numberType, situation.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, situation.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( situation );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenPermittedAfterArrive:
			//считаем, что поезд сюда уже не прибудет
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenRecreationAfterArrive:
			//считаем, что поезд сюда уже не прибудет
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::HouseHoldPairRecreateOtherBid:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckHouseHold( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::InfoSettingBeforeArriving()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 3> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay, ApplyingPlace::Span };
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::NotIntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & trainType, TrainSituation & to ){ to.trainType = trainType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );

	//собственно тестирование всех вариаций входных ситуаций
	for ( const auto & situation : situations )
	{
		TI.DetermineHeadClose( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"W", *tracker ); //это необходимо для генерации StationEntry
		TI.DetermineStripBusy( L"Q", *tracker );
		TI.DetermineStripBusy( L"A", *tracker );
		TI.DetermineStripBusy( L"B", *tracker );
		TI.DetermineStripBusy( L"C", *tracker );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		if ( auto initDescrPtr = makeInitInfo( situation.trainType ) )
		{
			SetInfo( TI.Get( L"C" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}

		//убедимся, что события прибытие не создавалось (для поезда)	
		const auto & tevents = tracker->GetTrainEvents();
		CPPUNIT_ASSERT( none_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::ARRIVAL; } ) );
		const auto & applyingBdg = TI.Get( getApplyingName( situation.applyingPlace ) ).bdg;
		const auto & newTrainDescr = makeNewInfo( tunityPtr, situation.numberType, situation.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, situation.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( situation );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenPermittedAfterArrive:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbiddenThenPermittedAfterArrive( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenRecreationAfterArrive:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbiddenThenRecreationAfterArrive( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::HouseHoldPairRecreateOtherBid:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckHouseHold( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::FormedNoDepartured()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 3> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay, ApplyingPlace::Span };
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::IntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & unType, TrainSituation & to ){ to.trainType = unType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );
	//собственно тестирование всех вариаций входных ситуаций
	for ( const auto & situation : situations )
	{
		TI.DetermineHeadClose( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"Q", *tracker, false );
		TI.DetermineStripBusy( L"A", *tracker, false );
		TI.DetermineStripBusy( L"B", *tracker, false );
		TI.DetermineStripBusy( L"C", *tracker, false );
		TI.FlushData( *tracker, false );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		if ( auto initDescrPtr = makeInitInfo( situation.trainType ) )
		{
			SetInfo( TI.Get( L"C" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}

		//убедимся, что событие формирования сгенерировано
		const auto & tevents = tracker->GetTrainEvents();
		CPPUNIT_ASSERT( tevents.size() == 1 );
		auto teventPtr = tevents.back(); 
		CPPUNIT_ASSERT( teventPtr->GetCode() == HCode::FORM );

		const auto & applyingBdg = TI.Get( getApplyingName( situation.applyingPlace ) ).bdg;
		const auto & newTrainDescr = makeNewInfo( tunityPtr, situation.numberType, situation.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, situation.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( situation );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::NeedTrainCreation:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckRecreated( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::FormedThenDepartured()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 2> applPlace = { ApplyingPlace::ADWay, ApplyingPlace::NoADWay/*, ApplyingPlace::Span*/ }; //перегон исключаем, т.к. в этом случае поезд будет проследовать ПО-путь
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::NotIntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & trainType, TrainSituation & to ){ to.trainType = trainType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );

	//собственно тестирование всех вариаций входных ситуаций
	for ( const auto & situation : situations )
	{
		TI.DetermineHeadClose( L"Head102", *tracker );
		TI.DetermineSwitchMinus( L"11", *tracker );
		TI.DetermineStripBusy( L"A", *tracker, false );
		TI.DetermineStripBusy( L"B", *tracker, false );
		TI.DetermineStripBusy( L"C", *tracker, false );
		TI.FlushData( *tracker, false );

		auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
		
		if ( auto initDescrPtr = makeInitInfo( situation.trainType ) )
		{
			SetInfo( TI.Get( L"C" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}

		//отправление
		TI.DetermineRouteSet( L"Head102->G", *tracker );
		TI.DetermineStripBusy( L"F", *tracker );

		//убедимся, что событие формирования сгенерировано (для поезда)
		const auto & tevents = tracker->GetTrainEvents( tunityPtr->GetId() );
		CPPUNIT_ASSERT( tevents.size() == 2 );
		CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::FORM; } ) );
		CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tevPtr ){ return tevPtr->GetCode() == HCode::DEPARTURE; } ) );

		const auto & applyingBdg = TI.Get( getApplyingName( situation.applyingPlace ) ).bdg;
		const auto & newTrainDescr = makeNewInfo( tunityPtr, situation.numberType, situation.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( applyingBdg, newTrainDescr, situation.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( situation );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::NeedTrainCreation:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckRecreated( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenPermittedAfterArrive:
			//считаем, что поезд сюда уже не прибудет
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::ForbiddenThenRecreationAfterArrive:
			//считаем, что поезд сюда уже не прибудет
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::HouseHoldPairRecreateOtherBid:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckHouseHold( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::FullyInSpan()
{
	//подготовка входных данных
	array<TrainType, 3> unityTypes = { /*TrainType::NoTrain,*/ TrainType::NoIdent, TrainType::Cargo, TrainType::Other };
	array<NumberType, 2> trainNums = { NumberType::Cargo, NumberType::Other };
	array<ApplyingPlace, 1> applPlace = { ApplyingPlace::Span };
	array<OddnessMatch, 2> oddnessMatch = { OddnessMatch::Matched, OddnessMatch::NoMatched };
	array<Source, 2> sources = { Source::Disposition, Source::Hem };
	TrainSituation seedVal;
	seedVal.intoWayFact = IntoADWayFact::NotIntoWay;
	vector<TrainSituation> situations( 1, seedVal );
	expand( situations, unityTypes, []( const TrainType & trainType, TrainSituation & to ){ to.trainType = trainType; } );
	expand( situations, trainNums, []( const NumberType & numType, TrainSituation & to ){ to.numberType = numType; } );
	expand( situations, applPlace, []( const ApplyingPlace & applPlace, TrainSituation & to ){ to.applyingPlace = applPlace; } );
	expand( situations, oddnessMatch, []( const OddnessMatch & oddnessMatch, TrainSituation & to ){ to.oddnessMatch = oddnessMatch; } );
	expand( situations, sources, []( const Source & source, TrainSituation & to ){ to.infoSource = source; } );

	//собственно тестирование всех вариаций входных ситуаций
	for ( const auto & situation : situations )
	{
		TI.DetermineStripBusy( L"Q", *tracker );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
		if ( auto initDescrPtr = makeInitInfo( situation.trainType ) )
		{
			SetInfo( TI.Get( L"Q" ).bdg, *initDescrPtr );
			if ( initDescrPtr->GetNumber().getNumberString().empty() )
			{
				auto & nconstTCont = tracker->GetTrainContainer();
				nconstTCont.SetTrainOddness( tunityPtr->GetId(), Oddness::EVEN );
			}
		}
		const auto & newTrainDescr = makeNewInfo( tunityPtr, situation.numberType, situation.oddnessMatch );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		SetInfo( TI.Get( L"Q" ).bdg, newTrainDescr, situation.infoSource ); //центральный вызов
		auto expectedResult = getExpectedResult( situation );
		switch( expectedResult )
		{
		case ApplyingResult::Permitted:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckPermitted( prevIden, newTrainDescr ) );
			break;
		case ApplyingResult::Forbidden:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckForbidden( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::NeedTrainCreation:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckRecreated( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		case ApplyingResult::HouseHoldPairRecreateOtherBid:
			CPPUNIT_ASSERT_MESSAGE( situation.ToStr(), CheckHouseHold( prevIden, prevDescrPtr, newTrainDescr ) );
			break;
		default:
			CPPUNIT_ASSERT_MESSAGE( "unexpected result", false );
			break;
		}
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::HouseHold()
{
	for ( auto i = 0; i < 4; ++i )
	{
		bool pairedNumber = ( i == 0 || i == 1 );
		bool houseHold = ( i == 1 || i == 2 );
		TI.DetermineStripBusy( L"X", *tracker );
		TrainDescr initDescr( houseHold ? L"8388" : L"5540" );
		SetInfo( TI.Get( L"X" ).bdg, initDescr, Source::User );
		auto tunityPtr = trainCont->GetUnity( TI.Get( L"X" ).bdg );
		auto prevDescrPtr = tunityPtr->GetDescrPtr();
		auto prevIden = tunityPtr->GetId();
		TrainDescr newDescr;
		if ( pairedNumber )
			newDescr = TrainDescr( initDescr.GetNumber().getPaired().getString() ); //парный номер
		else
			newDescr = TrainDescr( to_wstring( initDescr.GetNumber().getNumber() - 9 ) ); //любой непарный номер другой четности
		SetInfo( TI.Get( L"X" ).bdg, newDescr, Source::Disposition ); //центральный вызов
		if ( houseHold && pairedNumber )
			CPPUNIT_ASSERT( CheckRecreated( prevIden, prevDescrPtr, newDescr ) );
		else
			CPPUNIT_ASSERT( CheckForbidden( prevIden, prevDescrPtr, newDescr ) );
		CPPUNIT_ASSERT( tracker->CheckIntegrity() );
		tracker->Reset();
		TI.Reset();
	}
}

void TC_TrackerInfoSetting::ArrivalFactExpiration()
{
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	SetInfo( TI.Get( L"Q" ).bdg, TrainDescr( L"1468" ), Source::User );
	TI.DetermineHeadOpen( L"Head102", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );

	//выезд поезда с ПО-пути
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripFree( L"C", *tracker );

	//исчерпание времени ожидания прибытия
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto arrivalPeriod = timConstr.maxArrivalIntervalMs();
	TI.IncreaseTime( arrivalPeriod + 1000 );

	//возвращение ПЕ на ПО-путь
	TI.DetermineStripBusy( L"C", *tracker );
	TI.IncreaseTime( arrivalPeriod + 1000 );
	
	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 && !tunities.back()->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::ArrivalExpirationWhileLPFDelay()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto initLPFThreshold = timConstr.LPFThresholdSec;
	timConstr.LPFThresholdSec = 2;
	auto maxArrIntvalMs = timConstr.maxArrivalIntervalMs();

	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.IncreaseTime( timConstr.LPFThresholdSec * 1000 );
	SetInfo( TI.Get( L"Q" ).bdg, TrainDescr( L"1468" ), Source::User );
	TI.DetermineHeadClose( L"Head102", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.IncreaseTime( timConstr.LPFThresholdSec * 1000 );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.IncreaseTime( timConstr.LPFThresholdSec * 1000 );

	//занятость ПО-пути и приход инфопакета в один момент времени
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );

	TI.IncreaseTime( timConstr.LPFThresholdSec * 1000 ); //сигнал занятости ПО-пути дожидается обработки после сглажиивающей задержки

	TI.IncreaseTime( maxArrIntvalMs + 1000 ); //события прибытия пока нет (из-за сглаживания) и инфопакет пока не удаляется (нужно учесть сглаживающую задержку)

	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tdescrPtr = tunities.back()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"1920" ) );

	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr teventPtr ){ return teventPtr->GetCode() == HCode::ARRIVAL; }));
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	timConstr.LPFThresholdSec = initLPFThreshold;
}

void TC_TrackerInfoSetting::NotrainToTrain()
{
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );
	const auto & tunities = trainCont->GetUnities();
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	const auto & formEvent = tevents.back();
	CPPUNIT_ASSERT( formEvent->GetCode() == HCode::FORM );
	const auto & spotDetPtr = formEvent->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr );
	auto tdescrPtr = spotDetPtr->tdptr;
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"1920" ) );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::DuplicateInfoMustBeErased()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto currentDelay = chrono::seconds( 5 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( currentDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );
	//дублирующий дескриптор
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );

	//выезд старого поезда..
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	TI.DetermineStripFree( L"F", *tracker );

	//.. и появление новой ПЕ
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );

	TI.IncreaseTime( static_cast<unsigned int>( currentDelay.count() ) * 1000 );

	const auto & tunities = trainCont->GetUnities();

	//дублирующий дескриптор на новую ПЕ не попадает
	CPPUNIT_ASSERT( tunities.size() == 2 );
	auto notrainIt = find_if( tunities.cbegin(), tunities.cend(), [this]( TrainUnityCPtr tptr ){
		const auto & tplace = tptr->GetPlace();
		return tplace.size() == 3 &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"C" ).bdg ) != tplace.cend() &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"B" ).bdg ) != tplace.cend() &&
			find( tplace.cbegin(), tplace.cend(), TI.Get( L"A" ).bdg ) != tplace.cend() &&
			!tptr->IsTrain();
	} );
	CPPUNIT_ASSERT( notrainIt != tunities.cend() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
	timConstr.setDislocationDelays( DislocationDelays( initDelay, DislocationDelays::Detailization() ) );
}

void TC_TrackerInfoSetting::DuplicateInfoForDifferentUnities()
{
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"M", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1366" ) );
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripFree( L"C", *tracker );
	auto tunities = trainCont->GetUnities();

	SetInfo( TI.Get( L"M" ).bdg, TrainDescr( L"1366", L"index" ), Source::Disposition );
	tunities = trainCont->GetUnities();
	const auto & tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 3 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM );
	auto eventDescrPtr = eventsVec[0]->GetDetails() ? eventsVec[0]->GetDetails()->tdptr : nullptr;
	CPPUNIT_ASSERT( eventDescrPtr && *eventDescrPtr == TrainDescr( L"1366" ) );

	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::INFO_CHANGING );

	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::FORM );
	eventDescrPtr = eventsVec[2]->GetDetails() ? eventsVec[2]->GetDetails()->tdptr : nullptr;
	CPPUNIT_ASSERT( eventDescrPtr && *eventDescrPtr == TrainDescr( L"1366", L"index" ) );

	CPPUNIT_ASSERT( eventsVec[0]->GetId() != eventsVec[2]->GetId() );
	CPPUNIT_ASSERT( eventsVec[0]->GetId() == eventsVec[1]->GetId() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::DuplicateInfoForDifferentUnities2()
{
	TI.DetermineStripBusy( L"M", *tracker );
	auto mtrainPtr = trainCont->GetUnity( TI.Get( L"M" ).bdg );
	CPPUNIT_ASSERT( mtrainPtr );
	auto mtrainId = mtrainPtr->GetId();
	TI.DetermineStripBusy( L"Q", *tracker );
	auto qtrainPtr = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( mtrainPtr );
	auto qtrainId = qtrainPtr->GetId();
	SetInfo( TI.Get( L"M" ).bdg, TrainDescr( L"8366" ) );
	SetInfo( TI.Get( L"Q" ).bdg, TrainDescr( L"8365" ) );
	TI.DetermineStripBusy( L"A", *tracker );
	auto atrainPtr = trainCont->GetUnity( TI.Get( L"A" ).bdg );
	auto atrainId = atrainPtr->GetId();

	const auto & tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( eventsVec.size() == 5 );
	CPPUNIT_ASSERT( eventsVec[0]->GetCode() == HCode::FORM && eventsVec[0]->GetId() == mtrainId );
	CPPUNIT_ASSERT( eventsVec[1]->GetCode() == HCode::FORM && eventsVec[1]->GetId() == qtrainId );
	CPPUNIT_ASSERT( eventsVec[2]->GetCode() == HCode::DEATH && eventsVec[2]->GetId() == qtrainId );
	CPPUNIT_ASSERT( eventsVec[3]->GetCode() == HCode::INFO_CHANGING && eventsVec[3]->GetId() == mtrainId );
	CPPUNIT_ASSERT( eventsVec[4]->GetCode() == HCode::FORM && eventsVec[4]->GetId() == atrainId );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::AddingIncompleteInfo()
{
	TI.DetermineHeadClose( L"Head101", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );
	SetInfo( TI.Get( L"F" ).bdg, TrainDescr( L"8501", L"111-222-555" ) );
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"V8501" ), Source::Disposition );
	TI.DetermineStripFree( L"F", *tracker );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 && tunities.back()->IsTrain() );
	auto tdescrPtr = tunities.back()->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	auto numberStr = tdescrPtr->GetNumber().getString();
	auto indexStr = tdescrPtr->GetIndex().str();
	CPPUNIT_ASSERT( numberStr == L"V8501" && indexStr == L"0111-222-0555" );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::UndefOddnessAlsoApplyable()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto currentDelay = chrono::seconds( 5 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( currentDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	list<wstring> tplace;
	tplace.push_back( L"C" );
	tplace.push_back( L"F" );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), tplace ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto trainId = trainPtr->GetId();

	TrainDescr initDescr( L"1366", L"index" );
	SetInfo( TI.Get( L"C" ).bdg, initDescr, Source::Disposition );

	auto trainPtr2 = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr2 );
	CPPUNIT_ASSERT( trainPtr2->GetId() == trainId ); //нет пересоздания
	auto tdescrPtr = trainPtr2->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr );
	CPPUNIT_ASSERT( *tdescrPtr == initDescr ); //информация применена без ожидания прибытия

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::NearNoTrainInfoSettingMakesMoving()
{
	TI.DetermineSwitchPlus( L"16", *tracker );
	TI.DetermineRouteSet( L"Head106->P", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6014" ) ), list<wstring>( 1, L"M" ) ) );

	list<wstring> unityPlace;
	unityPlace.push_back( L"O" );
	unityPlace.push_back( L"P" );
	unityPlace.push_back( L"R" );
	CPPUNIT_ASSERT( LocateUnityByForce( nullptr, unityPlace ) );
	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	//вместо пересоздания поезда с существующим номером должно быть реализовано смещение существующего (т.к. он находится рядом с маневровой)
	SetInfo( TI.Get( L"R" ).bdg, TrainDescr( L"V6014", L"index" ), Source::Disposition );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	auto tdescrPtr = curtrain->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"V6014", L"index" ) );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 4 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::FormAfterDelayedForm()
{
	TI.DetermineStripBusy( L"F", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"5955", L"index" ), Source::Disposition );

	const TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto ensureArriveMs = timConstr.maxArrivalIntervalMs();
	TI.IncreaseTime( ensureArriveMs * 1000 ); //инфопакет назначается после задержки

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId1 = trainPtr->GetId();
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"5955", L"index" ) );
	auto tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	auto tevent = tevents.back();
	CPPUNIT_ASSERT( tevent->GetCode() == HCode::FORM && tevent->GetBadge() == TI.Get( L"C" ).bdg && tevent->GetId() == trainId1 );

	TI.DetermineStripFree( L"F", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"2732", L"index" ), Source::Disposition );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto trainId2 = trainPtr->GetId();
	tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2732", L"index" ) );
	tevents = tracker->GetTrainEvents();

	CPPUNIT_ASSERT( tevents.size() == 3 );
	vector<TrackerEventPtr> teventsVec( tevents.cbegin(), tevents.cend() );

	CPPUNIT_ASSERT( teventsVec[0]->GetCode() == HCode::FORM && teventsVec[0]->GetBadge() == TI.Get( L"C" ).bdg && teventsVec[0]->GetId() == trainId1 );
	CPPUNIT_ASSERT( teventsVec[1]->GetCode() == HCode::DISFORM && teventsVec[1]->GetBadge() == TI.Get( L"C" ).bdg && teventsVec[1]->GetId() == trainId1 );
	CPPUNIT_ASSERT( teventsVec[2]->GetCode() == HCode::FORM && teventsVec[2]->GetBadge() == TI.Get( L"C" ).bdg && teventsVec[2]->GetId() == trainId2 );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::SetInfoOnTechnodeWithOddnessIgnore()
{
	TI.DetermineStripBusy( L"AR5", *tracker );
	auto tunityPtr = SetInfo( TI.Get( L"AR5" ).bdg, TrainDescr( L"" ), Source::Disposition );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetOddness() == Oddness::UNDEF );
	auto trainId = tunityPtr->GetId();
	auto & mutTCont = tracker->GetTrainContainer();
	mutTCont.SetTrainOddness( trainId, Oddness::ODD );

	SetInfo( TI.Get( L"AR5" ).bdg, TrainDescr( L"1722" ), Source::Disposition );

	auto nowTrainPtr = mutTCont.GetUnity( trainId );
	CPPUNIT_ASSERT( nowTrainPtr && nowTrainPtr->GetOddness() == Oddness::EVEN );
	auto tdescrPtr = nowTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"1722" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::SetInfoOnTechnodeWOOddnessIgnore()
{
	TI.DetermineStripBusy( L"AV5", *tracker );
	auto tunityPtr = SetInfo( TI.Get( L"AV5" ).bdg, TrainDescr( L"" ), Source::Disposition );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetOddness() == Oddness::UNDEF );
	auto trainId = tunityPtr->GetId();
	auto & mutTCont = tracker->GetTrainContainer();
	mutTCont.SetTrainOddness( trainId, Oddness::ODD );

	SetInfo( TI.Get( L"AV5" ).bdg, TrainDescr( L"1722" ), Source::Disposition );

	auto nowTrainPtr = mutTCont.GetUnity( trainId );
	CPPUNIT_ASSERT( nowTrainPtr && nowTrainPtr->GetOddness() == Oddness::ODD );
	auto tdescrPtr = nowTrainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::TransitionShouldCleanDelayedInfo()
{
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );

	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"Q" ) );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1721" ), Source::Disposition ); //номер ожидает прибытия

	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initIden = initTrainPtr->GetId();

	//поезд проследует через ПО-путь:
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	TI.FlushData( *tracker, false );
	TI.DetermineStripFree( L"C", *tracker );

	auto forwtrainPtr = trainCont->GetUnity( TI.Get( L"F" ).bdg );
	CPPUNIT_ASSERT( forwtrainPtr && forwtrainPtr->IsTrain() && forwtrainPtr->GetId() == initIden );
	CPPUNIT_ASSERT( *forwtrainPtr->GetDescrPtr() == TrainDescr() ); //сохраненный инфопакет номер не считался

	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"F", *tracker );
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );
	TI.DetermineStripFree( L"I", *tracker );

	const auto & tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 7 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::TRANSITION );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::STATION_EXIT );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::SPAN_MOVE );
	CPPUNIT_ASSERT( evvec[6]->GetCode() == HCode::DEATH );

	//подъезжает следующий поезд
	LocateUnityByForce( TrainDescrPtr( new TrainDescr() ), list<wstring>( 1, L"B" ) );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineRouteUnset( L"Head102->G", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	const auto & tevents2 = tracker->GetTrainEvents();
	auto backtrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( backtrainPtr && backtrainPtr->IsTrain() && backtrainPtr->GetId() != initIden );
	CPPUNIT_ASSERT( *backtrainPtr->GetDescrPtr() == TrainDescr() ); //инфопакет должен быть удален предыдущим проследованием

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedExtDisformSuccess()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"C" ) );
	auto tevents = tracker->GetTrainEvents();
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	DisformTrain( trainPtr->GetId(), TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition );
	trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && !trainPtr->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedExtDisformReject()
{
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"C" ) );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	DisformTrain( trainPtr->GetId(), TI.Get( L"C" ).bdg, TrainDescr( L"2517" ), Source::Disposition );
	trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr && trainPtr->IsTrain() );
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"2516" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedExtDisformThruMoving()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"Q" ) );
	TI.DetermineSwitchMinus( L"11", *tracker );
	TI.DetermineSwitchPlus( L"13", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	auto inputMom = TI.DetermineStripBusy( L"C", *tracker );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	auto initId = trainPtr->GetId();
	DisformTrain( trainPtr->GetId(), TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"B", *tracker );
	TI.DetermineStripBusy( L"F", *tracker, false );
	TI.DetermineStripBusy( L"G", *tracker, false );
	auto outMom = TI.FlushData( *tracker, false );

	auto tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto newTrainPtr = tunities.back();
	auto newId = newTrainPtr->GetId();

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );

	auto cdetPtr = TI.TField()->GetDetails<BadgeE, SpotDetails>( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( cdetPtr );

	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"Q" ).bdg && evvec[0]->GetId() == initId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY && evvec[1]->GetBadge() == TI.Get( L"A" ).bdg && evvec[1]->GetId() == initId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::ARRIVAL && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg && evvec[2]->GetTime() == inputMom && 
		evvec[2]->GetId() == initId && evvec[2]->GetDetails() && evvec[2]->GetDetails()->parkway == cdetPtr->parkway );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DISFORM && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg && evvec[3]->GetTime() == inputMom && 
		evvec[3]->GetId() == initId && evvec[3]->GetDetails() && evvec[2]->GetDetails()->parkway == cdetPtr->parkway );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::FORM && evvec[4]->GetBadge() == TI.Get( L"C" ).bdg && evvec[4]->GetTime() == outMom && 
		evvec[4]->GetId() == newId && evvec[4]->GetDetails() && evvec[4]->GetDetails()->parkway == cdetPtr->parkway );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::DEPARTURE && evvec[5]->GetBadge() == TI.Get( L"C" ).bdg && evvec[5]->GetTime() == outMom && 
		evvec[5]->GetId() == newId && evvec[5]->GetDetails() && evvec[5]->GetDetails()->parkway == cdetPtr->parkway );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedExtDisformThruMoving2()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"O3" ) );
	auto trains = trainCont->GetUnities();
	auto trainPtr = trains.back();
	auto initId = trainPtr->GetId();
	TI.DetermineRouteSet( L"Head300->S3", *tracker );
	TI.DetermineRouteSet( L"Head302->T3", *tracker );

	DisformTrain( initId, TI.Get( L"P3" ).bdg, TrainDescr( L"2516" ), Source::Disposition );

	TI.DetermineStripBusy( L"P3", *tracker );
	TI.DetermineStripFree( L"O3", *tracker );
	TI.DetermineStripBusy( L"R3", *tracker );
	TI.DetermineStripFree( L"P3", *tracker );
	TI.DetermineStripBusy( L"S3", *tracker );
	auto tevents2 = tracker->GetTrainEvents();
	TI.DetermineStripBusy( L"T3", *tracker );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"O3" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::FORM && evvec[3]->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::DEPARTURE && evvec[4]->GetBadge() == TI.Get( L"P3" ).bdg );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::TRANSITION && evvec[5]->GetBadge() == TI.Get( L"S3" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedExtDisformThruMoving3()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) );
	auto trains = trainCont->GetUnities();
	auto trainPtr = trains.back();
	auto initId = trainPtr->GetId();
	TI.DetermineRouteSet( L"Head102->G", *tracker );

	DisformTrain( initId, TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition );

	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripBusy( L"F", *tracker );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 5 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetBadge() == TI.Get( L"B" ).bdg );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::FORM && evvec[3]->GetBadge() == TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::DEPARTURE && evvec[4]->GetBadge() == TI.Get( L"C" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedExtDisformUnsuccess()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition ); //под отложенное расформирование
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) );
	TI.IncreaseTime( 3 * 1000 );
	TI.DetermineStripBusy( L"C", *tracker );

	//поезд сохранил дескриптор (не произошло прибытия)
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && tunityPtr->Have( TI.Get( L"C" ).bdg ) && tunityPtr->Have( TI.Get( L"B" ).bdg ) &&
		*tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedExtDisformExpired()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition ); //под отложенное расформирование
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) );
	TI.IncreaseTime( static_cast<unsigned int>( globalDelay.count() ) * 1000 );
	TI.DetermineStripBusy( L"C", *tracker, false );
	TI.DetermineStripFree( L"B", *tracker, false );
	auto arrivalMom = TI.FlushData( *tracker, false ); //прибытие

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetTime() == arrivalMom );

	//поезд сохранил дескриптор (инфопакет удален по тайм-ауту)
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && tunityPtr->Have( TI.Get( L"C" ).bdg ) && *tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedExtDisformAlienNumber()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"2518" ), Source::Disposition ); //под отложенное расформирование
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) );
	TI.IncreaseTime( 3 * 1000 );
	TI.DetermineStripBusy( L"C", *tracker );
	auto arrivalMom = TI.DetermineStripFree( L"B", *tracker ); //прибытие

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetTime() == arrivalMom );

	//поезд сохранил дескриптор (дескриптор в инфопакете не совпадает с номером)
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && tunityPtr->Have( TI.Get( L"C" ).bdg ) && *tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedExtDisformArrivedWithDisform()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineHeadClose( L"Head102", *tracker );
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"2516" ), Source::Disposition ); //под отложенное расформирование
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) ) );
	TI.DetermineStripBusy( L"C", *tracker );

	TI.IncreaseTime( 3 * 1000 );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 1 && evvec[0]->GetCode() == HCode::FORM );

	//поезд сохраняет дескриптор
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && tunityPtr->Have( TI.Get( L"C" ).bdg ) && tunityPtr->Have( TI.Get( L"B" ).bdg ) &&
		*tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );

	//поезд прибывает на путь и расформировывается
	auto arrivalMom = TI.DetermineStripFree( L"B", *tracker );
	tevents = tracker->GetTrainEvents();
	evvec.assign( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 3 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetTime() == arrivalMom );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetTime() == arrivalMom );

	tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && !tunityPtr->IsTrain() && tunityPtr->Have( TI.Get( L"C" ).bdg ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedExtDisformArrivedWODisform()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineHeadClose( L"Head102", *tracker );
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"" ), Source::Disposition ); //пакет расформирования не сохраняется
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"B" ) ) );
	TI.DetermineStripBusy( L"C", *tracker );

	TI.IncreaseTime( 3 * 1000 );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 1 && evvec[0]->GetCode() == HCode::FORM );

	//поезд сохраняет дескриптор
	auto tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && tunityPtr->Have( TI.Get( L"C" ).bdg ) && tunityPtr->Have( TI.Get( L"B" ).bdg ) &&
		*tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );

	//поезд прибывает на путь и расформировывается
	auto arrivalMom = TI.DetermineStripFree( L"B", *tracker );
	tevents = tracker->GetTrainEvents();
	evvec.assign( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 2 );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::ARRIVAL && evvec[1]->GetTime() == arrivalMom );

	tunityPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	//поезд сохраняет дескриптор
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->GetDescrPtr() && *tunityPtr->GetDescrPtr() == TrainDescr( L"2516" ) );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::OneMomentDisformAndForm()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"2516" ) ), list<wstring>( 1, L"C" ) ) );
	TI.IncreaseTime( 1000 );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"2518" ), TrainCharacteristics::Source::Disposition ); //смена номера
	TI.IncreaseTime( 1000 );
	auto trainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( trainPtr );
	vector<TrainInfoPackage> infoPackages;
	infoPackages.emplace_back( CreateInfoPackage( true, trainPtr->GetId(), TrainDescr( L"2518" ), BadgeE(), TrainCharacteristics::Source::Disposition ) );
	infoPackages.emplace_back( CreateInfoPackage( false, 0, TrainDescr( L"3822" ), TI.Get( L"C" ).bdg, TrainCharacteristics::Source::Disposition ) );
	tracker->SetTrainsInfo( move( infoPackages ) );

	auto tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 4 );
	CPPUNIT_ASSERT( all_of( evvec.cbegin(), evvec.cend(), [this]( TrackerEventPtr tevPtr ){ return tevPtr->GetBadge() == TI.Get( L"C" ).bdg; } ) );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetDetails() && evvec[0]->GetDetails()->tdptr && 
		( *evvec[0]->GetDetails()->tdptr ) == TrainDescr( L"2516" ) );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::INFO_CHANGING && evvec[1]->GetDetails() && evvec[1]->GetDetails()->tdptr && 
		( *evvec[1]->GetDetails()->tdptr ) == TrainDescr( L"2518" ) );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::DISFORM && evvec[2]->GetDetails() && !evvec[2]->GetDetails()->tdptr );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::FORM && evvec[3]->GetDetails() && evvec[3]->GetDetails()->tdptr && 
		( *evvec[3]->GetDetails()->tdptr ) == TrainDescr( L"3822" ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::NoNumberDisformForbidTransition()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineSwitchMinus( L"11", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4514" ) ), list<wstring>( 1, L"Q" ) ) );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"" ), Source::Disposition ); //под отложенное расформирование

	TI.DetermineStripBusy( L"F", *tracker );
	auto newTrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( newTrainPtr );
	auto newTrainId = newTrainPtr->GetId();
	CPPUNIT_ASSERT( initTrainId != newTrainId );

	const auto & tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 6 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetDetails() && evvec[0]->GetDetails()->tdptr && 
		( *evvec[0]->GetDetails()->tdptr ) == TrainDescr( L"4514" ) && evvec[0]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY && evvec[1]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::ARRIVAL && evvec[2]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DISFORM && evvec[3]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::FORM && evvec[4]->GetId() == newTrainId );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::DEPARTURE && evvec[5]->GetId() == newTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::NoNumberOverlappedDisformForbidTransition()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineSwitchMinus( L"11", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4514" ) ), list<wstring>( 1, L"Q" ) ) );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();
	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineRouteSet( L"Head102->G", *tracker );
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"" ), Source::Disposition ); //под отложенное расформирование

	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"2418" ), Source::Disposition );

	TI.DetermineStripBusy( L"F", *tracker );
	auto newTrainPtr = trainCont->GetUnity( TI.Get( L"C" ).bdg );
	CPPUNIT_ASSERT( newTrainPtr );
	auto newTrainId = newTrainPtr->GetId();
	CPPUNIT_ASSERT( initTrainId != newTrainId );

	const auto & tunities = trainCont->GetUnities();
	auto tevents = tracker->GetTrainEvents();

	vector<TrackerEventPtr> evvec( tevents.cbegin(), tevents.cend() );
	CPPUNIT_ASSERT( evvec.size() == 7 );
	CPPUNIT_ASSERT( evvec[0]->GetCode() == HCode::FORM && evvec[0]->GetDetails() && evvec[0]->GetDetails()->tdptr && 
		( *evvec[0]->GetDetails()->tdptr ) == TrainDescr( L"4514" ) && evvec[0]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[1]->GetCode() == HCode::STATION_ENTRY && evvec[1]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[2]->GetCode() == HCode::ARRIVAL && evvec[2]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[3]->GetCode() == HCode::DISFORM && evvec[3]->GetId() == initTrainId );
	CPPUNIT_ASSERT( evvec[4]->GetCode() == HCode::FORM && evvec[4]->GetId() == newTrainId );
	CPPUNIT_ASSERT( evvec[5]->GetCode() == HCode::DEPARTURE && evvec[5]->GetId() == newTrainId );
	CPPUNIT_ASSERT( evvec[6]->GetCode() == HCode::INFO_CHANGING && evvec[6]->GetDetails() && evvec[6]->GetDetails()->tdptr && 
		( *evvec[6]->GetDetails()->tdptr ) == TrainDescr( L"2418" ) && evvec[6]->GetId() == newTrainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::HittedNoNumberDisform()
{
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4514" ) ), list<wstring>( 1, L"C" ) ) );
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"" ), Source::Disposition ); //под отложенное расформирование
	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( !trainPtr->IsTrain() );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::MissedNoNumberDisform()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();
	DisformTrain( 0, TI.Get( L"C" ).bdg, TrainDescr( L"" ), Source::Disposition ); //под отложенное расформирование

	TI.DetermineSwitchMinus( L"11", *tracker );
	CPPUNIT_ASSERT( LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"4514" ) ), list<wstring>( 1, L"Q" ) ) );
	auto initTrainPtr = trainCont->GetUnity( TI.Get( L"Q" ).bdg );
	CPPUNIT_ASSERT( initTrainPtr );
	auto initTrainId = initTrainPtr->GetId();
	TI.DetermineStripBusy( L"A", *tracker );
	const auto & tunities2 = trainCont->GetUnities();
	TI.DetermineStripBusy( L"B", *tracker );
	const auto & tunities3 = trainCont->GetUnities();
	TI.DetermineStripBusy( L"C", *tracker );
	const auto & tunities4 = trainCont->GetUnities();
	TI.DetermineStripFree( L"Q", *tracker );
	const auto & tunities5 = trainCont->GetUnities();
	TI.DetermineStripFree( L"A", *tracker );
	const auto & tunities6 = trainCont->GetUnities();
	TI.DetermineStripFree( L"B", *tracker );

	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto trainPtr = tunities.back();
	CPPUNIT_ASSERT( trainPtr->GetId() == initTrainId );
	CPPUNIT_ASSERT( trainPtr->IsTrain() );
	auto tdescrPtr = trainPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tdescrPtr && tdescrPtr->GetNumber().getNumber() == 4514 );

	auto tevents = tracker->GetTrainEvents();
	bool haveDisform = any_of( tevents.cbegin(), tevents.cend(), []( TrackerEventPtr tePtr ){
		return tePtr->GetCode() == HCode::DISFORM;
	} );
	CPPUNIT_ASSERT( !haveDisform );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::DelayedInfoReplacement()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto globalDelay = chrono::seconds( 25 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( globalDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"1920" ), Source::Disposition );
	TI.IncreaseTime( 1000 );
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"2628" ), Source::Disposition );

	TI.DetermineStripBusy( L"B", *tracker );
	TI.DetermineStripBusy( L"C", *tracker );
	TI.DetermineStripFree( L"B", *tracker );

	const auto & tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto tunityPtr = tunities.back();
	auto tdescrPtr = tunityPtr->GetDescrPtr();
	CPPUNIT_ASSERT( tunityPtr && tunityPtr->IsTrain() && tdescrPtr && tdescrPtr->GetNumber().getNumber() == 2628 );
	const auto & tevents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( tevents.size() == 1 );
	const auto & tevent = tevents.back();
	const auto & spotDetPtr = tevent->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->tdptr );
	CPPUNIT_ASSERT( tevent->GetCode() == HCode::FORM && spotDetPtr->tdptr->GetNumber().getNumber() == 2628 );
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerInfoSetting::TrainsMigrationAfterTrainInfoDuplication()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto currentDelay = chrono::seconds( 45 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( currentDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	SetInfo( TI.Get( L"AA" ).bdg, TrainDescr( L"6236" ), TrainCharacteristics::Source::Disposition );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"V" ) );
	auto tunities = trainCont->GetUnities();
	auto trainId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"Y", *tracker );
	TI.DetermineStripFree( L"V", *tracker );
	TI.DetermineStripBusy( L"AA", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( tunities.front()->IsTrain() != tunities.back()->IsTrain() );
	for ( auto tunity : tunities )
	{
		if ( tunity->IsTrain() )
		{
			const auto & tplace = tunity->GetPlace();
			CPPUNIT_ASSERT( tplace.size() == 1 && tplace.back() == TI.Get( L"AA" ).bdg );
			auto tdescrPtr = tunity->GetDescrPtr();
			CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"6236" ) );
		}
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	timConstr.setDislocationDelays( DislocationDelays( initDelay, DislocationDelays::Detailization() ) );
}

void TC_TrackerInfoSetting::TrainsMigrationAfterTrainInfoDuplication2()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	const auto & dislokDelays = tracker->GetTimeConstraints().getDislocationDelays();
	auto initDelay = dislokDelays.getDelay( EsrKit(), ParkWayKit() );
	auto currentDelay = chrono::seconds( 45 );
	tracker->GetTimeConstraints().setDislocationDelays( DislocationDelays( currentDelay, DislocationDelays::Detailization() ) );
	tracker->Reset();

	TI.DetermineSwitchMinus( L"17", *tracker );
	SetInfo( TI.Get( L"Z" ).bdg, TrainDescr( L"6236" ), TrainCharacteristics::Source::Disposition );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"6236" ) ), list<wstring>( 1, L"AC" ) );
	auto tunities = trainCont->GetUnities();
	auto trainId = tunities.back()->GetId();

	TI.DetermineStripBusy( L"Z", *tracker );

	tunities = trainCont->GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 2 );
	CPPUNIT_ASSERT( tunities.front()->IsTrain() != tunities.back()->IsTrain() );
	for ( auto tunity : tunities )
	{
		if ( tunity->IsTrain() )
		{
			const auto & tplace = tunity->GetPlace();
			CPPUNIT_ASSERT( tplace.size() == 1 && tplace.back() == TI.Get( L"Z" ).bdg );
			auto tdescrPtr = tunity->GetDescrPtr();
			CPPUNIT_ASSERT( tdescrPtr && *tdescrPtr == TrainDescr( L"6236" ) );
		}
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	timConstr.setDislocationDelays( DislocationDelays( initDelay, DislocationDelays::Detailization() ) );
}

void TC_TrackerInfoSetting::BunchOfIdenticalNumbers()
{
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	auto initLPFThreshold = timConstr.LPFThresholdSec;
	timConstr.LPFThresholdSec = 10;

	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"" ) ), list<wstring>( 1, L"C" ) );
	LocateUnityByForce( TrainDescrPtr( new TrainDescr( L"vagoni-2" ) ), list<wstring>( 1, L"D" ) );
	TI.IncreaseTime(timConstr.LPFThresholdSec * 1000 );
	list<TrackerEventPtr> tevents;
	tracker->TakeEvents( tevents ); //очистка
	SetInfo( TI.Get( L"C" ).bdg, TrainDescr( L"vagoni-2" ), TrainCharacteristics::Source::Disposition );
	tevents = tracker->GetTrainEvents();
	SetInfo( TI.Get( L"D" ).bdg, TrainDescr( L"vagoni-2" ), TrainCharacteristics::Source::Disposition );
	tevents = tracker->GetTrainEvents();
	TI.IncreaseTime(timConstr.LPFThresholdSec * 1000 );

	tevents = tracker->GetTrainEvents();
	vector<TrackerEventPtr> eventsVec( tevents.cbegin(), tevents.cend() );
	
	//разрешены два исхода:
	// 1. Приоритетной считается посылка для участка D, тогда поезда не меняют номера. Событий нет
	// 2. Приоритетной считается посылка для участка C, тогда поезд на С меняет номер, а поезд на D сбрасывает. 2 события
	CPPUNIT_ASSERT( eventsVec.empty() || eventsVec.size() == 2 );
	if ( !eventsVec.empty() )
	{
		CPPUNIT_ASSERT( find_if( eventsVec.cbegin(), eventsVec.cend(), [this]( TrackerEventPtr tevPtr ){ return tevPtr->GetBadge() == TI.Get( L"C" ).bdg; } ) != eventsVec.cend() );
		CPPUNIT_ASSERT( find_if( eventsVec.cbegin(), eventsVec.cend(), [this]( TrackerEventPtr tevPtr ){ return tevPtr->GetBadge() == TI.Get( L"D" ).bdg; } ) != eventsVec.cend() );
	}
	for ( const auto & event : eventsVec )
	{
		auto spotDetPtr = event->GetDetails();
		CPPUNIT_ASSERT( spotDetPtr );
		auto tdPtr = spotDetPtr->tdptr;
		CPPUNIT_ASSERT( tdPtr );
		if ( event->GetBadge() == TI.Get( L"C" ).bdg )
			CPPUNIT_ASSERT( tdPtr->GetNumber().getString() == L"vagoni-2" );
		else
			CPPUNIT_ASSERT( tdPtr->GetNumber().getString().empty() );
	}

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );

	timConstr.LPFThresholdSec = initLPFThreshold;
}