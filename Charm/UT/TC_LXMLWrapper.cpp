#include "stdafx.h"
#include "TC_LXMLWrapper.h"
#include "../helpful/Informator.h"
#include "LogicDocumentTest.h"
#include "../helpful/EsrKit.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LObjectManager.h"
#include "LXMLWrapperTest.h"
#include "../StatBuilder/LogicElement.h"
#include "../StatBuilder/LLoadResult.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicSemaleg.h"
#include "../StatBuilder/LogicJoint.h"
#include "../StatBuilder/LogicLandscape.h"
#include "../StatBuilder/LogicCommand.h"
#include "../StatBuilder/ObsoleteLoader.h"
#include "../StatBuilder/StationManager.h"
#include "../StatBuilder/RegionDescription.h"
#include "../helpful/ADProperties.h"
#include "../helpful/TechnicalTime.h"
#include "../helpful/RoadCode.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LXMLWrapper );

using namespace std;
using namespace Landscape;

typedef ADProperties::Attrubute ADAttr;

static RoadKit belorussiaRoad( RoadCode::belorussia );
static RoadKit latvianRoad( RoadCode::latvianNationalCode );
static RoadKit anyRoad;

void TC_LXMLWrapper::setUp()
{
    TC_Logic::setUp_helper();
}

void TC_LXMLWrapper::tearDown()
{
    TC_Logic::tearDown_helper();
}

void TC_LXMLWrapper::SaveStationCommon()
{
	//подготовка технического времени
	TechnicalTime techTime;
	TechnicalTime::SpecifiedTime specTimeVal;
	specTimeVal.duration = TechnicalTime::Minutes( 135 );
	specTimeVal.specification = TechnicalTime::Specification( Oddness::EVEN, TechnicalTime::Fullness::Full, belorussiaRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::FastAllYear );
	techTime.AddTime( specTimeVal );
	specTimeVal.duration = TechnicalTime::Minutes( 88 );
	specTimeVal.specification = TechnicalTime::Specification( Oddness::UNDEF, TechnicalTime::Fullness::Any, latvianRoad, latvianRoad,
		TrainCharacteristics::TrainFeature::Rapid );
	techTime.AddTime( specTimeVal );

	StationDescription sdescr( L"Test name", EsrKit( 12456 ), PicketingInfo(), StationDescription::Options( true, true, true ), techTime );
	pstam->SetDescription( sdescr, logdoc.get() );
	LLoadResult LR( L"" );
	LXMLWrapperTest lwrapper( logdoc.get() );

	attic::a_document * xdoc = logdoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	CPPUNIT_ASSERT( root_node.attribute( "name" ).as_wstring() == L"Test name" );
	CPPUNIT_ASSERT( root_node.attribute( "ESR_code" ).as_string() == EsrKit( 12456 ).to_string() );
	CPPUNIT_ASSERT( root_node.attribute( "transroutes" ).as_bool() == true );
	CPPUNIT_ASSERT( root_node.attribute( "hintlaunched" ).as_bool() == true );
	CPPUNIT_ASSERT( root_node.attribute( "departbyhpath" ).as_bool() == true );

	//проверка технического времени
	auto techTimeNode = root_node.child( "TECHNICAL_TIME" );
	CPPUNIT_ASSERT( techTimeNode );
	auto timeValueNode = techTimeNode.child( "value" );
	CPPUNIT_ASSERT( timeValueNode );
	CPPUNIT_ASSERT( timeValueNode.attribute( "minutes" ).as_uint() == 135 );
	CPPUNIT_ASSERT( timeValueNode.attribute( "oddness" ).as_string() == string( "even" ) );
	CPPUNIT_ASSERT( timeValueNode.attribute( "netto" ).as_string() == string( "full" ) );

	auto fromRoadNode = timeValueNode.child( "from_road" );
	auto nationalNode = fromRoadNode.child( "National" );
	auto codeValueAttr = nationalNode.attribute( "value" );
	CPPUNIT_ASSERT( codeValueAttr && codeValueAttr.as_uint() == 21 );
	auto toRoadNode = timeValueNode.child( "to_road" );
	nationalNode = toRoadNode.child( "National" );
	codeValueAttr = nationalNode.attribute( "value" );
	CPPUNIT_ASSERT( codeValueAttr && codeValueAttr.as_uint() == 25 );
	const TrainCharacteristics & tcharacts = TrainCharacteristics::instanceCRef();
	auto featInfo = tcharacts.GetTrainFeatureInfo( TrainCharacteristics::TrainFeature::FastAllYear );
	CPPUNIT_ASSERT( timeValueNode.attribute( "train_type" ).as_string() == featInfo.xml_name );
	timeValueNode = timeValueNode.next_sibling( "value" );
	CPPUNIT_ASSERT( timeValueNode.attribute( "minutes" ).as_uint() == 88 );
	CPPUNIT_ASSERT( timeValueNode.attribute( "oddness" ).as_string() == string( "undef" ) );
	CPPUNIT_ASSERT( timeValueNode.attribute( "netto" ).as_string() == string( "any" ) );
	fromRoadNode = timeValueNode.child( "from_road" );
	nationalNode = fromRoadNode.child( "National" );
	codeValueAttr = nationalNode.attribute( "value" );
	CPPUNIT_ASSERT( codeValueAttr && codeValueAttr.as_uint() == 25 );
	toRoadNode = timeValueNode.child( "to_road" );
	nationalNode = toRoadNode.child( "National" );
	codeValueAttr = nationalNode.attribute( "value" );
	CPPUNIT_ASSERT( codeValueAttr && codeValueAttr.as_uint() == 25 );
	featInfo = tcharacts.GetTrainFeatureInfo( TrainCharacteristics::TrainFeature::Rapid );
	CPPUNIT_ASSERT( timeValueNode.attribute( "train_type" ).as_string() == featInfo.xml_name );
}

