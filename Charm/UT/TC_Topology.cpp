#include "stdafx.h"
#include <boost/algorithm/string.hpp>
#include "TC_Topology.h"
#include "StationsNetTest.h"
#include "TopologyTest.h"
#include "UtReadFile.h"
#include "../helpful/StationsDigraph.h"
#include "../helpful/Serialization.h"
#include "../helpful/FotoFinish.h"
#include "../helpful/SynonymsResolver.h"


using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Topology );

void TC_Topology::IncorrectRequest()
{
	SynonymsResolver synoResolver;
	StationsNetTest::StationsDigraph digraph( synoResolver );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(100), StationEsr(200) );
	digraph.FixDirectedTrio( StationEsr(200), StationEsr(100), StationEsr() );
	digraph.Construct();
	const list <DFSVisitorT::VtxSequencePtr> & retpaths = digraph.FindAllPaths( EsrKit( 100 ), EsrKit( 500 ) );
	CPPUNIT_ASSERT( retpaths.empty() );
}

void TC_Topology::StationsDigraphCrotch()
{
	SynonymsResolver synoResolver;
	StationsNetTest::StationsDigraph digraph( synoResolver );
	digraph.FixDirectedTrio( StationEsr(100), StationEsr(200), StationEsr(300) );
	digraph.FixDirectedTrio( StationEsr(100), StationEsr(200), StationEsr(400) );
	digraph.FixDirectedTrio( StationEsr(200), StationEsr(300), StationEsr(500) );
	digraph.FixDirectedTrio( StationEsr(200), StationEsr(400), StationEsr(500) );
	digraph.FixDirectedTrio( StationEsr(300), StationEsr(500), StationEsr() );
	digraph.FixDirectedTrio( StationEsr(400), StationEsr(500), StationEsr() );

	//зеркально
	digraph.FixDirectedTrio( StationEsr(300), StationEsr(200), StationEsr(100) );
	digraph.FixDirectedTrio( StationEsr(400), StationEsr(200), StationEsr(100) );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(500), StationEsr(300) );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(500), StationEsr(400) );
	digraph.Construct();
	const list <DFSVisitorT::VtxSequencePtr> & retpaths = digraph.FindAllPaths( EsrKit( 100 ), EsrKit( 500 ) );
	CPPUNIT_ASSERT( retpaths.size() == 2 );
}

void TC_Topology::StationsDigraphUnidir()
{
	SynonymsResolver synoResolver;
	StationsNetTest::StationsDigraph digraph( synoResolver );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(100), StationEsr(400) );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(100), StationEsr(200) );
	digraph.FixDirectedTrio( StationEsr(200), StationEsr(100), StationEsr() );
	digraph.FixDirectedTrio( StationEsr(200), StationEsr(500), StationEsr() );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(500), StationEsr(200) );
	digraph.FixDirectedTrio( StationEsr(400), StationEsr(500), StationEsr() );
	digraph.FixDirectedTrio( StationEsr(), StationEsr(500), StationEsr(400) );
	digraph.FixDirectedTrio( StationEsr(100), StationEsr(400), StationEsr(500) );
	digraph.FixDirectedTrio( StationEsr(100), StationEsr(200), StationEsr(500) );
	digraph.FixDirectedTrio( StationEsr(500), StationEsr(200), StationEsr(100) );
	list <StationEsr> subpath;
	subpath.push_back( StationEsr(200) );
	subpath.push_back( StationEsr(300) );
	subpath.push_back( StationEsr(500) );
	digraph.FixSubpath( subpath );
	digraph.Construct();
	const list <DFSVisitorT::VtxSequencePtr> & dirpaths = digraph.FindAllPaths( EsrKit( 100 ), EsrKit( 500 ) );
	CPPUNIT_ASSERT( dirpaths.size() == 2 );
	const list <DFSVisitorT::VtxSequencePtr> & retpaths = digraph.FindAllPaths( EsrKit( 500 ), EsrKit( 100 ) );
	CPPUNIT_ASSERT( retpaths.size() == 1 );
}

