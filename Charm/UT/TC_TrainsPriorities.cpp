#include "stdafx.h"
#include "TC_TrainsPriorities.h"
#include "../helpful/TrainsPriorityDefiner.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include <random>       // std::default_random_engine
#include <chrono>       // std::chrono::system_clock
#include "../helpful/Serialization.h"
#include "XmlEqual.h"

using namespace std;
typedef TrainCharacteristics::TrainFeature TF;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrainsPriorities );

static const string offsetsTestStr = 
	"<PriorityOffsets>\n"\
		"<Train number=\"2845\" type=\"innerInc\" shift=\"1\"/>\n"\
		"<Train number=\"3368\" type=\"outerDec\" shift=\"7\"/>\n"\
		"<Train number=\"4123\" type=\"innerDec\" shift=\"1\"/>\n"\
		"<Train number=\"6820\" type=\"outerInc\" shift=\"5\"/>\n"\
	"</PriorityOffsets>\n";

ConstTrainDescrPtr TC_TrainsPriorities::MakeAnyDescr( const TF & purpose ) const
{
	auto & tcharacts = TrainCharacteristics::instanceCRef();
	auto numranges = tcharacts.GetPurposeNumRanges( purpose );
	TrainNumber testnumber( numranges.front().start );
	return ConstTrainDescrPtr( new TrainDescr( testnumber.getNumberString() ) );
}

void TC_TrainsPriorities::DefaultSorting()
{
	auto & tcharacts = TrainCharacteristics::instanceCRef();
	vector<ConstTrainDescrPtr> invec;
	auto unitedDescr = MakeAnyDescr( TF::United );
	invec.emplace_back( unitedDescr );
	auto trolleyDescr = MakeAnyDescr( TF::MotorTrolley );
	invec.emplace_back( trolleyDescr );
	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, TrainsPriorityOffsets() );
	CPPUNIT_ASSERT( sortedVec.size() == 2 );
	CPPUNIT_ASSERT( sortedVec.front().front() == unitedDescr );
	CPPUNIT_ASSERT( sortedVec.back().front() == trolleyDescr );
}

template<typename Iterator>
bool checkLevel( Iterator it1, Iterator it2, const TrainsPriorityDefiner::SamePriorityTrains & samelevelled )
{
	return all_of( samelevelled.cbegin(), samelevelled.cend(), [&it1, &it2]( ConstTrainDescrPtr tdescrPtr ){
		auto & tcharacts = TrainCharacteristics::instanceCRef();
		return find( it1, it2, tcharacts.GetPurposeFeature( tdescrPtr->GetNumber().getNumber() ) ) != it2;
	} );
};

void TC_TrainsPriorities::DefaultSorting2()
{
	TF tfeatarr[] = { TF::Restore, TF::Firestop, TF::Highrapid, TF::Touristic, TF::Express3, TF::PassengerAllSeason, TF::SuburbanRegional, 
		TF::ElectricTrainSpecialObjective, TF::Human, TF::United_350_520, TF::LiquidcargoRoundRoute, TF::Heavy_g9000, TF::WindowUnloadWagons, TF::Through,
		TF::RaillubricatorResLoco, TF::HopperBatcher, TF::RailsTransport, TF::RailsPolish, TF::Modular
	};
	auto & tcharacts = TrainCharacteristics::instanceCRef();
	vector<ConstTrainDescrPtr> invec;
	for ( auto tfIt = begin( tfeatarr ); tfIt != end( tfeatarr ); ++tfIt )
		invec.emplace_back( MakeAnyDescr( *tfIt ) );

	unsigned int seed = static_cast<unsigned int>( chrono::system_clock::now().time_since_epoch().count() );
	shuffle( invec.begin(), invec.end(), default_random_engine( seed ) );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, TrainsPriorityOffsets() );
	CPPUNIT_ASSERT( sortedVec.size() == 10 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 2 );
	TF checkTypes1[] = { TF::Restore, TF::Firestop };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );
	
	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 4 );
	TF checkTypes2[] = { TF::Highrapid, TF::Touristic, TF::Express3, TF::PassengerAllSeason };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 2 );
	TF checkTypes3[] = { TF::SuburbanRegional, TF::ElectricTrainSpecialObjective };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 1 );
	TF checkTypes4[] = { TF::Human };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 2 );
	TF checkTypes5[] = { TF::United_350_520, TF::Heavy_g9000 };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );

	const auto & level6 = sortedVec[5];
	CPPUNIT_ASSERT( level6.size() == 2 );
	TF checkTypes6[] = { TF::Through, TF::LiquidcargoRoundRoute };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes6 ), end( checkTypes6 ), level6 ) );

	const auto & level7 = sortedVec[6];
	CPPUNIT_ASSERT( level7.size() == 1 );
	TF checkTypes7[] = { TF::Modular };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes7 ), end( checkTypes7 ), level7 ) );

	const auto & level8 = sortedVec[7];
	CPPUNIT_ASSERT( level8.size() == 1 );
	TF checkTypes8[] = { TF::WindowUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes8 ), end( checkTypes8 ), level8 ) );

	const auto & level9 = sortedVec[8];
	CPPUNIT_ASSERT( level9.size() == 1 );
	TF checkTypes9[] = { TF::RaillubricatorResLoco };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes9 ), end( checkTypes9 ), level9 ) );

	const auto & level10 = sortedVec[9];
	CPPUNIT_ASSERT( level10.size() == 3 );
	TF checkTypes10[] = { TF::HopperBatcher, TF::RailsTransport, TF::RailsPolish };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes10 ), end( checkTypes10 ), level10 ) );
}