void TC_LXMLWrapper::SaveSpanCommon()
{
	OddnessPtr oddnessPtr( new Oddness( Oddness::UNDEF ) );
	SpanDescription sdescr( L"Test name", EsrKit( 1200, 5500 ), PicketingInfo(), WaysLocks( map<unsigned int, SpanLockType>() ), oddnessPtr, 6 );
	pstam->SetDescription( sdescr, logdoc.get() );
	LLoadResult LR( L"" );
	LXMLWrapperTest lwrapper( logdoc.get() );

	attic::a_document * xdoc = logdoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	CPPUNIT_ASSERT( root_node.attribute( "name" ).as_wstring() == L"Test name" );
	CPPUNIT_ASSERT( root_node.attribute( "ESR_code" ).as_string() == EsrKit( 1200, 5500 ).to_string() );
	CPPUNIT_ASSERT( root_node.attribute( "incoddness" ).as_string() == string( "undef" ) );
	CPPUNIT_ASSERT( root_node.attribute( "crossesnum" ).as_uint() == 6 );
}

void TC_LXMLWrapper::LoadStationCommon()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	root_node.attribute( "name" ).set_value( L"Test name" );
	root_node.attribute( "ESR_code" ).set_value( L"52140" );
	root_node.attribute( "transroutes" ).set_value( true );
	root_node.ensure_attribute( "hintlaunched" ).set_value( true );
	root_node.attribute( "departbyhpath" ).set_value( true );
	auto techTimeNode = root_node.append_child( "TECHNICAL_TIME" );
	auto timeValueNode = techTimeNode.append_child( "value" );
	timeValueNode.append_attribute( "minutes" ).set_value( 95 );
	timeValueNode.append_attribute( "oddness" ).set_value( "odd" );
	timeValueNode.append_attribute( "netto" ).set_value( "empty" );
	auto fromRoadNode = timeValueNode.append_child( "from_road" );
	auto nationalNode = fromRoadNode.append_child( "National" );
	nationalNode.append_attribute( "value" ).set_value( 25 );
	auto toRoadNode = timeValueNode.append_child( "to_road" );
	nationalNode = toRoadNode.append_child( "National" );
	nationalNode.append_attribute( "value" ).set_value( 21 );

	timeValueNode.append_attribute( "train_type" ).set_value( "hrapid" );
	timeValueNode = techTimeNode.append_child( "value" );
	timeValueNode.append_attribute( "minutes" ).set_value( 231 );
	timeValueNode.append_attribute( "oddness" ).set_value( "even" );
	timeValueNode.append_attribute( "netto" ).set_value( "full" );
	timeValueNode.append_child( "from_road" );
	timeValueNode.append_child( "to_road" );

	timeValueNode.append_attribute( "train_type" ).set_value( "noinfo" );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	CStationManager::SDESKS sdescrs;
	auto pstam = logdoc->GetStationManager();
	pstam->GetAllDescriptions( sdescrs );
	CPPUNIT_ASSERT( sdescrs.size() == 1 );
	auto sdescr = sdescrs.front().second;
	CPPUNIT_ASSERT( sdescr->name == L"Test name" );
	CPPUNIT_ASSERT( sdescr->esrCode == EsrKit( 52140 ) );
	auto statDescrPtr = pstam->GetDescription<StationDescription>( sdescr->esrCode );
	CPPUNIT_ASSERT( statDescrPtr->options.transroutesAble );
	CPPUNIT_ASSERT( statDescrPtr->options.hintLaunchedRoutes );
	CPPUNIT_ASSERT( statDescrPtr->options.departureByHemPath );
	const auto & techTime = statDescrPtr->techTime;
	const auto & timeVals = techTime.GetAll();
	CPPUNIT_ASSERT( timeVals.size() == 2 );
	CPPUNIT_ASSERT( timeVals[0].duration.count() == 95 );
	CPPUNIT_ASSERT( timeVals[0].specification.oddness == Oddness::ODD );
	CPPUNIT_ASSERT( timeVals[0].specification.fullness == TechnicalTime::Fullness::Empty );
	CPPUNIT_ASSERT( timeVals[0].specification.roadFrom == latvianRoad && timeVals[0].specification.roadTo == belorussiaRoad );
	CPPUNIT_ASSERT( timeVals[0].specification.trainType == TrainCharacteristics::TrainFeature::Highrapid );
	CPPUNIT_ASSERT( timeVals[1].duration.count() == 231 );
	CPPUNIT_ASSERT( timeVals[1].specification.oddness == Oddness::EVEN );
	CPPUNIT_ASSERT( timeVals[1].specification.fullness == TechnicalTime::Fullness::Full );
	CPPUNIT_ASSERT( timeVals[1].specification.roadFrom == anyRoad && timeVals[1].specification.roadTo == anyRoad );
	CPPUNIT_ASSERT( timeVals[1].specification.trainType == TrainCharacteristics::TrainFeature::NoInfo );
}

