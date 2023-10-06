#include "stdafx.h"

#include <vector>
#include <thread>

#include "TC_HemLayer.h"
#include "../Guess/SpotDetails.h"
#include "../Hem/SpotEvent.h"
#include "../Hem/PathManager.h"
#include "../Hem/ScheduledLayer.h"
#include "../Hem/ScheduledPath.h"
#include "../Hem/HappenPath.h"
#include "../helpful/TrainDescr.h"
#include "../Hem/BusyPath.h"
#include "../Hem/BusyEvent.h"
#include "../Hem/AutoIdentifyPolicy.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HemLayer );

// класс хранителя изменений (заглушка)
class UtChangesHolder
{
    void operator() () {};
};

class Ut_Layer : public Hem::PathManager<SpotEvent, Regulatory::ScheduledPath, UtChangesHolder>
{
public:
	Ut_Layer()
		: Hem::PathManager<SpotEvent, Regulatory::ScheduledPath, UtChangesHolder>("TestLayerName", "TestPathName", changesHolder)
	{}

    UtChangesHolder changesHolder;
};

const unsigned c_badgeMultiplier = 100u;
const unsigned c_timeMultiplier = 300u;

class ThreadPool
{
public:
	ThreadPool()
		: pool() 
	{};

	void add(std::thread&& thread) 
	{
		pool.push_back(std::move(thread));
	}

	~ThreadPool() 
	{ 
		for (std::thread& thread : pool)
			if (thread.joinable()) 
				thread.join(); 
	}

private:
	std::vector<std::thread> pool;

	ThreadPool(const ThreadPool&);
	ThreadPool& operator =(const ThreadPool&);
};

void TC_HemLayer::BasicSetup()
{
	Ut_Layer layer;
	BadgeE badge(L"hi", EsrKit(10000));
	
	auto container = layer.getWriteAccess();

	Ut_Layer::ConstEventPtr eventPtr = std::make_shared<SpotEvent>(HCode::ARRIVAL, badge, c_timeMultiplier);
	Ut_Layer::PathPtr pathPtr(new Ut_Layer::Path(110u));
	container->addEvent(eventPtr, pathPtr);

	eventPtr = std::make_shared<SpotEvent>(HCode::DEPARTURE, badge, c_timeMultiplier + 10u);
	pathPtr->PushBack(*eventPtr);
	container->addEvent(eventPtr, pathPtr);

	Ut_Layer::ConstPathSet filteredByTime = container->getByTimeInterval(c_timeMultiplier + 1u, c_timeMultiplier + 11u);
	CPPUNIT_ASSERT(filteredByTime.size() == 1u);

	Ut_Layer::PathPtr removedPathPtr = container->removePath(eventPtr);
	CPPUNIT_ASSERT(removedPathPtr);
	CPPUNIT_ASSERT(container->getByTimeInterval(c_timeMultiplier + 1u, c_timeMultiplier + 11u).empty());
}

