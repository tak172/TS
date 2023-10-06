#include "stdafx.h"
#include "TC_StationsRegistry.h"
#include "../helpful/Serialization.h"
#include "../helpful/StatWayInfo.h"
#include "../helpful/StationsRegistry.h"
#include "../helpful/Bypass.h"
#include "../helpful/RoadCode.h"
#include "../helpful/Attic.h"
#include "XmlEqual.h"

using namespace std;

typedef ADProperties ADAttr;
typedef StatWayInfo::Section Section;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StationsRegistry );

static RoadKit belorussiaRoad( RoadCode::belorussia );
static RoadKit latvianRoad( RoadCode::latvianNationalCode );
static RoadKit anyRoad;

static const string statRegistryTestStr =
	"<StationsRegistry>"
		"<Station EsrCode='11030'>"
			"<StatWayInfo wayNum='1' parkNum='14'>"
				"<section object_name='14/1' even_capacity='25' odd_capacity='74' psng='Y' embark='' maj='' tpassOddness='1' nonStop='Y' bidCargo=''/>"
			"</StatWayInfo>"
			"<StatWayInfo wayNum='3' parkNum='4'>"
				"<section object_name='4/3' even_capacity='8' odd_capacity='30' psng='' embark='Y' maj='' tpassOddness='2' nonStop='' bidCargo='Y'/>"
			"</StatWayInfo>"
			"<StatWayInfo wayNum='2' parkNum='1'>"
				"<section object_name='1/2' even_capacity='10' odd_capacity='0' psng='' embark='' maj='Y' tpassOddness='1' nonStop='Y' bidCargo='Y'/>"
			"</StatWayInfo>"
			"<TECHNICAL_TIME>"
				"<value oddness='odd' netto='any' train_type='tepsng' minutes='2'>"
				"<from_road>"
					"<National value = '25'/>"
				"</from_road>"
				"<to_road>"
					"<National value = '21'/>"
				"</to_road>"
				"</value>"
				"<value oddness='undef' netto='full' train_type='heavy_g8000' minutes='23'>"
				"<from_road>"
					"<National value = '21'/>"
				"</from_road>"
				"<to_road>"
					"<National value = '25'/>"
				"</to_road>"
				"</value>"
				"<value oddness='even' netto='empty' train_type='noinfo' minutes='44'>"
				"<from_road/>"
				"<to_road/>"
				"</value>"
			"</TECHNICAL_TIME>"
			"<PicketingInfo />"
		"</Station>"
			"<Station EsrCode='45123' departbyhpath='Y'>"
			"<StatWayInfo wayNum='4' parkNum='2'>"
				"<section object_name='2/4' even_capacity='37' odd_capacity='63' psng='' embark='Y' maj='Y' tpassOddness='0' nonStop='' bidCargo='Y'>"
					"<weightNorms>"
						"<value locoser='585' target='12200' tonweight='7650' />"
						"<value locoser='691' target='37190' tonweight='2100' />"
						"<value locoser='240' target='23580' tonweight='4990' />"
					"</weightNorms>"
				"</section>"
			"</StatWayInfo>"
			"<StatWayInfo wayNum='2' parkNum='3'>"
				"<section object_name='3/2' even_capacity='82' odd_capacity='21' psng='Y' embark='Y' maj='' tpassOddness='1' nonStop='Y' bidCargo=''/>"
			"</StatWayInfo>"
			"<StatWayInfo wayNum='7' parkNum='6'>"
				"<section object_name='6/7-1' even_capacity='55' odd_capacity='46' psng='Y' embark='' maj='Y' tpassOddness='2' nonStop='' bidCargo=''>"
					"<weightNorms>"
						"<value locoser='369' target='9812' tonweight='1300' />"
					"</weightNorms>"
				"</section>"
				"<section object_name='6/7-2' even_capacity='69' odd_capacity='28' psng='' embark='' maj='Y' tpassOddness='0' nonStop='Y' bidCargo='Y'>"
					"<weightNorms>"
						"<value locoser='515' target='27300' tonweight='5500' />"
					"</weightNorms>"
				"</section>"
			"</StatWayInfo>"
			"<TECHNICAL_TIME>"
				"<value oddness='odd' netto='empty' train_type='noinfo' minutes='1'>"
				"<from_road/>"
				"<to_road/>"
				"</value>"
				"<value oddness='even' netto='any' train_type='unit' minutes='250'>"
				"<from_road>"
					"<National value = '21'/>"
				"</from_road>"
				"<to_road>"
					"<National value = '25'/>"
				"</to_road>"
				"</value>"
			"</TECHNICAL_TIME>"
			"<PicketingInfo picketing1_val='1001~120' picketing1_comm='Belgrad' picketing2_val='150~67' picketing2_comm='Moscow'/>"
		"</Station>"
	"</StationsRegistry>";

static const string statRegistryTestStr2 =
	"<StationsRegistry>"
		"<Station EsrCode='06520'>"
			"<StatWayInfo wayNum='1' parkNum='1'/>"
			"<PicketingInfo />"
			"<RoutesDefinition/>"
		"</Station>"
		"<Station EsrCode='06521'>"
			"<StatWayInfo wayNum='1' parkNum='1'/>"
			"<PicketingInfo />"
			"<RoutesDefinition>"
				"<Transroutes>"
					"<value from='25000:55000' to='35000:65000'/>"
					"<value from='25001:55001' to='35001:65001'/>"
					"<value from='25002:55002' to='35002:65002'/>"
				"</Transroutes>"
			"</RoutesDefinition>"
		"</Station>"
		"<Station EsrCode='06522'>"
			"<StatWayInfo wayNum='1' parkNum='1'/>"
			"<PicketingInfo />"
		"</Station>"
	"</StationsRegistry>";