void TC_LXMLWrapper::LoadSpanCommon()
{
	//документ должен получить перегонный код
	OddnessPtr oddnessPtr( new Oddness( Oddness::UNDEF ) );
	SpanDescription sdescr( L"Test name", EsrKit( 1200, 5500 ), PicketingInfo(), WaysLocks( map<unsigned int, SpanLockType>() ), oddnessPtr, 0 );
	pstam->SetDescription( sdescr, logdoc.get() );
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L""); //процессс сохранения создает внутренний xml-документ
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	root_node.attribute( "name" ).set_value( L"Test span name" );
	root_node.attribute( "ESR_code" ).set_value( L"10200:21000" );
	root_node.attribute( "incoddness" ).set_value( L"even" );
	root_node.attribute( "crossesnum" ).set_value( 1 );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	auto pstam = logdoc->GetStationManager();
	auto sdescrPtr = pstam->GetDescription<SpanDescription>( logdoc.get() );
	CPPUNIT_ASSERT( sdescrPtr );
	CPPUNIT_ASSERT( sdescrPtr->name == L"Test span name" );
	CPPUNIT_ASSERT( sdescrPtr->esrCode == EsrKit( 10200, 21000 ) );
	CPPUNIT_ASSERT( sdescrPtr->incOddnessPtr && *sdescrPtr->incOddnessPtr == Oddness::EVEN );
	CPPUNIT_ASSERT( sdescrPtr->crossesNum == 1 );
}

void TC_LXMLWrapper::SaveLoadObjects()
{
	pstam->SetESR( EsrKit( 100 ), logdoc.get() );
	LLoadResult LR( L"" );
	list <ELEMENT_CLASS> logic_classes;
	CInformator::Instance()->GetLogicClasses( logic_classes );
	LXMLWrapperTest lwrapper( logdoc.get() );
	for( ELEMENT_CLASS lclass : logic_classes )
	{
		if ( lclass == HEAD )  //головы загружаются как подобъекты мачт
			continue;
		LogElemPtr lelem_prototype( CreateUncommonObject( lclass ) );
		CLogicElement * lelem = lelem_prototype->Clone();
		plom->AddObject( lelem, logdoc.get() );
		lwrapper.Save( L"" );
		plom->RemoveObject( lelem, logdoc.get() );
		set <const CLogicElement *> classed_objects;
		plom->GetObjects( lclass, logdoc.get(), classed_objects );
		CPPUNIT_ASSERT( classed_objects.empty() );
		lwrapper.Load( L"", LR );
		plom->GetObjects( lclass, logdoc.get(), classed_objects );
		CPPUNIT_ASSERT( classed_objects.size() == 1 );
		CPPUNIT_ASSERT( ( *classed_objects.begin() )->Equal( lelem_prototype.get() ) );

		//зачистка
		set <const CLogicElement *> allobjects;
		plom->GetObjects( ALL_CLASSES, logdoc.get(), allobjects );
		for( const CLogicElement * le : allobjects )
			plom->RemoveObject( le, logdoc.get() );
		plom->GetObjects( ALL_CLASSES, logdoc.get(), allobjects );
		CPPUNIT_ASSERT( allobjects.empty() );
	}
}