void TC_TrainsPriorities::OffsetsSerialize()
{
	TrainsPriorityOffsets tpoffsets;
	tpoffsets.InnerIncrease( TrainNumber( L"2845" ) );
	tpoffsets.GroupIncrease( TrainNumber( L"6820" ), 5 );
	tpoffsets.InnerDecrease( TrainNumber( L"4123" ) );
	tpoffsets.GroupDecrease( TrainNumber( L"3368" ), 7 );
	string serialStr = serialize( tpoffsets );
	CPPUNIT_ASSERT( xmlEqual( serialStr, offsetsTestStr ) );
}

void TC_TrainsPriorities::OffsetsDeserialize()
{
	string serialStr = offsetsTestStr;
	auto offsetsPtr = deserialize<TrainsPriorityOffsets>( serialStr );
	CPPUNIT_ASSERT( offsetsPtr );
	auto offset2845Ptr = offsetsPtr->GetOffset( TrainNumber( L"2845" ) );
	CPPUNIT_ASSERT( offset2845Ptr && offset2845Ptr->type == TrainsPriorityOffsets::InnerIncreased && offset2845Ptr->shift == 1 );
	auto offset6820Ptr = offsetsPtr->GetOffset( TrainNumber( L"6820" ) );
	CPPUNIT_ASSERT( offset6820Ptr && offset6820Ptr->type == TrainsPriorityOffsets::OuterIncreased && offset6820Ptr->shift == 5 );
	auto offset4123Ptr = offsetsPtr->GetOffset( TrainNumber( L"4123" ) );
	CPPUNIT_ASSERT( offset4123Ptr && offset4123Ptr->type == TrainsPriorityOffsets::InnerDecreased && offset4123Ptr->shift == 1 );
	auto offset3368Ptr = offsetsPtr->GetOffset( TrainNumber( L"3368" ) );
	CPPUNIT_ASSERT( offset3368Ptr && offset3368Ptr->type == TrainsPriorityOffsets::OuterDecreased && offset3368Ptr->shift == 7 );
}