void TC_HemLayer::AsyncAddRemove()
{
	const unsigned c_threadCount = 64u;

	Ut_Layer layer;

	CPPUNIT_ASSERT(layer.getReadAccess()->getByTimeInterval(0u, c_timeMultiplier * c_threadCount).empty());

	{
		ThreadPool threads;
		for (unsigned i = 0u; i < c_threadCount; ++i)
		{
			threads.add(std::thread([&layer, i]() {
				BadgeE badge(L"async", EsrKit(i * c_badgeMultiplier));
				Ut_Layer::ConstEventPtr eventPtr	= 
					std::make_shared<SpotEvent>(HCode::ARRIVAL, badge, i * c_timeMultiplier);

				auto writeAccess = layer.getWriteAccess();

				Ut_Layer::PathPtr pathPtr(new Ut_Layer::Path(110u));
				writeAccess->addEvent(eventPtr, pathPtr);
				
				CPPUNIT_ASSERT_MESSAGE("Asynchronous path creation failed", pathPtr);
				eventPtr = std::make_shared<SpotEvent>(HCode::DEPARTURE, 
					badge, i * (c_timeMultiplier + 10u));
				pathPtr->PushBack(*eventPtr);
				writeAccess->addEvent(eventPtr, pathPtr);
			}));
		}
	}

	CPPUNIT_ASSERT_EQUAL(c_threadCount, 
		(unsigned)layer.getReadAccess()->getByTimeInterval(0u, (c_timeMultiplier + 10u) * c_threadCount).size());

	{
		ThreadPool threads;
		for (unsigned i = 0u; i < c_threadCount; ++i)
		{
			threads.add(std::thread([&layer, i]() {
				BadgeE badge(L"async", EsrKit(i * c_badgeMultiplier));
				Ut_Layer::ConstEventPtr eventPtr	= 
					std::make_shared<SpotEvent>(HCode::ARRIVAL, badge, i * c_timeMultiplier);
				auto pathPtr = layer.getWriteAccess()->removePath(eventPtr);
				CPPUNIT_ASSERT_MESSAGE("Asynchronous path removal failed", pathPtr);
			}));
		}
	}

	CPPUNIT_ASSERT(layer.getReadAccess()->getByTimeInterval(0u, (c_timeMultiplier + 10u) * c_threadCount).empty());
}

void TC_HemLayer::ForEach()
{
	const unsigned c_threadCount = 4u;

	Ut_Layer layer;

	CPPUNIT_ASSERT(layer.getReadAccess()->getByTimeInterval(0u, c_timeMultiplier * c_threadCount).empty());

	for (unsigned i = 0u; i < c_threadCount; ++i)
	{
		BadgeE badge(L"async", EsrKit(1 + i * c_badgeMultiplier));
		Ut_Layer::ConstEventPtr eventPtr	= 
			std::make_shared<SpotEvent>(HCode::ARRIVAL, badge, i * c_timeMultiplier);
		
		Ut_Layer::PathPtr pathPtr(new Ut_Layer::Path(110u));
		layer.getWriteAccess()->addEvent(eventPtr, pathPtr);

		CPPUNIT_ASSERT_MESSAGE("Asynchronous path creation failed", pathPtr);

		if (i % 2u)
		{
			eventPtr = std::make_shared<SpotEvent>(HCode::DEPARTURE, 
				badge, i * (c_timeMultiplier + 10u));
			pathPtr->PushBack(*eventPtr);
			layer.getWriteAccess()->addEvent(eventPtr, pathPtr);
		}
	}

	std::vector<std::wstring> descriptions;
	auto collectDescriptions = [&descriptions](const Ut_Layer::Path& path) {
		descriptions.push_back(path.ToString());
	};
	layer.getReadAccess()->forEach(0, (c_timeMultiplier + 10u) * c_threadCount, collectDescriptions);
	CPPUNIT_ASSERT_EQUAL(c_threadCount, (unsigned)descriptions.size());
}

void TC_HemLayer::Timings()
{
	Ut_Layer layer;
	auto access = layer.getWriteAccess();

	CPPUNIT_ASSERT_EQUAL(time_t(0), access->getEarliestTime());
	CPPUNIT_ASSERT_EQUAL(time_t(0), access->getLatestTime());

	EsrKit station(1024u);
	BadgeE badge(L"Timings", station);

	Ut_Layer::PathPtr path(new Ut_Layer::Path(1042u));

	time_t time32 = 32;
	Ut_Layer::ConstEventPtr event(new SpotEvent(HCode::DEPARTURE, badge, time32));
	access->addEvent(event, path);
	CPPUNIT_ASSERT_EQUAL(time32, access->getEarliestTime());
	CPPUNIT_ASSERT_EQUAL(time32, access->getLatestTime());

	// Temporary event and path to be removed lately
	time_t time16 = 16;
	Ut_Layer::ConstEventPtr event16(new SpotEvent(HCode::DEPARTURE, badge, time16));
	Ut_Layer::PathPtr path16(new Ut_Layer::Path(1042u));
	access->addEvent(event16, path16);
	CPPUNIT_ASSERT_EQUAL(time16, access->getEarliestTime());
	CPPUNIT_ASSERT_EQUAL(time32, access->getLatestTime());

	time_t time128 = 128;
	event.reset(new SpotEvent(HCode::DEPARTURE, badge, time128));
	access->addEvent(event, path);
	CPPUNIT_ASSERT_EQUAL(time16, access->getEarliestTime());
	CPPUNIT_ASSERT_EQUAL(time128, access->getLatestTime());

	access->removePath(event16);
	CPPUNIT_ASSERT_EQUAL(time32, access->getEarliestTime());
	CPPUNIT_ASSERT_EQUAL(time128, access->getLatestTime());
}