void TC_LXMLWrapper::SaveStrip()
{
	auto lelem = BuildObject( STRIP );
	auto lstrip = static_cast <CLogicStrip *>( lelem );
	lstrip->SetName( L"Strip_name" );
	lstrip->SetType( APPROACHING );
	vector <CLink> links;
	EsrKit ecode( 200 );
	links.push_back( CLink( JOINT, BadgeE( L"Test_link", ecode ) ) );
	lstrip->SetLinks( links );
	lstrip->SetWaynum( 3 );
	lstrip->SetCapacity( Oddness::ODD, L"49" );
	lstrip->SetCapacity( Oddness::EVEN, L"38" );
	lstrip->SetLength( 70 );

    ADProperties::AttrSet adAttributes;
	adAttributes.set( ADAttr::PASSENGER );
	adAttributes.set( ADAttr::MAJOR );
	adAttributes.set( ADAttr::EMBARKATION );
	WeightNorms weightNorms;
	weightNorms.AddNorm( WeightNorms::NormValue( 604, StationEsr(12087), 1450 ) );
	weightNorms.AddNorm( WeightNorms::NormValue( 714, StationEsr(9520), 7190 ) );
	lstrip->SetADProperties( ADProperties( adAttributes, Oddness::ODD, weightNorms ) ); //с весовыми нормами
	lstrip->SetDirectionConflict( true );
	lstrip->SetParknum( 4 );

	auto lelem2 = BuildObject( STRIP );
	auto lstrip2 = static_cast <CLogicStrip *>( lelem2 );
	lstrip2->SetName( L"Strip_name2" );
	lstrip2->SetADProperties( ADProperties( adAttributes, Oddness::ODD, WeightNorms() ) ); //без весовых норм

	//сохраняем
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	auto strips_node = root_node.child( "STRIPS" );
	CPPUNIT_ASSERT( strips_node );

	//первый участок (с весовыми нормами)
	auto elem_node = strips_node.child( "element" );
	CPPUNIT_ASSERT( elem_node );
	CPPUNIT_ASSERT( elem_node.attribute( "name" ).as_wstring() == L"Strip_name" );
	CPPUNIT_ASSERT( elem_node.attribute( "type" ).as_uint() == APPROACHING );
	auto link_node = elem_node.child( "link" );
	CPPUNIT_ASSERT( link_node );
	CPPUNIT_ASSERT( link_node.attribute( "name" ).as_wstring() == L"Test_link" );
	CPPUNIT_ASSERT( link_node.attribute( "class" ).as_uint() == JOINT );
	CPPUNIT_ASSERT( link_node.attribute( "ESR_code" ).as_string() == ecode.to_string() );
	CPPUNIT_ASSERT( elem_node.attribute( "waynum" ).as_uint() == 3 );
	CPPUNIT_ASSERT( elem_node.attribute( "odd_capacity" ).as_uint() == 49 );
	CPPUNIT_ASSERT( elem_node.attribute( "even_capacity" ).as_uint() == 38 );
	CPPUNIT_ASSERT( elem_node.attribute( "length" ).as_uint() == 70 );
	CPPUNIT_ASSERT( elem_node.attribute( "passenger" ).as_bool() == true );
	CPPUNIT_ASSERT( elem_node.attribute( "embarkation" ).as_bool() == true );
	CPPUNIT_ASSERT( elem_node.attribute( "major" ).as_bool() == true );
	CPPUNIT_ASSERT( elem_node.attribute( "dirconflict" ).as_bool() == true );
	CPPUNIT_ASSERT( elem_node.attribute( "parknum" ).as_uint() == 4 );
	CPPUNIT_ASSERT( elem_node.attribute( "train_pass_oddness" ).as_uint() == 1 );
	auto weight_norms_node = elem_node.child( "weight_norms" );
	CPPUNIT_ASSERT( weight_norms_node );
	auto wvalue_node = weight_norms_node.child( "value" );
	CPPUNIT_ASSERT( wvalue_node.attribute( "locoseries" ).as_uint() == 604 );
	CPPUNIT_ASSERT( wvalue_node.attribute( "target" ).as_uint() == 12087 );
	CPPUNIT_ASSERT( wvalue_node.attribute( "maxweight" ).as_uint() == 1450 );
	wvalue_node = wvalue_node.next_sibling( "value" );
	CPPUNIT_ASSERT( wvalue_node.attribute( "locoseries" ).as_uint() == 714 );
	CPPUNIT_ASSERT( wvalue_node.attribute( "target" ).as_uint() == 9520 );
	CPPUNIT_ASSERT( wvalue_node.attribute( "maxweight" ).as_uint() == 7190 );
	CPPUNIT_ASSERT( !wvalue_node.next_sibling( "value" ) );

	//второй участок (без весовых норм)
	elem_node = elem_node.next_sibling( "element" );
	CPPUNIT_ASSERT( !elem_node.child( "weight_norms" ) );
}

void TC_LXMLWrapper::LoadStrip()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	auto strips_node = root_node.child( "STRIPS" );
	auto elem_node = strips_node.append_child( "element" );

	elem_node.append_attribute( "name" ).set_value( L"Strip_name" );
	elem_node.append_attribute( "type" ).set_value( APPROACHING );
	auto link_node = elem_node.append_child( "link" );
	link_node.append_attribute( "name" ).set_value( L"Test_link" );
	link_node.append_attribute( "class" ).set_value( JOINT );
	link_node.append_attribute( "ESR_code" ).set_value( "200" );
	elem_node.append_attribute( "waynum" ).set_value( 3 );
	elem_node.append_attribute( "odd_capacity" ).set_value( 58 );
	elem_node.append_attribute( "even_capacity" ).set_value( 44 );
	elem_node.append_attribute( "length" ).set_value( 70 );
	elem_node.append_attribute( "passenger" ).set_value( true );
	elem_node.append_attribute( "embarkation" ).set_value( true );
    elem_node.append_attribute( "major" ).set_value( true );
	elem_node.append_attribute( "dirconflict" ).set_value( true );
	elem_node.append_attribute( "parknum" ).set_value( 4 );
	elem_node.append_attribute( "train_pass_oddness" ).set_value( 2 );
	auto weight_norms_node = elem_node.append_child( "weight_norms" );
	auto value_node = weight_norms_node.append_child( "value" );
	value_node.append_attribute( "locoseries" ).set_value( 928 );
	value_node.append_attribute( "target" ).set_value( 6530 );
	value_node.append_attribute( "maxweight" ).set_value( 14230 );
	value_node = weight_norms_node.append_child( "value" );
	value_node.append_attribute( "locoseries" ).set_value( 585 );
	value_node.append_attribute( "target" ).set_value( 45900 );
	value_node.append_attribute( "maxweight" ).set_value( 500 );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	set <const CLogicElement *> lstrips;
	plom->GetObjects( STRIP, logdoc.get(), lstrips );
	CPPUNIT_ASSERT( lstrips.size() == 1 );
	auto lelem = *lstrips.begin();
	const CLogicStrip * lstrip = static_cast <const CLogicStrip *>( lelem );
	CPPUNIT_ASSERT( lstrip->GetName() == L"Strip_name" );
	CPPUNIT_ASSERT( lstrip->GetType() == APPROACHING );
	const auto & links = lstrip->GetLinks();
	CPPUNIT_ASSERT( links.size() == 1 );
	auto link = links.back();
	CPPUNIT_ASSERT( link.obj_badge == BadgeE( L"Test_link", EsrKit( 200 ) ) );
	CPPUNIT_ASSERT( link.object_class == JOINT );
	CPPUNIT_ASSERT( lstrip->GetWaynum() == "3" );
	CPPUNIT_ASSERT( lstrip->GetCapacity( Oddness::ODD ) == L"58" );
	CPPUNIT_ASSERT( lstrip->GetCapacity( Oddness::EVEN ) == L"44" );
	CPPUNIT_ASSERT( lstrip->GetLength() == 70 );
	const auto & adprops = lstrip->GetADProperties();
	CPPUNIT_ASSERT( adprops.HaveAttribute( ADAttr::PASSENGER ) );
	CPPUNIT_ASSERT( adprops.HaveAttribute( ADAttr::MAJOR ) );
	CPPUNIT_ASSERT( adprops.HaveAttribute( ADAttr::EMBARKATION ) );
	CPPUNIT_ASSERT( adprops.GetPassedTrainOddenss() == Oddness::EVEN );
	const auto & weightNorms = adprops.GetWeightNorms();
	const auto & normValues = weightNorms.GetAll();
	CPPUNIT_ASSERT( normValues.size() == 2 );
	CPPUNIT_ASSERT( normValues[0].locoseries == 928 );
	CPPUNIT_ASSERT( normValues[0].target == 6530 );
	CPPUNIT_ASSERT( normValues[0].tonweight == 14230 );
	CPPUNIT_ASSERT( normValues[1].locoseries == 585 );
	CPPUNIT_ASSERT( normValues[1].target == 45900 );
	CPPUNIT_ASSERT( normValues[1].tonweight == 500 );
	CPPUNIT_ASSERT( lstrip->HaveDirectionConflict() == true );
	CPPUNIT_ASSERT( lstrip->GetParknum() == "4" );
}