void TC_TrainsPriorities::OffsetsManipulations()
{
	TrainsPriorityOffsets tpoffsets;
	tpoffsets.InnerIncrease( TrainNumber( L"5549" ) );
	auto affNumbers = tpoffsets.AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.size() == 1 );
	CPPUNIT_ASSERT( affNumbers[0].getString() == L"5549" );
	auto offsetPtr = tpoffsets.GetOffset( TrainNumber( L"5549" ) );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::InnerIncreased && offsetPtr->shift == 1 );
	tpoffsets.InnerDecrease( TrainNumber( L"9411" ) );
	tpoffsets.ResetOffsets( TrainNumber( L"5549" ) );

	affNumbers = tpoffsets.AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.size() == 1 );
	CPPUNIT_ASSERT( affNumbers[0].getString() == L"9411" );
	offsetPtr = tpoffsets.GetOffset( TrainNumber( L"9411" ) );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::InnerDecreased && offsetPtr->shift == 1 );
	tpoffsets.GroupDecrease( TrainNumber( L"6190" ), 13 );
	tpoffsets.ResetOffsets( TrainNumber( L"9411" ) );

	affNumbers = tpoffsets.AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.size() == 1 );
	CPPUNIT_ASSERT( affNumbers[0].getString() == L"6190" );
	offsetPtr = tpoffsets.GetOffset( TrainNumber( L"6190" ) );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::OuterDecreased && offsetPtr->shift == 13 );
	tpoffsets.GroupIncrease( TrainNumber( L"8182" ), 9 );
	tpoffsets.ResetOffsets( TrainNumber( L"6190" ) );

	affNumbers = tpoffsets.AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.size() == 1 );
	CPPUNIT_ASSERT( affNumbers[0].getString() == L"8182" );
	offsetPtr = tpoffsets.GetOffset( TrainNumber( L"8182" ) );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::OuterIncreased && offsetPtr->shift == 9 );
	tpoffsets.InnerDecrease( TrainNumber( L"8182" ) );
	offsetPtr = tpoffsets.GetOffset( TrainNumber( L"8182" ) );
	CPPUNIT_ASSERT( offsetPtr && offsetPtr->type == TrainsPriorityOffsets::InnerDecreased && offsetPtr->shift == 1 );
	tpoffsets.ResetOffsets( TrainNumber( L"8182" ) );

	affNumbers = tpoffsets.AffectedNumbers();
	CPPUNIT_ASSERT( affNumbers.empty() );
}

void TC_TrainsPriorities::OffsetsSorting1()
{
	//Refrigerator = группа 1
	//Container, Animals, Divisional = группа 2
	//Modular = группа 3

	//тест = внутреннее повышение Animals
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr animalDescrPtr;
	TF checkTypes[] = { TF::Refrigerator, TF::Container, TF::Animals, TF::Divisional, TF::Modular };
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Animals )
			animalDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( animalDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.InnerIncrease( animalDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 4 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 1 );
	TF checkTypes1[] = { TF::Refrigerator };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 1 );
	TF checkTypes2[] = { TF::Animals };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 2 );
	TF checkTypes3[] = { TF::Container, TF::Divisional };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 1 );
	TF checkTypes4[] = { TF::Modular };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );
}

void TC_TrainsPriorities::OffsetsSorting2()
{
	//Refrigerator = группа 1
	//Container, Animals, Divisional = группа 2
	//Modular = группа 3

	//тест = внутреннее понижение Animals
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr animalDescrPtr;
	TF checkTypes[] = { TF::Refrigerator, TF::Container, TF::Animals, TF::Divisional, TF::Modular };
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Animals )
			animalDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( animalDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.InnerDecrease( animalDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 4 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 1 );
	TF checkTypes1[] = { TF::Refrigerator };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::Container, TF::Divisional };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 1 );
	TF checkTypes3[] = { TF::Animals };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 1 );
	TF checkTypes4[] = { TF::Modular };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );
}

void TC_TrainsPriorities::OffsetsSorting3()
{
	//Container, Universal, Animals = группа 1
	//Modular, DivisionalModular, Outbound = группа 2
	//Transfer, DispatchersLoco, WindowUnloadWagons = группа 3
	//CargoPusher, PassengerPusher, UtilityJoining = группа 4

	//тест = внешнее минимальное повышение Transfer с помещением в другую группу
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr transferDescrPtr;
	TF checkTypes[] = { TF::Container, TF::Universal, TF::Animals, TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer, TF::DispatchersLoco,
		TF::WindowUnloadWagons, TF::CargoPusher, TF::PassengerPusher, TF::UtilityJoining
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Transfer )
			transferDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( transferDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.GroupIncrease( transferDescrPtr->GetNumber(), 1 );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 4 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 3 );
	TF checkTypes1[] = { TF::Container, TF::Universal, TF::Animals };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 4 );
	TF checkTypes2[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 2 );
	TF checkTypes3[] = { TF::DispatchersLoco, TF::WindowUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 3 );
	TF checkTypes4[] = { TF::CargoPusher, TF::PassengerPusher, TF::UtilityJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );
}