void TC_HemLayer::BadgeAndTime()
{
	Ut_Layer layer;
	BadgeE badge(L"hi", EsrKit(10000));

	auto container = layer.getWriteAccess();

	Ut_Layer::ConstEventPtr eventPtr = std::make_shared<SpotEvent>(HCode::ARRIVAL, badge, c_timeMultiplier);
	Ut_Layer::PathPtr pathPtr(new Ut_Layer::Path(110u));
	container->addEvent(eventPtr, pathPtr);

	{
		Ut_Layer::Container::EventAddress_ address(*eventPtr);
		Ut_Layer::ConstEventPtr found = container->getByAddress(address);
		CPPUNIT_ASSERT_EQUAL(found, eventPtr);
	}

	{
		Ut_Layer::Container::EventAddress_ address(eventPtr->GetBadge(), eventPtr->GetTime() + 1, eventPtr->GetCode());
		CPPUNIT_ASSERT(!container->getByAddress(address));
	}

	{
		BadgeE badge(L"hi1", EsrKit(10000));
		Ut_Layer::Container::EventAddress_ address(badge, eventPtr->GetTime(), eventPtr->GetCode());
		CPPUNIT_ASSERT(!container->getByAddress(address));
	}
}

void TC_HemLayer::EsrAndTime()
{
	const EsrKit c_esr1(10000);
	const EsrKit c_esrUnused(10001);
	const time_t c_time1 = 24 * 60 * 60;
	const time_t c_time2 = c_time1 + 60 * 10;

	Ut_Layer layer;

	Ut_Layer::ConstEventPtr eventPtr1 = std::make_shared<SpotEvent>(HCode::DEPARTURE, BadgeE(L"BigBossString", c_esr1), c_time1);
	Ut_Layer::ConstEventPtr eventPtr2 = std::make_shared<SpotEvent>(HCode::ARRIVAL, BadgeE(L"NextRandomString", c_esr1), c_time2);
	Ut_Layer::PathPtr pathPtr(new Ut_Layer::Path(110u));

	{
		auto container = layer.getWriteAccess();
		container->addEvent(eventPtr1, pathPtr);
		container->addEvent(eventPtr2, pathPtr);
	}

	{
		auto container = layer.getReadAccess();
		CPPUNIT_ASSERT(container->getByTimeIntervalAndBadgeNumber(c_time1 - 60, c_time1 - 10, c_esr1).empty());
		CPPUNIT_ASSERT(container->getByTimeIntervalAndBadgeNumber(c_time1, c_time2, c_esrUnused).empty());
		CPPUNIT_ASSERT_EQUAL(2u, (unsigned)container->getByTimeIntervalAndBadgeNumber(c_time1, c_time2, c_esr1).size());
		CPPUNIT_ASSERT_EQUAL(1u, (unsigned)container->getByTimeIntervalAndBadgeNumber(c_time1, c_time1, c_esr1).size());
		CPPUNIT_ASSERT_EQUAL(1u, (unsigned)container->getByTimeIntervalAndBadgeNumber(c_time2, c_time2, c_esr1).size());
	}
}