void TC_Topology::StationsNetCrotch()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "=100,200,400" << endl;
	str << "=300,500,600" << endl;
	str << "=400,500,600" << endl;
	str << "=200,300,500" << endl;
	str << "=200,400,500" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list <list <EsrKit> > & retpaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 600 ) );
	CPPUNIT_ASSERT( retpaths.size() == 2 );
}

void TC_Topology::StationsNetDoubleCrotch()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "=100,200,400" << endl;
	str << "=200,300,500" << endl;
	str << "=200,400,500" << endl;
	str << "=300,500,600" << endl;
	str << "=400,500,600" << endl;
	str << "=500,600,700" << endl;
	str << "=500,600,800" << endl;
	str << "=600,700,900" << endl;
	str << "=600,800,900" << endl;
	str << "=700,900,1000" << endl;
	str << "=800,900,1000" << endl;

	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list <list <EsrKit> > & retpaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 1000 ) );
	CPPUNIT_ASSERT( retpaths.size() == 4 );
}

void TC_Topology::StationsNetUnidir()
{
	wstringstream str;
	str << "*100,200 {100,101,102,103,200}" << endl;
	str << "-100,200,900" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list <list <EsrKit> > & subdirpaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 103 ) );
	CPPUNIT_ASSERT( subdirpaths.size() == 1 );
	const list <list <EsrKit> > & subretpaths = stationsNet.findPaths( EsrKit( 103 ), EsrKit( 100 ) );
	CPPUNIT_ASSERT( subretpaths.size() == 1 );
	const list <list <EsrKit> > & extdirpaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 900 ) );
	CPPUNIT_ASSERT( extdirpaths.size() == 1 );
	const list <list <EsrKit> > & extretpaths = stationsNet.findPaths( EsrKit( 900 ), EsrKit( 100 ) );
	CPPUNIT_ASSERT( extretpaths.empty() );
}

void TC_Topology::StationsNetSynonyms()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "=100,200,400" << endl;
	str << "=200,300,500" << endl;
	str << "=200,400,500" << endl;
	str << "=300,500,600" << endl;
	str << "=400,500,600" << endl;
	str << "$300=301,302,303" << endl;
	str << "$600=601,605,610" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list <list <EsrKit> > & paths1 = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 302 ) );
	CPPUNIT_ASSERT( paths1.size() == 1 );
	const list <list <EsrKit> > & paths2 = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 610 ) );
	CPPUNIT_ASSERT( paths2.size() == 2 );
}

void TC_Topology::Commentaries()
{
	wstringstream str;
	str << "//комментарий additional commentary" << endl;
	str << "=100, 200, 300" << endl;
	str << "//comme ntary2" << endl;
	str << "//commentary 300" << endl;
	str << "= 100,200, 400" << endl;
	str << "//commentary 4" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list <list <EsrKit> > & paths1 = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 300 ) );
	CPPUNIT_ASSERT( paths1.size() == 1 );
	const list <list <EsrKit> > & paths2 = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 400 ) );
	CPPUNIT_ASSERT( paths2.size() == 1 );
}

void TC_Topology::StationsNetSpanToSpan()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "=100,200,400" << endl;
	str << "=200,300,500" << endl;
	str << "=0,100,200" << endl;
	str << "-200,400,500" << endl;
	str << "=300,500,600" << endl;
	str << "=400,500,600" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );

	const list <list <EsrKit> > & retpaths = stationsNet.findPaths( EsrKit( 200, 400 ), EsrKit( 400, 500 ) );
	CPPUNIT_ASSERT( retpaths.size() == 1 );
	const list <list <EsrKit> > & retpaths2 = stationsNet.findPaths( EsrKit( 100, 200 ), EsrKit( 500, 600 ) );
	CPPUNIT_ASSERT( retpaths2.size() == 2 );
}