void TC_TrainsPriorities::OffsetsSorting4()
{
	//MVPS_Served, SuburbanFast, ElectricTrainSpecialObjective = группа 1
	//Human, Cargopassenger, TechnicalMotorcar = группа 2
	//Animals, DriedcargoRoundRouted, Through = группа 3

	//тест = внешнее минимальное понижение Human с созданием отдельной группы
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr humanDescrPtr;
	TF checkTypes[] = { TF::MVPS_Served, TF::SuburbanFast, TF::ElectricTrainSpecialObjective, TF::Human, TF::Cargopassenger, TF::TechnicalMotorcar, 
		TF::Animals, TF::DriedcargoRoundRouted, TF::Through
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Human )
			humanDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( humanDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.GroupDecrease( humanDescrPtr->GetNumber(), 1 );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 4 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 3 );
	TF checkTypes1[] = { TF::MVPS_Served, TF::SuburbanFast, TF::ElectricTrainSpecialObjective };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::Cargopassenger, TF::TechnicalMotorcar };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 1 );
	TF checkTypes3[] = { TF::Human };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 3 );
	TF checkTypes4[] = { TF::Animals, TF::DriedcargoRoundRouted, TF::Through };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );
}

void TC_TrainsPriorities::OffsetsSorting5()
{
	//Modular, DivisionalModular, Outbound = группа 1
	//Transfer, DispatchersLoco, DocumentsUnloadWagons = группа 2
	//CargoPusher, PassengerResLoco, PassengerJoining = группа 3
	//TrackLaying, Defectoscopy, WaterTransport = группа 4

	//тест = внешнее минимальное понижение Transfer с переносом в нижнюю группу + внутреннее понижение PassengerJoining
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr transferDescrPtr, passjoiningDescrPtr;
	TF checkTypes[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons, 
		TF::CargoPusher, TF::PassengerResLoco, TF::PassengerJoining, TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Transfer )
			transferDescrPtr = tdescrPtr;
		else if ( proptype == TF::PassengerJoining )
			passjoiningDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( transferDescrPtr && passjoiningDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.GroupDecrease( transferDescrPtr->GetNumber(), 1 );
	priorityOffsets.InnerDecrease( passjoiningDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 5 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 3 );
	TF checkTypes1[] = { TF::Modular, TF::DivisionalModular, TF::Outbound };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::DispatchersLoco, TF::DocumentsUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 3 );
	TF checkTypes3[] = { TF::CargoPusher, TF::PassengerResLoco, TF::Transfer };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 1 );
	TF checkTypes4[] = { TF::PassengerJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 3 );
	TF checkTypes5[] = { TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );
}

void TC_TrainsPriorities::OffsetsSorting6()
{
	//Modular, DivisionalModular, Outbound = группа 1
	//Transfer, DispatchersLoco, DocumentsUnloadWagons = группа 2
	//CargoPusher, PassengerResLoco, PassengerJoining = группа 3
	//TrackLaying, Defectoscopy, WaterTransport = группа 4

	//тест = внешнее минимальное понижение Transfer с переносом в нижнюю группу + внутреннее повышение PassengerJoining
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr transferDescrPtr, passjoiningDescrPtr;
	TF checkTypes[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons, 
		TF::CargoPusher, TF::PassengerResLoco, TF::PassengerJoining, TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::Transfer )
			transferDescrPtr = tdescrPtr;
		else if ( proptype == TF::PassengerJoining )
			passjoiningDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( transferDescrPtr && passjoiningDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.GroupDecrease( transferDescrPtr->GetNumber(), 1 );
	priorityOffsets.InnerIncrease( passjoiningDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 5 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 3 );
	TF checkTypes1[] = { TF::Modular, TF::DivisionalModular, TF::Outbound };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::DispatchersLoco, TF::DocumentsUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 1 );
	TF checkTypes3[] = { TF::PassengerJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 3 );
	TF checkTypes4[] = { TF::CargoPusher, TF::PassengerResLoco, TF::Transfer };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 3 );
	TF checkTypes5[] = { TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );
}