void TC_HemLayer::GetAllPathes()
{
	Ut_Layer layer;
	auto access = layer.getWriteAccess();
	
	EsrKit station(1024u);
	BadgeE badge(L"Timings", station);

	CPPUNIT_ASSERT(access->getAllPathes().empty());

	Ut_Layer::PathPtr path(new Ut_Layer::Path(1042u));

	time_t time32 = 32;
	Ut_Layer::ConstEventPtr event(new SpotEvent(HCode::DEPARTURE, badge, time32));
	access->addEvent(event, path);

	CPPUNIT_ASSERT_EQUAL(1u, (unsigned)access->getAllPathes().size());

	// Temporary event and path to be removed lately
	time_t time16 = 16;
	Ut_Layer::ConstEventPtr event16(new SpotEvent(HCode::DEPARTURE, badge, time16));
	Ut_Layer::PathPtr path16(new Ut_Layer::Path(1042u));
	access->addEvent(event16, path16);

	CPPUNIT_ASSERT_EQUAL(2u, (unsigned)access->getAllPathes().size());

	time_t time128 = 128;
	event.reset(new SpotEvent(HCode::DEPARTURE, badge, time128));
	access->addEvent(event, path);

	CPPUNIT_ASSERT_EQUAL(2u, (unsigned)access->getAllPathes().size());

	access->removePath(event16);

	CPPUNIT_ASSERT_EQUAL(1u, (unsigned)access->getAllPathes().size());
}

void TC_HemLayer::RemoveEvents()
{
    UtChangesHolder changesHolder;
    Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("layerName", "pathName", changesHolder);
    TrainDescr trainInfo(L"N1379S", L"INDEX");
    SpotDetailsCPtr spotDetails(new SpotDetails(trainInfo));
    HemHelpful::SpotEventPtr firstSpotPtr(new SpotEvent(HCode::FORM, BadgeE(L"BDG", EsrKit(10000)), 60, spotDetails));
    std::shared_ptr<Hem::HappenPath> path(new Hem::HappenPath(firstSpotPtr));
    container.addEvent(firstSpotPtr, path);
    HemHelpful::SpotEventPtr secondSpotPtr(new SpotEvent(HCode::DEPARTURE, BadgeE(L"BDD", EsrKit(10000)), 65));
    path->DoAppend(secondSpotPtr);
    container.addEvent(secondSpotPtr, path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path->GetInfoSpots().size());
    auto events = path->GetAllEvents();
    CPPUNIT_ASSERT_EQUAL(size_t(2), events.size());
    CPPUNIT_ASSERT_EQUAL(time_t(60), events.front()->GetTime());

    CPPUNIT_ASSERT(container.removeEvents(firstSpotPtr, firstSpotPtr));
    CPPUNIT_ASSERT_EQUAL(size_t(0), path->GetInfoSpots().size());
    events = path->GetAllEvents();
    CPPUNIT_ASSERT_EQUAL(size_t(1), events.size());
    CPPUNIT_ASSERT_EQUAL(time_t(65), events.front()->GetTime());
}

void TC_HemLayer::ReplaceEvents()
{
    UtChangesHolder changesHolder;
	Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("layerName", "pathName", changesHolder);
	TrainDescr trainInfo(L"N1379S", L"1-1-1");
	SpotDetailsCPtr spotDetails(new SpotDetails(trainInfo));
	HemHelpful::SpotEventPtr firstSpotPtr(new SpotEvent(HCode::FORM, BadgeE(L"BDG", EsrKit(10000)), 60, spotDetails));
	std::shared_ptr<Hem::HappenPath> path(new Hem::HappenPath(firstSpotPtr));
	container.addEvent(firstSpotPtr, path);
	HemHelpful::SpotEventPtr secondSpotPtr(new SpotEvent(HCode::DEPARTURE, BadgeE(L"BDD", EsrKit(10000)), 65));
	path->DoAppend(secondSpotPtr);
	container.addEvent(secondSpotPtr, path);
	CPPUNIT_ASSERT_EQUAL(size_t(1), path->GetInfoSpots().size());
	auto events = path->GetAllEvents();
	CPPUNIT_ASSERT_EQUAL(size_t(2), events.size());
	CPPUNIT_ASSERT_EQUAL(time_t(60), events.front()->GetTime());

	std::vector<SpotEvent> replace;
	replace.emplace_back(HCode::FORM, BadgeE(L"BDG", EsrKit(10000)), 50);
    std::wstring errmsg;
	CPPUNIT_ASSERT(container.replaceEvents(firstSpotPtr, firstSpotPtr, replace, errmsg));
	CPPUNIT_ASSERT_EQUAL(size_t(0), path->GetInfoSpots().size());
	events = path->GetAllEvents();
	CPPUNIT_ASSERT_EQUAL(size_t(2), events.size());
	CPPUNIT_ASSERT_EQUAL(time_t(50), events.front()->GetTime());
}