void TC_LXMLWrapper::SaveLandscape()
{
	auto lelem = BuildObject( LANDSCAPE );
	CLogicLandscape * landscape = static_cast <CLogicLandscape *>( lelem );
	landscape->SetName( L"Landscape_name" );
	vector <CLink> links;
	EsrKit ecode( 200 );
	links.push_back( CLink( STRIP, BadgeE( L"Strip_link", ecode ) ) );
	landscape->SetLinks( links );
	landscape->SetBound( rwCoord( 10, 200 ), rwCoord( 20, 230 ) );
	landscape->SetAxisName( L"axisName" );
	MainSlope mslope1( make_pair( rwCoord( 15, 100 ), rwCoord( 16, 500 ) ), L"MSVal1", 1 );
	landscape->AddMainSlope( mslope1 );
	MainSlope mslope2( make_pair( rwCoord( 17, 400 ), rwCoord( 18, 150 ) ), L"MSVal2", 2 );
	landscape->AddMainSlope( mslope2 );
	list <unsigned int> ordlist;
	ordlist.push_back( 3 );
	ordlist.push_back( 1 );
	ordlist.push_back( 2 );
	landscape->SetOrderList( ordlist );

	//сохраняем
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	auto lands_node = root_node.child( "LANDSCAPES" );
	CPPUNIT_ASSERT( lands_node );
	auto elem_node = lands_node.child( "element" );
	CPPUNIT_ASSERT( lands_node );
	CPPUNIT_ASSERT( elem_node.attribute( "name" ).as_wstring() == L"Landscape_name" );
	CPPUNIT_ASSERT( elem_node.attribute( "picketing1_val" ).as_wstring() == L"10~200" );
	CPPUNIT_ASSERT( elem_node.attribute( "picketing1_comm" ).as_wstring() == L"axisName" );
	CPPUNIT_ASSERT( elem_node.attribute( "picketing2_val" ).as_wstring() == L"20~230" );
	CPPUNIT_ASSERT( elem_node.attribute( "picketing2_comm" ).as_wstring() == L"axisName" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_left1" ).as_wstring() == L"15~100" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_right1" ).as_wstring() == L"16~500" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_val1" ).as_wstring() == L"MSVal1" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_way1" ).as_wstring() == L"1" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_left2" ).as_wstring() == L"17~400" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_right2" ).as_wstring() == L"18~150" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_val2" ).as_wstring() == L"MSVal2" );
	CPPUNIT_ASSERT( elem_node.attribute( "mainslope_way2" ).as_wstring() == L"2" );
	CPPUNIT_ASSERT( elem_node.attribute( "way_order" ).as_wstring() == L"3,1,2" );
	auto link_node = elem_node.child( "link" );
	CPPUNIT_ASSERT( link_node );
	CPPUNIT_ASSERT( link_node.attribute( "name" ).as_wstring() == L"Strip_link" );
	CPPUNIT_ASSERT( link_node.attribute( "class" ).as_uint() == STRIP );
	CPPUNIT_ASSERT( link_node.attribute( "ESR_code" ).as_string() == ecode.to_string() );
}