void TC_Topology::ReversiveTrios()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "=400,200,100" << endl;

	TopologyTest stationsNet;
	stationsNet.Load( str );

	const list <list <EsrKit> > & retpaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 300 ) );
	CPPUNIT_ASSERT( retpaths.size() == 1 );
	const list <list <EsrKit> > & retpaths2 = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 400 ) );
	CPPUNIT_ASSERT( retpaths2.size() == 1 );
	const list <list <EsrKit> > & retpaths3 = stationsNet.findPaths( EsrKit( 400 ), EsrKit( 300 ) );
	CPPUNIT_ASSERT( retpaths3.empty() );
}

void TC_Topology::SingleSpanSearch()
{
    wstringstream str;
	str << "*100,200 {100,102,103,104,105,106,200}" << endl;
	str << "=300,100,200" << endl;

    TopologyTest stationsNet;
    stationsNet.Load( str );
    // нахождение цепочки внутри 
    const list <list <EsrKit> > & inner = stationsNet.findPaths( EsrKit( 102 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( inner.size() == 1 );
    // нахождение цепочки от края внутрь
    const list <list <EsrKit> > & edged  = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( edged.size() == 1 );
    // нахождение цепочки от края до края
    const list <list <EsrKit> > & whole = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 200 ) );
    CPPUNIT_ASSERT( whole.size() == 1 );
}

void TC_Topology::FileTopologyTest()
{
    std::string data_1251 = UtFileBody( L"E:/WorkCopy.all/Sample/logics/Topology.stn" );
    boost::algorithm::replace_all( data_1251, "\r\n", "\n" );
    std::wstring topo_data = From1251( data_1251 );

    CPPUNIT_ASSERT( !topo_data.empty() );
    {
        std::wistringstream wss( topo_data );
        TopologyTest stationsNet;
        auto errstr = stationsNet.Load( wss );
        CPPUNIT_ASSERT( errstr.empty() );
        // нахождение цепочки внутри 
        typedef std::tuple<EsrKit, EsrKit, unsigned> FROM_TO_COUNT;
        std::vector< FROM_TO_COUNT > speedData;
        speedData.emplace_back( std::make_tuple(EsrKit( 9150),EsrKit( 9000), 3) );
        speedData.emplace_back( std::make_tuple(EsrKit(11310),EsrKit( 9050), 8) );
        speedData.emplace_back( std::make_tuple(EsrKit( 9190),EsrKit(16000), 5) );
        speedData.emplace_back( std::make_tuple(EsrKit(11445),EsrKit(16000), 5) );
        speedData.emplace_back( std::make_tuple(EsrKit( 9750),EsrKit(16000), 29) );
        speedData.emplace_back( std::make_tuple(EsrKit(11419),EsrKit( 9640), 6) );
        speedData.emplace_back( std::make_tuple(EsrKit(11010),EsrKit( 9820), 19) );
        speedData.emplace_back( std::make_tuple(EsrKit(11010),EsrKit(11000), 4) );

        FotoFinish ff(__FUNCTIONW__, [](const std::wstring& ws){
            OutputDebugString(ws.c_str());
        }, boost::posix_time::milliseconds(100) );

        for( FROM_TO_COUNT sd : speedData )
        {
            const list <list <EsrKit> > & inner = stationsNet.findPaths( get<0>(sd), get<1>(sd) );
            CPPUNIT_ASSERT( inner.size() >= get<2>(sd) );
            const list <list <EsrKit> > & rever = stationsNet.findPaths( get<1>(sd), get<0>(sd) );
        }


        const list <list <EsrKit> > & inner = stationsNet.findPaths( EsrKit( 11420 ), EsrKit( 11000 ) );
        CPPUNIT_ASSERT( inner.size() >= 1 );
    }
}

void TC_Topology::MultiPaths()
{
	wstringstream str;
	str << "=11445,09180,11420" << endl;
	str << "=09180,11420,11445" << endl;
	str << "=11420,11445,11451" << endl;
	str << "=09180,11445,11451" << endl;
	str << "=11445,11451,11000";
	TopologyTest stationsNet;
	stationsNet.Load( str );

	auto allPaths = stationsNet.findPaths( EsrKit( 11420 ), EsrKit( 11000 ) );
	CPPUNIT_ASSERT( allPaths.size() == 2 );
}