void TC_StationsRegistry::Serialize()
{
    //пути
    map <EsrKit, StationsRegistry::StationInfo> statInfos;
    StatWayVec& wayVec1 = statInfos[ EsrKit( 45123 ) ].statWays;
    ADProperties::AttrSet adarray11;
    adarray11.set( ADAttr::EMBARKATION );
    adarray11.set( ADAttr::MAJOR );
    adarray11.set( ADAttr::BID_CARGO );
    WeightNorms wnorms11;
    wnorms11.AddNorm( WeightNorms::NormValue( 585, StationEsr( 12200 ), 7650 ) );
    wnorms11.AddNorm( WeightNorms::NormValue( 691, StationEsr( 37190 ), 2100 ) );
    wnorms11.AddNorm( WeightNorms::NormValue( 240, StationEsr( 23580 ), 4990 ) );
    Section section11( L"2/4", 37, 63, ADProperties( adarray11, Oddness::UNDEF, wnorms11 ) );
    wayVec1.emplace_back( new StatWayInfo( 4, 2, vector<Section>( 1, section11 ) ) );
    ADProperties::AttrSet adarray12;
    adarray12.set( ADAttr::PASSENGER );
    adarray12.set( ADAttr::EMBARKATION );
    adarray12.set( ADAttr::NON_STOP_PASSING );
    Section section12( L"3/2", 82, 21, ADProperties( adarray12, Oddness::ODD, WeightNorms() ) );
    wayVec1.emplace_back( new StatWayInfo( 2, 3, vector<Section>( 1, section12 ) ) );

    ADProperties::AttrSet adarray13_1;
    adarray13_1.set( ADAttr::PASSENGER );
    adarray13_1.set( ADAttr::MAJOR );
    WeightNorms wnorms13_1;
    wnorms13_1.AddNorm( WeightNorms::NormValue( 369, StationEsr( 9812 ), 1300 ) );
    Section section13_1( L"6/7-1", 55, 46, ADProperties( adarray13_1, Oddness::EVEN, wnorms13_1 ) );
    ADProperties::AttrSet adarray13_2;
    adarray13_2.set( ADAttr::MAJOR );
    adarray13_2.set( ADAttr::BID_CARGO );
    adarray13_2.set( ADAttr::NON_STOP_PASSING );

    WeightNorms wnorms13_2;
    wnorms13_2.AddNorm( WeightNorms::NormValue( 515, StationEsr( 27300 ), 5500 ) );
    Section section13_2( L"6/7-2", 69, 28, ADProperties( adarray13_2, Oddness::UNDEF, wnorms13_2 ) );

    vector<StatWayInfo::Section> sections13;
    sections13.push_back( section13_1 );
    sections13.push_back( section13_2 );
    wayVec1.emplace_back( new StatWayInfo( 7, 6, sections13 ) );

    StatWayVec& wayVec2 = statInfos[ EsrKit( 11030 ) ].statWays;
    ADProperties::AttrSet adarray21;
    adarray21.set( ADAttr::PASSENGER );
    adarray21.set( ADAttr::NON_STOP_PASSING );
    Section section21( L"14/1", 25, 74, ADProperties( adarray21, Oddness::ODD, WeightNorms() ) );
    wayVec2.emplace_back( new StatWayInfo( 1, 14, vector<Section>( 1, section21 ) ) );
    ADProperties::AttrSet adarray22;
    adarray22.set( ADAttr::EMBARKATION );
    adarray22.set( ADAttr::BID_CARGO );
    Section section22( L"4/3", 8, 30, ADProperties( adarray22, Oddness::EVEN, WeightNorms() ) );
    wayVec2.emplace_back( new StatWayInfo( 3, 4, vector<Section>( 1, section22 ) ) );
    ADProperties::AttrSet adarray23;
    adarray23.set( ADAttr::MAJOR );
    adarray23.set( ADAttr::NON_STOP_PASSING );
    adarray23.set( ADAttr::BID_CARGO );

	Section section23( L"1/2", 10, 0, ADProperties(adarray23, Oddness::ODD, WeightNorms()));
	wayVec2.emplace_back( new StatWayInfo( 2, 1, vector<Section>( 1, section23 ) ) );

	//техническое время
	auto & techTime1 = statInfos[EsrKit( 11030 )].techTime;
	TechnicalTime::SpecifiedTime spectime;
	spectime.duration = TechnicalTime::Minutes( 2 );
	spectime.specification = TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Any, latvianRoad, belorussiaRoad,
		TrainCharacteristics::TrainFeature::TechnicalPassenger );
	techTime1.AddTime( spectime );
	spectime.duration = TechnicalTime::Minutes( 23 );
	spectime.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Full, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::Heavy_g8000 );
	techTime1.AddTime( spectime );
	spectime.duration = TechnicalTime::Minutes( 44 );
	spectime.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Empty, anyRoad, anyRoad,
		TrainCharacteristics::TrainFeature::NoInfo );
	techTime1.AddTime( spectime );

	auto & techTime2 = statInfos[EsrKit( 45123 )].techTime;
	spectime.duration = TechnicalTime::Minutes( 1 );
	spectime.specification = TechnicalTime::Specification( Oddness::ODD, TechnicalTime::Fullness::Empty, anyRoad, anyRoad,
		TrainCharacteristics::TrainFeature::NoInfo );
	techTime2.AddTime( spectime );
	spectime.duration = TechnicalTime::Minutes( 250 );
	spectime.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Any, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::United );
	techTime2.AddTime( spectime );

	//отдельные признаки
	statInfos[EsrKit( 45123 )].departByHPath = true; // departByHPath.insert( EsrKit( 45123 ) );

	//пикетаж
	PicketingInfo pickInfo;
	pickInfo.SetCoord( rwRuledCoord( L"Moscow", rwCoord( 150, 67 ) ) );
	pickInfo.SetCoord( rwRuledCoord( L"Belgrad", rwCoord( 1001, 120 ) ) );
	statInfos[EsrKit( 45123 )].pickInfo = pickInfo;

	StationsRegistry stationsRegistry( statInfos );
	string serialStr = serialize( stationsRegistry );
	CPPUNIT_ASSERT( xmlEqual( serialStr, statRegistryTestStr ) );
}