void TC_LXMLWrapper::LoadLandscape()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	auto lands_node = root_node.child( "LANDSCAPES" );
	auto elem_node = lands_node.append_child( "element" );

	elem_node.append_attribute( "name" ).set_value( L"Landscape_name" );
	elem_node.append_attribute( "picketing1_val" ).set_value( L"10~200" );
	elem_node.append_attribute( "picketing1_comm" ).set_value( L"axisName" );
	elem_node.append_attribute( "picketing2_val" ).set_value( L"20~230" );
	elem_node.append_attribute( "picketing2_comm" ).set_value( L"axisName" );
	elem_node.append_attribute( "mainslope_left" ).set_value( L"15~100" );
	elem_node.append_attribute( "mainslope_right" ).set_value( L"16~500" );
	elem_node.append_attribute( "mainslope_val" ).set_value( L"MSVal" );
	elem_node.append_attribute( "mainslope_way" ).set_value( L"2" );
	elem_node.append_attribute( "way_order" ).set_value( L"3,1,2" );
	auto link_node = elem_node.append_child( "link" );
	link_node.append_attribute( "name" ).set_value( L"Strip_link" );
	link_node.append_attribute( "class" ).set_value( STRIP );
	link_node.append_attribute( "ESR_code" ).set_value( "200" );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	set <const CLogicElement *> llands;
	plom->GetObjects( LANDSCAPE, logdoc.get(), llands );
	CPPUNIT_ASSERT( llands.size() == 1 );
	auto lelem = *llands.begin();
	const CLogicLandscape * lland = static_cast <const CLogicLandscape *>( lelem );
	CPPUNIT_ASSERT( lland->GetName() == L"Landscape_name" );
	const auto & lbound = lland->GetBound( true );
	CPPUNIT_ASSERT( lbound == rwCoord( 10, 200 ) );
	const auto & rbound = lland->GetBound( false );
	CPPUNIT_ASSERT( rbound == rwCoord( 20, 230 ) );
	const auto & ordlist = lland->GetOrderList();
	vector <unsigned int> ordvec( ordlist.cbegin(), ordlist.cend() );
	CPPUNIT_ASSERT( ordvec[0] == 3 );
	CPPUNIT_ASSERT( ordvec[1] == 1 );
	CPPUNIT_ASSERT( ordvec[2] == 2 );
	CPPUNIT_ASSERT( lland->GetAxisName() == L"axisName" );
	const auto & mslopes = lland->GetMainSlopes();
	CPPUNIT_ASSERT( mslopes.size() == 1 );
	CPPUNIT_ASSERT( mslopes[0].left_bound == rwCoord( 15, 100 ) );
	CPPUNIT_ASSERT( mslopes[0].right_bound == rwCoord( 16, 500 ) );
	CPPUNIT_ASSERT( mslopes[0].value == L"MSVal" );
	CPPUNIT_ASSERT( mslopes[0].waynum == 2 );
	const auto & links = lland->GetLinks();
	CPPUNIT_ASSERT( links.size() == 1 );
	auto link = links.back();
	CPPUNIT_ASSERT( link.obj_badge == BadgeE( L"Strip_link", EsrKit( 200 ) ) );
	CPPUNIT_ASSERT( link.object_class == STRIP );
}

void TC_LXMLWrapper::SaveJoint()
{
	auto lelem = BuildObject( JOINT );
	CLogicJoint * ljoint = static_cast <CLogicJoint *>( lelem );
	ljoint->SetName( L"Joint_name" );
	ljoint->SetType( OUTSIZED );
	vector <CLink> links;
	EsrKit ecode( 200 );
	links.push_back( CLink( STRIP, BadgeE( L"Test_link", ecode ) ) );
	ljoint->SetLinks( links );
	PicketingInfo pckInfo;
	pckInfo.SetCoord( rwRuledCoord( L"axisName", rwCoord( 120, 202 ) ) );
	ljoint->SetPicketingInfo( pckInfo );
	ljoint->SetValidDirection( CLogicJoint::EXIT );

	//сохраняем
	LXMLWrapperTest lwrapper( logdoc.get() );
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	auto joints_node = root_node.child( "JOINTS" );
	CPPUNIT_ASSERT( joints_node );
	auto elem_node = joints_node.child( "element" );
	CPPUNIT_ASSERT( elem_node );
	CPPUNIT_ASSERT( elem_node.attribute( "name" ).as_wstring() == L"Joint_name" );
	CPPUNIT_ASSERT( elem_node.attribute( "type" ).as_uint() == OUTSIZED );
	auto link_node = elem_node.child( "link" );
	CPPUNIT_ASSERT( link_node );
	CPPUNIT_ASSERT( link_node.attribute( "name" ).as_wstring() == L"Test_link" );
	CPPUNIT_ASSERT( link_node.attribute( "class" ).as_uint() == STRIP );
	CPPUNIT_ASSERT( link_node.attribute( "ESR_code" ).as_string() == ecode.to_string() );
	PicketingInfo xmlPckInfo;
	xmlPckInfo.readFromXml( elem_node );
	CPPUNIT_ASSERT( xmlPckInfo == pckInfo );
	CPPUNIT_ASSERT( elem_node.attribute( "valid_dir" ).as_int() == CLogicJoint::EXIT );
}

