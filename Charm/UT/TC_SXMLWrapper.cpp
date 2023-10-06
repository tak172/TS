#include "stdafx.h"
#include "TC_SXMLWrapper.h"
#include "SignalDocumentTest.h"
#include "../StatBuilder/MultiEditor.h"
#include "../StatBuilder/SignalView.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/Abonent.h"
#include "../StatBuilder/SignalTypes.h"
#include "../StatBuilder/LtsSignal.h"
#include "../StatBuilder/SXMLWrapper.h"
#include "../StatBuilder/SignalManager.h"
#include "../StatBuilder/SignalXmlAttributes.h"
#include "../StatBuilder/VirtualSignal.h"
#include "../StatBuilder/FormulaSignal.h"
#include "../StatBuilder/FormulaElement.h"
#include "SXMLWrapperTest.h"
#include "../StatBuilder/SLoadResult.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SXMLWrapper );

void TC_SXMLWrapper::BasicSave()
{
	Abonent abonent( 24, 10 );
	CAbcSignal * signal = BuildSignal( abonent, SignalType::LTS );
	const CSignalManager * psim = sigdoc->GetSignalManager();
	const auto & stations = psim->GetStations( sigdoc.get() );
	CPPUNIT_ASSERT( stations.size() == 1 );
	const auto & fullname = stations.cbegin()->fullName;
	const auto & shortname = stations.cbegin()->shortName;

	//сохраняем
	CSXMLWrapper swrapper( sigdoc.get() );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	swrapper.Save( L"" );
	auto root_node = xdoc->document_element();

	CPPUNIT_ASSERT( root_node );
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.child( SignalXml::xml_station_str );
	CPPUNIT_ASSERT( station_node );
	CPPUNIT_ASSERT( station_node.attribute( SignalXml::xml_name_str ).as_wstring() == fullname );
	CPPUNIT_ASSERT( station_node.attribute( SignalXml::xml_short_name_str ).as_wstring() == shortname );

	auto plain_node = station_node.child( SignalXml::xml_plain_str );
	CPPUNIT_ASSERT( plain_node );
	CPPUNIT_ASSERT( plain_node.attribute( SignalXml::xml_abonent_str ).as_uint() == abonent.GetNumber() );
	CPPUNIT_ASSERT( plain_node.attribute( SignalXml::xml_shift_str ).as_uint() == abonent.GetShift() );

	auto kind_node = plain_node.child( SignalXml::xml_kind_str );
	CPPUNIT_ASSERT( kind_node );
	auto typestr = kind_node.attribute( SignalXml::xml_type_str ).as_string();
	CPPUNIT_ASSERT( kind_node.attribute( SignalXml::xml_type_str ).as_string() == string( "S_KIT" ) );
	CPPUNIT_ASSERT( kind_node.attribute( SignalXml::xml_select_str ).as_string() == string( "STAT" ) );
}

void TC_SXMLWrapper::SaveLts()
{
	Abonent abonent( 24, 10 );
	CAbcSignal * signal = BuildSignal( abonent, SignalType::LTS );
	LtsSignal * lts_signal = static_cast <LtsSignal *>( signal );

	lts_signal->SetName( L"newSignalName" );
	lts_signal->SetCommentary( L"signalCommentary" );
	lts_signal->SetLA( 191 );

	//сохраняем
	CSXMLWrapper swrapper( sigdoc.get() );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	swrapper.Save( L"" );
	auto root_node = xdoc->document_element();

	CPPUNIT_ASSERT( root_node );
	auto station_node = root_node.child( SignalXml::xml_station_str );
	CPPUNIT_ASSERT( station_node );
	auto plain_node = station_node.child( SignalXml::xml_plain_str );
	CPPUNIT_ASSERT( plain_node );
	auto kind_node = plain_node.child( SignalXml::xml_kind_str );
	CPPUNIT_ASSERT( kind_node );
	auto lts_node = kind_node.child( SignalXml::xml_lts_str );
	CPPUNIT_ASSERT( lts_node );

	CPPUNIT_ASSERT( lts_node.attribute( SignalXml::xml_name_str ).as_wstring() == L"newSignalName" );
	CPPUNIT_ASSERT( lts_node.attribute( SignalXml::xml_comment_str ).as_wstring() == L"signalCommentary" );
	CPPUNIT_ASSERT( lts_node.attribute( SignalXml::xml_la_str ).as_uint() == 191 );
	auto lts_groupnum = signal->GetGroupNum( SignalSystem::Kit );
	auto lts_impulse = signal->GetImpulse( SignalSystem::Kit );
	CPPUNIT_ASSERT( lts_node.attribute( SignalXml::xml_group_str ).as_uint() == lts_groupnum );
	CPPUNIT_ASSERT( lts_node.attribute( SignalXml::xml_impulse_str ).as_uint() == lts_impulse );
}