void TC_StationsRegistry::DeserializeWays()
{
	string serialStr = statRegistryTestStr;
	auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
	CPPUNIT_ASSERT( stationsRegPtr );
	const StationsRegistry & stationsReg = *stationsRegPtr;
	const auto & ecodes = stationsReg.getEsrCodes();
	CPPUNIT_ASSERT( ecodes.size() == 2 );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 45123 ) ) != ecodes.cend() );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 11030 ) ) != ecodes.cend() );

	const auto & statWays1 = stationsReg.getWays( EsrKit( 45123 ) );
	CPPUNIT_ASSERT( statWays1.size() == 3 );
	CPPUNIT_ASSERT( find_if( statWays1.cbegin(), statWays1.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 4; } ) != statWays1.cend() );
	CPPUNIT_ASSERT( find_if( statWays1.cbegin(), statWays1.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 2; } ) != statWays1.cend() );
	CPPUNIT_ASSERT( find_if( statWays1.cbegin(), statWays1.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 7; } ) != statWays1.cend() );
	for( const auto & statWayPtr : statWays1 )
	{
		const auto & sections = statWayPtr->GetSections();
		if ( statWayPtr->Num() == 4 )
		{
			CPPUNIT_ASSERT( sections.size() == 1 );
			const auto & section = sections[0];
			const auto & adProps = section.GetADProperties();
			const auto & weightNorms = adProps.GetWeightNorms();
			const auto & normValues = weightNorms.GetAll();
			CPPUNIT_ASSERT( normValues.size() == 3 );
			CPPUNIT_ASSERT( normValues[0] == WeightNorms::NormValue( 585, StationEsr(12200), 7650 ) );
			CPPUNIT_ASSERT( normValues[1] == WeightNorms::NormValue( 691, StationEsr(37190), 2100 ) );
			CPPUNIT_ASSERT( normValues[2] == WeightNorms::NormValue( 240, StationEsr(23580), 4990 ) );
			CPPUNIT_ASSERT( statWayPtr->Park() == 2 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 37 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 63 );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::PASSENGER ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::EMBARKATION ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::MAJOR ) );
			CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::UNDEF );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::BID_CARGO ) );
		}
		else if ( statWayPtr->Num() == 2 )
		{
			CPPUNIT_ASSERT( sections.size() == 1 );
			const auto & section = sections[0];
			const auto & adProps = section.GetADProperties();
			CPPUNIT_ASSERT( statWayPtr->Park() == 3 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 82 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 21 );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::PASSENGER ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::EMBARKATION ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::MAJOR ) );
			CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::ODD );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::BID_CARGO ) );
		}
		else
		{
			CPPUNIT_ASSERT( statWayPtr->Num() == 7 );
			CPPUNIT_ASSERT( statWayPtr->Park() == 6 );
			CPPUNIT_ASSERT( sections.size() == 2 );
			for ( unsigned int k = 0; k < sections.size(); ++k )
			{
				const auto & section = sections[k];
				const auto & adProps = section.GetADProperties();
				const auto & weightNorms = adProps.GetWeightNorms();
				const auto & normValues = weightNorms.GetAll();
				CPPUNIT_ASSERT( normValues.size() == 1 );
				const auto & wnorm = normValues[0];
				switch( k )
				{
					case 0:
						{
							CPPUNIT_ASSERT( wnorm == WeightNorms::NormValue( 369, StationEsr(9812), 1300 ) );
							CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 55 );
							CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 46 );
							CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::PASSENGER ) );
							CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::EMBARKATION ) );
							CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::MAJOR ) );
							CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::EVEN );
							CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
							CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::BID_CARGO ) );
							break;
						}
					case 1:
						{
							CPPUNIT_ASSERT( wnorm == WeightNorms::NormValue( 515, StationEsr(27300), 5500 ) );
							CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 69 );
							CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 28 );
							CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::PASSENGER ) );
							CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::EMBARKATION ) );
							CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::MAJOR ) );
							CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::UNDEF );
							CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
							CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::BID_CARGO ) );
							break;
						}
				}
			}
		}
	}

	const auto & statWays2 = stationsReg.getWays( EsrKit( 11030 ) );
	CPPUNIT_ASSERT( statWays2.size() == 3 );
	CPPUNIT_ASSERT( find_if( statWays2.cbegin(), statWays2.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 1; } ) != statWays2.cend() );
	CPPUNIT_ASSERT( find_if( statWays2.cbegin(), statWays2.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 3; } ) != statWays2.cend() );
	CPPUNIT_ASSERT( find_if( statWays2.cbegin(), statWays2.cend(), []( StatWayInfoCPtr statWayPtr ){ return statWayPtr->Num() == 2; } ) != statWays2.cend() );
	for( const auto & statWayPtr : statWays2 )
	{
		const auto & sections = statWayPtr->GetSections();
		CPPUNIT_ASSERT( sections.size() == 1 );
		const auto & section = sections[0];
		const auto & adProps = section.GetADProperties();
		if ( statWayPtr->Num() == 1 )
		{
			CPPUNIT_ASSERT( statWayPtr->Park() == 14 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 25 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 74 );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::PASSENGER ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::EMBARKATION ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::MAJOR ) );
			CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::ODD );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::BID_CARGO ) );
		}
		else if ( statWayPtr->Num() == 3 )
		{
			CPPUNIT_ASSERT( statWayPtr->Park() == 4 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 8 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 30 );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::PASSENGER ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::EMBARKATION ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::MAJOR ) );
			CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::EVEN );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::BID_CARGO ) );
		}
		else
		{
			CPPUNIT_ASSERT( statWayPtr->Num() == 2 );
			CPPUNIT_ASSERT( statWayPtr->Park() == 1 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::EVEN ) == 10 );
			CPPUNIT_ASSERT( section.Capacity( Oddness::ODD ) == 0 );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::PASSENGER ) );
			CPPUNIT_ASSERT( !adProps.HaveAttribute( ADAttr::EMBARKATION ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::MAJOR ) );
			CPPUNIT_ASSERT( adProps.GetPassedTrainOddenss() == Oddness::ODD );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::NON_STOP_PASSING ) );
			CPPUNIT_ASSERT( adProps.HaveAttribute( ADAttr::BID_CARGO ) );
		}
	}
}