void TC_LXMLWrapper::LoadJoint()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	auto joints_node = root_node.child( "JOINTS" );
	auto elem_node = joints_node.append_child( "element" );

	elem_node.append_attribute( "name" ).set_value( L"Joint_name" );
	elem_node.append_attribute( "type" ).set_value( OUTSIZED );
	auto link_node = elem_node.append_child( "link" );
	link_node.append_attribute( "name" ).set_value( L"Strip_link" );
	link_node.append_attribute( "class" ).set_value( STRIP );
	link_node.append_attribute( "ESR_code" ).set_value( "200" );
	PicketingInfo xmlPickInfo;
	xmlPickInfo.SetCoord( rwRuledCoord( L"axisName", rwCoord( 120, 380 ) ) );
	xmlPickInfo.saveToXml( elem_node );
	elem_node.append_attribute( "valid_dir" ).set_value( CLogicJoint::EXIT );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	set <const CLogicElement *> ljoints;
	plom->GetObjects( JOINT, logdoc.get(), ljoints );
	CPPUNIT_ASSERT( ljoints.size() == 1 );
	auto lelem = *ljoints.begin();
	const CLogicJoint * ljoint = static_cast <const CLogicJoint *>( lelem );
	CPPUNIT_ASSERT( ljoint->GetName() == L"Joint_name" );
	CPPUNIT_ASSERT( ljoint->GetType() == OUTSIZED );
	const auto & links = ljoint->GetLinks();
	CPPUNIT_ASSERT( links.size() == 1 );
	auto link = links.back();
	CPPUNIT_ASSERT( link.obj_badge == BadgeE( L"Strip_link", EsrKit( 200 ) ) );
	CPPUNIT_ASSERT( link.object_class == STRIP );
	const PicketingInfo & jointPickInfo = ljoint->GetPicketingInfo();
	CPPUNIT_ASSERT( jointPickInfo == xmlPickInfo );
	CPPUNIT_ASSERT( ljoint->GetValidDirection() == CLogicJoint::EXIT );
}

CLogicElement * TC_LXMLWrapper::CreateUncommonObject( ELEMENT_CLASS lclass ) const
{
	CLogicElement * le = CLogicElementFactory().CreateObject( lclass );
	le->SetName( L"Specific name" );
	CInformator * pI = CInformator::Instance();
	vector <pair <wstring, unsigned int> > typeVec;
	pI->GetAllTypesStrings( lclass, typeVec );
	if ( !typeVec.empty() )
		le->SetType( typeVec.back().second );
	vector <CLink> links;
	links.push_back( CLink( STRIP, BadgeE( L"Test Link", EsrKit( 200 ) ) ) );
	le->SetLinks( links );
	switch( lclass )
	{
	case SEMALEG:
		{
			CLogicSemaleg * lleg = static_cast <CLogicSemaleg *>( le );
			lleg->SetOddness( lleg->GetOddness() == Oddness::EVEN ? Oddness::ODD : Oddness::EVEN );
			break;
		}
	case COMMAND:
		{
			CLogicCommand * lcommand = static_cast <CLogicCommand *>( le );
			lcommand->setCrucial( !lcommand->isCrucial() );
			lcommand->addBut( 0, BadgeE( L"BdgName", EsrKit( 14521 ) ) );
			lcommand->attachFollower( wstring( L"follower" ) );
			lcommand->setComment( wstring( L"command_commentary" ) );
			lcommand->setKeyboardSequence( wstring( L"keyboard_sequence" ) );
			lcommand->setFollowEnabler( BadgeU( wstring( L"follow_enabler" ), 250 ) );
			lcommand->setEnablerInverted( !lcommand->isEnablerInverted() );
			break;
		}
	}
	return le;
}

void TC_LXMLWrapper::LoadJointWithInvalidPicketage()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	auto joints_node = root_node.child( "JOINTS" );
	auto elem_node = joints_node.append_child( "element" );

	elem_node.append_attribute( "name" ).set_value( L"Joint_name" );

	//пикетаж в xml-документе задан невалидно (только пикетаж без направления)
	PicketingInfo xmlPickInfo;
	xmlPickInfo.SetCoord( rwRuledCoord( L"axisName", rwCoord( 120, 380 ) ) );
	xmlPickInfo.saveToXml( elem_node );
	string name_val;
	string name_comm;
	boost::tie(name_val, name_comm) = PicketingInfo::xml_picketing_val_and_comm_str( 1 );
	elem_node.attribute( name_comm ).set_value( L"" );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	CPPUNIT_ASSERT( lresult.ChangedByLoading() );
}

void TC_LXMLWrapper::LoadStationWithInvalidAxis()
{
	LXMLWrapperTest lwrapper( logdoc.get() );
	lwrapper.Save(L"");
	attic::a_document * xdoc = logdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();

	//пикетаж в xml-документе задан невалидно (только пикетаж без направления)
	PicketingInfo xmlPickInfo;
	xmlPickInfo.SetCoord( rwRuledCoord( L"axisName", rwCoord( 120, 380 ) ) );
	xmlPickInfo.saveToXml( root_node );
	string name_val;
	string name_comm;
	boost::tie(name_val, name_comm) = PicketingInfo::xml_picketing_val_and_comm_str( 1 );
	root_node.attribute( name_comm ).set_value( L"" );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	CPPUNIT_ASSERT( lresult.ChangedByLoading() );
}

