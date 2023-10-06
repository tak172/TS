#include "stdafx.h"
#include "TestSpanKit.h"
#include "../StatBuilder/LogicElement.h"

using namespace std;

set <const CLogicElement *> TestSpanKit ::mockstrips;

bool TestSpanKit::AlienLinked( const LogicSpanWay & spanWay ) const
{
	//в качестве критерия связности со смежными станциями рассмотрим количество линков со стыками для граничных участков пути
	bool alienLinked = false;
	const auto & lstrips = spanWay.Strips();
	if ( !lstrips.empty() )
	{
		if ( lstrips.size() == 1 )
		{
			const auto & jlinks = lstrips.front()->GetLinks( JOINT );
			alienLinked = !jlinks.empty();
		}
		else
		{
			auto fstrip = lstrips.front();
			auto bstrip = lstrips.back();
			auto doubleLinked = []( const CLogicElement * lstrip )->bool{
				const auto & jlinks = lstrip->GetLinks( JOINT );
				ASSERT( jlinks.size() <= 2 );
				return jlinks.size() > 1;
			};
			alienLinked = doubleLinked( fstrip ) || doubleLinked( bstrip );
		}
	}
	return alienLinked;
}