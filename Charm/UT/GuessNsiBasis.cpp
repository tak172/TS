#include "stdafx.h"
#include "GuessNsiBasis.h"
#include "../helpful/Amqp/RestClient.h"
#include "../helpful/Junctions.h"
#include "../helpful/LocoCharacteristics.h"

using namespace std;

TestGuessNsiBasis::TestGuessNsiBasis() : NsiBasis( Rest::Response( move( unordered_map<string, string>() ) ) )
{
	MakeJunctions();
}

TestGuessNsiBasis::~TestGuessNsiBasis()
{
    LocoCharacteristics::Shutdowner();
}

void TestGuessNsiBasis::MakeJunctions()
{
	vector<Junctions::StationUnion> techNodes;

	//фиктивные станции
	Junctions::StationUnion fictUnion( false );
	fictUnion.codes.insert( EsrKit( 506 ) );
	techNodes.push_back( fictUnion );
	Junctions::StationUnion fictUnion2( true );
	fictUnion2.codes.insert( EsrKit( 505 ) );
	fictUnion2.codes.insert( EsrKit( 507 ) );
	techNodes.push_back( fictUnion2 );

	//реальные станции
	Junctions::StationUnion realUnion( true );
	realUnion.codes.insert( EsrKit( 9000 ) );
	techNodes.push_back( realUnion );

	Junctions::StationCodes statCodes;
	statCodes.insert( StationEsr( 502 ) );
	statCodes.insert( StationEsr( 504 ) );
	vector<Junctions::StationCodes> equalStations( 1, statCodes );
	junctionsPtr.reset( new Junctions( techNodes, equalStations ) );
}