#include "stdafx.h"
#include "TopologyTest.h"
#include "../helpful/StationsNet.h"
#include "../helpful/Serialization.h"
#include "../helpful/Topology.h"

using namespace std;

std::wstring TopologyTest::init(const std::list <EsrKit> &ecodes)
{
	list<EsrKit> statCodes;
	for_each( ecodes.cbegin(), ecodes.cend(), [&statCodes]( const EsrKit & ecode ){
		if ( ecode.terminal() )
			statCodes.emplace_back( ecode );
	} );
	wstringstream wss;
	if ( statCodes.size() > 1 )
	{
		//"*100,200 {100,102,103,104,105,106,200}"
		wss << L"*" << statCodes.front().to_wstring() << L"," << statCodes.back().to_wstring() << L" {";
		for ( auto mcIt = statCodes.cbegin(); mcIt != statCodes.cend(); ++mcIt )
		{
			wss << mcIt->to_wstring();
			if ( mcIt != --statCodes.cend() )
				wss << L",";
			else
				wss << L"}";
		}
	}
	wss<<std::endl;
	return wss.str();
}

std::wstring TopologyTest::initF(const std::list <EsrKit> &fork)
{
	wstringstream wss;
	if ( fork.size() > 1 )
	{
		wss << L"=" ;
		//"*100,200 {100,102,103,104,105,106,200}"
		for ( auto it = fork.cbegin(); it != fork.cend(); ++it )
		{
			wss<< it->getTerm();
			if ( it != --fork.cend() )
				wss << L",";
		}
		wss<<std::endl;
	}
	return wss.str();
}

TopologyTest::TopologyTest( const list <EsrKit> & ecodes ) : Topology( std::shared_ptr<StationsNet>(), std::shared_ptr<TopologyHelper>(), std::wstring() )
{
	wstringstream wss;
	wss<<init(ecodes);
	std::shared_ptr<StationsNet> testStatNet( new StationsNet() );
	testStatNet->Load( wss );
	setupStationsNet( testStatNet );
}

TopologyTest::TopologyTest(const VL_ESR & vEcodes, const VL_ESR& fork):
	Topology( std::shared_ptr<StationsNet>(), std::shared_ptr<TopologyHelper>(), std::wstring() )
{
	wstringstream wss;
	for (const auto& t: vEcodes)
		wss<<init(t);
	for (const auto& t: fork)
		wss<<initF(t);

	std::shared_ptr<StationsNet> testStatNet( new StationsNet() );
	testStatNet->Load( wss );
	setupStationsNet( testStatNet );
}

std::wstring TopologyTest::Load( std::wistream & iostr )
{
	std::shared_ptr<StationsNet> stationsNet(new StationsNet());
	std::wstring errors = stationsNet->Load(iostr);
	setupStationsNet(stationsNet);
	return errors;
}

std::wstring TopologyTest::Load( const std::wstring& data )
{
	std::wstringstream iostr;
	iostr << data;

	std::shared_ptr<StationsNet> stationsNet(new StationsNet());
	std::wstring errors = stationsNet->Load(iostr);
	setupStationsNet(stationsNet);
	return errors;
}

template <>
std::shared_ptr<TopologyTest> deserialize(const std::string& source)
{
	auto topoPtr = deserialize<Topology>( source );
	return topoPtr ? make_shared<TopologyTest>( *topoPtr ) : nullptr;
}