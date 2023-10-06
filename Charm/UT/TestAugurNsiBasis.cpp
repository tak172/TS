/*!
	created:	2017/12/04
	created: 	20:10  04 Декабрь 2017
	filename: 	F:\potap\Charm\UT\TestNsiBasis.cpp
	file path:	F:\potap\Charm\UT
	file base:	TestNsiBasis
	file ext:	cpp
	author:		 Dremin
	
	purpose: НСИ с доступом на изменение к данным для тестов
		
  */
 
#include "stdafx.h"
#include "TestAugurNsiBasis.h"
#include "../helpful/Junctions.h"
 
using namespace std;

TestAugurNsiBasis::TestAugurNsiBasis() :
	NsiBasis( Rest::Response( std::move( std::unordered_map<std::string, std::string>() ) ) )
{
	stationsRegPtr.reset( new StationsRegistry( map<EsrKit, StationsRegistry::StationInfo>() ) );
	spansRegPtr.reset( new SpansRegistry( map <EsrKit, SpansRegistry::SpanInfo>() ) );
	spanTravTimePtr.reset( new SpanTravelTime() );
	stIntPtr.reset( new ACollStInt() );
	topologyPtr.reset( new LRUCachedTopologyTest() );
}

static std::list<EsrKit> addLine(unsigned from,unsigned next,unsigned to)
{
	list<EsrKit> ecodes;
	ecodes.emplace_back(from);
	ecodes.emplace_back(next);
	ecodes.emplace_back(to);
	return ecodes;
}

TestAugurNsiBasis::TestAugurNsiBasis( const std::map <EsrKit, SpansRegistry::SpanInfo> & spanmap, const std::map<EsrKit, StationsRegistry::StationInfo> & statmap ) : 
	NsiBasis( Rest::Response( std::move( std::unordered_map<std::string, std::string>() ) ) )
{
	junctionsPtr.reset(new Junctions( vector<Junctions::StationUnion>(), vector<Junctions::StationCodes>() ) );
	stationsRegPtr.reset( new StationsRegistry( statmap ) );
	spansRegPtr.reset( new SpansRegistry( spanmap ) );
	spanTravTimePtr.reset( new SpanTravelTime() );
	stIntPtr.reset( new ACollStInt() );
	VL_ESR vC;
	unsigned fork=0;
	std::set<unsigned> node;
	for (auto it=spanmap.cbegin();it!=spanmap.cend();it++)
	{
		auto it1=it;
		for (it1++;it1!=spanmap.cend();it1++)
		if(it->first.getSpan().first==it1->first.getSpan().first)
		{
			node.insert(it->first.getSpan().second);
			node.insert(it1->first.getSpan().second);
			fork=it->first.getSpan().first;
		}
	}
	ASSERT(node.size()<3 && (!fork || node.size()==2));//Может быть не больше одного ветвления
	std::list<EsrKit> ecodes;
	for (const auto& t: statmap)
	{
		auto esr=t.first.getTerm();
		if(esr==fork)
		{
			ecodes.emplace_back(esr);
			vC.push_back(ecodes);
			ecodes.clear();
		}
		else
		{
			if(node.find(esr)!=node.end())
			{
				if(!ecodes.empty())
					vC.push_back(ecodes);
				ecodes.clear();
				ecodes.emplace_back(fork);
			}
			ecodes.emplace_back(esr);
		}
	}
	vC.push_back(ecodes);
	VL_ESR vF;
	if(vC.size()==3)
	{
		vF.push_back(addLine(0,statmap.begin()->first.getTerm(),fork));
		vF.push_back(addLine(statmap.begin()->first.getTerm(),fork,(--statmap.cend())->first.getTerm()));
		vF.push_back(addLine((*node.rbegin())-1,fork,(--statmap.cend())->first.getTerm()));
		vF.push_back(addLine(fork,(--statmap.cend())->first.getTerm(),0));
		vF.push_back(addLine(statmap.begin()->first.getTerm(),fork,(*node.rbegin())-1));
		vF.push_back(addLine(fork,(*node.rbegin())-1,0));
	}
// 	for ( auto wmIt = statmap.cbegin(); wmIt != statmap.cend(); ++wmIt )
// 		ecodes.emplace_back( wmIt->first );
	topologyPtr.reset( new LRUCachedTopologyTest(vC/* ecodes*/ ,vF) );
}

void TestAugurNsiBasis::setDepartureByHemPath(long esr, bool t/*=true*/)
{
	auto all= stationsRegPtr->getEsrCodes();
// 	if(all.find(EsrKit(esr))!=all.end())
// 		all.find(EsrKit(esr))->
}