void TC_SXMLWrapper::SaveVirtual()
{
	Abonent abonent( 24, 10 );
	CAbcSignal * signal = BuildSignal( abonent, SignalType::VIRTUAL );
	VirtualSignal * virtual_signal = static_cast <VirtualSignal *>( signal );

	//обязательно нужно создать некоторые зависимые Lts-сигналы и вирутализировать их (иначе виртуальный сигнал сохранен не будет)
	CAbcSignal * lts_signal1 = BuildSignal( abonent, SignalType::LTS );
	CAbcSignal * lts_signal2 = BuildSignal( abonent, SignalType::LTS );
	sim->VirtualizeSignal( sigdoc.get(), lts_signal1, virtual_signal );
	sim->VirtualizeSignal( sigdoc.get(), lts_signal2, virtual_signal );
	
	virtual_signal->SetName( L"newSignalName" );
	virtual_signal->SetCommentary( L"signalCommentary" );
	virtual_signal->SetAttrVal( "test_attr", L"test_val" );

	//сохраняем
	CSXMLWrapper swrapper( sigdoc.get() );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	swrapper.Save( L"" );
	auto root_node = xdoc->document_element();

	CPPUNIT_ASSERT( root_node );
	auto station_node = root_node.child( SignalXml::xml_station_str );
	CPPUNIT_ASSERT( station_node );
	auto plain_node = station_node.child( SignalXml::xml_plain_str );
	CPPUNIT_ASSERT( plain_node );
	auto kind_node = plain_node.child( SignalXml::xml_kind_str );
	CPPUNIT_ASSERT( kind_node );
	auto virt_node = kind_node.child( SignalXml::xml_element_str );

	CPPUNIT_ASSERT( virt_node.attribute( SignalXml::xml_small_name_str ).as_wstring() == L"newSignalName" );
	CPPUNIT_ASSERT( virt_node.attribute( SignalXml::xml_comment_str ).as_wstring() == L"signalCommentary" );
	CPPUNIT_ASSERT( virt_node.attribute( "test_attr" ).as_wstring() == L"test_val" );
}