void TC_StationsRegistry::DeserializeTechTime()
{
	string serialStr = statRegistryTestStr;
	auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
	CPPUNIT_ASSERT( stationsRegPtr );
	const StationsRegistry & stationsReg = *stationsRegPtr;
	const auto & ecodes = stationsReg.getEsrCodes();
	CPPUNIT_ASSERT( ecodes.size() == 2 );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 45123 ) ) != ecodes.cend() );
	CPPUNIT_ASSERT( ecodes.find( EsrKit( 11030 ) ) != ecodes.cend() );

	auto techTimePtr1 = stationsReg.getTechTime( EsrKit( 45123 ) );
	CPPUNIT_ASSERT( techTimePtr1 );
	const auto & timeVals1 = techTimePtr1->GetAll();
	CPPUNIT_ASSERT( timeVals1.size() == 2 );
	auto tvIt1 = find_if( timeVals1.cbegin(), timeVals1.cend(), []( const TechnicalTime::SpecifiedTime & specifiedTime ){
		const auto & specif = specifiedTime.specification;
		return specifiedTime.duration == TechnicalTime::Minutes( 1 ) && specif.oddness == Oddness::ODD && specif.fullness == TechnicalTime::Fullness::Empty &&
			specif.roadFrom == anyRoad && specif.roadTo == anyRoad && specif.trainType == TrainCharacteristics::TrainFeature::NoInfo;
	} );
	CPPUNIT_ASSERT( tvIt1 != timeVals1.cend() );

	tvIt1 = find_if( timeVals1.cbegin(), timeVals1.cend(), []( const TechnicalTime::SpecifiedTime & specifiedTime ){
		const auto & specif = specifiedTime.specification;
		return specifiedTime.duration == TechnicalTime::Minutes( 250 ) && specif.oddness == Oddness::EVEN && specif.fullness == TechnicalTime::Fullness::Any &&
			specif.roadFrom == belorussiaRoad && specif.roadTo == latvianRoad && specif.trainType == TrainCharacteristics::TrainFeature::United;
	} );
	CPPUNIT_ASSERT( tvIt1 != timeVals1.cend() );

	auto techTimePtr2 = stationsReg.getTechTime( EsrKit( 11030 ) );
	CPPUNIT_ASSERT( techTimePtr2 );
	const auto & timeVals2 = techTimePtr2->GetAll();
	CPPUNIT_ASSERT( timeVals2.size() == 3 );
	auto tvIt2 = find_if( timeVals2.cbegin(), timeVals2.cend(), []( const TechnicalTime::SpecifiedTime & specifiedTime ){
		const auto & specif = specifiedTime.specification;
		return specifiedTime.duration == TechnicalTime::Minutes( 2 ) && specif.oddness == Oddness::ODD && specif.fullness == TechnicalTime::Fullness::Any &&
			specif.roadFrom == latvianRoad && specif.roadTo == belorussiaRoad && specif.trainType == TrainCharacteristics::TrainFeature::TechnicalPassenger;
	} );
	CPPUNIT_ASSERT( tvIt2 != timeVals2.cend() );
	tvIt2 = find_if( timeVals2.cbegin(), timeVals2.cend(), []( const TechnicalTime::SpecifiedTime & specifiedTime ){
		const auto & specif = specifiedTime.specification;
		return specifiedTime.duration == TechnicalTime::Minutes( 23 ) && specif.oddness == Oddness::UNDEF && specif.fullness == TechnicalTime::Fullness::Full &&
			specif.roadFrom == belorussiaRoad && specif.roadTo == latvianRoad && specif.trainType == TrainCharacteristics::TrainFeature::Heavy_g8000;
	} );
	tvIt2 = find_if( timeVals2.cbegin(), timeVals2.cend(), []( const TechnicalTime::SpecifiedTime & specifiedTime ){
		const auto & specif = specifiedTime.specification;
		return specifiedTime.duration == TechnicalTime::Minutes( 44 ) && specif.oddness == Oddness::EVEN && specif.fullness == TechnicalTime::Fullness::Empty &&
			specif.roadFrom == anyRoad && specif.roadTo == anyRoad && specif.trainType == TrainCharacteristics::TrainFeature::NoInfo;
	} );
}

void TC_StationsRegistry::DeserializePicketing()
{
	string serialStr = statRegistryTestStr;
	auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
	CPPUNIT_ASSERT( stationsRegPtr );
	const StationsRegistry & stationsReg = *stationsRegPtr;
	
	PicketingInfo tstPickInfo;
	tstPickInfo.SetCoord( rwRuledCoord( L"Moscow", rwCoord( 150, 67 ) ) );
	tstPickInfo.SetCoord( rwRuledCoord( L"Belgrad", rwCoord( 1001, 120 ) ) );
	auto deserPickInfoPtr = stationsReg.getPicketage( EsrKit( 45123 ) );
	CPPUNIT_ASSERT( deserPickInfoPtr && *deserPickInfoPtr == tstPickInfo );
}

