#include "stdafx.h"
#include "TC_Signal.h"
#include "SignalDocumentTest.h"
#include "../StatBuilder/SignalFactory.h"
#include "../StatBuilder/AbcSignal.h"
#include "../StatBuilder/SignalManager.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/SignalView.h"

using namespace std;

TC_Signal::TC_Signal()
{
	sigdoc.reset( new CSignalDocumentTest );
	sim = sigdoc->GetSignalManager();
}

CAbcSignal * TC_Signal::BuildSignal( const Abonent & abonent, const SignalType & sigtype, bool add_to_doc, CSignalDocument * sdoc )
{
	CSignalDocument * aimdoc = ( sdoc == 0 ) ? sigdoc.get() : sdoc;
	CAbcSignal * sigobject = CSignalFactory().CreateObject( sigtype );
	sigobject->SetName( GenerateSignalName( L"signame" ) );
	if ( add_to_doc )
		sim->AddObject( sigobject, AbcStation( L"station_longname", L"station_shortname" ), abonent, SignalBreed( SignalSystem::Kit, Haron::STATIC_TABLE ), aimdoc );
	return sigobject;
}

wstring TC_Signal::GenerateSignalName( wstring initial_name )
{
	wstring retname = initial_name;
	unsigned int num = 1;
	while( used_names.find( retname ) != used_names.end() )
	{
		retname = initial_name;
		wostringstream wostream;
		wostream << retname << ++num;
		retname = wostream.str();
	}
	used_names.insert( retname );
	return retname;
}