void TC_HemLayer::ReplaceEventsRefuse()
{
    BadgeE BUM(L"KUDD2", EsrKit(1234,4321));
    UtChangesHolder changesHolder;
    Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("layerName", "pathName",changesHolder);

    HemHelpful::SpotEventPtr m1( new SpotEvent(HCode::SPAN_MOVE, BUM, 3600) );
    std::shared_ptr<Hem::HappenPath> path(new Hem::HappenPath(m1));
    container.addEvent(m1, path);

    HemHelpful::SpotEventPtr d2(new SpotEvent(HCode::DEATH, BUM, 3600));
    path->DoAppendCopy(*d2);
    container.addEvent(d2, path);

    HemHelpful::SpotEventPtr f3(new SpotEvent(HCode::FORM, BUM, 3600));
    path->DoAppendCopy(*f3);
    container.addEvent(f3, path);

    HemHelpful::SpotEventPtr m4(new SpotEvent(HCode::SPAN_MOVE, BUM, 3677));
    path->DoAppendCopy(*m4);
    container.addEvent(m4, path);

    // проверка предусловий
    auto events = path->GetAllEvents();
    CPPUNIT_ASSERT( 4 == events.size() );
    CPPUNIT_ASSERT( 3600 == events.front()->GetTime() );
    CPPUNIT_ASSERT( 3677 == events.back() ->GetTime() );
    // проверка отказа при замене 
    std::vector<SpotEvent> replace;
    std::wstring errmsg;
    replace.emplace_back(HCode::SPAN_MOVE, BUM, 3600);
    replace.emplace_back(HCode::SPAN_MOVE, BUM, 3600);
    CPPUNIT_ASSERT(!container.replaceEvents(d2, f3, replace, errmsg));
}