void TC_SXMLWrapper::SaveFormula()
{
	Abonent abonent( 24, 10 );
	CAbcSignal * signal = BuildSignal( abonent, SignalType::FORMULA );
	FormulaSignal * formula_signal = static_cast <FormulaSignal *>( signal );
	formula_signal->SetName( L"newSignalName" );
	formula_signal->SetCommentary( L"signalCommentary" );
	formula_signal->SetUserName( L"someUserName" );
	auto operand1 = new FormulaOperand( L"lts1", Abonent( abonent ) );
	auto operand2 = new FormulaOperand( L"lts2", Abonent( abonent ) );
	auto formula_operation = new FormulaOperation( "opersym" );
	formula_signal->AddElement( operand1 );
	formula_signal->AddElement( formula_operation );
	formula_signal->AddElement( operand2 );

	//сохраняем
	CSXMLWrapper swrapper( sigdoc.get() );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	swrapper.Save( L"" );
	auto root_node = xdoc->document_element();

	CPPUNIT_ASSERT( root_node );
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.child( SignalXml::xml_station_str );
	CPPUNIT_ASSERT( station_node );

	auto formula_node = station_node.child( SignalXml::xml_formula_str );
	CPPUNIT_ASSERT( formula_node );
	CPPUNIT_ASSERT( formula_node.attribute( SignalXml::xml_abonent_str ).as_uint() == abonent.GetNumber() + abonent.GetShift() );

	auto exp_node = formula_node.child( SignalXml::xml_exp_str );
	CPPUNIT_ASSERT( exp_node );
	CPPUNIT_ASSERT( exp_node.attribute( SignalXml::xml_name_str ).as_wstring() == L"newSignalName" );
	CPPUNIT_ASSERT( exp_node.attribute( SignalXml::xml_comment_str ).as_wstring() == L"signalCommentary" );
	CPPUNIT_ASSERT( exp_node.attribute( SignalXml::xml_ex_str ).as_wstring() == formula_signal->GetUserName() );

	auto lts1_node = exp_node.first_child();
	CPPUNIT_ASSERT( lts1_node && lts1_node.name_is( SignalXml::xml_lts_str ) );
	CPPUNIT_ASSERT( lts1_node.attribute( SignalXml::xml_name_str ).as_wstring() == L"lts1" );
	CPPUNIT_ASSERT( lts1_node.attribute( SignalXml::xml_abonent_str ).as_uint() == abonent.GetNumber() );
	auto literal_node = lts1_node.next_sibling();
	CPPUNIT_ASSERT( literal_node && literal_node.name_is( SignalXml::xml_literal_str ) );
	CPPUNIT_ASSERT( literal_node.attribute( SignalXml::xml_symbol_str ).as_wstring() == L"opersym" );
	auto lts2_node = literal_node.next_sibling();
	CPPUNIT_ASSERT( lts2_node && lts2_node.name_is( SignalXml::xml_lts_str ) );
	CPPUNIT_ASSERT( lts2_node.attribute( SignalXml::xml_name_str ).as_wstring() == L"lts2" );
	CPPUNIT_ASSERT( lts2_node.attribute( SignalXml::xml_abonent_str ).as_uint() == abonent.GetNumber() );
}

void TC_SXMLWrapper::BasicLoad()
{
	SXMLWrapperTest swrapper( sigdoc.get() );
	swrapper.Save( L"" );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.append_child( "Station" );
	station_node.append_attribute( "Name" ).set_value( L"Jelgava" );
	station_node.append_attribute( "ShortName" ).set_value( L"JEL" );
	auto plain_node = station_node.append_child( "Plain" );
	plain_node.append_attribute( "Abonent" ).set_value( 9 );
	plain_node.append_attribute( "Shift" ).set_value( 100 );
	auto kind_node = plain_node.append_child( "Kind" );
	kind_node.append_attribute( "Type" ).set_value( "S_KIT" );
	kind_node.append_attribute( "Select" ).set_value( "STAT" );

	auto lts_node = kind_node.append_child( "LTS" );
	lts_node.append_attribute( "Name" ).set_value( L"SomeLtsName" );
	SLoadResult sresult( L"" );
	swrapper.Load( L"", sresult );
	const auto & stations = sim->GetStations( sigdoc.get() );
	CPPUNIT_ASSERT( stations.size() == 1 );
	auto station = *stations.cbegin();
	CPPUNIT_ASSERT( station.fullName == L"Jelgava" );
	CPPUNIT_ASSERT( station.shortName == L"JEL" );
	set <Abonent> abonents = sim->GetAbonents( sigdoc.get(), station );
	CPPUNIT_ASSERT( abonents.size() == 1 );
	const auto & abonent = *abonents.cbegin();
	CPPUNIT_ASSERT( abonent.GetNumber() == 9 );
	CPPUNIT_ASSERT( abonent.GetShift() == 100 );
}

