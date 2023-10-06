#include "stdafx.h"
#include "TC_SignalValidator.h"
#include "SignalDocumentTest.h"
#include "../StatBuilder/AbcSignal.h"
#include "../StatBuilder/SignalValidator.h"
#include "../StatBuilder/SLoadResult.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/SignalView.h"
#include "../StatBuilder/LtsSignal.h"
#include "../StatBuilder/Abonent.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SignalValidator );

void TC_SignalValidator::LADuplicates()
{
	Abonent abonent( 2430, 50 );
	LtsSignal * signal1 = static_cast <LtsSignal *>( BuildSignal( abonent, SignalType::LTS ) );
	signal1->SetLA( 10 );
	LtsSignal * signal2 = static_cast <LtsSignal *>( BuildSignal( abonent, SignalType::LTS ) );
	signal2->SetLA( 10 );
	boost::shared_ptr <SLoadResult> lresPtr( new SLoadResult( L"filename ") );
	SignalValidator svalidator( sigdoc.get(), lresPtr );
	svalidator.Validate();
	wstring loadinfo = lresPtr->GetDetailedInfo();
	CPPUNIT_ASSERT( !loadinfo.empty() );
}