void TC_HemLayer::ConcatenatePathes()
{
    UtChangesHolder changesHolder;
	Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("UT", "UT_Path", changesHolder);

	using namespace HemHelpful;
	BadgeE badge0(L"UT", EsrKit(1379));
	time_t time0(100);

	SpotEventPtr p0s0(new SpotEvent(HCode::FORM, badge0, time0));
	SpotEventPtr p0s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 1));
	std::shared_ptr<Hem::HappenPath> path0(new Hem::HappenPath(p0s0));
	path0->DoAppendCopy(*p0s1);
	container.addEvent(p0s0, path0);
	container.addEvent(p0s1, path0);

    std::vector<SpotEventPtr> bridge;

	CPPUNIT_ASSERT(!container.concatenatePath(path0, p0s1, bridge, identifyCategory, p0s1, nullptr));
	CPPUNIT_ASSERT(!container.concatenatePath(path0, p0s1, bridge, identifyCategory, p0s1, path0));
	CPPUNIT_ASSERT(!container.concatenatePath(nullptr, p0s1, bridge, identifyCategory, p0s1, path0));
	
	SpotEventPtr p1s0(new SpotEvent(HCode::ARRIVAL, badge0, time0 + 2));
	SpotEventPtr p1s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 3));
	std::shared_ptr<Hem::HappenPath> path1(new Hem::HappenPath(p1s0));
	path1->DoAppendCopy(*p1s1);
	container.addEvent(p1s0, path1);
	container.addEvent(p1s1, path1);
	CPPUNIT_ASSERT(container.getPath(p1s1) == path1);
	
	CPPUNIT_ASSERT(!container.concatenatePath(path1, p1s1, bridge, identifyCategory, p0s0, path0));
	CPPUNIT_ASSERT(container.getPath(p0s0) == path0);
	CPPUNIT_ASSERT(container.getPath(p0s1) == path0);
	CPPUNIT_ASSERT(container.getPath(p1s0) == path1);
	CPPUNIT_ASSERT(container.getPath(p1s1) == path1);

	CPPUNIT_ASSERT(container.concatenatePath(path0, p0s1, bridge, identifyCategory, p1s0, path1));
	CPPUNIT_ASSERT(container.getPath(p1s0) == path1);
	CPPUNIT_ASSERT(container.getPath(p1s1) == path1);

	// Попробуем связить нитки, если одна из них не добавлена в контейнер
	SpotEventPtr p2s0(new SpotEvent(HCode::TRANSITION, badge0, time0 + 4));
	SpotEventPtr p2s1(new SpotEvent(HCode::TRANSITION, badge0, time0 + 5));
	SpotEventPtr p2s2(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 6));
	std::shared_ptr<Hem::HappenPath> path2(new Hem::HappenPath(p2s0));
	path2->DoAppendCopy(*p2s1);
	// Забыли добавить события в контейнер
	// container.addEvent(p2s0, path2);
	// container.addEvent(p2s1, path2);
	// container.addEvent(p2s2, path2);

	CPPUNIT_ASSERT(!container.concatenatePath(path0, p1s1, bridge, identifyCategory, p2s0, path2));
	CPPUNIT_ASSERT_EQUAL(size_t(2), path2->GetAllEvents().size());

	// В текущей реализации принимается на веру присутствие первой нити в контейнере
	// Такие конструкции возможны, хотя и маловероятны
	// CPPUNIT_ASSERT_THROW(container.concatenatePath(path2, path0), HemException);
	// CPPUNIT_ASSERT(!container.concatenatePath(path2, path0));
}

void TC_HemLayer::PushBackCopy()
{
    UtChangesHolder changesHolder;
    Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("UT", "UT_Container",changesHolder);
    SpotEvent spotForm(HCode::EXPLICIT_FORM, BadgeE(L"TMP", EsrKit(1000)), 50);
    CPPUNIT_ASSERT_THROW(container.pushBackCopy(spotForm, nullptr), HemHelpful::HemException);

    HemHelpful::SpotEventPtr spotFormPtr(new SpotEvent(spotForm));
    std::shared_ptr<Hem::HappenPath> path(new Hem::HappenPath(spotFormPtr));
    container.addEvent(spotFormPtr, path);
    CPPUNIT_ASSERT(container.getPath(spotFormPtr) == path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path->GetEventsCount());
    CPPUNIT_ASSERT(*path->GetLastEvent() == spotForm);

    CPPUNIT_ASSERT(!container.pushBackCopy(spotForm, path));
    CPPUNIT_ASSERT(container.getPath(spotFormPtr) == path);
    CPPUNIT_ASSERT_EQUAL(size_t(1), path->GetEventsCount());
    CPPUNIT_ASSERT(*path->GetLastEvent() == spotForm);

    SpotEvent spotDep(HCode::DEPARTURE, BadgeE(L"TMP", EsrKit(1000)), 55);
    HemHelpful::SpotEventPtr spotDepPtr = container.pushBackCopy(spotDep, path);
    CPPUNIT_ASSERT(spotDepPtr);
    CPPUNIT_ASSERT(container.getPath(spotFormPtr) == path);
    CPPUNIT_ASSERT(container.getPath(spotDepPtr) == path);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path->GetEventsCount());
    CPPUNIT_ASSERT(*path->GetLastEvent() == spotDep);

    CPPUNIT_ASSERT(!container.pushBackCopy(spotDep, path));
    CPPUNIT_ASSERT(container.getPath(spotFormPtr) == path);
    CPPUNIT_ASSERT(container.getPath(spotDepPtr) == path);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path->GetEventsCount());
    CPPUNIT_ASSERT(*path->GetLastEvent() == spotDep);
}

