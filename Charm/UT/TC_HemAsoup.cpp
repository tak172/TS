#include "stdafx.h"
#include "TC_HemAsoup.h"
#include "../Hem/AsoupEvent.h"
#include "../Hem/HemEventRuler.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/HemPath.h"
#include "../Guess/TrackerEvent.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemAsoup );

void TC_HemAsoup::tearDown()
{
	HemEventRegistry::Shutdowner();
}