void TC_Topology::StationPassingBid()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "*200,300 {200,231,232,233,300}" << endl;
	str << "=400,500,300" << endl;
	str << "*500,300 {500,531,532,533,300}" << endl;

	TopologyTest stationsNet;
	stationsNet.Load( str );

	auto allPaths = stationsNet.findPaths( EsrKit( 100 ), EsrKit( 400 ) );
	CPPUNIT_ASSERT( allPaths.empty() );
	allPaths = stationsNet.findPaths( EsrKit( 232 ), EsrKit( 532 ) );
	CPPUNIT_ASSERT( allPaths.empty() );
}

void TC_Topology::BidirectionSearch()
{
    // данные одинаковые, но в прямом и обратном порядке
    char * bidirectional_data[] = {
        "*803,700 {803,802,801,800,700}",
        "*700,803 {700,800,801,802,803}",
    };

    std::vector<size_t> numberOfPaths;
    for( auto data : bidirectional_data )
    {
        wstringstream str;
        str << data << endl;

        TopologyTest stationsNet;
        stationsNet.Load( str );

        auto allPaths = stationsNet.findPaths( EsrKit( 802, 803 ), EsrKit( 800 ) );
        CPPUNIT_ASSERT( 1 == allPaths.size() ); // при поиске в любом направлении должны найти ровно один путь
    }
}