void TC_SXMLWrapper::LoadLts()
{
	SXMLWrapperTest swrapper( sigdoc.get() );
	swrapper.Save( L"" );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.append_child( "Station" );
	station_node.append_attribute( "Name" ).set_value( L"Jelgava" );
	auto plain_node = station_node.append_child( "Plain" );
	plain_node.append_attribute( "Abonent" ).set_value( 9 );
	auto kind_node = plain_node.append_child( "Kind" );
	kind_node.append_attribute( "Type" ).set_value( "S_KIT" );

	auto lts_node = kind_node.append_child( "LTS" );
	lts_node.append_attribute( "Name" ).set_value( L"SomeLtsName" );
	lts_node.append_attribute( "Gr" ).set_value( 1 );
	lts_node.append_attribute( "Imp" ).set_value( 10 );
	lts_node.append_attribute( "LA" ).set_value( 9 );
	lts_node.append_attribute( "Comment" ).set_value( L"SomeCommentary" );
	SLoadResult sresult( L"" );
	swrapper.Load( L"", sresult );
	const auto & stations = sim->GetStations( sigdoc.get() );
	CPPUNIT_ASSERT( stations.size() == 1 );
	auto station = *stations.cbegin();
	set <Abonent> abonents = sim->GetAbonents( sigdoc.get(), station );
	CPPUNIT_ASSERT( abonents.size() == 1 );
	const auto & abonent = *abonents.cbegin();
	const auto & ltsSignals = sim->GetSignals( sigdoc.get(), abonent, SignalType::LTS );
	CPPUNIT_ASSERT( ltsSignals.size() == 1 );
	auto ltsSignal = *ltsSignals.cbegin();
	CPPUNIT_ASSERT( ltsSignal->GetName() == L"SomeLtsName" );
	CPPUNIT_ASSERT( ltsSignal->GetCommentary() == L"SomeCommentary" );
	CPPUNIT_ASSERT( ltsSignal->GetGroupNum( SignalSystem::Kit ) == 1 );
	CPPUNIT_ASSERT( ltsSignal->GetImpulse( SignalSystem::Kit ) == 10 );
	CPPUNIT_ASSERT( ltsSignal->GetLA() == 9 );
}

void TC_SXMLWrapper::LoadVirtual()
{
	SXMLWrapperTest swrapper( sigdoc.get() );
	swrapper.Save( L"" );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.append_child( "Station" );
	station_node.append_attribute( "Name" ).set_value( L"Jelgava" );
	auto plain_node = station_node.append_child( "Plain" );
	plain_node.append_attribute( "Abonent" ).set_value( 9 );
	auto kind_node = plain_node.append_child( "Kind" );
	kind_node.append_attribute( "Type" ).set_value( "S_EBILOC" );

	auto lts1_node = kind_node.append_child( "LTS" );
	lts1_node.append_attribute( "Name" ).set_value( L"ltsName1" );
	lts1_node.append_attribute( "LA" ).set_value( L"1" );
	auto lts2_node = kind_node.append_child( "LTS" );
	lts2_node.append_attribute( "Name" ).set_value( L"ltsName2" );
	lts2_node.append_attribute( "LA" ).set_value( L"2" );
	auto elem_node = kind_node.append_child( "element" );
	elem_node.append_attribute( "name" ).set_value( "virtName" );
	elem_node.append_attribute( "anyattrib" ).set_value( L"attribVal" );

	SLoadResult sresult( L"" );
	swrapper.Load( L"", sresult );
	const auto & stations = sim->GetStations( sigdoc.get() );
	CPPUNIT_ASSERT( stations.size() == 1 );
	auto station = *stations.cbegin();
	set <Abonent> abonents = sim->GetAbonents( sigdoc.get(), station );
	CPPUNIT_ASSERT( abonents.size() == 1 );
	const auto & abonent = *abonents.cbegin();
	const auto & virtSignals = sim->GetSignals( sigdoc.get(), abonent, SignalType::VIRTUAL );
	CPPUNIT_ASSERT( virtSignals.size() == 1 );
	const VirtualSignal * virtSignal = static_cast <const VirtualSignal *>( *virtSignals.cbegin() );
	CPPUNIT_ASSERT( virtSignal->GetName() == L"virtName" );
	wstring attrVal;
	CPPUNIT_ASSERT( virtSignal->GetAttrVal( "anyattrib", attrVal ) );
	CPPUNIT_ASSERT( attrVal == L"attribVal" );
}