void TC_HemLayer::PushBack()
{
    UtChangesHolder changesHolder;
    Hem::PathContainer<Busy::Event, Busy::Path, UtChangesHolder> container("UT", "UT_Container",changesHolder);

    Busy::EventPtr eventPtr(new Busy::Event(HCode::FORM, BadgeE(L"TMP", EsrKit(1000)), 500, ParkWayKit()));
    std::shared_ptr<Busy::Path> pathPtr(new Busy::Path());
    CPPUNIT_ASSERT(container.pushBack(eventPtr, pathPtr));
    CPPUNIT_ASSERT_EQUAL(size_t(1), pathPtr->GetEventsCount());
    CPPUNIT_ASSERT(container.getPath(eventPtr) == pathPtr);

    CPPUNIT_ASSERT(!container.pushBack(eventPtr, pathPtr));
    CPPUNIT_ASSERT_EQUAL(size_t(1), pathPtr->GetEventsCount());
    CPPUNIT_ASSERT(container.getPath(eventPtr) == pathPtr);
}


void TC_HemLayer::SeparatePathBeginning()
{
    UtChangesHolder changesHolder;
    Hem::PathContainer<SpotEvent, Hem::HappenPath, UtChangesHolder> container("UT", "UT_Path", changesHolder);

    using namespace HemHelpful;
    BadgeE badge0(L"UT", EsrKit(1379));
    time_t time0(100);

    SpotEventPtr p0s0(new SpotEvent(HCode::FORM, badge0, time0));
    SpotEventPtr p0s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 1));
    SpotEventPtr p1s0(new SpotEvent(HCode::ARRIVAL, badge0, time0 + 2));
    SpotEventPtr p1s1(new SpotEvent(HCode::DEPARTURE, badge0, time0 + 3));

    std::shared_ptr<Hem::HappenPath> path1(new Hem::HappenPath(p0s0));
    container.registerEvent(p0s0, path1);
    container.pushBack(p0s1, path1);
    container.pushBack(p1s0, path1);
    container.pushBack(p1s1, path1);

    path1->IdentifyWith(p1s1, TrainDescr(L"1379", L"1111-222-3333"), identifyCategory, Hem::Chance::System(), Hem::SoftIdentifyPolicy());
    CPPUNIT_ASSERT(path1->GetLastFragment());
    CPPUNIT_ASSERT_EQUAL(1379u, path1->GetLastFragment()->GetNumber().getNumber());

    std::vector<SpotEvent> prefix;
    prefix.push_back(SpotEvent(HCode::FORM, badge0, time0 + 2));
    HemHelpful::SpotEventPtr ptrFirstEvent = std::make_shared<SpotEvent>(prefix.front());
    CPPUNIT_ASSERT(!container.getPath(ptrFirstEvent));

    std::shared_ptr<Hem::HappenPath> path0 = container.separatePathBeginning(p1s0, prefix, true);
    CPPUNIT_ASSERT(path0);
    CPPUNIT_ASSERT_EQUAL(size_t(2), path0->GetEventsCount());
    CPPUNIT_ASSERT_EQUAL(size_t(3), path1->GetEventsCount());

    CPPUNIT_ASSERT(path1->GetLastFragment());
    CPPUNIT_ASSERT_EQUAL(1379u, path1->GetLastFragment()->GetNumber().getNumber());

    CPPUNIT_ASSERT(path0->GetLastFragment());
    CPPUNIT_ASSERT_EQUAL(1379u, path0->GetLastFragment()->GetNumber().getNumber());

    CPPUNIT_ASSERT(container.getPath(p0s1) == path0);
    CPPUNIT_ASSERT(container.getPath(p0s0) == path0);

    CPPUNIT_ASSERT(container.getPath(p1s1) == path1);
    CPPUNIT_ASSERT(container.getPath(p1s0) == path1);
    CPPUNIT_ASSERT(container.getPath(ptrFirstEvent) == path1);
}