void TC_StationsRegistry::DeserializeCommon()
{
	string serialStr = statRegistryTestStr;
	auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
	CPPUNIT_ASSERT( stationsRegPtr );
	const StationsRegistry & stationsReg = *stationsRegPtr;
	CPPUNIT_ASSERT( stationsReg.departureByHemPath( EsrKit( 45123 ) ) );
	CPPUNIT_ASSERT( !stationsReg.departureByHemPath( EsrKit( 11030 ) ) );
}

void TC_StationsRegistry::LengthBetween()
{
	map <EsrKit, StationsRegistry::StationInfo> statInfos;
	PicketingInfo pickInfo1;
	pickInfo1.SetCoord( rwRuledCoord( L"Moscow", rwCoord( 47, 672 ) ) );
	pickInfo1.SetCoord( rwRuledCoord( L"Belgrad", rwCoord( 510, 597 ) ) );
	statInfos[EsrKit( 100 )].pickInfo = pickInfo1;

	PicketingInfo pickInfo2;
	pickInfo2.SetCoord( rwRuledCoord( L"London", rwCoord( 640, 123 ) ) );
	pickInfo2.SetCoord( rwRuledCoord( L"Belgrad", rwCoord( 247, 409 ) ) );
	pickInfo2.SetCoord( rwRuledCoord( L"Berlin", rwCoord( 391, 653 ) ) );
	statInfos[EsrKit( 200 )].pickInfo = pickInfo2;

	PicketingInfo pickInfo3;
	pickInfo3.SetCoord( rwRuledCoord( L"Berlin", rwCoord( 970, 384 ) ) );
	pickInfo3.SetCoord( rwRuledCoord( L"Moscow", rwCoord( 12, 787 ) ) );
	statInfos[EsrKit( 300 )].pickInfo = pickInfo3;

	PicketingInfo pickInfo4;
	pickInfo4.SetCoord( rwRuledCoord( L"London", rwCoord( 410, 784 ) ) );
	pickInfo4.SetCoord( rwRuledCoord( L"Belgrad", rwCoord( 223, 409 ) ) );
	statInfos[EsrKit( 400 )].pickInfo = pickInfo4;

	StationsRegistry statReg( statInfos );
	//100 - 200: 510 * 1000 + 597 - 247 * 1000 + 409 = 510597 - 247409 = 263.188 (Belgrad)
	//200 - 300: 391 * 1000 + 653 - 970 * 1000 + 384 = 391653 - 970384 = 578.731 (Berlin)
	//100 - 300: 47 * 1000 + 672 - 12 * 1000 + 787 = 47672 - 12787 = 34.885 (Moscow)
	auto kmLength12Ptr = statReg.kmBetween( EsrKit( 100 ), EsrKit( 200 ) );
	CPPUNIT_ASSERT( kmLength12Ptr && *kmLength12Ptr == 263.188 );
	auto kmLength23Ptr = statReg.kmBetween( EsrKit( 200 ), EsrKit( 300 ) );
	CPPUNIT_ASSERT( kmLength23Ptr && *kmLength23Ptr == 578.731 );
	auto kmLength13Ptr = statReg.kmBetween( EsrKit( 100 ), EsrKit( 300 ) );
	CPPUNIT_ASSERT( kmLength13Ptr && *kmLength13Ptr == 34.885 );
	CPPUNIT_ASSERT( !statReg.kmBetween( EsrKit( 100 ), EsrKit( 800 ) ) );
	CPPUNIT_ASSERT( !statReg.kmBetween( EsrKit( 450 ), EsrKit( 200 ) ) );
	CPPUNIT_ASSERT( !statReg.kmBetween( EsrKit( 300 ), EsrKit( 400 ) ) );
}

void TC_StationsRegistry::SerializeRoutes()
{
	typedef StationsRegistry::TransrouteDescr TransrouteDescr;
	map <EsrKit, StationsRegistry::StationInfo> statInfos;
	auto & statInfo1 = statInfos[EsrKit( 6520 )];
	statInfo1.routesDefPtr.reset( new StationsRegistry::RoutesDefinition() );
	StatWayVec & wayVec1 = statInfo1.statWays;
	wayVec1.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );

	auto & statInfo2 = statInfos[EsrKit( 6521 )];
	statInfo2.routesDefPtr.reset( new StationsRegistry::RoutesDefinition() );
	auto & transroutes = statInfo2.routesDefPtr->transroutes;
	transroutes.emplace_back( TransrouteDescr( EsrKit( 55000, 25000 ), EsrKit( 65000, 35000 ) ) );
	transroutes.emplace_back( TransrouteDescr( EsrKit( 55001, 25001 ), EsrKit( 65001, 35001 ) ) );
	transroutes.emplace_back( TransrouteDescr( EsrKit( 55002, 25002 ), EsrKit( 65002, 35002 ) ) );
	StatWayVec & wayVec2 = statInfo2.statWays;
	wayVec2.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );

	auto & statInfo3 = statInfos[EsrKit( 6522 )];
	statInfo3.routesDefPtr.reset(); //нет описания маршрутов
	StatWayVec & wayVec3 = statInfo3.statWays;
	wayVec3.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );

	StationsRegistry stationsRegistry( statInfos );
	string serialStr = serialize( stationsRegistry );
	CPPUNIT_ASSERT( xmlEqual( serialStr, statRegistryTestStr2 ) );
}