void TC_Topology::Deserialize()
{
    CPPUNIT_ASSERT(nullptr == deserialize<Topology>("").get());
    CPPUNIT_ASSERT(nullptr == deserialize<Topology>("<Topology></Topology>"));

    std::string content = "<Topology><Graph>*100,200 {100,102,103,104,105,106,200}\n"
                          "=300,100,200</Graph></Topology>";

    std::shared_ptr<TopologyTest> topology = deserialize<TopologyTest>(content);
    CPPUNIT_ASSERT(topology);

    // нахождение цепочки внутри 
    const list <list <EsrKit> > & inner = topology->findPaths( EsrKit( 102 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( inner.size() == 1 );
    // нахождение цепочки от края внутрь
    const list <list <EsrKit> > & edged  = topology->findPaths( EsrKit( 100 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( edged.size() == 1 );
    // нахождение цепочки от края до края
    const list <list <EsrKit> > & whole = topology->findPaths( EsrKit( 100 ), EsrKit( 200 ) );
    CPPUNIT_ASSERT( whole.size() == 1 );
}

void TC_Topology::DeserializeHelper()
{
    std::string content = "<Topology>"
                          "<Graph>*100,200 {100,102,103,104,105,106,200}\n"
                          "=300,100,200</Graph>"
                          "<Helper>10001-10002..10003-10004</Helper>"
                          "</Topology>";

    std::shared_ptr<TopologyTest> topology = deserialize<TopologyTest>(content);
    CPPUNIT_ASSERT(topology);

    // нахождение цепочки внутри 
    const list <list <EsrKit> > & inner = topology->findPaths( EsrKit( 102 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( inner.size() == 1 );
    // нахождение цепочки от края внутрь
    const list <list <EsrKit> > & edged  = topology->findPaths( EsrKit( 100 ), EsrKit( 104 ) );
    CPPUNIT_ASSERT( edged.size() == 1 );
    // нахождение цепочки от края до края
    const list <list <EsrKit> > & whole = topology->findPaths( EsrKit( 100 ), EsrKit( 200 ) );
    CPPUNIT_ASSERT( whole.size() == 1 );

    std::wstring warning;
    EsrKit adjacent = topology->findAdjacentStation(EsrKit(10001), EsrKit(10004), true, warning);
    CPPUNIT_ASSERT(adjacent.filled());
    CPPUNIT_ASSERT(adjacent == EsrKit(10002));
}

void TC_Topology::PathBetweenSynonyms()
{
	wstringstream str;
	str << "=100,200,300" << endl;
	str << "$100=101" << endl;
	str << "$200=201" << endl;
	str << "$300=301" << endl;
	TopologyTest stationsNet;
	stationsNet.Load( str );
	const list<list<EsrKit> > & paths = stationsNet.findPaths( EsrKit( 101 ), EsrKit( 301 ) );
	CPPUNIT_ASSERT( paths.size() == 1 );
	const auto & retpath = paths.front();
	CPPUNIT_ASSERT( retpath.size() == 5 );
	vector<EsrKit> pathVec( retpath.cbegin(), retpath.cend() );
	CPPUNIT_ASSERT( pathVec[0] == EsrKit( 101 ) );
	CPPUNIT_ASSERT( pathVec[1] == EsrKit( 101, 200 ) );
	CPPUNIT_ASSERT( pathVec[2] == EsrKit( 200 ) );
	CPPUNIT_ASSERT( pathVec[3] == EsrKit( 200, 301 ) );
	CPPUNIT_ASSERT( pathVec[4] == EsrKit( 301 ) );
}

void TC_Topology::ShortestPath() // поиск кратчайшего пути
{
    wstringstream str;
    str << L"=11001,11002,11451\n"  // это меньший маршрут по длине
        << L"=11001,11002,11071\n"  // это меньший маршрут лексикографически
        << L"=11002,11071,11076\n"
        << L"=11071,11076,11075\n"
        << L"=11076,11075,11771\n"
        << L"=11075,11771,11445\n"
        << L"=11771,11445,11451\n"
        << L"=11445,11451,11002\n";

    // требуемый маршрут 11001 - 11001:11002 - 11002 - 11002:11451
    // неверный маршрут  11001 - 11001:11002 - 11002 - 11002:11071 - 11071 - 11071:11076 - 11076 - 11075 - 11771 - 11445 - 11451 - 11002:11451

    TopologyTest stationsNet;
    stationsNet.Load( str );

    EsrKit from(11001);
    EsrKit to( 11002,11451 );

    auto ft = stationsNet.findPrimaryPath( from, to );
    CPPUNIT_ASSERT( ft.size()==4 );
    auto tf = stationsNet.findPrimaryPath( to, from );
    CPPUNIT_ASSERT( tf.size()==4 );
}

void TC_Topology::WishPath() // поиск пути через указанные станции
{
    // возможные пути
    // 1-2-3-----4-5
    // 1-2----7----5
    // 1-2-3-8-9-4-5
    wstringstream str;
    str << L"=10001,10002,10003\n" // узлы для 12345
        << L"=10002,10003,10004\n"
        << L"=10003,10004,10005\n"
        << L"=10001,10002,10007\n" // узлы для 1275
        << L"=10002,10007,10005\n"
        << L"=10001,10002,10003\n" // узлы для 1238945
        << L"=10002,10003,10008\n"
        << L"=10003,10008,10009\n"
        << L"=10008,10009,10004\n"
        << L"=10009,10004,10005\n";

    TopologyTest stationsNet;
    stationsNet.Load( str );

    EsrKit from(10001);
    EsrKit to(  10005 );
    std::set<EsrKit> wish;
    // нет пожеланий - короткий путь 1275
    wish.clear();
    auto ft = stationsNet.findWishPath( from, to, wish );
    CPPUNIT_ASSERT( ft.size()==4*2-1 ); // с перегонами
    // через 234
    wish.clear();
    wish.insert( EsrKit(10002) );
    wish.insert( EsrKit(10003) );
    wish.insert( EsrKit(10004) );
    ft = stationsNet.findWishPath( from, to, wish );
    CPPUNIT_ASSERT( ft.size()==5*2-1 ); // с перегонами
    // через 689
    wish.clear();
    wish.insert( EsrKit(10006) );
    wish.insert( EsrKit(10008) );
    wish.insert( EsrKit(10009) );
    ft = stationsNet.findWishPath( from, to, wish );
    CPPUNIT_ASSERT( ft.size()==7*2-1 ); // с перегонами
}