void TC_SXMLWrapper::LoadFormula()
{
	SXMLWrapperTest swrapper( sigdoc.get() );
	swrapper.Save( L"" );
	attic::a_document * xdoc = sigdoc->GetXMLDoc();
	auto root_node = xdoc->document_element();
	CPPUNIT_ASSERT( root_node.name_is( "AbcTable" ) );
	auto station_node = root_node.append_child( "Station" );
	station_node.append_attribute( "Name" ).set_value( L"Jelgava" );
	auto formula_node = station_node.append_child( "Formula" );
	formula_node.append_attribute( "Abonent" ).set_value( 101 );
	auto exp_node = formula_node.append_child( "Exp" );
	exp_node.append_attribute( "Ex" ).set_value( L"ExprVal" );
	exp_node.append_attribute( "Name" ).set_value( L"FormulaName" );
	auto liter_node = exp_node.append_child( "Literal" );
	liter_node.append_attribute( "Symbol" ).set_value( "~" );
	auto lts_node = exp_node.append_child( "LTS" );
	lts_node.append_attribute( "Name" ).set_value( L"LtsName" );
	lts_node.append_attribute( "Abonent" ).set_value( 101 );

	SLoadResult sresult( L"" );
	swrapper.Load( L"", sresult );
	const auto & stations = sim->GetStations( sigdoc.get() );
	CPPUNIT_ASSERT( stations.size() == 1 );
	auto station = *stations.cbegin();
	set <Abonent> abonents = sim->GetAbonents( sigdoc.get(), station );
	CPPUNIT_ASSERT( abonents.size() == 1 );
	const auto & abonent = *abonents.cbegin();
	const auto & formulas = sim->GetSignals( sigdoc.get(), abonent, SignalType::FORMULA );
	CPPUNIT_ASSERT( formulas.size() == 1 );
	const FormulaSignal * formulaSignal = static_cast <const FormulaSignal *>( *formulas.cbegin() );
	CPPUNIT_ASSERT( formulaSignal->GetName() == L"FormulaName" );
	CPPUNIT_ASSERT( formulaSignal->GetUserName() == L"ExprVal" );
	const auto & formulaElements = formulaSignal->GetContent();
	CPPUNIT_ASSERT( formulaElements.size() == 2 );
	for ( unsigned int k = 0; k < formulaElements.size(); ++k )
	{
		const FormulaElement * formelem = formulaElements[k];
		attic::a_document xdoc( "root" );
		attic::a_node root_node = xdoc.document_element();
		attic::a_node fenode = root_node.append_child( "felem" );
		attic::a_node examnode = root_node.append_child( "felem" );
		formelem->SaveTo( fenode );
		if ( k == 0 )
		{
			FormulaOperation exampleOperation( "~" );
			static_cast <const FormulaElement &> ( exampleOperation ).SaveTo( examnode );
		}
		else
		{
			FormulaOperand exampleOperand( L"LtsName", Abonent( 101, 0 ) );
			static_cast <const FormulaElement &> ( exampleOperand ).SaveTo( examnode );
		}
		CPPUNIT_ASSERT( !fenode.to_str().empty() && fenode.to_str() == examnode.to_str() );
	}
}