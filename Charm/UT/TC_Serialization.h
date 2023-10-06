#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

#include "../StatBuilder/GraphicElement.h"

//Проверка сериализации объектов
class TC_Serialization :
	private TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Serialization );
		CPPUNIT_TEST( GraphicSerialize );
		CPPUNIT_TEST( LogicSerialize );
	CPPUNIT_TEST_SUITE_END();

public:
	~TC_Serialization();

protected:
	void GraphicSerialize();
	void LogicSerialize();

private:
	std::set <const CGraphicElement *> GenerateGraphicSet() const;
	std::list <LogElemPtr> GenerateLogicList() const;

	template <typename CT1, typename CT2, typename EqFun>
	bool GraphicTestEquality( const CT1 &, const CT2 &, EqFun ) const;
	template <typename CT1, typename CT2, typename EqFun>
	bool LogicTestEquality( const CT1 &, const CT2 &, EqFun ) const;
};

template <typename CT1, typename CT2, typename EqFun>
bool TC_Serialization::GraphicTestEquality( const CT1 & cont1, const CT2 & cont2, EqFun eqFun ) const
{
	CT2 cont2_loc( cont2 );
	bool equal = ( cont1.size() == cont2_loc.size() );
	if ( equal )
	{
		typedef CT1::const_iterator Cont1It;
		typedef CT2::const_iterator Cont2It;
		for ( Cont1It cont1It = cont1.begin(); cont1It != cont1.end() && equal; ++cont1It )
		{
			Cont2It cont2It = find_if( cont2_loc.begin(), cont2_loc.end(), bind(mem_fn(eqFun), std::placeholders::_1, *cont1It));
			if ( cont2It != cont2_loc.end() )
			{	
				equal = GraphicTestEquality( ( *cont1It )->GetSubObjects(), ( *cont2It )->GetSubObjects(), eqFun );
				cont2_loc.erase( cont2It );
			}
			else
				equal = false;
		}
	}
	return equal;
}

template <typename CT1, typename CT2, typename EqFun>
bool TC_Serialization::LogicTestEquality( const CT1 & cont1, const CT2 & cont2, EqFun eqFun ) const
{
	CT2 cont2_loc( cont2 );
	bool equal = ( cont1.size() == cont2_loc.size() );
	if ( equal )
	{
		typedef CT1::const_iterator Cont1It;
		typedef CT2::const_iterator Cont2It;
		for ( Cont1It cont1It = cont1.begin(); cont1It != cont1.end() && equal; ++cont1It )
		{
			Cont2It cont2It = find_if( cont2_loc.begin(), cont2_loc.end(), bind( eqFun, placeholders::_1, *cont1It ) );
			if ( cont2It != cont2_loc.end() )
			{	
				vector<BadgeE> cont1_subvec = ( *cont1It )->GetSubObjects();
				sort( cont1_subvec.begin(), cont1_subvec.end() );
				vector<BadgeE> cont2_subvec = ( *cont2It )->GetSubObjects();
				sort( cont2_subvec.begin(), cont2_subvec.end() );
				equal = ( cont1_subvec == cont2_subvec );
				cont2_loc.erase( cont2It );
			}
			else
				equal = false;
		}
	}
	return equal;
}