void TC_TrainsPriorities::OffsetsSorting7()
{
	//Modular, DivisionalModular, Outbound = группа 1
	//Transfer, DispatchersLoco, DocumentsUnloadWagons = группа 2
	//CargoPusher, PassengerResLoco, PassengerJoining = группа 3
	//TrackLaying, Defectoscopy, WaterTransport = группа 4

	//тест = внешнее повышение CargoPusher на два уровня + внутреннее повышение Transfer
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr pusherDescrPtr, transferDescrPtr;
	TF checkTypes[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons, 
		TF::CargoPusher, TF::PassengerResLoco, TF::PassengerJoining, TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::CargoPusher )
			pusherDescrPtr = tdescrPtr;
		else if ( proptype == TF::Transfer )
			transferDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( pusherDescrPtr && transferDescrPtr );
	TrainsPriorityOffsets priorityOffsets;

	priorityOffsets.GroupIncrease( pusherDescrPtr->GetNumber(), 2 );
	priorityOffsets.InnerIncrease( transferDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 5 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 4 );
	TF checkTypes1[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::CargoPusher };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 1 );
	TF checkTypes2[] = { TF::Transfer };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 2 );
	TF checkTypes3[] = { TF::DispatchersLoco, TF::DocumentsUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 2 );
	TF checkTypes4[] = { TF::PassengerResLoco, TF::PassengerJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 3 );
	TF checkTypes5[] = { TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );
}

void TC_TrainsPriorities::OffsetsSorting8()
{
	//Modular, DivisionalModular, Outbound = группа 1
	//Transfer, DispatchersLoco, DocumentsUnloadWagons = группа 2
	//CargoPusher, PassengerResLoco, PassengerJoining = группа 3
	//TrackLaying, Defectoscopy, WaterTransport = группа 4

	//тест = внешнее повышение CargoPusher на два уровня + внутреннее понижение Transfer
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr pusherDescrPtr, transferDescrPtr;
	TF checkTypes[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons, 
		TF::CargoPusher, TF::PassengerResLoco, TF::PassengerJoining, TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::CargoPusher )
			pusherDescrPtr = tdescrPtr;
		else if ( proptype == TF::Transfer )
			transferDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( pusherDescrPtr && transferDescrPtr );
	TrainsPriorityOffsets priorityOffsets;

	priorityOffsets.GroupIncrease( pusherDescrPtr->GetNumber(), 2 );
	priorityOffsets.InnerDecrease( transferDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 5 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 4 );
	TF checkTypes1[] = { TF::Modular, TF::DivisionalModular, TF::Outbound, TF::CargoPusher };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::DispatchersLoco, TF::DocumentsUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 1 );
	TF checkTypes3[] = { TF::Transfer };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 2 );
	TF checkTypes4[] = { TF::PassengerResLoco, TF::PassengerJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 3 );
	TF checkTypes5[] = { TF::TrackLaying, TF::Defectoscopy, TF::WaterTransport };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );
}

void TC_TrainsPriorities::OffsetsSorting9()
{
	//FastAllYear, PassengerAllSeason, Rapid, Touristic = группа 1
	//PassengerResLoco, FastResLoco, CargoJoining, UtilityJoining = группа 2

	//тест = внешнее повышение CargoJoining на 7 уровней, внешнее понижение Rapid на 7 уровней, внутреннее повышение Touristic,
	//внтреннее понижение UtilityJoining
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr crgjoiningDescrPtr, rapidDescrPtr, touristicDescrPtr, utijoiningPtr;
	TF checkTypes[] = { TF::FastAllYear, TF::PassengerAllSeason, TF::Rapid, TF::Touristic, TF::PassengerResLoco, TF::FastResLoco, TF::CargoJoining, 
		TF::UtilityJoining
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::CargoJoining )
			crgjoiningDescrPtr = tdescrPtr;
		else if ( proptype == TF::Rapid )
			rapidDescrPtr = tdescrPtr;
		else if ( proptype == TF::Touristic )
			touristicDescrPtr = tdescrPtr;
		else if ( proptype == TF::UtilityJoining )
			utijoiningPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( crgjoiningDescrPtr && rapidDescrPtr && touristicDescrPtr && utijoiningPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.GroupIncrease( crgjoiningDescrPtr->GetNumber(), 7 );
	priorityOffsets.GroupDecrease( rapidDescrPtr->GetNumber(), 7 );
	priorityOffsets.InnerDecrease( utijoiningPtr->GetNumber() );
	priorityOffsets.InnerIncrease( touristicDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 4 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 1 );
	TF checkTypes1[] = { TF::Touristic };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 3 );
	TF checkTypes2[] = { TF::FastAllYear, TF::PassengerAllSeason, TF::CargoJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 3 );
	TF checkTypes3[] = { TF::Rapid, TF::PassengerResLoco, TF::FastResLoco };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 1 );
	TF checkTypes4[] = { TF::UtilityJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );
}