void TC_StationsRegistry::DeserializeRoutes()
{
	typedef StationsRegistry::TransrouteDescr TransrouteDescr;
	string serialStr = statRegistryTestStr2;
	auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
	CPPUNIT_ASSERT( stationsRegPtr );
	const StationsRegistry & stationsReg = *stationsRegPtr;

	const auto & allCodes = stationsReg.getEsrCodes();
	CPPUNIT_ASSERT( allCodes.size() == 3 );

	bool haveAnyRoutes1 = stationsReg.haveAnyRoutes( EsrKit( 6520 ) );
	const auto & transroutes1 = stationsReg.getTransRoutes( EsrKit( 6520 ) );
	CPPUNIT_ASSERT( haveAnyRoutes1 && transroutes1.empty() );

	bool haveAnyRoutes2 = stationsReg.haveAnyRoutes( EsrKit( 6521 ) );
	const auto & transroutes2 = stationsReg.getTransRoutes( EsrKit( 6521 ) );
	CPPUNIT_ASSERT( haveAnyRoutes2 && transroutes2.size() == 3 );
	CPPUNIT_ASSERT( find( transroutes2.cbegin(), transroutes2.cend(), TransrouteDescr( EsrKit( 55000, 25000 ), EsrKit( 65000, 35000 ) ) ) != transroutes2.cend() );
	CPPUNIT_ASSERT( find( transroutes2.cbegin(), transroutes2.cend(), TransrouteDescr( EsrKit( 55001, 25001 ), EsrKit( 65001, 35001 ) ) ) != transroutes2.cend() );
	CPPUNIT_ASSERT( find( transroutes2.cbegin(), transroutes2.cend(), TransrouteDescr( EsrKit( 55002, 25002 ), EsrKit( 65002, 35002 ) ) ) != transroutes2.cend() );

	bool haveAnyRoutes3 = stationsReg.haveAnyRoutes( EsrKit( 6523 ) );
	const auto & transroutes3 = stationsReg.getTransRoutes( EsrKit( 6523 ) );
	CPPUNIT_ASSERT( !haveAnyRoutes3 && transroutes3.empty() );
}

void TC_StationsRegistry::TransroutePostprocess()
{
	typedef StationsRegistry::TransrouteDescr TransrouteDescr;
	map <EsrKit, StationsRegistry::StationInfo> statInfos;

	auto & statInfo = statInfos[EsrKit( 6521 )];
	statInfo.routesDefPtr.reset( new StationsRegistry::RoutesDefinition() );
	auto & transroutes = statInfo.routesDefPtr->transroutes;
	transroutes.emplace_back( TransrouteDescr( EsrKit( 1002, 1003 ), EsrKit( 4002, 4003 ) ) );
	transroutes.emplace_back( TransrouteDescr( EsrKit( 1004 ), EsrKit( 4004, 4005 ) ) );
	transroutes.emplace_back( TransrouteDescr( EsrKit( 1005, 1006 ), EsrKit( 4006 ) ) );
	transroutes.emplace_back( TransrouteDescr( EsrKit( 1007 ), EsrKit( 4007 ) ) );
	StatWayVec & wayVec = statInfo.statWays;
	wayVec.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );

	StationsRegistry stationsRegistry( statInfos );

	const auto & spannedRoutes = stationsRegistry.getSpannedTransRoutes( EsrKit( 6521 ) );
	CPPUNIT_ASSERT( spannedRoutes.size() == 4 );
	CPPUNIT_ASSERT( any_of( spannedRoutes.cbegin(), spannedRoutes.cend(), []( const TransrouteDescr & trtDescr ){
		return trtDescr.from == EsrKit( 1002, 1003 ) && trtDescr.to == EsrKit( 4002, 4003 );
	} ) );

	CPPUNIT_ASSERT( any_of( spannedRoutes.cbegin(), spannedRoutes.cend(), []( const TransrouteDescr & trtDescr ){
		return trtDescr.from == EsrKit( 1004, 6521 ) && trtDescr.to == EsrKit( 4004, 4005 );
	} ) );

	CPPUNIT_ASSERT( any_of( spannedRoutes.cbegin(), spannedRoutes.cend(), []( const TransrouteDescr & trtDescr ){
		return trtDescr.from == EsrKit( 1005, 1006 ) && trtDescr.to == EsrKit( 4006, 6521 );
	} ) );

	CPPUNIT_ASSERT( any_of( spannedRoutes.cbegin(), spannedRoutes.cend(), []( const TransrouteDescr & trtDescr ){
		return trtDescr.from == EsrKit( 1007, 6521 ) && trtDescr.to == EsrKit( 4007, 6521 );
	} ) );
}

const std::wstring wsBypass =
    L"<StationsRegistry>"
    //  ..........
    L"    <Station EsrCode='09100'>"
    L"        <StatWayInfo wayNum='1' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='9~100' picketing1_comm='Рига' />"
    L"    </Station>"
    //  ..........
    L"    <Station EsrCode='09104'>"
    L"        <StatWayInfo wayNum='2' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='9~104' picketing1_comm='Рига' />"
    L"        <BypassDefinition>"
    L"            <Bypass wayNum='5' aggregate='09100:09150' />"
    L"        </BypassDefinition>"
    L"    </Station>"
    //  ..........
    L"    <Station EsrCode='09150'>"
    L"        <StatWayInfo wayNum='1' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='9~150' picketing1_comm='Рига' />"
    L"    </Station>"
    //  :-:-:-:-:-:-:-:-:-:
    L"    <Station EsrCode='09333'>"
    L"        <StatWayInfo wayNum='1' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='9~333' picketing1_comm='Запад' />"
    L"    </Station>"
    //  :-:-:-:-:-:-:-:-:-:
    L"    <Station EsrCode='09555'>"
    L"        <StatWayInfo wayNum='1' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='209~555' picketing1_comm='Восток' "
    L"                       picketing2_val='9~555'   picketing2_comm='Запад' />"
    L"        <BypassDefinition>"
    L"            <Bypass wayNum='3' aggregate='09333:09777' />"
    L"            <Bypass wayNum='7' aggregate='09333:09777' />"
    L"        </BypassDefinition>"
    L"    </Station>"
    //  :-:-:-:-:-:-:-:-:-:
    L"    <Station EsrCode='09777'>"
    L"        <StatWayInfo wayNum='1' parkNum='1'/>"
    L"        <PicketingInfo picketing1_val='9~777' picketing1_comm='Запад' />"
    L"    </Station>"
    L"</StationsRegistry>";