void TC_LXMLWrapper::SaveWaysLocks()
{
	//сохраняем блокировки путей в документ перегона
	std::unique_ptr<CLogicDocumentTest> spandoc( new CLogicDocumentTest() );
	auto spansman = spandoc->GetStationManager();
	SpanDescription spanDescr;
	spanDescr.esrCode = EsrKit( 42000, 43150 );
	spanDescr.waysLocks.setLock( 2, SpanLockType::SEMI_AUTOBLOCK );
	spanDescr.waysLocks.setLock( 5, SpanLockType::UNIDIR_AUTOBLOCK );
	spanDescr.waysLocks.setLock( 3, SpanLockType::ELECTRIC_TOKEN );
	spansman->SetDescription( spanDescr, spandoc.get() );

	LXMLWrapperTest lwrapper( spandoc.get() );

	attic::a_document * xdoc = spandoc->GetXMLDoc();
	lwrapper.Save( L"" );
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node );
	auto waylocks_node = root_node.child( "WAYSLOCKS" );
	CPPUNIT_ASSERT( waylocks_node );
	auto elem_node = waylocks_node.first_child();
	CPPUNIT_ASSERT( elem_node );
	CPPUNIT_ASSERT( elem_node.name_is( "element" ) );
	CPPUNIT_ASSERT( elem_node.attribute( "way" ).as_wstring() == L"2" );
	CPPUNIT_ASSERT( elem_node.attribute( "lock" ).as_wstring() == L"3" );
	elem_node = elem_node.next_sibling();
	CPPUNIT_ASSERT( elem_node.name_is( "element" ) );
	CPPUNIT_ASSERT( elem_node.attribute( "way" ).as_wstring() == L"3" );
	CPPUNIT_ASSERT( elem_node.attribute( "lock" ).as_wstring() == L"4" );
	elem_node = elem_node.next_sibling();
	CPPUNIT_ASSERT( elem_node.name_is( "element" ) );
	CPPUNIT_ASSERT( elem_node.attribute( "way" ).as_wstring() == L"5" );
	CPPUNIT_ASSERT( elem_node.attribute( "lock" ).as_wstring() == L"2" );
	elem_node = elem_node.next_sibling();
	CPPUNIT_ASSERT( !elem_node );

	//вторичное сохранение не приводит к повторному добавлению узла
	lwrapper.Save( L"" );
	waylocks_node = root_node.child( "WAYSLOCKS" );
	CPPUNIT_ASSERT( !waylocks_node.next_sibling( "WAYSLOCKS" ) );
	auto elemRange = waylocks_node.children( "element" );
	auto elemCount = distance( elemRange.begin(), elemRange.end() );
	CPPUNIT_ASSERT( elemCount == 3 );
}

void TC_LXMLWrapper::LoadWaysLocks()
{
	std::unique_ptr<CLogicDocumentTest> spandoc( new CLogicDocumentTest() );
	auto spansman = spandoc->GetStationManager();
	StationDescription spanDescr;
	spanDescr.esrCode = EsrKit( 42000, 43150 );
	spansman->SetDescription( spanDescr, spandoc.get() );
	LXMLWrapperTest lwrapper( spandoc.get() );
	lwrapper.Save(L""); //создаем минимальный xml-документ

	attic::a_document * xdoc = spandoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	auto waylocks_node = root_node.child( "WAYSLOCKS" );
	if ( !waylocks_node )
		waylocks_node = root_node.append_child( "WAYSLOCKS" );
	auto elem_node = waylocks_node.append_child( "element" );
	elem_node.append_attribute( "way" ).set_value( 3 );
	elem_node.append_attribute( "lock" ).set_value( 5 );
	elem_node = waylocks_node.append_child( "element" );
	elem_node.append_attribute( "way" ).set_value( 7 );
	elem_node.append_attribute( "lock" ).set_value( 3 );
	elem_node = waylocks_node.append_child( "element" );
	elem_node.append_attribute( "way" ).set_value( 4 );
	elem_node.append_attribute( "lock" ).set_value( 2 );
	elem_node = waylocks_node.append_child( "element" );
	elem_node.append_attribute( "way" ).set_value( 1 );
	elem_node.append_attribute( "lock" ).set_value( 4 );

	LLoadResult lresult( L"" );
	lwrapper.Load( L"", lresult );
	const SpanDescription * loadedSDescr = spansman->GetDescription<SpanDescription>( EsrKit( 42000, 43150 ) );
	const WaysLocks & waysLocks = loadedSDescr->waysLocks;
	const auto & waysVec = waysLocks.getWays();
	CPPUNIT_ASSERT( waysVec.size() == 4 );
	CPPUNIT_ASSERT( find( waysVec.cbegin(), waysVec.cend(), 3 ) != waysVec.cend() );
	CPPUNIT_ASSERT( find( waysVec.cbegin(), waysVec.cend(), 7 ) != waysVec.cend() );
	CPPUNIT_ASSERT( find( waysVec.cbegin(), waysVec.cend(), 4 ) != waysVec.cend() );
	CPPUNIT_ASSERT( find( waysVec.cbegin(), waysVec.cend(), 1 ) != waysVec.cend() );
	for ( auto wayNum : waysVec )
	{
		auto lockTypePtr = waysLocks.getLock( wayNum );
		CPPUNIT_ASSERT( lockTypePtr );
		switch( wayNum )
		{
		case 3:
			CPPUNIT_ASSERT( *lockTypePtr == SpanLockType::TELEPHONE_CONNECTION );
			break;
		case 7:
			CPPUNIT_ASSERT( *lockTypePtr == SpanLockType::SEMI_AUTOBLOCK );
			break;
		case 4:
			CPPUNIT_ASSERT( *lockTypePtr == SpanLockType::UNIDIR_AUTOBLOCK );
			break;
		case 1:
			CPPUNIT_ASSERT( *lockTypePtr == SpanLockType::ELECTRIC_TOKEN );
			break;
		}
	}
}