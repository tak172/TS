#include "stdafx.h"

#include "TC_LOS_landscape.h"
#include "../Actor/LOSLandscape.h"
#include "BuildTrainDescr.h"

using namespace std;
using namespace boost::posix_time;


CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_landscape );

void TC_LOS_landscape::setUp()
{
}
void TC_LOS_landscape::tearDown()
{
}

void TC_LOS_landscape::test_path()
{
    using namespace Landscape;
    vector<Path> exP = generate_Path();

    boost::scoped_ptr<LOS_base> landscape( LOS_base::fabric_from_object(LANDSCAPE) );
    CPPUNIT_ASSERT( landscape->get_Subitems<Path>().empty() );

    CPPUNIT_ASSERT( landscape->insert_Subitem<Path>(exP.front()) );
    CPPUNIT_ASSERT( landscape->insert_Subitem<Path>(exP.back()) );

    vector<Path> vPath = landscape->get_Subitems<Path>();
    CPPUNIT_ASSERT( vPath.size()==exP.size() );
    CPPUNIT_ASSERT( vPath.front() == exP.front() );
    CPPUNIT_ASSERT( vPath.back()  == exP.back()  );

    landscape->reset_signals();
    //CPPUNIT_ASSERT( landscape->get_Subitems<Path>().size()==exP.size() );
}

void TC_LOS_landscape::test_pe()
{
    using namespace Landscape;
    const vector<PeMark> exM = generate_PeMark();
    boost::scoped_ptr<LOS_base> los( LOS_base::fabric_from_object(LANDSCAPE) );
	auto landlos = static_cast <LOS_landscape *>( los.get() );
    // начальное
	auto paths = landlos->get_Subitems<Path>();
	CPPUNIT_ASSERT( paths.empty() );
    // добавление
    for( const PeMark& pm : exM )
    {
		static int pathNum = 1;
		landlos->insert_Path( Path( pathNum ) );
		landlos->insert_PeMark( pm, pathNum++ );
    }
	auto paths2 = landlos->get_Subitems<Path>();
	vector<PeMark> tstMarks;
	for ( auto path : paths2 )
	{
		auto pathMarks = path.get_data().get_marks();
		tstMarks.insert( tstMarks.end(), pathMarks.cbegin(), pathMarks.cend() );
	}
    CPPUNIT_ASSERT( exM == tstMarks );
    // очистка        
    landlos->reset_signals();
	auto paths3 = landlos->get_Subitems<Path>();
	for ( auto path : paths3 )
	{
		auto pathMarks = path.get_data().get_marks();
		CPPUNIT_ASSERT( pathMarks.empty() );
	}
}

void TC_LOS_landscape::test_bounds()
{
    using namespace Landscape;
    wstring ruler;
    rwCoord l,r;
    generate_Bounds(&ruler,&l,&r);

    boost::scoped_ptr<LOS_base> landscape( LOS_base::fabric_from_object(LANDSCAPE) );
    static_cast<LOS_landscape&>(*landscape).setBound(ruler, l, r );
    wstring ruler2;
    rwCoord l2,r2;
    static_cast<LOS_landscape&>(*landscape).getBound(&ruler2, &l2, &r2 );
    CPPUNIT_ASSERT(ruler==ruler2);
    CPPUNIT_ASSERT(l==l2);
    CPPUNIT_ASSERT(r==r2);
}

vector<Landscape::Path> TC_LOS_landscape::generate_Path()
{
    using namespace Landscape;
    vector<Path> vP;

    Path way1(1);
    Measure m1(rwCoord("345~100"),7000);
    Measure m2(rwCoord("345~300"),7911);
    Measure m3(rwCoord("345~400"),7380);
    way1.append_sample( m1,m2, 200,"1.1" );
    way1.append_sample( m2,m3, 100,"-2.2" );
    vP.push_back(way1);

    Path way2(2);
    Measure n1(rwCoord("345~100"),7000);
    Measure n2(rwCoord("345~300"),7922);
    Measure n3(rwCoord("345~400"),7380);
    way2.append_sample( n1,n2, 200,"1.3" );
    way2.append_sample( n2,n3, 100,"-2.5" );
    vP.push_back(way2);

    return vP;
}

vector<Landscape::PeMark> TC_LOS_landscape::generate_PeMark()
{
    using namespace Landscape;
    vector<PeMark> vM;
    PeMark pm;

    pm.pe = BuildTrain( 78, L"1-1-1", L"АБ", 60, 1111, 111, L"0:0" );
    pm.pos = rwCoord("345~120");
    //pm.path_num = 9;
    vM.push_back(pm);

    pm.pe = BuildTrain( 6565, L"1-1-1", L"ПМ", 19, 2222, 222, L"77" );
    pm.pos = rwCoord("345~700");
    //pm.path_num = 11;
    vM.push_back(pm);

    return vM;
}

void TC_LOS_landscape::generate_Bounds( std::wstring* ruler, rwCoord* l, rwCoord* r )
{
    *ruler = L"Кудыкино";
    *l = rwCoord("345~70");
    *r = rwCoord("345~700");
}