void TC_StationsRegistry::SerializeBypass()
{
    map <EsrKit, StationsRegistry::StationInfo> statInfos;
    // ..........
    {
        auto & s = statInfos[ EsrKit( 9100 ) ];
        s.statWays.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Рига", "9~100") );
    }
    {
        auto & s = statInfos[ EsrKit( 9104 ) ];
        s.statWays.emplace_back( new StatWayInfo( 2, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Рига", "9~104") );
        s.bypassDef.append( EsrKit(9100,9150 ), 5 );
    }
    {
        auto & s = statInfos[ EsrKit( 9150 ) ];
        s.statWays.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Рига", "9~150") );
    }
    // :-:-:-:-:-:-:-:-:-:
    {
        auto & s = statInfos[ EsrKit( 9333 ) ];
        s.statWays.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Запад", "9~333") );
    }
    {
        auto & s = statInfos[ EsrKit( 9555 ) ];
        s.statWays.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Запад", "9~555") );
        s.pickInfo.SetCoord( rwRuledCoord( L"Восток", "209~555") );
        s.bypassDef.append( EsrKit(9333,9777 ), 3 );
        s.bypassDef.append( EsrKit(9333,9777 ), 7 );
    }
    {
        auto & s = statInfos[ EsrKit( 9777 ) ];
        s.statWays.emplace_back( new StatWayInfo( 1, 1, vector<StatWayInfo::Section>() ) );
        s.pickInfo.SetCoord( rwRuledCoord( L"Запад", "9~777") );
    }

    StationsRegistry stationsRegistry( statInfos );
    string serialStr = serialize( stationsRegistry );
    CPPUNIT_ASSERT( xmlEqual( serialStr, ToUtf8(wsBypass) ) );
}

void TC_StationsRegistry::DeserializeBypass()
{
    string serialStr = ToUtf8(wsBypass);
    auto stationsRegPtr = deserialize<StationsRegistry>( serialStr );
    CPPUNIT_ASSERT( stationsRegPtr );
    const Bypass& sbp = stationsRegPtr->getBypass();
    {
        auto terminals = sbp.getTerminals();
        CPPUNIT_ASSERT_EQUAL( size_t(2), terminals.size());
        CPPUNIT_ASSERT( EsrKit(9104) == terminals.front() );
        CPPUNIT_ASSERT( EsrKit(9555) == terminals.back() );
    }
    {
        const EsrKit term( 9104 );
        EsrKit aggr = sbp.getAggregate( term );
        CPPUNIT_ASSERT( EsrKit( 9100, 9150 ) == aggr );
        auto ways = sbp.getWays( aggr  );
        CPPUNIT_ASSERT_EQUAL( size_t(1), ways.size() );
        CPPUNIT_ASSERT_EQUAL( 5u, ways.front() );
        // проверка конвертации
        const EsrKit ab( 9100, 9104 );
        const EsrKit bc( 9104, 9150 );
        const EsrKit AC( 9100, 9150 );
        CPPUNIT_ASSERT( AC == sbp.toGeneral( ab, 5 ) ); // 5 путь объездной
        CPPUNIT_ASSERT( ab == sbp.toGeneral( ab, 2 ) ); // 2 путь НЕ объездной
        CPPUNIT_ASSERT( ab == sbp.toGeneral( ab, 3 ) ); // 3 путь НЕ существует

        CPPUNIT_ASSERT( AC == sbp.toGeneral( bc, 5 ) ); // 5 путь объездной
        CPPUNIT_ASSERT( bc == sbp.toGeneral( bc, 2 ) ); // 2 путь НЕ объездной
        CPPUNIT_ASSERT( bc == sbp.toGeneral( bc, 3 ) ); // 3 путь не существует
    }
    {
        const EsrKit bigterm( 9555 );
        EsrKit aggr = sbp.getAggregate( bigterm );
        CPPUNIT_ASSERT( EsrKit( 9333, 9777 ) == aggr );
        auto ways37 = sbp.getWays( aggr );
        CPPUNIT_ASSERT_EQUAL( size_t(2), ways37.size() );
        CPPUNIT_ASSERT_EQUAL( 3u, ways37.front() );
        CPPUNIT_ASSERT_EQUAL( 7u, ways37.back() );
        // негативный тест
        CPPUNIT_ASSERT( sbp.getAggregate( EsrKit( 11111 )  ).empty() );
        CPPUNIT_ASSERT( sbp.getWays( EsrKit(22222,33333)  ).empty() );
        // проверка конвертации
        const EsrKit s35( 9333, 9555 );
        const EsrKit s57( 9555, 9777 );
        const EsrKit gen37( 9333, 9777 );
        // translate
        CPPUNIT_ASSERT( gen37 == sbp.toGeneral( s35, 3 ) );
        CPPUNIT_ASSERT( gen37 == sbp.toGeneral( s57, 7 ) );
    }
}


void TC_StationsRegistry::XtransBypass()
{
    const std::wstring stat_XtransBypass =
        L"<StationsRegistry>"
        L"  <!-- Tornakalns -->"
        L"  <Station EsrCode='09100'>"
        L"    <StatWayInfo wayNum='1' parkNum='1'>"
        L"      <section even_capacity='55' odd_capacity='55' psng='Y' embark='Y' maj='Y' tpassOddness='2' nonStop='' bidCargo='' />"
        L"    </StatWayInfo>"
        L"    <PicketingInfo picketing1_val='2~700' picketing1_comm='Рига' picketing2_val='2~700' picketing2_comm='Рига-Тукумс' picketing3_val='0~1' picketing3_comm='Торнякалнс' picketing4_val='302~700' picketing4_comm='Торнянкалнс-Елгава' />"
        L"    <RoutesDefinition />"
        L"  </Station>"
        L"  <!--Блок-пост 8 км с объездом по 1 пути-->"
        L"  <Station EsrCode='09104'>"
        L"    <PicketingInfo picketing1_val='7~200' picketing1_comm='Рига' picketing2_val='207~200' picketing2_comm='Торнянкалнс-Елгава' />"
        L"    <BypassDefinition>"
        L"      <Bypass wayNum='1' aggregate='09100:09150' />"
        L"    </BypassDefinition>"
        L"  </Station>"
        L"  <!-- Olaine -->"
        L"  <Station EsrCode='09150'>"
        L"    <StatWayInfo wayNum='1' parkNum='1'>"
        L"      <section even_capacity='47' odd_capacity='47' psng='' embark='' maj='' tpassOddness='1' nonStop='' bidCargo='' />"
        L"    </StatWayInfo>"
        L"    <PicketingInfo picketing1_val='22~' picketing1_comm='Рига' picketing2_val='122~' picketing2_comm='Торнянкалнс-Елгава' />"
        L"    <RoutesDefinition />"
        L"  </Station>"
        L"</StationsRegistry>";

    auto stationsRegPtr = deserialize<StationsRegistry>( ToUtf8(stat_XtransBypass) );
    CPPUNIT_ASSERT( stationsRegPtr );
    const Bypass& bp = stationsRegPtr->getBypass();

    const EsrKit Cp8( 9104 );
    const EsrKit Tor( 9100 );
    const EsrKit Ola( 9150 );
    const EsrKit Tor_Cp8( 9100, 9104 );
    const EsrKit     Cp8_Ola(   9104, 9150 );
    const EsrKit Tor_Ola( 9100,       9150 );
    PicketingInfo R_Tor_8;
    R_Tor_8.SetCoord( rwRuledCoord( L"Рига", rwCoord( "4~400") ) );
    PicketingInfo R_8_Ola;
    R_8_Ola.SetCoord( rwRuledCoord( L"Рига", rwCoord("19~190") ) );
    PicketingInfo TE_Tor_8;
    TE_Tor_8.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord("277~") ) );
    PicketingInfo TE_8_Ola;
    TE_8_Ola.SetCoord( rwRuledCoord( L"Торнянкалнс-Елгава", rwCoord("155~") ) );
    // объездной путь - транслируется в конкретные перегоны
    CPPUNIT_ASSERT( Tor_Ola == bp.toGeneral( Tor_Cp8,     1 ) );
    CPPUNIT_ASSERT( Tor_Ola == bp.toGeneral(     Cp8_Ola, 1 ) );
    CPPUNIT_ASSERT( Tor_Cp8     == bp.toPartial( Tor_Ola, 1, R_Tor_8 ) );
    CPPUNIT_ASSERT( Tor_Cp8     == bp.toPartial( Tor_Ola, 1, TE_Tor_8 ) );
    CPPUNIT_ASSERT(     Cp8_Ola == bp.toPartial( Tor_Ola, 1, R_8_Ola ) );
    CPPUNIT_ASSERT(     Cp8_Ola == bp.toPartial( Tor_Ola, 1, TE_8_Ola ) );
    // НЕ объездной путь - не транслируется (без изменений)
    CPPUNIT_ASSERT( Tor_Ola != bp.toGeneral( Tor_Cp8,     3 ) );
    CPPUNIT_ASSERT( Tor_Ola != bp.toGeneral(     Cp8_Ola, 3 ) );
    CPPUNIT_ASSERT( Tor_Ola == bp.toPartial( Tor_Ola,     3, R_Tor_8 ) );
    CPPUNIT_ASSERT( Tor_Ola == bp.toPartial( Tor_Ola,     3, TE_Tor_8 ) );
    CPPUNIT_ASSERT( Tor_Ola == bp.toPartial( Tor_Ola,     3, R_8_Ola ) );
    CPPUNIT_ASSERT( Tor_Ola == bp.toPartial( Tor_Ola,     3, TE_8_Ola ) );
    // поиск пропущенной станции (из-за объезда)
    CPPUNIT_ASSERT( Cp8 == bp.missingTerminal( Tor,         Ola ) );
    CPPUNIT_ASSERT( Cp8 == bp.missingTerminal( Tor,     Cp8_Ola ) );
    CPPUNIT_ASSERT( Cp8 == bp.missingTerminal( Tor_Cp8,     Ola ) );
    CPPUNIT_ASSERT( Cp8 == bp.missingTerminal( Tor_Cp8, Cp8_Ola ) );
    // проверка обнаружения всех кодов, имеющих отношение к объезду
    CPPUNIT_ASSERT( bp.related( Tor ) );
    CPPUNIT_ASSERT( bp.related( Tor_Cp8 ) );
    CPPUNIT_ASSERT( bp.related( Cp8 ) );
    CPPUNIT_ASSERT( bp.related( Cp8_Ola ) );
    CPPUNIT_ASSERT( bp.related( Ola ) );
    CPPUNIT_ASSERT( bp.related( Tor_Ola ) );
}
