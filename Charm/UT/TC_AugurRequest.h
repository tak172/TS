#pragma once

#include <cppunit/extensions/HelperMacros.h>

class AEvent;
class SpotEvent;
class FixableEvent;
struct LimitationInfo;
class RouteIntrusion;
template <typename T> class TrainRoutes;
class TrainsPriorityOffsets;
class SpanTravelTime;
class TC_AugurRequest : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_AugurRequest );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST( SerializeType );
	CPPUNIT_TEST( DeserializeType );
	CPPUNIT_TEST( DeserializePrioritiesWOIntrusions );
	CPPUNIT_TEST_SUITE_END();

	typedef TrainRoutes<SpotEvent> TrainSpotPaths;
	typedef TrainRoutes<FixableEvent> TrainFixablePaths;
	typedef std::shared_ptr<TrainSpotPaths> TrainSpotPathsPtr;
	typedef std::shared_ptr<TrainFixablePaths> TrainFixablePathsPtr;
	typedef TrainRoutes<AEvent> TrainAEventPaths;
	typedef std::shared_ptr<TrainAEventPaths> TrainAEventPathsPtr;
	typedef std::shared_ptr<TrainsPriorityOffsets> TrainsPriorityOffsetsPtr;
    typedef std::shared_ptr<SpanTravelTime> SpanTravelTimePtr;

	void Serialize();
	void Deserialize();
	void SerializeType();
	void DeserializeType();
	void DeserializePrioritiesWOIntrusions();
    void DeserializeSpanTravelTime();

	TrainFixablePathsPtr MakeInputPaths() const;
	std::vector<LimitationInfo> MakeLimitations() const;
	TrainSpotPathsPtr MakeSchedulePaths() const;
	std::vector<RouteIntrusion> MakeIntrusions() const;
	TrainsPriorityOffsetsPtr MakePriorityOffsets() const;
    SpanTravelTimePtr MakeUserSpanTravelTime() const;

	void TestCurrentMoment( time_t ) const;
	void TestInputPaths( TrainAEventPathsPtr ) const;
	void TestLimitations( const std::vector<LimitationInfo> & ) const;
	void TestSchedulePaths( TrainAEventPathsPtr ) const;
	void TestIntrusions( const std::vector<RouteIntrusion> & ) const;
	void TestPriorityOffsets( TrainsPriorityOffsetsPtr ) const;
    void TestUserSpanTravelTime( SpanTravelTimePtr ) const;
};