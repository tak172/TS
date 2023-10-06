#pragma once

#include "../helpful/Topology.h"
#include "../helpful/LRUCache.h"

typedef LRUCache<std::pair<EsrKit, EsrKit>, std::list<EsrKit>, Topology> LRUCachedTopology;
typedef std::vector<std::list <EsrKit>> VL_ESR;

class TopologyTest : public Topology
{
	std::wstring init(const std::list <EsrKit> &);
	std::wstring initF(const std::list <EsrKit> &);
public:
	using Topology::findPaths;
	TopologyTest( const Topology & other ) : Topology( other ){}
	TopologyTest( const std::list <EsrKit> & );
	TopologyTest( const VL_ESR & , const VL_ESR& fork);

	TopologyTest()
		: Topology(std::shared_ptr<StationsNet>(), std::shared_ptr<TopologyHelper>(), std::wstring() )
	{
	}
	std::wstring Load( std::wistream & iostr );
	std::wstring Load( const std::wstring& data );
};

class LRUCachedTopologyTest : public LRUCachedTopology
{
public:
	LRUCachedTopologyTest::LRUCachedTopologyTest( const std::list <EsrKit> & ecodes = std::list<EsrKit>() ) : 
		LRUCachedTopology( std::shared_ptr<TopologyTest>( new TopologyTest( ecodes ) ) ){}
	LRUCachedTopologyTest::LRUCachedTopologyTest( const VL_ESR & ecodes , const VL_ESR& fork ) : 
		LRUCachedTopology( std::shared_ptr<TopologyTest>( new TopologyTest( ecodes,fork ) ) ){}
};