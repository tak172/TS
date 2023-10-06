#include "stdafx.h"
#include "TestPossessions.h"
#include "../helpful/NsiBasis.h"

using namespace std;

TestPossessions::TestPossessions( shared_ptr<const NsiBasis> nsiBasisPtr ) : 
	Possessions( LimitInfoVec(), nsiBasisPtr->stationsRegPtr, nsiBasisPtr->spansRegPtr )
{
}