void TC_TrainsPriorities::OffsetsSorting10()
{
	//Transfer, DispatchersLoco, DocumentsUnloadWagons = группа 1
	//CargoPusher, PassengerPusher, PassengerResLoco, PassengerJoining, UtilityJoining = группа 2
	//MotorTrolley, LowPowerLoco, TrialRunLoco = группа 3

	//тест = внутреннее минимальное понижение PassengerJoining и UtilityJoining, внутреннее повышение CargoPusher и PassengerPusher
	vector<ConstTrainDescrPtr> invec;
	ConstTrainDescrPtr passjoinDescrPtr, utijoinDescrPtr, crgpusherDescrPtr, passpusherDescrPtr;
	TF checkTypes[] = { TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons, TF::CargoPusher, TF::PassengerPusher, TF::PassengerResLoco, 
		TF::PassengerJoining, TF::UtilityJoining, TF::MotorTrolley, TF::LowPowerLoco, TF::TrialRunLoco
	};
	for ( auto proptype : checkTypes )
	{
		auto tdescrPtr = MakeAnyDescr( proptype );
		if ( proptype == TF::PassengerJoining )
			passjoinDescrPtr = tdescrPtr;
		else if ( proptype == TF::UtilityJoining )
			utijoinDescrPtr = tdescrPtr;
		else if ( proptype == TF::CargoPusher )
			crgpusherDescrPtr = tdescrPtr;
		else if ( proptype == TF::PassengerPusher )
			passpusherDescrPtr = tdescrPtr;
		invec.push_back( tdescrPtr );
	}
	CPPUNIT_ASSERT( passjoinDescrPtr && utijoinDescrPtr && crgpusherDescrPtr && passpusherDescrPtr );
	TrainsPriorityOffsets priorityOffsets;
	priorityOffsets.InnerIncrease( crgpusherDescrPtr->GetNumber() );
	priorityOffsets.InnerIncrease( passpusherDescrPtr->GetNumber() );
	priorityOffsets.InnerDecrease( passjoinDescrPtr->GetNumber() );
	priorityOffsets.InnerDecrease( utijoinDescrPtr->GetNumber() );

	TrainsPriorityDefiner tpriorityDefiner;
	const auto & sortedVec = tpriorityDefiner.Sort( invec, priorityOffsets );
	CPPUNIT_ASSERT( sortedVec.size() == 5 );

	const auto & level1 = sortedVec[0];
	CPPUNIT_ASSERT( level1.size() == 3 );
	TF checkTypes1[] = { TF::Transfer, TF::DispatchersLoco, TF::DocumentsUnloadWagons };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes1 ), end( checkTypes1 ), level1 ) );

	const auto & level2 = sortedVec[1];
	CPPUNIT_ASSERT( level2.size() == 2 );
	TF checkTypes2[] = { TF::CargoPusher, TF::PassengerPusher };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes2 ), end( checkTypes2 ), level2 ) );

	const auto & level3 = sortedVec[2];
	CPPUNIT_ASSERT( level3.size() == 1 );
	TF checkTypes3[] = { TF::PassengerResLoco };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes3 ), end( checkTypes3 ), level3 ) );

	const auto & level4 = sortedVec[3];
	CPPUNIT_ASSERT( level4.size() == 2 );
	TF checkTypes4[] = { TF::PassengerJoining, TF::UtilityJoining };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes4 ), end( checkTypes4 ), level4 ) );

	const auto & level5 = sortedVec[4];
	CPPUNIT_ASSERT( level5.size() == 3 );
	TF checkTypes5[] = { TF::MotorTrolley, TF::LowPowerLoco, TF::TrialRunLoco };
	CPPUNIT_ASSERT( checkLevel( begin( checkTypes5 ), end( checkTypes5 ), level5 ) );
}