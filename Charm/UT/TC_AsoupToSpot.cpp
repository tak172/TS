#include "stdafx.h"
#include "TC_AsoupToSpot.h"
#include "../Hem/AsoupLinker.h"
#include "../Hem/HappenLayer.h"
#include "../Hem/AsoupLayer.h"
#include "../Hem/RegulatoryLayer.h"
#include "../Hem/StationEvents.h"
#include "../Hem/AutoIdentifyPolicy.h"
#include "../Hem/UserIdentifyPolicy.h"
#include "../Guess/SpotDetails.h"
#include "UtHelpfulDateTime.h"
#include "../helpful/EsrGuide.h"
#include "TopologyTest.h"
#include "../Hem/UserEditInventory.h"
#include "../Asoup/MessagePassengerLDZ.h"
#include "../helpful/TrainSpecialNotes.h"
#include "../helpful/NsiBasis.h"
#include "../Hem/Appliers/aeAsoupToSpot.h"
#include "../helpful/Serialization.h"
#include "../Hem/GriffinUtility.h"
#include "UtNsiBasis.h"
#include "../Hem/GuessTransciever.h"
#include "../Hem/Appliers/Context.h"
#include "../helpful/Junctions.h"
#include "../helpful/Crew.h"
#include "../Hem/XlatEsrParkWay.h"
#include "../Hem/UserChart.h"
#include "../helpful/NsiBasis.h"
#include "../Hem/Appliers/aeAttach.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_AsoupToSpot );

using namespace Hem;

typedef AsoupToSpot::SpotPtr SpotPtr;
typedef AsoupToSpot::AsoupPtr AsoupPtr;
typedef AsoupToSpot::SpotAndPath SpotAndPath;
typedef AsoupToSpot::PathPtr PathPtr;
typedef std::pair<BadgeE, ParkWayKit> BadgeParkWay;

static Hem::Chance sysChance = Hem::Chance::System();

TC_AsoupToSpot::TC_AsoupToSpot(void)
{
}

TC_AsoupToSpot::~TC_AsoupToSpot(void)
{
}

void TC_AsoupToSpot::setUp()
{
    topology.reset( new TopologyTest );
    esrGuide.reset( new EsrGuide );
    guessTransciever.reset( new GuessTransciever );
    m_happenLayer.reset(new HappenLayer());
    m_asoupLayer.reset(new AsoupLayer());
    m_regular.reset(new Regulatory::Layer());
}

void TC_AsoupToSpot::tearDown()
{
    m_happenLayer.reset();
    m_asoupLayer.reset();
    m_regular.reset();
    esrGuide.reset();
    topology.reset();
    DistrictSection::Shutdowner();
}

// Создание события асоуп по событию нити с регистрацией в слое
AsoupPtr createAsoup(AsoupLayer& layer, const SpotEvent& spot, unsigned number, 
                     const std::wstring& index, time_t deltaTime)
{
    HCode code = spot.GetCode();
    if (HCode::FORM == code)
        code = HCode::EXPLICIT_FORM;
    BadgeE const badge(L"AsoupString", spot.GetBadge().num());
    AsoupPtr asoup(new AsoupEvent(code, badge, spot.ParkWay(), 
        spot.GetTime() + deltaTime, TrainDescr(std::to_wstring(number), index)));
    layer.AddAsoupEvent(asoup);
    CPPUNIT_ASSERT(layer.FindEvent(*asoup));
    return asoup;
}

// Создание события асоуп с регистрацией в слое
AsoupPtr createAsoup(AsoupLayer& layer, HCode code, const EsrKit& esr, time_t time,
                     unsigned number, const std::wstring& index, const ParkWayKit& parkway = ParkWayKit(),
                     const std::wstring& adjacentFrom = std::wstring(), const std::wstring& adjacentTo = std::wstring())
{
    BadgeE const badge(L"AsoupString", esr);
    AsoupPtr asoup(new AsoupEvent(code, badge, parkway, time, TrainDescr(std::to_wstring(number), index), adjacentFrom, adjacentTo));
    layer.AddAsoupEvent(asoup);
    CPPUNIT_ASSERT(layer.FindEvent(*asoup));
    return asoup;
}

AsoupPtr createAsoup( AsoupLayer& layer, std::shared_ptr<TopologyTest> topology, const std::wstring& str, boost::gregorian::date& receivedData )
{
    auto msg = Asoup::Message::parse(str, receivedData, Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(msg);
    auto asoup = AsoupEvent::Create(*msg, topology.get(), nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(asoup);
    layer.AddAsoupEvent(asoup);
    CPPUNIT_ASSERT(layer.FindEvent(*asoup));
    return asoup;
}

// Создание события нити без регистрации в слое
SpotPtr createSpot(HCode code, time_t time, const BadgeParkWay& badgeParkWay)
{
    SpotDetailsCPtr details;
    if (!badgeParkWay.second.empty())
        details.reset(new SpotDetails(badgeParkWay.second, std::pair<PicketingInfo, PicketingInfo>()));
    return std::make_shared<SpotEvent>(code, badgeParkWay.first, time, details);
}

// Используемый для генерации нити инкремент времени,
// реализующий межстанционные перегоны или интервалы внутри станций
void increaseTime(time_t& time, bool interStationMove)
{
    time_t const c_interStation = 10 * 60;
    time_t const c_lesserMovements = 60;

    time += interStationMove ? c_interStation : c_lesserMovements;
}

// Создание в слое нитки с типовыми событиями для указанных кодов ЕСР, парков и путей.
// Если установлен флаг generateSpanMoves, между станциями будут созданы события движения по перегону.
PathPtr createPath(HappenLayer& layer, const std::vector<BadgeParkWay>& badgeParkWayVec, time_t time, 
                                               bool generateSpanMoves)
{
    if (badgeParkWayVec.empty())
        throw std::runtime_error("Expected to get badges, parks and ways");

    SpotPtr form = createSpot(HCode::FORM, time, badgeParkWayVec.front());
    PathPtr path(new HappenPath(form));

    auto writeAccess = layer.GetWriteAccess();
    writeAccess->addEvent(form, path);
    CPPUNIT_ASSERT(writeAccess->getPath(form) == path);

    for (auto it = badgeParkWayVec.cbegin(); it != badgeParkWayVec.cend(); ++it)
    {
        if (it->first.num().terminal())
        {
            // Передан код станционный
            auto const nextIt = std::next(it);
            if (badgeParkWayVec.cend() == nextIt)
            {
                // Последнее событие - прибытие
                increaseTime(time, true);
                SpotPtr arrival = createSpot(HCode::ARRIVAL, time, *it);
                CPPUNIT_ASSERT(writeAccess->pushBack(arrival, path));
            }
            else
            {
                // Не конец нити
                if (badgeParkWayVec.cbegin() == it)
                {
                    // Отправление после формирования
                    increaseTime(time, false);
                    SpotPtr departure = createSpot(HCode::DEPARTURE, time, *it);
                    CPPUNIT_ASSERT(writeAccess->pushBack(departure, path));
                }
                else
                {
                    // Проследование в середине нити
                    increaseTime(time, true);
                    SpotPtr transition = createSpot(HCode::TRANSITION, time, *it);
                    CPPUNIT_ASSERT(writeAccess->pushBack(transition, path));
                }
                
                if (generateSpanMoves && nextIt->first.num().terminal())
                {
                    // Межстанционные движения по перегону
                    increaseTime(time, false);
                    BadgeParkWay spanBadge = std::make_pair(BadgeE(L"SPAN", 
                        EsrKit(it->first.num().getTerm(), nextIt->first.num().getTerm())),
                        ParkWayKit());
                    SpotPtr spanMove = createSpot(HCode::SPAN_MOVE, time, spanBadge);
                    CPPUNIT_ASSERT(writeAccess->pushBack(spanMove, path));
                }
            }
        }
        else
        {
            // Игнорируем generateSpanMoves?
            increaseTime(time, false);
            SpotPtr spanMove = createSpot(HCode::SPAN_MOVE, time, *it);
            CPPUNIT_ASSERT(writeAccess->pushBack(spanMove, path));
        }
    }

    return path;
}

PathPtr createPath(HappenLayer& layer, const std::wstring& str)
{
    attic::a_document doc;
    doc.load_wide( str );
    auto parentNode = doc.document_element();

    PathPtr path;
    auto writeAccess = layer.GetWriteAccess();
    for ( auto chi = parentNode.child(SpotEvent_xAttr); chi; chi=chi.next_sibling(SpotEvent_xAttr) )
    {
        SpotPtr pSpot(new SpotEvent(chi));
        if ( !path )
        {
            path.reset(new HappenPath(pSpot));
            writeAccess->addEvent(pSpot, path);
            CPPUNIT_ASSERT(writeAccess->getPath(pSpot) == path);
        }
        else
            CPPUNIT_ASSERT(writeAccess->pushBack(pSpot, path));
    }
    return path;
}


void TC_AsoupToSpot::identify()
{
    StrictIdentifyPolicy strictPolicy;
    
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, timeForDate(2015, 10, 10));
    AsoupLinker linker(happenAccess, asoupAccess, context, strictPolicy, nullptr);

    CPPUNIT_ASSERT_THROW(linker.identify(AsoupPtr(), SpotAndPath()), HemHelpful::HemException);

    EsrKit const c_esr(1000);
    ParkWayKit const c_parkWay(1, 1);
    time_t const c_time = timeForDate(2016, 12, 13);
    TrainDescr const c_descr(L"222", L"2223-222-2224");

    AsoupPtr asoup(new AsoupEvent(HCode::EXPLICIT_FORM, BadgeE(L"Asoup", c_esr), c_parkWay, c_time, c_descr));
    m_asoupLayer->AddAsoupEvent(asoup);

    CPPUNIT_ASSERT_THROW(linker.identify(asoup, SpotAndPath()), HemHelpful::HemException);

    SpotPtr spot(new SpotEvent(HCode::FORM, BadgeE(L"Ч1", c_esr), timeForDate(2015, 10, 10), 
        std::make_shared<SpotDetails>(c_parkWay, std::pair<PicketingInfo, PicketingInfo>())));
    SpotAndPath spotAndPath = std::make_pair(spot, std::make_shared<HappenPath>(spot));
    happenAccess->registerEvent(spotAndPath.first, spotAndPath.second);

    CPPUNIT_ASSERT(linker.identify(asoup, spotAndPath));

    ConstTrainDescrPtr pathFragment = spotAndPath.second->GetLastFragment();
    CPPUNIT_ASSERT(pathFragment);
    CPPUNIT_ASSERT(*pathFragment == c_descr);

    // Какие нити были проидентифицированы таким же индексом
    AsoupToSpot asoupToSpot(strictPolicy, 0);
    AsoupToSpot::PathSet pathes = asoupToSpot.findIdentifiedPathes(happenAccess, asoupAccess, *m_regular, *asoup);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pathes.size());
    CPPUNIT_ASSERT(*pathes.cbegin() == spotAndPath.second);

    // Какая нить содержит привязку АСОУП
    CPPUNIT_ASSERT(spotAndPath.second == AsoupToSpot::findLinkedPath(happenAccess, asoupAccess, *asoup).second);
}

std::vector<BadgeParkWay> createPathData(size_t length, unsigned shift = 0)
{
    std::vector<BadgeParkWay> result;
    result.reserve(length);

    unsigned const c_startStationEsr = 1000;
    unsigned const c_nextStationIncrement = 100;

    unsigned esr = c_startStationEsr + shift * c_nextStationIncrement;
    for (size_t i = 0u; i < length; ++i)
    {
        result.push_back(std::make_pair(BadgeE(L"UT", EsrKit(esr)), ParkWayKit(1, 1)));
        esr += c_nextStationIncrement;
    }

    return result;
}

void TC_AsoupToSpot::identifyChaining()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2016, 12, 16), true);

    const time_t oldFirstMoment = path->GetFirstEvent()->GetTime();
    AsoupPtr form = createAsoup(*m_asoupLayer, *path->GetFirstEvent(), 1340u, L"1000-134-2000", -30);
    AsoupPtr arrival = createAsoup(*m_asoupLayer, *path->GetLastEvent(), 1340u, L"1000-134-2000", 30);
    const int c_relativeLength = 77;
    {
        // добавляем данные подвижной единицы в событие отправления (не в первое событие формирования!)
        TrainDescr td(L"1340", L"1000-134-2000");
        td.SetRelLength( c_relativeLength );
        auto events = path->GetAllEvents();
        path->SetInfoToSpot( *next(events.begin()), td );
    }

    // до увязки с АСОУП на первом событии нити нет данных
    CPPUNIT_ASSERT( !path->GetFragment( path->GetFirstEvent() ) );
    CPPUNIT_ASSERT( path->GetInfoSpots().size() == 1 );

    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(arrival));

	Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    {
	auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    AsoupLinker linker(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
	path = linker.identify(arrival, std::make_pair(path->GetLastEvent(), path));
    CPPUNIT_ASSERT( path );
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(arrival));
	}
    
	{
		Hem::ApplierCarryOut::aeAttach_NearesrtAsoup att( m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), 
			context, path, arrival );
		att.DoAction();
		path = att.getResult();
	}
	
	CPPUNIT_ASSERT(m_asoupLayer->IsServed(form));
    // после увязки с формированием АСОУП будет
    // - изменено первое событие (время)
    // - полные данные ПЕ со второго события перейдут на первое (тот же номер и индекс)
    const time_t newFirstMoment = path->GetFirstEvent()->GetTime();
    CPPUNIT_ASSERT( newFirstMoment + 30 == oldFirstMoment );
    CPPUNIT_ASSERT( path->GetInfoSpots().size() == 1 );
    ConstTrainDescrPtr ptd = path->GetFragment( path->GetFirstEvent() );
    CPPUNIT_ASSERT( ptd ); 
    CPPUNIT_ASSERT( ptd->GetRelLength() == c_relativeLength );
}

void TC_AsoupToSpot::identifyDeparture()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2016, 12, 16), true);

    SpotEvent formSpot = *path->GetFirstEvent();
    CPPUNIT_ASSERT(HCode::FORM == formSpot.GetCode());
    AsoupPtr form = createAsoup(*m_asoupLayer, formSpot, 1340u, L"1000-134-2000", -30);
    SpotEvent departureSpot = *path->GetNextEvent(formSpot);
    CPPUNIT_ASSERT(HCode::DEPARTURE == departureSpot.GetCode());
    AsoupPtr departure = createAsoup(*m_asoupLayer, departureSpot, 1340u, L"1000-134-2000", 30);

    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(departure));

    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linker(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);

    CPPUNIT_ASSERT(linker.identify(departure, std::make_pair(path->GetLastEvent(), path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(departure));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(form));

    // При отправлении со станции проверяются все АСОУП "формирование" с тем же индексом
    // и осуществляется подмена формирования в нити на новое событие
    SpotEvent explicitFormSpot = *path->GetFirstEvent();
    CPPUNIT_ASSERT(HCode::EXPLICIT_FORM == explicitFormSpot.GetCode());
    CPPUNIT_ASSERT(departureSpot == *path->GetNextEvent(explicitFormSpot));
}

void TC_AsoupToSpot::identifyArrival()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2016, 12, 16), true);

    SpotEvent arrivalSpot = *path->GetLastEvent();
    CPPUNIT_ASSERT(HCode::ARRIVAL == arrivalSpot.GetCode());
    AsoupPtr arrival = createAsoup(*m_asoupLayer, arrivalSpot, 1340u, L"1000-134-2000", 30);
    
    AsoupPtr disform = createAsoup(*m_asoupLayer, HCode::DISFORM, arrivalSpot.GetBadge().num(),
        arrivalSpot.GetTime() + 300, 1340u, L"1000-134-2000", ParkWayKit());

    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(disform));

    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linker(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);

    CPPUNIT_ASSERT(linker.identify(arrival, std::make_pair(path->GetLastEvent(), path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(disform));
    ConstTrainDescrPtr fragment = path->GetLastFragment();
    CPPUNIT_ASSERT(fragment);
    CPPUNIT_ASSERT(*fragment == *arrival->GetDescr());

    // При идентификации по прибытию на станцию используются 
    // сообщения о расформировании поезда, порождаются события
    SpotEvent disformSpot = *path->GetLastEvent();
    CPPUNIT_ASSERT(HCode::DISFORM == disformSpot.GetCode());
    CPPUNIT_ASSERT(disformSpot == *path->GetNextEvent(arrivalSpot));
    CPPUNIT_ASSERT(disformSpot.ParkWay() == arrivalSpot.ParkWay());
    CPPUNIT_ASSERT(!disformSpot.ParkWay().empty());
}

void TC_AsoupToSpot::unlink()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2016, 12, 16), true);

    AsoupPtr form = createAsoup(*m_asoupLayer, *path->GetFirstEvent(), 1340u, L"1000-134-2000", -30);
    AsoupPtr arrival = createAsoup(*m_asoupLayer, *path->GetLastEvent(), 1340u, L"1000-134-2000", 30);
    AsoupPtr manifest = createAsoup(*m_asoupLayer, HCode::INFO_CHANGING, arrival->GetBadge().num(), arrival->GetTime() + 40, 1340u, L"1000-134-2000");

    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(manifest));

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    
	{
		AsoupLinker linker(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), context, SoftIdentifyPolicy(), nullptr);

		path = linker.identify(arrival, std::make_pair(path->GetLastEvent(), path));
		CPPUNIT_ASSERT( path );
		CPPUNIT_ASSERT(m_asoupLayer->IsServed(arrival));

		path = linker.identify(manifest, std::make_pair(path->GetLastEvent(), path));
		CPPUNIT_ASSERT(m_asoupLayer->IsServed(manifest));
	}

	{
		Hem::ApplierCarryOut::aeAttach_NearesrtAsoup att( m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), 
			context, path, arrival );
		att.DoAction();
		path = att.getResult();

		CPPUNIT_ASSERT(m_asoupLayer->IsServed(form));
	   ConstTrainDescrPtr fragment = path->GetLastFragment();
    CPPUNIT_ASSERT(fragment);
    CPPUNIT_ASSERT(*fragment == *arrival->GetDescr());
	}


	{
		AsoupLinker linker(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), context, SoftIdentifyPolicy(), nullptr);
		linker.unlink(*arrival);

		CPPUNIT_ASSERT(!m_asoupLayer->IsServed(form));
		CPPUNIT_ASSERT(!m_asoupLayer->IsServed(arrival));
		CPPUNIT_ASSERT(!m_asoupLayer->IsServed(manifest));

		CPPUNIT_ASSERT(!path->GetLastFragment());
	}
}

class UtUserChart : public UserChart
{
public:
	UserChart::getHappenLayer;
	UserChart::getAsoupLayer;
	UserChart::SimilarFreeAsoupToQueue;
};


void TC_AsoupToSpot::raiiUnlinker()
{
	UtUserChart m_chart;
	auto& hl = m_chart.getHappenLayer();
	auto& al = m_chart.getAsoupLayer();
	PathPtr path = createPath(hl, createPathData(10u), timeForDate(2016, 12, 16), true);

    std::wstring const c_index = L"1000-134-2000";
    unsigned const c_validNumber = 1340u;
    unsigned const c_wrongNumber = 999u;

	SpotPtr spot;
	for ( spot=path->GetLastEvent(); spot && !HemEventRegistry::isDepartureCode( spot->GetCode() ); spot = path->GetPreviousEvent(*spot) )
		;
	CPPUNIT_ASSERT( spot );

    AsoupPtr form = createAsoup(al, *path->GetFirstEvent(), c_wrongNumber, c_index, -30);
    AsoupPtr departure = createAsoup(al, *spot, c_wrongNumber, c_index, 30);
    AsoupPtr manifest = createAsoup(al, HCode::INFO_CHANGING, departure->GetBadge().num(), departure->GetTime() + 40, c_wrongNumber, c_index);
	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 3 );
		CPPUNIT_ASSERT(!al.IsServed(departure));
		CPPUNIT_ASSERT(!al.IsServed(form));
		CPPUNIT_ASSERT(!al.IsServed(manifest));
	}

	time_t currentTime = departure->GetTime() + 40;
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, currentTime);
	{
    AsoupLinker linker(hl.GetWriteAccess(), al.GetWriteAccess(), context, SoftIdentifyPolicy(), nullptr);

	path = linker.identify(departure, std::make_pair(spot, path));
    CPPUNIT_ASSERT( path );
    CPPUNIT_ASSERT(al.IsServed(departure));
	}
	
	LinkFreeAsoup(departure, m_chart, currentTime + 120);
	CPPUNIT_ASSERT(al.IsServed(form));
	CPPUNIT_ASSERT(al.IsServed(manifest));
	//после склейки указатель может быть невалидный
	CPPUNIT_ASSERT( path );


    {
        // Удаление всех фрагментов из нити
        AsoupUnlinkerRaii unlinker(al.GetWriteAccess(), path);
        path->RemoveFragmentsIf([c_index](const TrainDescr& fragment) {
            return fragment.GetIndex().str() == c_index;
        });
    }

    CPPUNIT_ASSERT(!al.IsServed(form));
    CPPUNIT_ASSERT(!al.IsServed(departure));
    CPPUNIT_ASSERT(!al.IsServed(manifest));
	
	{
		AsoupLinker linker(hl.GetWriteAccess(), al.GetWriteAccess(), context, SoftIdentifyPolicy(), nullptr);

	path = linker.identify(departure, std::make_pair(spot, path));
    CPPUNIT_ASSERT( path );
    CPPUNIT_ASSERT(al.IsServed(departure));
	}
	
	LinkFreeAsoup(departure, m_chart, currentTime + 120);

	CPPUNIT_ASSERT(al.IsServed(departure));
	CPPUNIT_ASSERT(al.IsServed(form));
	CPPUNIT_ASSERT(al.IsServed(manifest));

    path->SetInfoToSpot(spot, TrainDescr(std::to_wstring(c_validNumber), c_index));
    CPPUNIT_ASSERT(*path->GetLastFragment() != *departure->GetDescr());

	CPPUNIT_ASSERT(al.IsServed(departure));
	CPPUNIT_ASSERT(al.IsServed(form));
	CPPUNIT_ASSERT(al.IsServed(manifest));

    {
        // Удаление части фрагментов из нити (фрагмент с индексом остаётся)
        AsoupUnlinkerRaii unlinker(al.GetWriteAccess(), path);
        path->RemoveFragmentsIf([c_wrongNumber](const TrainDescr& fragment) {
            return fragment.GetNumber().getNumber() == c_wrongNumber;
        });
        CPPUNIT_ASSERT_EQUAL(c_validNumber, path->GetLastFragment()->GetNumber().getNumber());
    }

	CPPUNIT_ASSERT(!al.IsServed(departure));
	CPPUNIT_ASSERT(!al.IsServed(form));
	CPPUNIT_ASSERT(!al.IsServed(manifest));

	LinkFreeAsoup(departure, m_chart, currentTime);
	m_chart.updateTime(currentTime + 40, nullptr);

    CPPUNIT_ASSERT(al.IsServed(departure));
    CPPUNIT_ASSERT(al.IsServed(form));
    CPPUNIT_ASSERT(al.IsServed(manifest));
}

void TC_AsoupToSpot::LinkFreeAsoup(AsoupPtr asoup, UtUserChart &m_chart, time_t currentTime)
{
	AsoupLayer::ConstAsoupSet vUsed;
	vUsed.insert(asoup);
	m_chart.SimilarFreeAsoupToQueue(vUsed, currentTime);
	m_chart.updateTime(currentTime, nullptr);
}

void TC_AsoupToSpot::raiiUnlinkerDouble()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2016, 12, 16), true);

    std::wstring const c_index = L"1000-134-2000";
    unsigned const c_validNumber = 1340u;
    unsigned const c_wrongNumber = 999u;

    AsoupPtr form = createAsoup(*m_asoupLayer, *path->GetFirstEvent(), c_wrongNumber, c_index, -30);

    AsoupPtr arrival = createAsoup(*m_asoupLayer, *path->GetLastEvent(), c_wrongNumber, c_index, 30);
    AsoupPtr manifest = createAsoup(*m_asoupLayer, HCode::INFO_CHANGING, arrival->GetBadge().num(), arrival->GetTime() + 40, c_wrongNumber, c_index);

    std::wstring const c_secondIndex = L"1000-134-1020";
    unsigned const c_secondNumber = 1344u;
    AsoupPtr secondAsoup = createAsoup(*m_asoupLayer, *path->GetLastEvent(), c_secondNumber, c_secondIndex, 0);

    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linker(happenAccess, asoupAccess, context, SoftIdentifyPolicy(), nullptr);

    CPPUNIT_ASSERT(linker.identify(arrival, std::make_pair(path->GetLastEvent(), path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(manifest));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(secondAsoup));

    std::unique_ptr<AsoupUnlinkerRaii> unlinker(new AsoupUnlinkerRaii(asoupAccess, path));
    unlinker.reset();    

    CPPUNIT_ASSERT(m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(manifest));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(secondAsoup));

    linker.identify(secondAsoup, std::make_pair(path->GetLastEvent(), path));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(secondAsoup));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(arrival));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(form));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(manifest));
}

void TC_AsoupToSpot::gluingPathesOnSameStation()
{
    unsigned const c_path0length = 6u;
    std::vector<BadgeParkWay> path0data = createPathData(c_path0length);
    std::vector<BadgeParkWay> path1data = createPathData(10u, c_path0length - 1u);
    CPPUNIT_ASSERT(path0data.back().first == path1data.front().first);

    time_t const c_startTime = timeForDate(2016, 12, 19);
    std::wstring const c_index = L"1000-123-2000";
    PathPtr path0 = createPath(*m_happenLayer, path0data, c_startTime, true);
    path0->SetInfoToSpot(path0->GetFirstEvent(), TrainDescr(L"", c_index));

    std::wstring const c_number = L"123";
    PathPtr path1 = createPath(*m_happenLayer, path1data, path0->GetLastEvent()->GetTime() + 180, true);
    
    // Удаление события формирования для возможности склейки
    // ToDo: Нужно реализовать склейку вне зависимости от наличия формирования
    m_happenLayer->GetWriteAccess()->removeEvent(path1->GetFirstEvent());
    path1->SetInfoToSpot(path1->GetFirstEvent(), TrainDescr(c_number, c_index));

    size_t const c_expectedLength = path0->GetEventsCount() + path1->GetEventsCount();

    // Не должно ничего измениться
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path0, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL(size_t(2), readAccess->getAllPathes().size());
    }

    SpotPtr lastEvent = path0->GetLastEvent();
    m_happenLayer->GetWriteAccess()->pushBack(
        std::make_shared<SpotEvent>(HCode::DEATH, lastEvent->GetBadge(), lastEvent->GetTime()),
        path0);
    CPPUNIT_ASSERT(HCode::DEATH == path0->GetLastEvent()->GetCode());

    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path0, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL(size_t(1), readAccess->getAllPathes().size());

        CPPUNIT_ASSERT_EQUAL(c_expectedLength, path1->GetEventsCount());
        CPPUNIT_ASSERT(readAccess->getPath(path1->GetFirstEvent()) == path1);
        CPPUNIT_ASSERT(readAccess->getPath(path1->GetLastEvent()) == path1);
    }
}

void TC_AsoupToSpot::gluingPathesTerminalNearSpan()
{
    PathPtr path1 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200314T013928Z' name='Form' Bdg='N1P_PAS[06610:11290]' waynum='1' index='0000-014-0001' num='0001' length='25' weight='813' fastyear='Y'>"
        L"<rwcoord picketing1_val='290~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='280~200' picketing1_comm='Рига-Зилупе' />"
        L"<Locomotive Series='ТЭП70' NumLoc='250' Depo='956' Consec='1' CarrierCode='27'>"
        L"<Crew EngineDriver='OЛЬШEBCKИЙ' TabNum='17996' Tim_Beg='2020-03-13 19:21' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T014045Z' name='Arrival' Bdg='1C[11290]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200314T022903Z' name='Departure' Bdg='1C[11290]' waynum='1' parknum='1' optCode='11290:11291' />"
        L"<SpotEvent create_time='20200314T024222Z' name='Transition' Bdg='1C[11291]' waynum='1' parknum='1' intervalSec='37' optCode='11291:11292' />"
        L"<SpotEvent create_time='20200314T024909Z' name='Station_entry' Bdg='NAP[11292]'>"
        L"<rwcoord picketing1_val='259~113' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T024924Z' name='Death' Bdg='1-3SP[11292]' />"
        L"</HemPath>"
        );
    PathPtr path2 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200314T021151Z' name='Form' Bdg='P6P[11291:11292]' waynum='1' index='0000-014-0001' num='0001' length='25' weight='813' fastyear='Y'>"
        L"<rwcoord picketing1_val='263~750' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='265~' picketing1_comm='Рига-Зилупе' />"
        L"<Locomotive Series='ТЭП70' NumLoc='250' Depo='956' Consec='1' CarrierCode='27'>"
        L"<Crew EngineDriver='OЛЬШEBCKИЙ' TabNum='17996' Tim_Beg='2020-03-13 19:21' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T021209Z' name='Span_stopping_begin' Bdg='P6P[11291:11292]' waynum='1'>"
        L"<rwcoord picketing1_val='263~750' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='265~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T024549Z' name='Span_stopping_end' Bdg='P6P[11291:11292]' waynum='1'>"
        L"<rwcoord picketing1_val='263~750' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='265~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T024952Z' name='Transition' Bdg='1C[11292]' waynum='1' parknum='1' intervalSec='28' optCode='11292:11300' />"
        L"<SpotEvent create_time='20200314T025703Z' name='Transition' Bdg='1C[11300]' waynum='1' parknum='1' intervalSec='23' optCode='11300:11301' />"
        L"<SpotEvent create_time='20200314T030540Z' name='Transition' Bdg='1C[11301]' waynum='1' parknum='1' intervalSec='27' optCode='11301:11302' />"
        L"<SpotEvent create_time='20200314T030849Z' name='Transition' Bdg='1C[11302]' waynum='1' parknum='1' intervalSec='21' optCode='11302:11320' />"
        L"<SpotEvent create_time='20200314T031226Z' name='Transition' Bdg='1PG[11320]' waynum='1' parknum='1' intervalSec='68' />"
        L"<SpotEvent create_time='20200314T031431Z' name='Arrival' Bdg='1C[11310]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200314T033058Z' name='Departure' Bdg='1AC[11310]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200314T033432Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200314T034227Z' name='Span_move' Bdg='N1PC[11311:11321]' waynum='1'>"
        L"<rwcoord picketing1_val='212~715' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='213~330' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( path1 && path1->GetEventsCount()==6 );
    CPPUNIT_ASSERT( path2 && path2->GetEventsCount()==12 );
    //склейка невозможна из-за противоположного направления нитей
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path2, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL( size_t(2), readAccess->getAllPathes().size() );
    }
}

void TC_AsoupToSpot::dontGluingPairedWithoutIndexOnSameStation()
{
    unsigned const c_path0length = 6u;
    std::vector<BadgeParkWay> path0data = createPathData(c_path0length);
    std::vector<BadgeParkWay> path1data = createPathData(10u, c_path0length - 1u);
    CPPUNIT_ASSERT(path0data.back().first == path1data.front().first);

    TrainDescr td0(L"6233");
    TrainDescr td1(L"6234");
    CPPUNIT_ASSERT( td0.GetNumber().isPairedWith( td1.GetNumber() ) );

    PathPtr path0 = createPath(*m_happenLayer, path0data, timeForDate(2016, 12, 19), true);
    path0->SetInfoToSpot(path0->GetFirstEvent(), td0);
    
    {
        auto ee = path0->GetLastEvent();
        auto disf = std::make_shared<SpotEvent>( HCode::DISFORM, ee->GetBadge(), ee->GetTime()+60, ee->GetDetails() );
        CPPUNIT_ASSERT( m_happenLayer->GetWriteAccess()->pushBack( disf, path0 ) );
    }

    PathPtr path1 = createPath(*m_happenLayer, path1data, path0->GetLastEvent()->GetTime() + 180, true);
    path1->SetInfoToSpot(path1->GetFirstEvent(), td1);

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path1, context);
    CPPUNIT_ASSERT_EQUAL( size_t(2), m_happenLayer->GetReadAccess()->getAllPathes().size() );
}

void TC_AsoupToSpot::linkIdentifiedPathFromSpan()
{
    std::vector<BadgeParkWay> pathData = createPathData(8u);

    time_t const c_startTime = timeForDate(2017, 04, 03);
    PathPtr path = createPath(*m_happenLayer, pathData, c_startTime, true);
    SpotPtr arrival = path->GetLastEvent();
    CPPUNIT_ASSERT(arrival->GetCode() == HCode::ARRIVAL);

    // Вместо прибытия на станцию создадим смерть на перегоне до неё
    m_happenLayer->GetWriteAccess()->removeEvent(arrival);

    SpotPtr spanMove = path->GetLastEvent();
    CPPUNIT_ASSERT(HCode::SPAN_MOVE == spanMove->GetCode());
    m_happenLayer->GetWriteAccess()->pushBack(
        std::make_shared<SpotEvent>(HCode::DEATH, spanMove->GetBadge(), arrival->GetTime()),
        path);
    CPPUNIT_ASSERT(HCode::DEATH == path->GetLastEvent()->GetCode());

    path->IdentifyWith(path->GetFirstEvent(), TrainDescr(L"4825", L""), identifyCategory, sysChance, Hem::SoftIdentifyPolicy());

    auto stationsPair = spanMove->GetBadge().num().split_span();
    EsrKit const previousStation = stationsPair.first == arrival->GetBadge().num() ? stationsPair.second : stationsPair.first;

    unsigned const c_deltaTime = 10u;
    AsoupPtr asoup = createAsoup(*m_asoupLayer, HCode::ARRIVAL, arrival->GetBadge().num(), 
        arrival->GetTime() + c_deltaTime, 4825u, L"1234-222-3333", ParkWayKit(),
        previousStation.to_wstring());
    AsoupToSpot::PretendersVec pretenders = AsoupToSpot::getArrivalSpanPretenders(m_happenLayer->GetWriteAccess(), *asoup, c_deltaTime * 2u);
    CPPUNIT_ASSERT_EQUAL(size_t(1), pretenders.size());
}

void TC_AsoupToSpot::repeatlyIdentify()
{
    PathPtr path = createPath(*m_happenLayer, createPathData(10u), timeForDate(2017, 11, 10), false);
    auto events = path->GetAllEvents();
    auto itMid = next( events.begin(), 4 );
    AsoupPtr a_front = createAsoup( *m_asoupLayer, *events.front(), 2735, L"1100-128-0987", -30 );
    AsoupPtr a_mid   = createAsoup( *m_asoupLayer, **itMid,         9503, L"1100-128-0987", +0 );
    // до увязки с АСОУП на нити нет данных
    CPPUNIT_ASSERT( path->GetInfoSpots().empty() );
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(a_front));
    CPPUNIT_ASSERT(!m_asoupLayer->IsServed(a_mid));

	auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_front, std::make_pair(path->GetFirstEvent(), path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_front));
	CPPUNIT_ASSERT(linkerStrict.identify(a_mid, std::make_pair(*itMid, path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_mid));

    // после увязки двух событий АСОУП будет два фрагмента с разными номерами
    // последний фрагмент находится на среднем событии
    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    CPPUNIT_ASSERT( gis.back().first == (*itMid)->GetTime() );

    // Пользователь привязывает третье событие на конец нити - повтор номера и индекса НЕ дает нового фрагмента
    AsoupPtr a_back  = createAsoup( *m_asoupLayer, *events.back(),  9503, L"1100-128-0987",  30 );
    AsoupLinker linkerUser(happenAccess, asoupAccess, context, UserIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerUser.identify(a_back, std::make_pair(path->GetLastEvent(), path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_front));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_mid));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_back));
    // остается два фрагмента и последний фрагмент остался на среднем событии
    gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    CPPUNIT_ASSERT( gis.back().first == (*itMid)->GetTime() );
}

// Создание события асоуп по событию нити с регистрацией в слое
AsoupPtr createPvLdzAsoup( AsoupLayer& layer, const std::wstring& str )
{
    auto msg = Asoup::MessagePassengerLDZ::parse(str);
    CPPUNIT_ASSERT(msg);
    auto asoup = AsoupEvent::Create(*msg, nullptr);
    CPPUNIT_ASSERT(asoup);
    layer.AddAsoupEvent(asoup);
    CPPUNIT_ASSERT(layer.FindEvent(*asoup));
    return asoup;
}

void TC_AsoupToSpot::IdentifyWithPassagirArrivalMessage()
{
	UtUserChart m_chart;
	auto& hl = m_chart.getHappenLayer();
	auto& al = m_chart.getAsoupLayer();

    PathPtr path = createPath( hl,
    L"<HemPath>"
        L"<SpotEvent create_time='20190212T084639Z' name='Form' Bdg='7C[09010]' waynum='7' parknum='1' index='' num='6713' suburbreg='Y' />"
        L"<SpotEvent create_time='20190212T085247Z' name='Departure' Bdg='7C[09010]' waynum='7' parknum='1' optCode='09010:09100' />"
        L"<SpotEvent create_time='20190212T085614Z' name='Arrival' Bdg='2p[09100]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190212T085751Z' name='Departure' Bdg='2p[09100]' waynum='2' parknum='1' optCode='09100:09104' />"
        L"<SpotEvent create_time='20190212T090608Z' name='Transition' Bdg='KPU13A/6A[09104]'>"
        L"<rwcoord picketing1_val='5~750' picketing1_comm='Торнянкалнс-Елгава' />"
        L"<rwcoord picketing1_val='8~800' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190212T091634Z' name='Arrival' Bdg='4p[09150]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190212T091820Z' name='Departure' Bdg='4Ap[09150]' waynum='4' parknum='1' optCode='09150:09160' />"
        L"<SpotEvent create_time='20190212T092552Z' name='Arrival' Bdg='2p[09160]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190212T092720Z' name='Departure' Bdg='2p[09160]' waynum='2' parknum='1' optCode='09160:09162' />"
        L"<SpotEvent create_time='20190212T093506Z' name='Transition' Bdg='AGP[09162]' waynum='1' parknum='1' intervalSec='101'>"
        L"<rwcoord picketing1_val='42~500' picketing1_comm='Торнянкалнс-Елгава' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190212T093732Z' name='Arrival' Bdg='32C[09180]' waynum='32' parknum='1' />"
        L"<SpotEvent create_time='20190212T093732Z' name='Disform' Bdg='32C[09180]' waynum='32' parknum='1' />"
        L"</HemPath>" );
    
    CPPUNIT_ASSERT( path && path->GetEventsCount()==12 );
    // до увязки с АСОУП на нити 1 фрагмент
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==1 );

    auto vEvents = path->GetAllEvents();
    auto it9100 = next( vEvents.begin(), 3 );
    auto it9150 = next( vEvents.begin(), 6 );
    AsoupPtr a_9100 = createPvLdzAsoup(al,
        L"<TrainAssignment operation='1' date='12-02-2019 10:56' esr='09100' location='0,0' number='6713'>"
        L"<Unit number='123432' driver='Petrovs' driverTime='12-02-2019 00:00' weight='234' length='24' specialConditions='M' carrier='22'>"
        L"<Wagon number='123157' axisCount='4' weight='12' length='1' type='1' owner='22'/>"
        L"<Wagon number='123438' axisCount='3' weight='15' length='1' type='2' owner='22'/>"
        L"<Wagon number='213143' axisCount='3' weight='23' length='1' type='3' owner='23'/>"
        L"<Wagon number='213459' axisCount='4' weight='23' length='1' type='3' owner='23'/>"
        L"</Unit>"
        L"</TrainAssignment>"
        );

    AsoupPtr a_9150 = createPvLdzAsoup(al,
    L"<TrainAssignment operation='5' date='12-02-2019 12:20' esr='09150' location='0,0' number='6713'>"
        L"<Unit number='123332' driver='Kuzins' driverTime='12-02-2019 00:00' weight='234' length='24' specialConditions='M' carrier='22'>"
        L"<Wagon number='1200007' axisCount='3' weight='12' length='1' type='1' owner='22'/>"
        L"<Wagon number='1000038' axisCount='3' weight='15' length='1' type='2' owner='6'/>"
        L"<Wagon number='200043' axisCount='3' weight='13' length='1' type='3' owner='23'/>"
        L"<Wagon number='21009' axisCount='4' weight='23' length='1' type='3' owner='23'/>"
        L"</Unit>"
        L"<Unit number='35654' driver='Merz' driverTime='12-02-2019 00:00' weight='235' length='34' specialConditions='' carrier='34234'>"
        L"<Wagon number='17777' axisCount='3' weight='23' length='1' type='1' owner='6'/>"
        L"<Wagon number='23333' axisCount='3' weight='24' length='2' type='2' owner='6'/>"
        L"<Wagon number='11142125' axisCount='4' weight='30' length='2' type='3' owner='34'/>"
        L"</Unit>"
        L"</TrainAssignment>"
        );

    
    CPPUNIT_ASSERT( it9100!=vEvents.end() && a_9100 && 
                    !al.IsServed(a_9100) && 
                    a_9150 && !al.IsServed(a_9150));
    // увязка АСОУП
	{
    auto happenAccess = hl.GetWriteAccess();
    auto asoupAccess = al.GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_9100, std::make_pair(*it9100, path)));
    CPPUNIT_ASSERT(al.IsServed(a_9100));
	}

	time_t currentTime = time_from_iso("20190212T093732Z");
	LinkFreeAsoup(a_9100, m_chart, currentTime);

	CPPUNIT_ASSERT(al.IsServed(a_9150));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    CPPUNIT_ASSERT( gis.back().first == (*it9150)->GetTime() );
}

void TC_AsoupToSpot::identifyDepartureWithChangeFeature()
{
    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20190320T041047Z' name='Form' Bdg='10C[11310]' waynum='10' parknum='1' index='1131-061-0918' num='2411' length='58' weight='4621' net_weight='3028' through='Y'>"
        L"<feat_texts typeinfo='ДТ' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3422' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GOLOVNJOVS' Tim_Beg='2019-03-20 04:39' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190320T041047Z' name='Departure' Bdg='10C[11310]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20190320T041706Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190320T043618Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='9' optCode='11311:11380' />"
        L"<SpotEvent create_time='20190320T045734Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20190320T051700Z' name='Arrival' Bdg='3C[11381]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190320T052212Z' name='Departure' Bdg='3C[11381]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190320T053601Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' intervalSec='4' optCode='11390:11391' />"
        L"<SpotEvent create_time='20190320T055810Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11391:11401' />"
        L"<SpotEvent create_time='20190320T061202Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' intervalSec='7' optCode='11401:11412' />"
        L"<SpotEvent create_time='20190320T063254Z' name='Transition' Bdg='1C[11412]' waynum='1' parknum='1' intervalSec='23' optCode='11412:11420' />"
        L"<SpotEvent create_time='20190320T065927Z' name='Arrival' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190320T075801Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190320T082119Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' intervalSec='13' optCode='09190:09191' />"
        L"<SpotEvent create_time='20190320T083622Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='12' optCode='09191:09193' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==15 );
    // до увязки с АСОУП на нити 1 фрагмент
    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size()==1 );
    auto notes = gis.back().second->GetNotes();
    CPPUNIT_ASSERT( notes.HaveLong() && notes.HaveHeavy() );

    auto vEvents = path->GetAllEvents();
    auto itDeparture11420 = next( vEvents.begin(), 12 );
    CPPUNIT_ASSERT( itDeparture11420!=vEvents.end() );

    AsoupPtr a_11420 = createAsoup(*m_asoupLayer, topology,
        L"(:1042 909/000+11420 2411 1131 061 0918 03 09180 20 03 09 57 04/04 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 04449 02905 220 42320523 50532282 000 000 00 055 000.40 005 000 000 60 011 000 000 70 025 000 000 73 001 000 000 90 014 000 000 95 004 000 000 96 010 000 000\n"
        L"Ю3 575 00034221 1 04 39 0000 00000 GOLOVNJOVS  \n"
        L"Ю3 575 00034222 9\n"
        L"Ю4 09170 2 10 017 000.70 007 000 73 001 000 90 010 000 96 010 000 \n"
        L"Ю4 00000 1 01 038 000.40 005 000 60 011 000 70 018 000 90 004 000 95 004 000 \n"
        L"Ю4 09330 1 01 006 000.40 005 000 60 001 000 \n"
        L"Ю4 09170 1 01 001 000.60 001 000 \n"
        L"Ю4 09240 1 01 005 000.60 004 000 90 001 000 95 001 000 \n"
        L"Ю4 09340 1 01 004 000.60 004 000 \n"
        L"Ю4 09790 1 01 001 000.60 001 000 \n"
        L"Ю4 09150 1 01 018 000.70 018 000 \n"
        L"Ю4 09290 1 01 003 000.90 003 000 95 003 000 \n"
        L"Ю12 00 42320523 1 060 09330 08118 5967 400 00 00 00 00 00 0262 30 40 0404 04 105 09180 09330 00000 01 09180 24 07000 3085 026 0 0000 0      132 064 00000000\n"
        L"Ю12 00 54820931 255237255255 02575406\n"
        L"Ю12 00 54832597 255255255255 \n"
        L"Ю12 00 54821194 255239255255 0260\n" 
        L"Ю12 00 54824180 255239255255 0257\n"
        L"Ю12 00 51833093 129225064037 066100002140315003600102672070570008612550091701011310030452094030209140OXP   000\n"
        L"Ю12 00 50358191 191239255255 0590278\n"
        L"Ю12 00 57211260 191239255255 0660265\n"
        L"Ю12 00 51097855 191239255255 0590235\n"
        L"Ю12 00 51114684 191239255255 0670247\n"
        L"Ю12 00 75031385 191237255255 06602600732\n"
        L"Ю12 00 57031056 255237255255 02675700\n"
        L"Ю12 00 76694272 131229195111 0400915022609826545703673007690915000000010915067430000\n"
        L"Ю12 00 76691682 255239255255 0352\n"
        L"Ю12 00 55206619 191237255255 04203535080\n"
        L"Ю12 00 58239096 171239254127 039226263570367811802157\n"
        L"Ю12 00 58256553 191239255255 0340376\n"
        L"Ю12 00 58241092 191239255255 0380367\n"
        L"Ю12 00 55089254 129233088119 060093404330458483000002336056001000986009180172264052910     \n"
        L"Ю12 00 62878806 255237255255 02500600\n"
        L"Ю12 00 56940604 251237255255 40002435600\n"
        L"Ю12 00 58439316 255239255255 0242\n"
        L"Ю12 00 56161714 219239223255 09330000023809330\n"
        L"Ю12 00 55146112 135239218119 0630917043304767902420917011310302504862COБCTB\n"
        L"Ю12 00 54100870 135239218247 067097904361911440243098200918030260ПГK   \n"
        L"Ю12 00 94821600 131161064103 0591001000300662308502024020960946141125500917010113104269570130391400     \n"
        L"Ю12 00 94832870 255239255255 0230\n"
        L"Ю12 00 94825387 255239255255 0240\n"
        L"Ю12 00 94212859 187141127255 058080010101950942105\n"
        L"Ю12 00 94852050 251141127255 085020002400946141\n"
        L"Ю12 00 94397288 187237126255 0590800210094210566780\n"
        L"Ю12 00 94453545 191239255255 0580196\n"
        L"Ю12 00 62880380 131161068111 0600924043304584850000025030600600100092400000001172264052910000\n"
        L"Ю12 00 56578289 255237255255 02425600\n"
        L"Ю12 00 63985774 163237254119 06543302606208002440600637202843CУЛЬФT\n"
        L"Ю12 00 63605505 255255255255 \n"
        L"Ю12 00 94784246 129161068103 0581001000300662308502020240209609461411255009170104269570130391400     \n"
        L"Ю12 00 94832052 255255255255 \n"
        L"Ю12 00 94785219 191255255255 059\n"
        L"Ю12 00 59891614 129161068111 055092405422358480600400023430955935106092400000001386130044940000\n"
        L"Ю12 00 95747408 129237219247 0620929054223590036002023209500986009180APEHДA\n"
        L"Ю12 00 95737979 191255255255 061\n"
        L"Ю12 00 95742417 255255255255 \n"
        L"Ю12 00 58216367 129233088119 03309150226268265557010369705080086091500915003796101684OXP   \n"
        L"Ю12 00 58187873 191239255255 0340347\n"
        L"Ю12 00 58168030 255239255255 0346\n"
        L"Ю12 00 58184789 191239255255 0370367\n"
        L"Ю12 00 58188087 191239255255 0340347\n"
        L"Ю12 00 76631456 187237254127 03735703510769811802157\n"
        L"Ю12 00 58170556 191237255255 03303465080\n"
        L"Ю12 00 58247032 251255255255 557\n"
        L"Ю12 00 50517416 255255255255\n" 
        L"Ю12 00 58245549 187239254127 0383570367761609376\n"
        L"Ю12 00 50898238 191255255255 037\n"
        L"Ю12 00 50532282 187239255255 0360570371:)", 
        boost::gregorian::date(2019,3,20)
        );


    CPPUNIT_ASSERT( a_11420 && !m_asoupLayer->IsServed(a_11420) );
    // увязка АСОУП
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_11420, std::make_pair(*itDeparture11420, path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_11420));

    gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    CPPUNIT_ASSERT( gis.back().first == (*itDeparture11420)->GetTime() && gis.back().second );
    notes = gis.back().second->GetNotes();
    CPPUNIT_ASSERT( !notes.HaveLong() && !notes.HaveHeavy() );
}

void TC_AsoupToSpot::IdentifyWithPassagirArrivalMessage2()
{

    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20190401T134330Z' name='Span_move' Bdg='#6140[09042:09650]' index='' num='6140' suburbreg='Y'>"
        L"<rwcoord picketing1_val='7~125' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190401T134400Z' name='Arrival' Bdg='№6140[09650]' index='' num='6140' suburbreg='Y' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T134500Z' name='Departure' Bdg='№6140[09650]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T134800Z' name='Arrival' Bdg='№6140[09640]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T134900Z' name='Departure' Bdg='№6140[09640]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T135230Z' name='Arrival' Bdg='№6140[09630]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T135330Z' name='Departure' Bdg='№6140[09630]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T135800Z' name='Arrival' Bdg='№6140[09616]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T135900Z' name='Departure' Bdg='№6140[09616]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T141100Z' name='Arrival' Bdg='№6140[09612]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T141200Z' name='Departure' Bdg='№6140[09612]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T141400Z' name='Arrival' Bdg='№6140[09609]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T141500Z' name='Departure' Bdg='№6140[09609]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T142030Z' name='Arrival' Bdg='№6140[09603]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T142130Z' name='Departure' Bdg='№6140[09603]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T142700Z' name='Arrival' Bdg='№6140[09604]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T142730Z' name='Departure' Bdg='№6140[09604]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T143400Z' name='Arrival' Bdg='№6140[09600]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T143500Z' name='Departure' Bdg='№6140[09600]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T144300Z' name='Arrival' Bdg='№6140[09070]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20190401T144300Z' name='Death' Bdg='№6140[09070]' waynum='2' parknum='1' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==21 );
    // до увязки с АСОУП на нити 1 фрагмент
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==1 );

    auto vEvents = path->GetAllEvents();
    auto it9070 = next( vEvents.begin(), 19 );
    AsoupPtr a_9070 = createPvLdzAsoup(*m_asoupLayer,
        L"<TrainAssignment operation='6' date='01-04-2019 17:43' esr='09070' send_esr='09070' location='null,null' number='6140' issue_moment='20190401T145535Z'>"
        L"<Unit number='111--2ТЭ10У' driver='Ivanovs' driverTime='01-04-2019 13:00' weight='10' length='10' specialConditions='M' carrier='42'>"
        L"<Wagon number='111111' axisCount='2' weight='2' length='2' type='1' owner='6' />"
        L"<Wagon number='2222' axisCount='2' weight='1' length='1' type='3' owner='4' />"
        L"<Wagon number='3333' axisCount='2' weight='1' length='1' type='2' owner='6' />"
        L"</Unit>"
        L"</TrainAssignment>"        
);

    CPPUNIT_ASSERT( it9070!=vEvents.end() && a_9070 && !m_asoupLayer->IsServed(a_9070) );
    // увязка АСОУП
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_9070, std::make_pair(*it9070, path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_9070));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    //CPPUNIT_ASSERT( gis.back().first == (*it9150)->GetTime() );
}


void TC_AsoupToSpot::IdentifyWithSynonym()
{

    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20190212T082307Z' name='Form' Bdg='8JC[09006]' waynum='8' parknum='1' />"
        L"<SpotEvent create_time='20190212T084158Z' name='Departure' Bdg='8JC[09006]' waynum='8' parknum='1' />"
        L"<SpotEvent create_time='20190212T084654Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='21' />"
        L"<SpotEvent create_time='20190212T084931Z' name='Transition' Bdg='2AC[09000]' waynum='2' parknum='1' intervalSec='20' optCode='09000:09420' />"
        L"<SpotEvent create_time='20190212T090017Z' name='Transition' Bdg='2C[09420]' waynum='2' parknum='1' intervalSec='15' />"
        L"<SpotEvent create_time='20190212T091747Z' name='Transition' Bdg='1C[09410]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190212T092836Z' name='Span_move' Bdg='8P_O[09400:09410]' waynum='2'>"
        L"<rwcoord picketing1_val='45~' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='47~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==7 );
    // до увязки с АСОУП на нити нет фрагментов
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==0 );

    auto vEvents = path->GetAllEvents();

    topology->Load(
        L"=09006,09008,09000\n"
        L"=09006,09000,11760\n"
        L"*09000,11760 {09000,09420,09410,09400,09380,09370,09360,11760}\n");

    AsoupPtr a_9006 = createAsoup(*m_asoupLayer, topology, 
        L"(:1042 909/000+09006 2740 0900 568 1800 03 11760 12 02 11 47 01/08 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01373 00000 228 61053419 53198479 000 000 00 000 057.60 000 057 000\n"
        L"Ю3 530 00012591 1 09 36 0000 00000 SIVUHA      \n"
        L"Ю3 530 00012592 9\n"
        L"Ю4 11290 2 58 000 022.60 000 022 \n"
        L"Ю4 11290 2 83 000 035.60 000 035 \n"
        L"Ю12 00 61053419 1 000 58320 16112 2006 300 00 00 00 00 00 0241 12 60 0600 04 100 18000 00000 11290 58 09000 01 00000 0000 020 1 1020 APEHДA 128 000 00000000\n"
        L"Ю12 00 61459723 255239255255 0240\n"
        L"Ю12 00 62171442 195239247255 86280161173126400024183\n"
        L"Ю12 00 61534426 195239255255 862201611258605000238\n"
        L"Ю12 00 61580056 255255255255 \n"
        L"Ю12 00 55183107 255237255247 0245560009069-\n"
        L"Ю12 00 62171814 255237255247 02480600APEHДA\n"
        L"Ю12 00 62090311 255255255255 \n"
        L"Ю12 00 61393526 195239255255 862801611731264000240\n"
        L"Ю12 00 60544566 195239255255 862201611258605000237\n"
        L"Ю12 00 61907275 255239255255 0239\n"
        L"Ю12 00 61537924 195239247255 43800161177067300024558\n"
        L"Ю12 00 55770002 255237255247 0230560009069-\n"
        L"Ю12 00 60533502 255237255247 02430600APEHДA\n"
        L"Ю12 00 54864434 195237247247 86280161173126400023356008309069-\n"
        L"Ю12 00 60887775 195237247247 438001611770673000243060058APEHДA\n"
        L"Ю12 00 63056667 255239255255 0242\n"
        L"Ю12 00 62250378 255255255255 \n"
        L"Ю12 00 57651424 255237255247 0232560009069-\n"
        L"Ю12 00 61510152 255237255247 02430600APEHДA\n"
        L"Ю12 00 55541601 255237255247 0241560009069-\n"
        L"Ю12 00 60100054 255253255247 0600APEHДA\n"
        L"Ю12 00 61461075 255239255255 0240\n"
        L"Ю12 00 62081336 199239247255 86220161175860024783\n"
        L"Ю12 00 61005625 255255255255 \n"
        L"Ю12 00 63532832 227239255255 1611228460000245\n"
        L"Ю12 00 63671804 255255255255 \n"
        L"Ю12 00 60223963 195239247255 58320161122006400024458\n"
        L"Ю12 00 62641261 255239255255 0241\n"
        L"Ю12 00 54476106 195253255247 43800161177067500560009069-\n"
        L"Ю12 00 59601823 255239255255 0233\n"
        L"Ю12 00 62691217 255237255247 02420600APEHДA\n"
        L"Ю12 00 63348767 255255255255 \n"
        L"Ю12 00 55751432 255237255247 0240560009069-\n"
        L"Ю12 00 62128970 195237247247 862201611758603000248060083APEHДA\n"
        L"Ю12 00 61875670 255239255255 0239\n"
        L"Ю12 00 61021085 255239255255 0248\n"
        L"Ю12 00 61287470 255239255255 0247\n"
        L"Ю12 00 60913852 255239255255 0239\n"
        L"Ю12 00 61073672 255239255255 0237\n"
        L"Ю12 00 57853368 195237247247 58320161122006400024056005809069-\n"
        L"Ю12 00 60255528 195237247247 862801611731265000242060083APEHДA\n"
        L"Ю12 00 55769350 255237255247 0231560009069-\n"
        L"Ю12 00 60167335 195237247247 583201611220064000242060058APEHДA\n"
        L"Ю12 00 62339510 195239247255 86220161175860300024983\n"
        L"Ю12 00 62040126 255239255255 0246\n"
        L"Ю12 00 61899738 255255255255 \n"
        L"Ю12 00 60991858 235239255247 161010000238ARENDA\n"
        L"Ю12 00 61573689 255239255255 0239\n"
        L"Ю12 00 61878112 255255255255 \n"
        L"Ю12 00 61799722 255239255255 0238\n"
        L"Ю12 00 61656740 255255255255 \n"
        L"Ю12 00 53105177 199237255255 8527016101273802375600\n"
        L"Ю12 00 53199253 255239255255 0236\n"
        L"Ю12 00 60747243 255237255255 02350600\n"
        L"Ю12 00 60803962 255239255255 0240\n"
        L"Ю12 00 53198479 255237255255 02365600:)",
        boost::gregorian::date(2019,2,12)
        );

    CPPUNIT_ASSERT( a_9006 && !m_asoupLayer->IsServed(a_9006) );

    // увязка АСОУП
    const std::string nsiBaseStr = 
        "<Test>"
        "<Junctions>"
        "<Technodes>"
        "<Union ignoreOddness='Y'>"
        "<Station esrCode='09000'/>"
        "<Station esrCode='09008'/>"
        "<Station esrCode='09006'/>"
        "</Union>"
        "</Technodes>"
        "<EqualAreas>"
        "<Union>"
        "<Station esrCode='09000'/>"
        "<Station esrCode='09008'/>"
        "<Station esrCode='09006'/>"
        "</Union>"
        "</EqualAreas>"
        "</Junctions>"
        "</Test>";
    
    UtNsiBasis utNsi( nsiBaseStr );
    std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(utNsi);
    CPPUNIT_ASSERT( nsiBasisPtr );
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    Hem::aeAsoupToSpotFind applier( m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), *m_regular, 
        context, dg, nsiBasisPtr, a_9006, true, nullptr );
    CPPUNIT_ASSERT_NO_THROW(applier.Action());
	auto res = applier.getResult();
    CPPUNIT_ASSERT( res.first == AsoupToSpotResult::Succeeded && res.second  );
	CPPUNIT_ASSERT( res.second->GetBadge().num()==EsrKit(9006) && res.second->GetCode()==HCode::DEPARTURE );
}

void TC_AsoupToSpot::IdentifyByDeparture()
{

    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20180702T225538Z' name='Form' Bdg='11C[11310]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20180702T225538Z' name='Departure' Bdg='11C[11310]' waynum='11' parknum='1' />"
        L"<SpotEvent create_time='20180702T230143Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20180702T230453Z' name='Span_move' Bdg='P16PC[11311:11321]' waynum='1'>"
        L"<rwcoord picketing1_val='219~300' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='219~' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==4 );
    // до увязки с АСОУП на нити нет фрагментов
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==0 );



    topology->Load(
        L"=11420,11321,11310\n"
        L"=11321,11310,11271\n"
        L"=11760,11420,11321\n"
        L"*11420,11321 {11420,11412,11401,11391,11390,11381,11380,11311,11321}");

    AsoupPtr a_11310Form = createAsoup(*m_asoupLayer, topology, 
        L"(:1042 909/000+11310 2403 1131 087 0983 07 11420 02 07 22 34 00/00 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 03745 02211 240 77239705 94357761 000 000 00 044 016.40 001 000 000 70 018 016 000 90 025 000 000 96 025 000 000\n"
        L"Ю4 00000 1 01 044 016.40 001 000 70 018 016 90 025 000 96 025 000 \n"
        L"Ю4 09830 1 01 044 016.40 001 000 70 018 016 90 025 000 96 025 000 \n"
        L"Ю12 00 77239705 1 000 09830 55620 7649 000 01 00 00 00 00 0271 13 70 0774 04 086 09830 09820 00000 01 11310 26 00000 0000 020 0 0000 19409- 128 000 00000000\n"
        L"Ю12 00 77216810 235239255255 556184000275\n"
        L"Ю12 00 77238160 255239255255 0269\n"
        L"Ю12 00 77236073 235239255255 556205000270\n"
        L"Ю12 00 77238582 235239255255 556184000269\n"
        L"Ю12 00 77241255 235255255255 55620500\n"
        L"Ю12 00 77240257 251239255247 0000270556208\n"
        L"Ю12 00 77218758 167247255241 06555622705130000   132064\n"
        L"Ю12 00 77212785 255239255255 0276\n"
        L"Ю12 00 50080746 171237253245 062215210600270570003OXP   000\n"
        L"Ю12 00 77209229 163229253243 00055618764900002751307742619409-128\n"
        L"Ю12 00 77216430 235255255247 556205000     \n"
        L"Ю12 00 77216752 255255255255 \n"
        L"Ю12 00 77215929 255255255255 \n"
        L"Ю12 00 50763994 163229253243 064215217051060024030570003OXP   132\n"
        L"Ю12 00 50760891 191255255255 062\n"
        L"Ю12 00 77216034 163229253243 0005561863020000275130774260     128\n"
        L"Ю12 00 53866604 163229253243 064215217051060027230570003OXP   132\n"
        L"Ю12 00 77212017 161229253243 00055618764900000027513077426556180128\n"
        L"Ю12 00 77211910 255255255255 \n"
        L"Ю12 00 77207538 239255255247 55620556208\n"
        L"Ю12 00 76776046 161245253243 0542150770510600130076803OXP   132\n"
        L"Ю12 00 77211324 163245253243 0005561876490001307742622006 128\n"
        L"Ю12 00 51481984 161229253179 05921521705106002026830570003026OXP   132\n"
        L"Ю12 00 51383594 191239255255 0580271\n"
        L"Ю12 00 51241990 255239255191 0270025\n"
        L"Ю12 00 51685667 255239255191 0255027\n"
        L"Ю12 00 94320819 161169125183 0480030083193670402021096094210542020AKPИЛ \n"
        L"Ю12 00 59919373 255253255255 5942\n"
        L"Ю12 00 94355609 255253255255 0942\n"
        L"Ю12 00 94358967 255255255255 \n"
        L"Ю12 00 94362548 255255255255 \n"
        L"Ю12 00 94239563 251255255255 467\n"
        L"Ю12 00 94355427 255255255255 \n"
        L"Ю12 00 94359668 255255255255 \n"
        L"Ю12 00 94359601 255255255255 \n"
        L"Ю12 00 94359437 255255255255 \n"
        L"Ю12 00 94239902 255255255255 \n"
        L"Ю12 00 78023298 161169125247 0474881670510570100038770076908618OXP   \n"
        L"Ю12 00 78026192 191239255255 0460386\n"
        L"Ю12 00 78026796 255239255255 0387\n"
        L"Ю12 00 77965796 191239255255 0440361\n"
        L"Ю12 00 77964492 191239255255 0450360\n"
        L"Ю12 00 77634095 191239255255 0380373\n"
        L"Ю12 00 58237165 191237255255 02803495080\n"
        L"Ю12 00 42302604 161169125247 0480030083193670402020740040410542AKPИЛ \n"
        L"Ю12 00 94351384 255233255255 0210960942\n"
        L"Ю12 00 94351368 255255255255 \n"
        L"Ю12 00 94351343 255255255255 \n"
        L"Ю12 00 94359445 255255255255 \n"
        L"Ю12 00 94355484 251255255255 467\n"
        L"Ю12 00 94358884 255255255255 \n"
        L"Ю12 00 94320744 255255255255 \n"
        L"Ю12 00 59919407 255253255255 5942\n"
        L"Ю12 00 94351459 255253255255 0942\n"
        L"Ю12 00 94362472 251255255255 367\n"
        L"Ю12 00 94362712 255255255255 \n"
        L"Ю12 00 94362746 255255255255 \n"
        L"Ю12 00 59918664 255253255255 5942\n"
        L"Ю12 00 94357761 255253255255 0942:)",
        boost::gregorian::date(2018,7,2)
        );

    AsoupPtr a_11310Departure = createAsoup(*m_asoupLayer, topology, 
    L"(:1042 909/000+11310 2405 1131 087 0983 03 11420 03 07 01 53 01/11 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 03745 02211 240 77239705 94357761 000 000 00 044 016.40 001 000 000 70 018 016 000 90 025 000 000 96 025 000 000\n"
        L"Ю3 550 00002151 1 00 26 0000 00000 MARKUSINS   \n"
        L"Ю3 550 00002152 9\n"
        L"Ю4 00000 1 01 044 016.40 001 000 70 018 016 90 025 000 96 025 000 \n"
        L"Ю4 09830 1 01 044 016.40 001 000 70 018 016 90 025 000 96 025 000 \n"
        L"Ю12 00 77239705 1 000 09830 55620 7649 000 01 00 00 00 00 0271 13 70 0774 04 086 09830 09820 00000 01 11310 26 00000 0000 020 0 0000 19409- 128 000 00000000\n"
        L"Ю12 00 77216810 235239255255 556184000275\n"
        L"Ю12 00 77238160 255239255255 0269\n"
        L"Ю12 00 77236073 235239255255 556205000270\n"
        L"Ю12 00 77238582 235239255255 556184000269\n"
        L"Ю12 00 77241255 235255255255 55620500\n"
        L"Ю12 00 77240257 251239255247 0000270556208\n"
        L"Ю12 00 77218758 167247255241 06555622705130000   132064\n"
        L"Ю12 00 77212785 255239255255 0276\n"
        L"Ю12 00 50080746 171237253245 062215210600270570003OXP   000\n"
        L"Ю12 00 77209229 163229253243 00055618764900002751307742619409-128\n"
        L"Ю12 00 77216430 235255255247 556205000     \n"
        L"Ю12 00 77216752 255255255255 \n"
        L"Ю12 00 77215929 255255255255 \n"
        L"Ю12 00 50763994 163229253243 064215217051060024030570003OXP   132\n"
        L"Ю12 00 50760891 191255255255 062\n"
        L"Ю12 00 77216034 163229253243 0005561863020000275130774260     128\n"
        L"Ю12 00 53866604 163229253243 064215217051060027230570003OXP   132\n"
        L"Ю12 00 77212017 161229253243 00055618764900000027513077426556180128\n"
        L"Ю12 00 77211910 255255255255 \n"
        L"Ю12 00 77207538 239255255247 55620556208\n"
        L"Ю12 00 76776046 161245253243 0542150770510600130076803OXP   132\n"
        L"Ю12 00 77211324 163245253243 0005561876490001307742622006 128\n"
        L"Ю12 00 51481984 161229253179 05921521705106002026830570003026OXP   132\n"
        L"Ю12 00 51383594 191239255255 0580271\n"
        L"Ю12 00 51241990 255239255191 0270025\n"
        L"Ю12 00 51685667 255239255191 0255027\n"
        L"Ю12 00 94320819 161169125183 0480030083193670402021096094210542020AKPИЛ \n"
        L"Ю12 00 59919373 255253255255 5942\n"
        L"Ю12 00 94355609 255253255255 0942\n"
        L"Ю12 00 94358967 255255255255 \n"
        L"Ю12 00 94362548 255255255255 \n"
        L"Ю12 00 94239563 251255255255 467\n"
        L"Ю12 00 94355427 255255255255 \n"
        L"Ю12 00 94359668 255255255255 \n"
        L"Ю12 00 94359601 255255255255 \n"
        L"Ю12 00 94359437 255255255255 \n"
        L"Ю12 00 94239902 255255255255 \n"
        L"Ю12 00 78023298 161169125247 0474881670510570100038770076908618OXP   \n"
        L"Ю12 00 78026192 191239255255 0460386\n"
        L"Ю12 00 78026796 255239255255 0387\n"
        L"Ю12 00 77965796 191239255255 0440361\n"
        L"Ю12 00 77964492 191239255255 0450360\n"
        L"Ю12 00 77634095 191239255255 0380373\n"
        L"Ю12 00 58237165 191237255255 02803495080\n"
        L"Ю12 00 42302604 161169125247 0480030083193670402020740040410542AKPИЛ \n"
        L"Ю12 00 94351384 255233255255 0210960942\n"
        L"Ю12 00 94351368 255255255255 \n"
        L"Ю12 00 94351343 255255255255 \n"
        L"Ю12 00 94359445 255255255255 \n"
        L"Ю12 00 94355484 251255255255 467\n"
        L"Ю12 00 94358884 255255255255 \n"
        L"Ю12 00 94320744 255255255255 \n"
        L"Ю12 00 59919407 255253255255 5942\n"
        L"Ю12 00 94351459 255253255255 0942\n"
        L"Ю12 00 94362472 251255255255 367\n"
        L"Ю12 00 94362712 255255255255 \n"
        L"Ю12 00 94362746 255255255255 \n"
        L"Ю12 00 59918664 255253255255 5942\n"
        L"Ю12 00 94357761 255253255255 0942:)",
        boost::gregorian::date(2018,7,3)
        );

    CPPUNIT_ASSERT( a_11310Form && !m_asoupLayer->IsServed(a_11310Form) );
    CPPUNIT_ASSERT( a_11310Departure && !m_asoupLayer->IsServed(a_11310Departure) );


    // увязка АСОУП
    const std::string nsiBaseStr = "<Test><Junctions /></Test>";
    std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(UtNsiBasis(nsiBaseStr));
    CPPUNIT_ASSERT( nsiBasisPtr );

	std::pair<AsoupToSpotResult, HemHelpful::SpotEventPtr> res = std::make_pair(AsoupToSpotResult::Impossible, nullptr);
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);

	{
    Hem::aeAsoupToSpotFind applier( m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), *m_regular, 
        context, dg, nsiBasisPtr, a_11310Departure, false, nullptr );
    CPPUNIT_ASSERT_NO_THROW(applier.Action());
	res = applier.getResult();
	}

	CPPUNIT_ASSERT( res.first==AsoupToSpotResult::Succeeded && res.second );
	path = m_happenLayer->GetWriteAccess()->getPath(res.second);
	CPPUNIT_ASSERT( path );
	{
	AsoupLinker linkerStrict(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), context, StrictIdentifyPolicy(), nullptr);
	CPPUNIT_ASSERT(linkerStrict.identify(a_11310Departure, std::make_pair(res.second, path)));
	}

    auto gis = path->GetInfoSpots();    
    CPPUNIT_ASSERT( gis.size() == 2 );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==4 );

    auto vEvents = path->GetAllEvents();
    const auto& ev11310Form = **vEvents.begin();

    CPPUNIT_ASSERT( ev11310Form.GetCode()==a_11310Form->GetCode() && ev11310Form.GetTime()==a_11310Form->GetTime() && gis.front().first == ev11310Form.GetTime() );
    CPPUNIT_ASSERT( gis.front().second &&  gis.front().second->GetNumber().getNumber()==2403 );
    CPPUNIT_ASSERT( gis.back().second &&  gis.back().second->GetNumber().getNumber()==2405 );
}


void TC_AsoupToSpot::IdentifyByDepartureIncludeInfoChanging()
{
	UtUserChart m_chart;
	auto& hl = m_chart.getHappenLayer();
	auto& al = m_chart.getAsoupLayer();

    PathPtr path = createPath( hl,
        L"<HemPath>"
        L"<SpotEvent create_time='20181113T062619Z' name='Form' Bdg='8JC[11110]' waynum='8' parknum='1' num='2730' index='' />"
        L"<SpotEvent create_time='20181113T065426Z' name='Departure' Bdg='8JC[11110]' waynum='8' parknum='1' />"
        L"<SpotEvent create_time='20181113T070018Z' name='Arrival' Bdg='9C[22220]' waynum='9' parknum='1' />"
        L"<SpotEvent create_time='20181113T070024Z' name='Departure' Bdg='9C[22220]' waynum='9' parknum='1' />"
        L"<SpotEvent create_time='20181113T070535Z' name='Transition' Bdg='2AC[30000]' waynum='2' parknum='1' intervalSec='37' optCode='09000:09420' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==5 );
    // до увязки с АСОУП на нити нет фрагментов
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==1 );



    topology->Load(
        L"*11110,55550 {11110,22220,33330,44440,55550}\n");

    //смена индекса - должно привязаться (берём индекс, номер игнорируем)
    AsoupPtr a_11110Info1 = createAsoup(al, topology, 
    L"(:1042 909/000+11110 3601 0905 020 1800 10 55550 13 11 09 26 01/08 0 0/00 00 0\n"
        L"Ю1 3601 0905 020 0900 01\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 01347 00000 224 61414454 59396739 000 000 00 000 056.60 000 056 000\n"
        L"Ю4 11290 2 58 000 004.60 000 004 \n"
        L"Ю4 11290 2 83 000 052.60 000 052 \n"
        L"Ю12 00 61414454 1 000 86280 16111 3126 300 00 00 00 00 00 0239 12 60 0600 04 100 18000 00000 11290 83 09050 01 00000 0000 020 1 1020 APEHДA 128 000 00000000\n"
        L"Ю12 00 61350567 255255255255 \n"
        L"Ю12 00 63091003 255239255255 0240\n"
        L"Ю12 00 56128168 255237255255 02345600\n"
        L"Ю12 00 60066719 255237255255 02440600\n"
        L"Ю12 00 61416210 255239255255 0243\n"
        L"Ю12 00 61051785 255255255255 \n"
        L"Ю12 00 63725063 255239255255 0240\n"
        L"Ю12 00 61262937 255255255255 \n"
        L"Ю12 00 63702328 255239255255 0241\n"
        L"Ю12 00 62060116 255239255255 0243\n"
        L"Ю12 00 63880223 255239255255 0241\n"
        L"Ю12 00 60264777 255255255255 \n"
        L"Ю12 00 61325064 255255255255 \n"
        L"Ю12 00 62717707 195255255255 86140161128023000\n"
        L"Ю12 00 61494704 199255255255 86280161123126\n"
        L"Ю12 00 62171269 195239255255 862101611258604000247\n"
        L"Ю12 00 62416565 255255255255 \n"
        L"Ю12 00 61446084 219239255255 862205000241\n"
        L"Ю12 00 61531158 255239255255 0239\n"
        L"Ю12 00 60623378 255239255255 0238\n"
        L"Ю12 00 55183818 219237255255 8621040002465600\n"
        L"Ю12 00 59728402 195239255247 86280161113126300024309069-\n"
        L"Ю12 00 63446389 255237255247 02380600APEHДA\n"
        L"Ю12 00 61152658 255239255255 0240\n"
        L"Ю12 00 61271318 255239255255 0239\n"
        L"Ю12 00 63762603 255239255255 0240\n"
        L"Ю12 00 68815133 255237255255 02120608\n"
        L"Ю12 00 61729463 255237255255 02370600\n"
        L"Ю12 00 60982329 255239255255 0240\n"
        L"Ю12 00 64869647 255255255255 \n"
        L"Ю12 00 64198179 255239255255 0241\n"
        L"Ю12 00 61006862 255239255255 0242\n"
        L"Ю12 00 63101745 255255255255 \n"
        L"Ю12 00 63513287 255239255255 0238\n"
        L"Ю12 00 63838577 255239255255 0243\n"
        L"Ю12 00 61460010 255239255255 0240\n"
        L"Ю12 00 63215271 195239255255 862201611128464000245\n"
        L"Ю12 00 63533681 251255255255 000\n"
        L"Ю12 00 68117787 251237255255 40002250608\n"
        L"Ю12 00 62694849 235237255255 1611700002450600\n"
        L"Ю12 00 62565718 251255255255 500\n"
        L"Ю12 00 68042944 255237255255 02260608\n"
        L"Ю12 00 62653852 255237255255 02450600\n"
        L"Ю12 00 63215925 255255255255 \n"
        L"Ю12 00 62916481 255255255255 \n"
        L"Ю12 00 62052170 255239255255 0246\n"
        L"Ю12 00 60136843 255239255255 0245\n"
        L"Ю12 00 63689665 251255255255 000\n"
        L"Ю12 00 63689657 255255255255 \n"
        L"Ю12 00 62908405 251255255255 500\n"
        L"Ю12 00 61153987 255255255255 \n"
        L"Ю12 00 56175037 195237247247 58320161172006300023356005809069-\n"
        L"Ю12 00 53159083 255239255255 0240\n"
        L"Ю12 00 55662357 195239255255 438001611270674000233\n"
        L"Ю12 00 59396739 255239255255 0240:)",
        boost::gregorian::date(2018,11,13)
        );
        
        //Прицепка вагонов - привязываем (не меняя номер и индекс)
        AsoupPtr a_11110Info2 = createAsoup(al, topology, 
        L"(:1042 909/000+11110 3601 0905 020 1800 09 00000 13 11 09 36 01/08 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 01369 00000 228 65019051 59396739 000 000 00 000 057.60 000 057 000\n"
        L"Ю4 11290 2 58 000 004.60 000 004 \n"
        L"Ю4 11290 2 83 000 053.60 000 053 \n"
        L"Ю12 14 65019051 1 000 87150 16101 5656 000 00 00 00 00 00 0220 12 60 0600 04 100 18000 00000 11290 83 09050 01 00000 0000 020 1 1020 ARENDA 144 000 00000000\n"
        L"Ю12 00 61414454 195239255243 862801611131263000239APEHДA128\n"
        L"Ю12 00 61350567 255255255255 \n"
        L"Ю12 00 63091003 255239255255 0240\n"
        L"Ю12 00 56128168 255237255255 02345600\n"
        L"Ю12 00 60066719 255237255255 02440600\n"
        L"Ю12 00 61416210 255239255255 0243\n"
        L"Ю12 00 61051785 255255255255 \n"
        L"Ю12 00 63725063 255239255255 0240\n"
        L"Ю12 00 61262937 255255255255 \n"
        L"Ю12 00 63702328 255239255255 0241\n"
        L"Ю12 00 62060116 255239255255 0243\n"
        L"Ю12 00 63880223 255239255255 0241\n"
        L"Ю12 00 60264777 255255255255 \n"
        L"Ю12 00 61325064 255255255255 \n"
        L"Ю12 00 62717707 195255255255 86140161128023000\n"
        L"Ю12 00 61494704 199255255255 86280161123126\n"
        L"Ю12 00 62171269 195239255255 862101611258604000247\n"
        L"Ю12 00 62416565 255255255255 \n"
        L"Ю12 00 61446084 219239255255 862205000241\n"
        L"Ю12 00 61531158 255239255255 0239\n"
        L"Ю12 00 60623378 255239255255 0238\n"
        L"Ю12 00 55183818 219237255255 8621040002465600\n"
        L"Ю12 00 59728402 195239255247 86280161113126300024309069-\n"
        L"Ю12 00 63446389 255237255247 02380600APEHДA\n"
        L"Ю12 00 61152658 255239255255 0240\n"
        L"Ю12 00 61271318 255239255255 0239\n"
        L"Ю12 00 63762603 255239255255 0240\n"
        L"Ю12 00 68815133 255237255255 02120608\n"
        L"Ю12 00 61729463 255237255255 02370600\n"
        L"Ю12 00 60982329 255239255255 0240\n"
        L"Ю12 00 64869647 255255255255 \n"
        L"Ю12 00 64198179 255239255255 0241\n"
        L"Ю12 00 61006862 255239255255 0242\n"
        L"Ю12 00 63101745 255255255255 \n"
        L"Ю12 00 63513287 255239255255 0238\n"
        L"Ю12 00 63838577 255239255255 0243\n"
        L"Ю12 00 61460010 255239255255 0240\n"
        L"Ю12 00 63215271 195239255255 862201611128464000245\n"
        L"Ю12 00 63533681 251255255255 000\n"
        L"Ю12 00 68117787 251237255255 40002250608\n"
        L"Ю12 00 62694849 235237255255 1611700002450600\n"
        L"Ю12 00 62565718 251255255255 500\n"
        L"Ю12 00 68042944 255237255255 02260608\n"
        L"Ю12 00 62653852 255237255255 02450600\n"
        L"Ю12 00 63215925 255255255255 \n"
        L"Ю12 00 62916481 255255255255 \n"
        L"Ю12 00 62052170 255239255255 0246\n"
        L"Ю12 00 60136843 255239255255 0245\n"
        L"Ю12 00 63689665 251255255255 000\n"
        L"Ю12 00 63689657 255255255255 \n"
        L"Ю12 00 62908405 251255255255 500\n"
        L"Ю12 00 61153987 255255255255 \n"
        L"Ю12 00 56175037 195237247247 58320161172006300023356005809069-\n"
        L"Ю12 00 53159083 255239255255 0240\n"
        L"Ю12 00 55662357 195239255255 438001611270674000233\n"
        L"Ю12 00 59396739 255239255255 0240:)",
        boost::gregorian::date(2018,11,13)
        );

        AsoupPtr a_11110Info3 = createAsoup(al, topology, 
            L"(:1042 909/000+11110 2730 0905 020 1800 02 55550 13 11 09 17 01/08 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 057 01369 00000 228 65019051 59396739 000 000 00 000 057.60 000 057 000\n"
            L"Ю4 11290 2 58 000 004.60 000 004 \n"
            L"Ю4 11290 2 83 000 053.60 000 053 :)\n",
            boost::gregorian::date(2018,11,13)
            );

        AsoupPtr a_11110Info4 = createAsoup(al, topology, 
            L"(:1042 909/000+11110 2730 0905 020 1800 03 55550 13 11 09 54 01/08 0 0/00 00 0\n"
            L"Ю2 0 00 00 00 00 0000 0 0 057 01369 00000 228 65019051 59396739 000 000 00 000 057.60 000 057 000\n"
            L"Ю3 579 00002691 1 07 42 0000 00000 SAVICKI     \n"
            L"Ю3 579 00000000 9\n"
            L"Ю4 11290 2 58 000 004.60 000 004 \n"
            L"Ю4 11290 2 83 000 053.60 000 053 \n"
            L"Ю12 00 65019051 1 000 87150 16101 5656 000 00 00 00 00 00 0220 12 60 0600 04 100 18000 00000 11290 83 09050 01 00000 0000 020 1 1020 ARENDA 144 000 00000000\n"
            L"Ю12 00 61414454 195239255243 862801611131263000239APEHДA128\n"
            L"Ю12 00 61350567 255255255255 \n"
            L"Ю12 00 63091003 255239255255 0240\n"
            L"Ю12 00 56128168 255237255255 02345600\n"
            L"Ю12 00 60066719 255237255255 02440600\n"
            L"Ю12 00 61416210 255239255255 0243\n"
            L"Ю12 00 61051785 255255255255 \n"
            L"Ю12 00 63725063 255239255255 0240\n"
            L"Ю12 00 61262937 255255255255 \n"
            L"Ю12 00 63702328 255239255255 0241\n"
            L"Ю12 00 62060116 255239255255 0243\n"
            L"Ю12 00 63880223 255239255255 0241\n"
            L"Ю12 00 60264777 255255255255 \n"
            L"Ю12 00 61325064 255255255255 \n"
            L"Ю12 00 62717707 195255255255 86140161128023000\n"
            L"Ю12 00 61494704 199255255255 86280161123126\n"
            L"Ю12 00 62171269 195239255255 862101611258604000247\n"
            L"Ю12 00 62416565 255255255255 \n"
            L"Ю12 00 61446084 219239255255 862205000241\n"
            L"Ю12 00 61531158 255239255255 0239\n"
            L"Ю12 00 60623378 255239255255 0238\n"
            L"Ю12 00 55183818 219237255255 8621040002465600\n"
            L"Ю12 00 59728402 195239255247 86280161113126300024309069-\n"
            L"Ю12 00 63446389 255237255247 02380600APEHДA\n"
            L"Ю12 00 61152658 255239255255 0240\n"
            L"Ю12 00 61271318 255239255255 0239\n"
            L"Ю12 00 63762603 255239255255 0240\n"
            L"Ю12 00 68815133 255237255255 02120608\n"
            L"Ю12 00 61729463 255237255255 02370600\n"
            L"Ю12 00 60982329 255239255255 0240\n"
            L"Ю12 00 64869647 255255255255 \n"
            L"Ю12 00 64198179 255239255255 0241\n"
            L"Ю12 00 61006862 255239255255 0242\n"
            L"Ю12 00 63101745 255255255255 \n"
            L"Ю12 00 63513287 255239255255 0238\n"
            L"Ю12 00 63838577 255239255255 0243\n"
            L"Ю12 00 61460010 255239255255 0240\n"
            L"Ю12 00 63215271 195239255255 862201611128464000245\n"
            L"Ю12 00 63533681 251255255255 000\n"
            L"Ю12 00 68117787 251237255255 40002250608\n"
            L"Ю12 00 62694849 235237255255 1611700002450600\n"
            L"Ю12 00 62565718 251255255255 500\n"
            L"Ю12 00 68042944 255237255255 02260608\n"
            L"Ю12 00 62653852 255237255255 02450600\n"
            L"Ю12 00 63215925 255255255255 \n"
            L"Ю12 00 62916481 255255255255 \n"
            L"Ю12 00 62052170 255239255255 0246\n"
            L"Ю12 00 60136843 255239255255 0245\n"
            L"Ю12 00 63689665 251255255255 000\n"
            L"Ю12 00 63689657 255255255255 \n"
            L"Ю12 00 62908405 251255255255 500\n"
            L"Ю12 00 61153987 255255255255 \n"
            L"Ю12 00 56175037 195237247247 58320161172006300023356005809069-\n"
            L"Ю12 00 53159083 255239255255 0240\n"
            L"Ю12 00 55662357 195239255255 438001611270674000233\n"
            L"Ю12 00 59396739 255239255255 0240:)",
            boost::gregorian::date(2018,11,13)
            );


    CPPUNIT_ASSERT( a_11110Info1 && !al.IsServed(a_11110Info1) &&
                a_11110Info2 && !al.IsServed(a_11110Info2) &&
                a_11110Info3 && !al.IsServed(a_11110Info3) &&
                a_11110Info4 && !al.IsServed(a_11110Info4) );


	{
     const std::string nsiBaseStr = "<Test><Junctions /></Test>";
     UtNsiBasis utNsi( nsiBaseStr );
     std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(utNsi);
     CPPUNIT_ASSERT( nsiBasisPtr );
     Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
     Hem::aeAsoupToSpotFind applier( hl.GetWriteAccess(), al.GetWriteAccess(), *m_regular, context, dg,
         nsiBasisPtr, a_11110Info4, true, nullptr );
     CPPUNIT_ASSERT_NO_THROW(applier.Action());
	}

	// увязка АСОУП
	auto currentTime = time_from_iso("20181113T070000");
	LinkFreeAsoup(a_11110Info4, m_chart, currentTime);

	auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 && al.IsServed(a_11110Info4) && 
                    al.IsServed(a_11110Info1) && 
                    al.IsServed(a_11110Info2) &&
                    al.IsServed(a_11110Info3) );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==5 );

    auto vEvents = path->GetAllEvents();
    const auto& ev11310Form = **vEvents.begin();

    CPPUNIT_ASSERT( gis.front().second &&  gis.front().second->GetNumber().getNumber()==2730 );
}

void TC_AsoupToSpot::IdentifyByArrival()
{
    UtUserChart m_chart;
    auto& hl = m_chart.getHappenLayer();
    auto& al = m_chart.getAsoupLayer();

    PathPtr path = createPath( hl,
        L"<HemPath>"
        L"<SpotEvent create_time='20190826T082437Z' name='Form' Bdg='2UP_PK[09042:09501]' waynum='2' index='' num='2746' through='Y'>"
        L"<rwcoord picketing1_val='0~900' picketing1_comm='Рига-Кр' />"
        L"<rwcoord picketing1_val='1~300' picketing1_comm='Рига-Кр' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190826T082641Z' name='Transition' Bdg='7SP[09042]'>"
        L"<rwcoord picketing1_val='5~857' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190826T083159Z' name='Transition' Bdg='1C[09500]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190826T084508Z' name='Arrival' Bdg='5JC[09006]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20190826T105242Z' name='Departure' Bdg='5JC[09006]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20190826T105950Z' name='Transition' Bdg='3C[09008]' waynum='3' parknum='1' intervalSec='31' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==6 );
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==1 );

    topology->Load(
        L"*09501,09008 {09501,09042,09500,09006,09008}\n");

    AsoupPtr a_9006Info1 = createAsoup(al, topology, 
    L"(:1042 909/000+09006 3603 0905 002 3100 01 09500 26 08 11 45 01/05 1 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 056 01265 00000 224 53248878 59236620 000 000 00 000 056.90 000 056 000 92 000 056 000\n"
    L"Ю4 11290 2 17 000 002.90 000 002 92 000 002 \n"
    L"Ю4 11290 2 24 000 009.90 000 009 92 000 009 \n"
    L"Ю4 11290 2 76 000 045.90 000 045 92 000 045 \n"
    L"Ю12 00 53248878 1 000 76900 43403 4953 300 00 00 00 00 00 0227 12 92 5931 04 106 31000 00000 11290 76 09050 17 00000 0000 020 1 1250 09069- 128 000 00000000\n"
    L"Ю12 00 58320029 255255255255 \n"
    L"Ю12 00 59851618 255237127255 02405905100\n"
    L"Ю12 00 53366613 255237127255 02255931106\n"
    L"Ю12 00 90126194 255253255247 0901APEHДA\n"
    L"Ю12 00 59815175 255253127247 590409509069-\n"
    L"Ю12 00 59811109 255239255255 0229\n"
    L"Ю12 00 53246286 255237127255 02275931106\n"
    L"Ю12 00 53248597 255255255255 \n"
    L"Ю12 00 54944699 255255255255 \n"
    L"Ю12 00 59056986 255237127255 02355904095\n"
    L"Ю12 00 53369849 195237127255 7694043304284340002255931106\n"
    L"Ю12 00 53370318 255255255255 \n"
    L"Ю12 00 53359386 255255255255 \n"
    L"Ю12 00 53821112 255237127255 02265904095\n"
    L"Ю12 00 59255513 255239255255 0223\n"
    L"Ю12 00 53820759 255239255255 0226\n"
    L"Ю12 00 58898115 255239255255 0225\n"
    L"Ю12 00 53293650 255237127255 02265931106\n"
    L"Ю12 00 59256115 255237127255 02235904095\n"
    L"Ю12 00 90135393 251237127247 00002310901106APEHДA\n"
    L"Ю12 00 59262386 251237127247 4000223590409509069-\n"
    L"Ю12 00 58888397 255239255255 0229\n"
    L"Ю12 00 53333670 255237127255 02255931106\n"
    L"Ю12 00 53331963 255255255255 \n"
    L"Ю12 00 59041707 195253119255 22640436195291560590409517\n"
    L"Ю12 00 59041368 255239255255 0228\n"
    L"Ю12 00 59041384 195255247255 7694043307284336076\n"
    L"Ю12 00 90053596 255237127247 02270901106APEHДA\n"
    L"Ю12 00 58653890 255237127247 0224590409509069-\n"
    L"Ю12 00 53821682 255239255255 0227\n"
    L"Ю12 00 59054205 195239247255 27190433071666060022424\n"
    L"Ю12 00 90009994 199237119247 769404330728430234090110676APEHДA\n"
    L"Ю12 00 59047076 251237127247 3600224590409509069-\n"
    L"Ю12 00 59038406 255255255255 \n"
    L"Ю12 00 58898230 255239255255 0219\n"
    L"Ю12 00 53357919 255237127255 02255931106\n"
    L"Ю12 00 59043968 255253127255 5904095\n"
    L"Ю12 00 59233601 255255255255 \n"
    L"Ю12 00 58652371 255239255255 0220\n"
    L"Ю12 00 59251033 255239255255 0223\n"
    L"Ю12 00 53363404 255237127255 02255931106\n"
    L"Ю12 00 90232638 195237247247 273604330769874600228090124APEHДA\n"
    L"Ю12 00 59233288 255237127247 0225590409509069-\n"
    L"Ю12 00 90232406 255237127247 02280901106APEHДA\n"
    L"Ю12 00 59030817 255237127247 0226590409509069-\n"
    L"Ю12 00 58892969 255239255255 0225\n"
    L"Ю12 00 53821245 255239255255 0227\n"
    L"Ю12 00 59038125 255239255255 0224\n"
    L"Ю12 00 53380184 255237127255 02285931106\n"
    L"Ю12 00 59041442 195253119255 76940433072843360590409576\n"
    L"Ю12 00 59036319 255239255255 0217\n"
    L"Ю12 00 59037952 255239255255 0225\n"
    L"Ю12 00 59047357 255239255255 0220\n"
    L"Ю12 00 53821450 255239255255 0227\n"
    L"Ю12 00 59236620 255239255255 0222:)",
        boost::gregorian::date(2019,8,26)
        );

    AsoupPtr a_9006Info2 = createAsoup(al, topology, 
    L"(:1042 909/000+09006 2746 0905 002 3100 03 09008 26 08 13 54 01/05 0 0/00 00 0\n"
    L"Ю2 0 00 00 00 00 0000 0 0 056 01265 00000 224 53248878 59236620 000 000 00 000 056.90 000 056 000 92 000 056 000\n"
    L"Ю3 530 00010501 1 12 43 0000 00000 SMIRNOV     \n"
    L"Ю3 530 00010502 9\n"
    L"Ю4 11290 2 17 000 002.90 000 002 92 000 002 \n"
    L"Ю4 11290 2 24 000 009.90 000 009 92 000 009 \n"
    L"Ю4 11290 2 76 000 045.90 000 045 92 000 045 \n"
    L"Ю12 00 53248878 1 000 76900 43403 4953 300 00 00 00 00 00 0227 12 92 5931 04 106 31000 00000 11290 76 09050 17 00000 0000 020 1 1250 09069- 128 000 00000000\n"
    L"Ю12 00 58320029 255255255255 \n"
    L"Ю12 00 59851618 255237127255 02405905100\n"
    L"Ю12 00 53366613 255237127255 02255931106\n"
    L"Ю12 00 90126194 255253255247 0901APEHДA\n"
    L"Ю12 00 59815175 255253127247 590409509069-\n"
    L"Ю12 00 59811109 255239255255 0229\n"
    L"Ю12 00 53246286 255237127255 02275931106\n"
    L"Ю12 00 53248597 255255255255 \n"
    L"Ю12 00 54944699 255255255255 \n"
    L"Ю12 00 59056986 255237127255 02355904095\n"
    L"Ю12 00 53369849 195237127255 7694043304284340002255931106\n"
    L"Ю12 00 53370318 255255255255 \n"
    L"Ю12 00 53359386 255255255255 \n"
    L"Ю12 00 53821112 255237127255 02265904095\n"
    L"Ю12 00 59255513 255239255255 0223\n"
    L"Ю12 00 53820759 255239255255 0226\n"
    L"Ю12 00 58898115 255239255255 0225\n"
    L"Ю12 00 53293650 255237127255 02265931106\n"
    L"Ю12 00 59256115 255237127255 02235904095\n"
    L"Ю12 00 90135393 251237127247 00002310901106APEHДA\n"
    L"Ю12 00 59262386 251237127247 4000223590409509069-\n"
    L"Ю12 00 58888397 255239255255 0229\n"
    L"Ю12 00 53333670 255237127255 02255931106\n"
    L"Ю12 00 53331963 255255255255 \n"
    L"Ю12 00 59041707 195253119255 22640436195291560590409517\n"
    L"Ю12 00 59041368 255239255255 0228\n"
    L"Ю12 00 59041384 195255247255 7694043307284336076\n"
    L"Ю12 00 90053596 255237127247 02270901106APEHДA\n"
    L"Ю12 00 58653890 255237127247 0224590409509069-\n"
    L"Ю12 00 53821682 255239255255 0227\n"
    L"Ю12 00 59054205 195239247255 27190433071666060022424\n"
    L"Ю12 00 90009994 199237119247 769404330728430234090110676APEHДA\n"
    L"Ю12 00 59047076 251237127247 3600224590409509069-\n"
    L"Ю12 00 59038406 255255255255 \n"
    L"Ю12 00 58898230 255239255255 0219\n"
    L"Ю12 00 53357919 255237127255 02255931106\n"
    L"Ю12 00 59043968 255253127255 5904095\n"
    L"Ю12 00 59233601 255255255255 \n"
    L"Ю12 00 58652371 255239255255 0220\n"
    L"Ю12 00 59251033 255239255255 0223\n"
    L"Ю12 00 53363404 255237127255 02255931106\n"
    L"Ю12 00 90232638 195237247247 273604330769874600228090124APEHДA\n"
    L"Ю12 00 59233288 255237127247 0225590409509069-\n"
    L"Ю12 00 90232406 255237127247 02280901106APEHДA\n"
    L"Ю12 00 59030817 255237127247 0226590409509069-\n"
    L"Ю12 00 58892969 255239255255 0225\n"
    L"Ю12 00 53821245 255239255255 0227\n"
    L"Ю12 00 59038125 255239255255 0224\n"
    L"Ю12 00 53380184 255237127255 02285931106\n"
    L"Ю12 00 59041442 195253119255 76940433072843360590409576\n"
    L"Ю12 00 59036319 255239255255 0217\n"
    L"Ю12 00 59037952 255239255255 0225\n"
    L"Ю12 00 59047357 255239255255 0220\n"
    L"Ю12 00 53821450 255239255255 0227\n"
    L"Ю12 00 59236620 255239255255 0222:)",
        boost::gregorian::date(2019,8,26)
        );


	{
		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
	}
   CPPUNIT_ASSERT( a_9006Info1 && !al.IsServed(a_9006Info1) &&
        a_9006Info2 && !al.IsServed(a_9006Info2) );

    // увязка АСОУП
   const std::string nsiBaseStr = "<Test><Junctions /></Test>";
   UtNsiBasis utNsi( nsiBaseStr );
   std::shared_ptr<const NsiBasis> nsiBasisPtr = std::make_shared<const NsiBasis>(utNsi);
   CPPUNIT_ASSERT( nsiBasisPtr );
   Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
   std::pair<AsoupToSpotResult, HemHelpful::SpotEventPtr> res = std::make_pair(AsoupToSpotResult::Impossible, nullptr);
   {
    Hem::aeAsoupToSpotFind applier( hl.GetWriteAccess(), al.GetWriteAccess(), *m_regular, context, dg,
        nsiBasisPtr, a_9006Info2, true, nullptr );
    CPPUNIT_ASSERT_NO_THROW(applier.Action());
	res = applier.getResult();
   }

   CPPUNIT_ASSERT( res.first==AsoupToSpotResult::Succeeded && res.second );
   path = hl.GetWriteAccess()->getPath(res.second);
   CPPUNIT_ASSERT( path );

   {
	AsoupLinker linkerStrict(hl.GetWriteAccess(), al.GetWriteAccess(), context, StrictIdentifyPolicy(), nullptr);
	CPPUNIT_ASSERT(linkerStrict.identify(a_9006Info2, std::make_pair(res.second, path)));
   }
  
   LinkFreeAsoup(a_9006Info2, m_chart, time_from_iso("0190826T105950Z"));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 && 
        al.IsServed(a_9006Info1) && 
        al.IsServed(a_9006Info2) );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==6 );

    auto vEvents = path->GetAllEvents();
    const auto& ev11310Form = **vEvents.begin();

    CPPUNIT_ASSERT( gis.front().second && gis.front().second->GetNumber().getNumber()==3603 &&
                    gis.back().second && gis.back().second->GetNumber().getNumber()==2746 );
}

void TC_AsoupToSpot::IdentifyWithPassagirArrivalMessage_5522()
{

    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20190916T163654Z' name='Form' Bdg='7C[09710]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190916T163654Z' name='Departure' Bdg='7C[09710]' waynum='7' parknum='1' />"
        L"<SpotEvent create_time='20190916T164535Z' name='Transition' Bdg='1C[09680]' waynum='1' parknum='1' intervalSec='12' />"
        L"<SpotEvent create_time='20190916T165225Z' name='Transition' Bdg='1BC[09670]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20190916T165853Z' name='Transition' Bdg='1C[09670]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20190916T170106Z' name='Transition' Bdg='3AC[09670]' waynum='3' parknum='3' />"
        L"<SpotEvent create_time='20190916T170701Z' name='Transition' Bdg='3p[09100]' waynum='3' parknum='1' intervalSec='3' />"
        L"<SpotEvent create_time='20190916T171304Z' name='Transition' Bdg='9C[09010]' waynum='9' parknum='1' />"
        L"<SpotEvent create_time='20190916T172527Z' name='Transition' Bdg='3JC[09006]' waynum='3' parknum='1' intervalSec='56' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==9 );
    // до увязки с АСОУП на нити 1 фрагмент
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==0 );

    auto vEvents = path->GetAllEvents();
    auto it9010 = next( vEvents.begin(), 7 );
    AsoupPtr a_9010 = createPvLdzAsoup(*m_asoupLayer,
        L"<TrainAssignment operation='6' date='16-09-2019 20:16' esr='09010' location='24.121025,56.946324' number='6526' issue_moment='20190916T172009Z'>"
        L"<Unit number='7114-01--ER2T' driver='Robots' driverTime='16-09-2019 19:11' weight='363' length='6' specialConditions='M' carrier='24'>"
        L"<Wagon number='7114-01' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"<Wagon number='7114-08' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7116-08' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7114-05' axisCount='4' weight='51' length='1' type='3' owner='24' />"
        L"<Wagon number='7116-02' axisCount='4' weight='70' length='1' type='2' owner='24' />"
        L"<Wagon number='7114-09' axisCount='4' weight='51' length='1' type='1' owner='24' />"
        L"</Unit>"
        L"</TrainAssignment>"
        );

    CPPUNIT_ASSERT( it9010!=vEvents.end() && a_9010 && !m_asoupLayer->IsServed(a_9010) );
    // увязка АСОУП
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = m_asoupLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_9010, std::make_pair(*it9010, path)));
    CPPUNIT_ASSERT(m_asoupLayer->IsServed(a_9010));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    //CPPUNIT_ASSERT( gis.back().first == (*it9150)->GetTime() );
}

void TC_AsoupToSpot::AttachPathWithSingleForm()
{
    PathPtr path1 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200322T082803Z' name='Form' Bdg='N5P[09000:09420]' waynum='1' index='' num='6219' length='6' weight='228' suburbreg='Y'>"
        L"<rwcoord picketing1_val='10~600' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='11~' picketing1_comm='Рига-Зилупе' />"
        L"<Locomotive Series='ЭР2' NumLoc='964' CarrierCode='24'>"
        L"<Crew EngineDriver='Guzenko' Tim_Beg='2020-03-22 09:23' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T082803Z' name='Span_move' Bdg='N3P[09000:09420]' waynum='1'>"
        L"<rwcoord picketing1_val='10~200' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='10~600' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T082825Z' name='Span_move' Bdg='N1P[09000:09420]' waynum='1'>"
        L"<rwcoord picketing1_val='9~797' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='10~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T082856Z' name='Station_entry' Bdg='NSP[09000]'>"
        L"<rwcoord picketing1_val='9~797' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T082919Z' name='Arrival' Bdg='1AC[09000]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200322T083108Z' name='Departure' Bdg='1AC[09000]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200322T083143Z' name='Station_exit' Bdg='PPJP[09000]'>"
        L"<rwcoord picketing1_val='7~503' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083143Z' name='Span_move' Bdg='JP1P[09000:09006]' waynum='1'>"
        L"<rwcoord picketing1_val='7~503' picketing1_comm='Рига Пассажирская' />"
        L"<rwcoord picketing1_val='7~100' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083251Z' name='Span_move' Bdg='JP3P[09000:09006]' waynum='1'>"
        L"<rwcoord picketing1_val='7~100' picketing1_comm='Рига Пассажирская' />"
        L"<rwcoord picketing1_val='6~600' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083330Z' name='Span_move' Bdg='JP5P[09000:09006]' waynum='1'>"
        L"<rwcoord picketing1_val='6~600' picketing1_comm='Рига Пассажирская' />"
        L"<rwcoord picketing1_val='6~200' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083350Z' name='Span_move' Bdg='JP7P[09000:09006]' waynum='1'>"
        L"<rwcoord picketing1_val='6~200' picketing1_comm='Рига Пассажирская' />"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083518Z' name='Station_entry' Bdg='1JC[09006]' waynum='1' parknum='1'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083613Z' name='Transition' Bdg='1JC[09006]' waynum='1' parknum='1' intervalSec='50'>"
        L"<rwcoord picketing1_val='5~609' picketing1_comm='Рига Пассажирская' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083703Z' name='Station_exit' Bdg='PPP[09006]'>"
        L"<rwcoord picketing1_val='3~880' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083703Z' name='Span_move' Bdg='R1P[09006:09010]' waynum='1'>"
        L"<rwcoord picketing1_val='3~880' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='3~665' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083719Z' name='Span_move' Bdg='R3P[09006:09010]' waynum='1'>"
        L"<rwcoord picketing1_val='3~665' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='3~265' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083741Z' name='Span_move' Bdg='R5P[09006:09010]' waynum='1'>"
        L"<rwcoord picketing1_val='3~265' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='2~865' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083800Z' name='Span_move' Bdg='R7P[09006:09010]' waynum='1'>"
        L"<rwcoord picketing1_val='2~865' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='2~465' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T083939Z' name='Station_entry' Bdg='NJP[09010]'>"
        L"<rwcoord picketing1_val='2~465' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200322T084259Z' name='Arrival' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200322T084259Z' name='Disform' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    PathPtr path2 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200322T085251Z' name='Form' Bdg='3-17SP17+[09010]' index='' num='6219' suburbreg='Y'  />"
        L"</HemPath>"
        );

    CPPUNIT_ASSERT( path1 && path1->GetEventsCount()==21 );
    CPPUNIT_ASSERT( path2 && path2->GetEventsCount()==1 );
    //склейка невозможна из-за противоположного направления нитей
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path2, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL( size_t(1), readAccess->getAllPathes().size() );
    }
}

void TC_AsoupToSpot::SkipGluingPathesOnSameStation()
{
    PathPtr path0 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200616T162633Z' name='Form' Bdg='5C[09010]' waynum='5' parknum='1' index='' num='6148' length='6' weight='242' suburbreg='Y'>"
        L"<Locomotive Series='ЭР2Т' NumLoc='7115'>"
        L"<Crew EngineDriver='Djakonovs' Tim_Beg='2020-06-16 19:32' PersonMode='1' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T163404Z' name='Departure' Bdg='5C[09010]' waynum='5' parknum='1' />"
        L"<SpotEvent create_time='20200616T163558Z' name='Station_exit' Bdg='NpP[09010]'>"
        L"<rwcoord picketing1_val='1~171' picketing1_comm='Рига-Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T163558Z' name='Span_move' Bdg='IIGP[09010:09500]' waynum='2'>"
        L"<rwcoord picketing1_val='1~171' picketing1_comm='Рига-Земитани' />"
        L"<rwcoord picketing1_val='1~846' picketing1_comm='Рига-Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T163626Z' name='Span_move' Bdg='1UPP[09010:09500]' waynum='2'>"
        L"<rwcoord picketing1_val='1~846' picketing1_comm='Рига-Земитани' />"
        L"<rwcoord picketing1_val='3~162' picketing1_comm='Рига-Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T163737Z' name='Station_entry' Bdg='4SP:4+[09500]'>"
        L"<rwcoord picketing1_val='3~162' picketing1_comm='Рига-Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T163943Z' name='Arrival' Bdg='2C[09500]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T164129Z' name='Departure' Bdg='2C[09500]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T164219Z' name='Station_exit' Bdg='15SP[09500]'>"
        L"<rwcoord picketing1_val='4~894' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164219Z' name='Span_move' Bdg='2BGP[09042:09500]' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
        L"<rwcoord picketing1_val='4~894' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164411Z' name='Station_entry' Bdg='211SP+[09042]'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164419Z' name='Transition' Bdg='211SP+[09042]' optCode='09042:09650'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164428Z' name='Station_exit' Bdg='207SP+[09042]'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164428Z' name='Span_move' Bdg='2BP[09042:09650]' waynum='2'>"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~735' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164535Z' name='Death' Bdg='2BP[09042:09650]' waynum='2' >"
        L"<rwcoord picketing1_val='6~33' picketing1_comm='Рига-Скулте' />"
        L"<rwcoord picketing1_val='6~735' picketing1_comm='Рига-Скулте' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );

    PathPtr path1 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200616T164330Z' name='Span_move' Bdg='#6148[09042:09650]' index='' num='6148' suburbreg='Y'>"
        L"<rwcoord picketing1_val='7~125' picketing1_comm='Земитани' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200616T164400Z' name='Arrival' Bdg='№6148[09650]' index='' num='6148' suburbreg='Y' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T164500Z' name='Departure' Bdg='№6148[09650]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200616T164500Z' name='Death' Bdg='№6148[09650]' waynum='2' parknum='1' />"
        L"</HemPath>"
        );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), m_asoupLayer->GetWriteAccess(), path0, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL(size_t(2), readAccess->getAllPathes().size());
    }
}

void TC_AsoupToSpot::RelinkAsoupAfterGluingPathes()
{
    PathPtr path0 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200805T093700Z' name='Departure' Bdg='ASOUP 1042[09340]' waynum='3' parknum='1' index='0934-050-0918' num='3536' length='6' weight='142' outbnd='Y'>"
        L"<Locomotive Series='М62' NumLoc='1358' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MATICAKS' Tim_Beg='2020-08-05 05:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200805T101200Z' name='Arrival' Bdg='ASOUP 1042[09330]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200805T101200Z' name='Death' Bdg='ASOUP 1042[09330]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    PathPtr path1 = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200805T101500Z' name='Arrival' Bdg='ASOUP 1042[09330]' waynum='1' parknum='1' index='0934-050-0918' num='3536' length='6' weight='142' outbnd='Y'>"
        L"<Locomotive Series='М62' NumLoc='1358' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MATICAKS' Tim_Beg='2020-08-05 05:50' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200805T110600Z' name='Departure' Bdg='ASOUP 1042[09330]' waynum='1' parknum='1' />"
        L"<SpotEvent create_time='20200805T110600' name='Death' Bdg='ASOUP 1042[09330]' waynum='1' parknum='1' />"
        L"</HemPath>"
        );

    UtLayer<AsoupLayer> al;
    al.createPath(
        L"<AsoupEvent create_time='20200805T101500Z' name='Arrival' Bdg='ASOUP 1042[09330]' index='0934-050-0918' num='3536' length='6' weight='142' outbnd='Y' waynum='1' parknum='1' dirFrom='09860' adjFrom='09340' linkBadge='ASOUP 1042[09330]' linkTime='20200805T101500Z' linkCode='Arrival' ><![CDATA[(:1042 909/000+09330 3536 0934 050 0918 01 09860 05 08 13 15 01/01 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 006 00142 00000 024 59877282 56536998 000 000 00 000 006.60 000 006 000\n"
        L"Ю4 11290 2 83 000 005.60 000 005 \n"
        L"Ю4 16169 2 13 000 001.60 000 001 \n"
        L"Ю12 00 59877282 1 000 86210 43307 1548 300 00 00 00 00 00 0242 12 60 5600 04 100 09180 09000 11290 83 09340 17 00000 0000 020 1 1020 09349  128 000 00000000\n"
        L"Ю12 00 58499583 255239255255 0236\n"
        L"Ю12 00 52487402 255239255255 0232\n"
        L"Ю12 00 55070734 255239255255 0230\n"
        L"Ю12 00 63960710 195237231255 14040436196302000023806001616913\n"
        L"Ю12 00 56536998 195237231255 86210433071548300023356001129083:)]]>"
        L"</AsoupEvent>"
        );

    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL(size_t(2), readAccess->getAllPathes().size());

        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        CPPUNIT_ASSERT( al.IsServed( asoup ) );
        auto firstEvent = path1->GetAllEvents().front();
        Hem::EventAddress<SpotEvent> sea(*firstEvent);
        AsoupLayer::ConstAsoupSet linkedAsoup = al.GetLinkedEvents(sea);
        CPPUNIT_ASSERT( !linkedAsoup.empty() );
    }

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupToSpot::findAndGlueSameNumberedPathesOnTheSameStation(m_happenLayer->GetWriteAccess(), al.GetWriteAccess(), path0, context);
    {
        auto readAccess = m_happenLayer->GetReadAccess();
        CPPUNIT_ASSERT_EQUAL(size_t(1), readAccess->getAllPathes().size());
        
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
        auto asoup = *pathAsoupList.cbegin();
        Hem::EventAddress<SpotEvent> sea = al.GetServedMessage( asoup );
        CPPUNIT_ASSERT( !sea.empty() && readAccess->getByAddress(sea));
    }

}

void TC_AsoupToSpot::IdentifyByArrival_5823()
{

    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20200825T130100Z' name='Departure' Bdg='ASOUP 1042_5p7w[09890]' waynum='7' parknum='5' index='0982-028-3100' num='V1914' length='57' weight='2374' liqrr='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T131036Z' name='Transition' Bdg='?[09859]' />"
        L"<SpotEvent create_time='20200825T131349Z' name='Transition' Bdg='?p2w1[09840]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20200825T132600Z' name='Transition' Bdg='4C[09813]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20200825T133451Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' intervalSec='3' optCode='09810:09812' />"
        L"<SpotEvent create_time='20200825T135100Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' intervalSec='7' optCode='09803:09810' />"
        L"<SpotEvent create_time='20200825T140014Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' intervalSec='2' optCode='09802:09803' />"
        L"<SpotEvent create_time='20200825T140923Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' intervalSec='4' optCode='09801:09802' />"
        L"<SpotEvent create_time='20200825T142154Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' intervalSec='8' optCode='09800:09801' />"
        L"<SpotEvent create_time='20200825T143437Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' intervalSec='15' optCode='09790:09800' />"
        L"<SpotEvent create_time='20200825T144257Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' intervalSec='7' optCode='09780:09790' />"
        L"<SpotEvent create_time='20200825T145340Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' intervalSec='4' optCode='09751:09780' />"
        L"<SpotEvent create_time='20200825T150835Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' intervalSec='4' optCode='09750:09751' />"
        L"<SpotEvent create_time='20200825T151930Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' intervalSec='3' optCode='09750:09772' />"
        L"<SpotEvent create_time='20200825T153558Z' name='Transition' Bdg='1C[09772]' waynum='1' parknum='1' intervalSec='5' optCode='09764:09772' />"
        L"<SpotEvent create_time='20200825T155203Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' intervalSec='2' optCode='09180:09764' />"
        L"<SpotEvent create_time='20200825T161735Z' name='Transition' Bdg='5C[09180]' waynum='5' parknum='1' intervalSec='5' />"
        L"<SpotEvent create_time='20200825T162225Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' intervalSec='24' optCode='09181:09251' />"
        L"<SpotEvent create_time='20200825T163459Z' name='Transition' Bdg='1C[09251]' waynum='1' parknum='1' optCode='09241:09251' index='0982-028-3100' num='J1914' length='57' weight='2374' liqrr='Y'>"
        L"<Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T164304Z' name='Transition' Bdg='1C[09241]' waynum='1' parknum='1' optCode='09240:09241' />"
        L"<SpotEvent create_time='20200825T165240Z' name='Transition' Bdg='1C[09240]' waynum='1' parknum='1' intervalSec='3' optCode='09230:09240' />"
        L"<SpotEvent create_time='20200825T170347Z' name='Transition' Bdg='1bC[09230]' waynum='1' parknum='1' intervalSec='6' optCode='09220:09230' />"
        L"<SpotEvent create_time='20200825T171218Z' name='Transition' Bdg='1C[09220]' waynum='1' parknum='1' intervalSec='2' optCode='09211:09220' />"
        L"<SpotEvent create_time='20200825T172941Z' name='Transition' Bdg='1C[09211]' waynum='1' parknum='1' intervalSec='1' optCode='09210:09211' />"
        L"<SpotEvent create_time='20200825T174043Z' name='Transition' Bdg='1C[09210]' waynum='1' parknum='1' intervalSec='3' optCode='09202:09210' />"
        L"<SpotEvent create_time='20200825T175135Z' name='Transition' Bdg='1C[09202]' waynum='1' parknum='1' intervalSec='2' optCode='09200:09202' />"
        L"<SpotEvent create_time='20200825T180133Z' name='Transition' Bdg='1C[09200]' waynum='1' parknum='1' intervalSec='2' optCode='09193:09200' />"
        L"<SpotEvent create_time='20200825T180907Z' name='Transition' Bdg='1C[09193]' waynum='1' parknum='1' optCode='09191:09193' />"
        L"<SpotEvent create_time='20200825T182746Z' name='Transition' Bdg='1C[09191]' waynum='1' parknum='1' intervalSec='8' optCode='09190:09191' />"
        L"<SpotEvent create_time='20200825T183540Z' name='Transition' Bdg='1C[09190]' waynum='1' parknum='1' intervalSec='4' optCode='09190:11420' />"
        L"<SpotEvent create_time='20200825T184813Z' name='Transition' Bdg='3C[11420]' waynum='3' parknum='1' intervalSec='13' />"
        L"<SpotEvent create_time='20200825T190611Z' name='Transition' Bdg='1C[11412]' waynum='1' parknum='1' intervalSec='8' optCode='11401:11412' />"
        L"<SpotEvent create_time='20200825T191707Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' intervalSec='4' optCode='11391:11401' />"
        L"<SpotEvent create_time='20200825T192654Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11390:11391' />"
        L"<SpotEvent create_time='20200825T194629Z' name='Arrival' Bdg='2C[11390]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T195412Z' name='Departure' Bdg='2C[11390]' waynum='2' parknum='1' optCode='11381:11390' />"
        L"<SpotEvent create_time='20200825T200418Z' name='Transition' Bdg='1C[11381]' waynum='1' parknum='1' intervalSec='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20200825T201604Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' intervalSec='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20200825T203139Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' intervalSec='8' optCode='11311:11321' />"
        L"<SpotEvent create_time='20200825T204322Z' name='Transition' Bdg='701SP:701+[11321]'>"
        L"<rwcoord picketing1_val='223~200' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200825T205001Z' name='Arrival' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205028Z' name='Departure' Bdg='2C[11310]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20200825T205616Z' name='Arrival' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T022342Z' name='Departure' Bdg='3AC[11320]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20200826T022959Z' name='Transition' Bdg='2C[11310]' waynum='2' parknum='1' intervalSec='16' />"
        L"<SpotEvent create_time='20200826T023543Z' name='Transition' Bdg='1SP[11271]'>"
        L"<rwcoord picketing1_val='440~780' picketing1_comm='Россия' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20200826T024738Z' name='Transition' Bdg='1C[11270]' waynum='1' parknum='1' intervalSec='5' optCode='11267:11270' />"
        L"<SpotEvent create_time='20200826T025517Z' name='Transition' Bdg='1AC[11267]' waynum='1' parknum='1' intervalSec='34' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T025632Z' name='Transition' Bdg='1C&apos;2[11267]' waynum='1' parknum='2' intervalSec='14' optCode='11266:11267' />"
        L"<SpotEvent create_time='20200826T030502Z' name='Transition' Bdg='1C[11266]' waynum='1' parknum='1' intervalSec='8' optCode='11265:11266' />"
        L"<SpotEvent create_time='20200826T031254Z' name='Transition' Bdg='1AC[11265]' waynum='1' parknum='1' intervalSec='4' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T031429Z' name='Transition' Bdg='1C&apos;2[11265]' waynum='1' parknum='2' intervalSec='10' optCode='11260:11265' />"
        L"<SpotEvent create_time='20200826T032635Z' name='Arrival' Bdg='1C[11260]' waynum='1' parknum='1' />"
//         L"<SpotEvent create_time='20200826T035412Z' name='Departure' Bdg='1C[11260]' waynum='1' parknum='1' />"
        L"</HemPath>" );

    CPPUNIT_ASSERT( path && path->GetEventsCount()==53 );
    CPPUNIT_ASSERT( path->GetInfoSpots().size()==1 );

    topology->Load(
        L"*09501,09008 {09501,09042,09500,09006,09008}\n");

    UtLayer<AsoupLayer> al;
    al.createPath(
        L"<AsoupEvent create_time='20200825T130100Z' name='Departure' Bdg='ASOUP 1042_5p7w[09890]' index='0982-028-3100' num='1914' length='57' weight='2374' liqrr='Y' waynum='7' parknum='5' dirTo='09750' adjTo='09859' linkBadge='ASOUP 1042_5p7w[09890]' linkTime='20200825T130100Z' linkCode='Departure'><![CDATA[(:1042 909/000+09820 1914 0982 028 3100 03 09750 25 08 16 01 05/07 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 77996395 57878712 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 14 30 0000 00000 CIBULSKIS   \n"
        L"Ю3 530 00009282 9\n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 77996395 1 000 27360 48816 6987 357 01 00 00 00 00 0362 12 70 0769 04 086 31000 00000 07792 24 09820 18 00000 0000 020 1 1250 09836  128 000 00000000\n"
        L"Ю12 00 77966794 255239255255 0361\n"
        L"Ю12 00 77937696 255255255255 \n"
        L"Ю12 00 78067394 255239255255 0387\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 77631299 255239255255 0373\n"
        L"Ю12 00 58237389 255237255255 03495080\n"
        L"Ю12 00 58260555 255239255255 0346\n"
        L"Ю12 00 58260977 255255255255 \n"
        L"Ю12 00 76660430 255237255255 03780769\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 78077294 255239255255 0387\n"
        L"Ю12 00 58242629 255237255255 03165080\n"
        L"Ю12 00 76641943 255237255255 03150769\n"
        L"Ю12 00 58242470 255253255255 5080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 50832476 255239255255 0368\n"
        L"Ю12 00 76624659 255237255255 03160769\n"
        L"Ю12 00 50518489 251237255255 45703465080\n"
        L"Ю12 00 58237736 255239255255 0347\n"
        L"Ю12 00 76659184 255237255255 03400769\n"
        L"Ю12 00 58237322 255237255255 03455080\n"
        L"Ю12 00 58260621 255239255255 0346\n"
        L"Ю12 00 77302693 255237255255 03730769\n"
        L"Ю12 00 77706398 255239255255 0372\n"
        L"Ю12 00 78089992 255239255255 0388\n"
        L"Ю12 00 77988392 255239255255 0361\n"
        L"Ю12 00 77936193 255255255255 \n"
        L"Ю12 00 58267931 255237255255 03465080\n"
        L"Ю12 00 76660653 255237255255 03780769\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77942894 255239255255 0361\n"
        L"Ю12 00 58237116 251237255255 55703455080\n"
        L"Ю12 00 77998193 255237255255 03620769\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77936391 255239255255 0361\n"
        L"Ю12 00 57715617 255237255255 03495080\n"
        L"Ю12 00 77714293 255237255255 03720769\n"
        L"Ю12 00 76657675 255239255255 0373\n"
        L"Ю12 00 77635498 255255255255 \n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 76657741 255237255255 03740769\n"
        L"Ю12 00 77302396 255239255255 0372\n"
        L"Ю12 00 76660380 255239255255 0351\n"
        L"Ю12 00 58237306 251237255255 35703475080\n"
        L"Ю12 00 58237793 255239255255 0346\n"
        L"Ю12 00 78078797 255237255255 03890769\n"
        L"Ю12 00 78080496 255239255255 0386\n"
        L"Ю12 00 77965291 255239255255 0362\n"
        L"Ю12 00 58236993 255237255255 03455080\n"
        L"Ю12 00 58242298 255239255255 0316\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242413 255237255255 03155080\n"
        L"Ю12 00 58237330 255239255255 0345\n"
        L"Ю12 00 77966596 255237255255 03610769\n"
        L"Ю12 00 78084894 255239255255 0390\n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 77632594 255239255255 0373\n"
        L"Ю12 00 77719797 255255255255 \n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77971190 255239255255 0361\n"
        L"Ю12 00 57878712 255237255255 03685780:)]]><Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    al.createPath(
    L"<AsoupEvent create_time='20200825T161700Z' name='Transition' Bdg='ASOUP 1042[09180]' index='0982-028-3100' num='1914' length='57' weight='2374' liqrr='Y' dirFrom='09820' dirTo='11420' adjFrom='09764' adjTo='09181' linkBadge='5C[09180]' linkTime='20200825T161735Z' linkCode='Transition'><![CDATA[(:1042 909/000+09180 1914 0982 028 3100 04 09820+11420 25 08 19 17 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 77996395 57878712 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 14 30 0000 00000 CIBULSKIS   \n"
        L"Ю3 530 00009282 9\n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 77996395 1 000 27360 48816 6987 357 01 00 00 00 00 0362 12 70 0769 04 086 31000 00000 07792 24 09820 18 00000 0000 020 1 1250 09836  128 000 00000000\n"
        L"Ю12 00 77966794 255239255255 0361\n"
        L"Ю12 00 77937696 255255255255 \n"
        L"Ю12 00 78067394 255239255255 0387\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 77631299 255239255255 0373\n"
        L"Ю12 00 58237389 255237255255 03495080\n"
        L"Ю12 00 58260555 255239255255 0346\n"
        L"Ю12 00 58260977 255255255255 \n"
        L"Ю12 00 76660430 255237255255 03780769\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 78077294 255239255255 0387\n"
        L"Ю12 00 58242629 255237255255 03165080\n"
        L"Ю12 00 76641943 255237255255 03150769\n"
        L"Ю12 00 58242470 255253255255 5080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 50832476 255239255255 0368\n"
        L"Ю12 00 76624659 255237255255 03160769\n"
        L"Ю12 00 50518489 251237255255 45703465080\n"
        L"Ю12 00 58237736 255239255255 0347\n"
        L"Ю12 00 76659184 255237255255 03400769\n"
        L"Ю12 00 58237322 255237255255 03455080\n"
        L"Ю12 00 58260621 255239255255 0346\n"
        L"Ю12 00 77302693 255237255255 03730769\n"
        L"Ю12 00 77706398 255239255255 0372\n"
        L"Ю12 00 78089992 255239255255 0388\n"
        L"Ю12 00 77988392 255239255255 0361\n"
        L"Ю12 00 77936193 255255255255 \n"
        L"Ю12 00 58267931 255237255255 03465080\n"
        L"Ю12 00 76660653 255237255255 03780769\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77942894 255239255255 0361\n"
        L"Ю12 00 58237116 251237255255 55703455080\n"
        L"Ю12 00 77998193 255237255255 03620769\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77936391 255239255255 0361\n"
        L"Ю12 00 57715617 255237255255 03495080\n"
        L"Ю12 00 77714293 255237255255 03720769\n"
        L"Ю12 00 76657675 255239255255 0373\n"
        L"Ю12 00 77635498 255255255255 \n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 76657741 255237255255 03740769\n"
        L"Ю12 00 77302396 255239255255 0372\n"
        L"Ю12 00 76660380 255239255255 0351\n"
        L"Ю12 00 58237306 251237255255 35703475080\n"
        L"Ю12 00 58237793 255239255255 0346\n"
        L"Ю12 00 78078797 255237255255 03890769\n"
        L"Ю12 00 78080496 255239255255 0386\n"
        L"Ю12 00 77965291 255239255255 0362\n"
        L"Ю12 00 58236993 255237255255 03455080\n"
        L"Ю12 00 58242298 255239255255 0316\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242413 255237255255 03155080\n"
        L"Ю12 00 58237330 255239255255 0345\n"
        L"Ю12 00 77966596 255237255255 03610769\n"
        L"Ю12 00 78084894 255239255255 0390\n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 77632594 255239255255 0373\n"
        L"Ю12 00 77719797 255255255255 \n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77971190 255239255255 0361\n"
        L"Ю12 00 57878712 255237255255 03685780:)]]><Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</AsoupEvent>");

    al.createPath(
    L"<AsoupEvent create_time='20200825T184800Z' name='Transition' Bdg='ASOUP 1042[11420]' index='0982-028-3100' num='1914' length='57' weight='2374' liqrr='Y' dirFrom='09180' dirTo='11310' adjFrom='09190' adjTo='11412' linkBadge='3C[11420]' linkTime='20200825T184813Z' linkCode='Transition'><![CDATA[(:1042 909/000+11420 1914 0982 028 3100 04 09180+11310 25 08 21 48 00/00 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 77996395 57878712 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 14 30 0000 00000 CIBULSKIS   \n"
        L"Ю3 530 00009282 9\n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 77996395 1 000 27360 48816 6987 357 01 00 00 00 00 0362 12 70 0769 04 086 31000 00000 07792 24 09820 18 00000 0000 020 1 1250 09836  128 000 00000000\n"
        L"Ю12 00 77966794 255239255255 0361\n"
        L"Ю12 00 77937696 255255255255 \n"
        L"Ю12 00 78067394 255239255255 0387\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 77631299 255239255255 0373\n"
        L"Ю12 00 58237389 255237255255 03495080\n"
        L"Ю12 00 58260555 255239255255 0346\n"
        L"Ю12 00 58260977 255255255255 \n"
        L"Ю12 00 76660430 255237255255 03780769\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 78077294 255239255255 0387\n"
        L"Ю12 00 58242629 255237255255 03165080\n"
        L"Ю12 00 76641943 255237255255 03150769\n"
        L"Ю12 00 58242470 255253255255 5080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 50832476 255239255255 0368\n"
        L"Ю12 00 76624659 255237255255 03160769\n"
        L"Ю12 00 50518489 251237255255 45703465080\n"
        L"Ю12 00 58237736 255239255255 0347\n"
        L"Ю12 00 76659184 255237255255 03400769\n"
        L"Ю12 00 58237322 255237255255 03455080\n"
        L"Ю12 00 58260621 255239255255 0346\n"
        L"Ю12 00 77302693 255237255255 03730769\n"
        L"Ю12 00 77706398 255239255255 0372\n"
        L"Ю12 00 78089992 255239255255 0388\n"
        L"Ю12 00 77988392 255239255255 0361\n"
        L"Ю12 00 77936193 255255255255 \n"
        L"Ю12 00 58267931 255237255255 03465080\n"
        L"Ю12 00 76660653 255237255255 03780769\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77942894 255239255255 0361\n"
        L"Ю12 00 58237116 251237255255 55703455080\n"
        L"Ю12 00 77998193 255237255255 03620769\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77936391 255239255255 0361\n"
        L"Ю12 00 57715617 255237255255 03495080\n"
        L"Ю12 00 77714293 255237255255 03720769\n"
        L"Ю12 00 76657675 255239255255 0373\n"
        L"Ю12 00 77635498 255255255255 \n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 76657741 255237255255 03740769\n"
        L"Ю12 00 77302396 255239255255 0372\n"
        L"Ю12 00 76660380 255239255255 0351\n"
        L"Ю12 00 58237306 251237255255 35703475080\n"
        L"Ю12 00 58237793 255239255255 0346\n"
        L"Ю12 00 78078797 255237255255 03890769\n"
        L"Ю12 00 78080496 255239255255 0386\n"
        L"Ю12 00 77965291 255239255255 0362\n"
        L"Ю12 00 58236993 255237255255 03455080\n"
        L"Ю12 00 58242298 255239255255 0316\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242413 255237255255 03155080\n"
        L"Ю12 00 58237330 255239255255 0345\n"
        L"Ю12 00 77966596 255237255255 03610769\n"
        L"Ю12 00 78084894 255239255255 0390\n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 77632594 255239255255 0373\n"
        L"Ю12 00 77719797 255255255255 \n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77971190 255239255255 0361\n"
        L"Ю12 00 57878712 255237255255 03685780:)]]><Locomotive Series='2ТЭ116' NumLoc='928' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='CIBULSKIS' Tim_Beg='2020-08-25 14:30' />"
        L"</Locomotive>"
        L"</AsoupEvent>" );

    al.createPath(
        L"<AsoupEvent create_time='20200825T204800Z' name='Arrival' Bdg='ASOUP 1042_1p3w[11320]' index='0982-028-3100' num='1914' length='57' weight='2374' liqrr='Y' waynum='3' parknum='1' dirFrom='11420' adjFrom='11310' linkBadge='3AC[11320]' linkTime='20200825T205616Z' linkCode='Arrival'><![CDATA[(:1042 909/000+11310 1914 0982 028 3100 01 11420 25 08 23 48 01/26 1 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 77996395 57878712 000 000 00 000 066.70 000 066 000\n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 77996395 1 000 27360 48816 6987 357 01 00 00 00 00 0362 12 70 0769 04 086 31000 00000 07792 24 09820 18 00000 0000 020 1 1250 09836  128 000 00000000\n"
        L"Ю12 00 77966794 255239255255 0361\n"
        L"Ю12 00 77937696 255255255255 \n"
        L"Ю12 00 78067394 255239255255 0387\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 77631299 255239255255 0373\n"
        L"Ю12 00 58237389 255237255255 03495080\n"
        L"Ю12 00 58260555 255239255255 0346\n"
        L"Ю12 00 58260977 255255255255 \n"
        L"Ю12 00 76660430 255237255255 03780769\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 78077294 255239255255 0387\n"
        L"Ю12 00 58242629 255237255255 03165080\n"
        L"Ю12 00 76641943 255237255255 03150769\n"
        L"Ю12 00 58242470 255253255255 5080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 50832476 255239255255 0368\n"
        L"Ю12 00 76624659 255237255255 03160769\n"
        L"Ю12 00 50518489 251237255255 45703465080\n"
        L"Ю12 00 58237736 255239255255 0347\n"
        L"Ю12 00 76659184 255237255255 03400769\n"
        L"Ю12 00 58237322 255237255255 03455080\n"
        L"Ю12 00 58260621 255239255255 0346\n"
        L"Ю12 00 77302693 255237255255 03730769\n"
        L"Ю12 00 77706398 255239255255 0372\n"
        L"Ю12 00 78089992 255239255255 0388\n"
        L"Ю12 00 77988392 255239255255 0361\n"
        L"Ю12 00 77936193 255255255255 \n"
        L"Ю12 00 58267931 255237255255 03465080\n"
        L"Ю12 00 76660653 255237255255 03780769\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77942894 255239255255 0361\n"
        L"Ю12 00 58237116 251237255255 55703455080\n"
        L"Ю12 00 77998193 255237255255 03620769\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77936391 255239255255 0361\n"
        L"Ю12 00 57715617 255237255255 03495080\n"
        L"Ю12 00 77714293 255237255255 03720769\n"
        L"Ю12 00 76657675 255239255255 0373\n"
        L"Ю12 00 77635498 255255255255 \n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 76657741 255237255255 03740769\n"
        L"Ю12 00 77302396 255239255255 0372\n"
        L"Ю12 00 76660380 255239255255 0351\n"
        L"Ю12 00 58237306 251237255255 35703475080\n"
        L"Ю12 00 58237793 255239255255 0346\n"
        L"Ю12 00 78078797 255237255255 03890769\n"
        L"Ю12 00 78080496 255239255255 0386\n"
        L"Ю12 00 77965291 255239255255 0362\n"
        L"Ю12 00 58236993 255237255255 03455080\n"
        L"Ю12 00 58242298 255239255255 0316\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242413 255237255255 03155080\n"
        L"Ю12 00 58237330 255239255255 0345\n"
        L"Ю12 00 77966596 255237255255 03610769\n"
        L"Ю12 00 78084894 255239255255 0390\n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 77632594 255239255255 0373\n"
        L"Ю12 00 77719797 255255255255 \n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77971190 255239255255 0361\n"
        L"Ю12 00 57878712 255237255255 03685780:)]]></AsoupEvent>"
        );

    {
        auto readAccess = m_happenLayer->GetReadAccess();
        auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
        CPPUNIT_ASSERT( pathAsoupList.size() == 4 );
        for ( auto asoup : pathAsoupList )
        {
            al.IsServed( asoup );
            Hem::EventAddress<SpotEvent> sea = al.GetServedMessage( asoup );
            CPPUNIT_ASSERT( !sea.empty() && readAccess->getByAddress(sea));
        }
    }

    std::wstring a_11260Str = 
        L"(:1042 909/000+11260 2968 0982 028 3100 01 11310 26 08 06 26 01/01 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02374 00000 264 57878712 77996395 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 530 00009281 1 03 45 0000 17996 OЛЬШEBCKИЙ  \n"
        L"Ю3 530 00009282 9\n"
        L"Ю3 531 00013580 2 03 45 0000 27668 PAXHO       \n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 57878712 1 000 27360 48816 6987 357 01 00 00 00 00 0368 12 70 5780 04 086 31000 00000 07792 24 09820 18 09830 7051 020 1 1280 09836  128 000 00000000\n"
        L"Ю12 00 77971190 255237255239 036107691250\n"
        L"Ю12 00 77935492 255239255255 0360\n"
        L"Ю12 00 77719797 255239255255 0373\n"
        L"Ю12 00 77632594 255255255255 \n"
        L"Ю12 00 77943090 255239255255 0361\n"
        L"Ю12 00 78084894 255239255239 03901280\n"
        L"Ю12 00 77966596 255239255239 03611250\n"
        L"Ю12 00 58237330 255237255255 03455080\n"
        L"Ю12 00 58242413 255239255255 0315\n"
        L"Ю12 00 77942092 255237255255 03610769\n"
        L"Ю12 00 58242298 255237255255 03165080\n"
        L"Ю12 00 58236993 255239255255 0345\n"
        L"Ю12 00 77965291 255237255255 03620769\n"
        L"Ю12 00 78080496 255239255239 03861280\n"
        L"Ю12 00 78078797 255239255255 0389\n"
        L"Ю12 00 58237793 255237255239 034650801250\n"
        L"Ю12 00 58237306 255239255255 0347\n"
        L"Ю12 00 76660380 251237255239 557035107691280\n"
        L"Ю12 00 77302396 255239255239 03721250\n"
        L"Ю12 00 76657741 255239255239 03741280\n"
        L"Ю12 00 57739336 255237255255 03775080\n"
        L"Ю12 00 77635498 255237255239 037307691250\n"
        L"Ю12 00 76657675 255255255239 1280\n"
        L"Ю12 00 77714293 255239255239 03721250\n"
        L"Ю12 00 57715617 255237255239 034950801280\n"
        L"Ю12 00 77936391 255237255239 036107691250\n"
        L"Ю12 00 77972099 255239255255 0360\n"
        L"Ю12 00 77998193 255239255255 0362\n"
        L"Ю12 00 58237116 255237255255 03455080\n"
        L"Ю12 00 77942894 251237255255 45703610769\n"
        L"Ю12 00 77721199 255239255255 0375\n"
        L"Ю12 00 77970895 255239255255 0362\n"
        L"Ю12 00 77988491 255239255255 0361\n"
        L"Ю12 00 77644995 255239255255 0371\n"
        L"Ю12 00 77705499 255239255255 0372\n"
        L"Ю12 00 76660653 255239255239 03781280\n"
        L"Ю12 00 58267931 255237255239 034650801250\n"
        L"Ю12 00 77936193 255237255255 03610769\n"
        L"Ю12 00 77988392 255255255255 \n"
        L"Ю12 00 78089992 255239255239 03881280\n"
        L"Ю12 00 77706398 255239255239 03721250\n"
        L"Ю12 00 77302693 255239255255 0373\n"
        L"Ю12 00 58260621 255237255255 03465080\n"
        L"Ю12 00 58237322 255239255255 0345\n"
        L"Ю12 00 76659184 255237255239 034007691280\n"
        L"Ю12 00 58237736 255237255239 034750801250\n"
        L"Ю12 00 50518489 255239255239 03461280\n"
        L"Ю12 00 76624659 251237255255 35703160769\n"
        L"Ю12 00 50832476 255237255255 03685080\n"
        L"Ю12 00 50823566 255239255255 0367\n"
        L"Ю12 00 58242470 255239255239 03151250\n"
        L"Ю12 00 76641943 255253255239 07691280\n"
        L"Ю12 00 58242629 255237255239 031650801250\n"
        L"Ю12 00 78077294 255237255239 038707691280\n"
        L"Ю12 00 78065893 255239255255 0386\n"
        L"Ю12 00 76660430 255239255255 0378\n"
        L"Ю12 00 58260977 255237255239 034650801250\n"
        L"Ю12 00 58260555 255255255255 \n"
        L"Ю12 00 58237389 255239255255 0349\n"
        L"Ю12 00 77631299 255237255255 03730769\n"
        L"Ю12 00 77293793 255239255255 0374\n"
        L"Ю12 00 78067394 255239255239 03871280\n"
        L"Ю12 00 77937696 255239255239 03611250\n"
        L"Ю12 00 77966794 255255255255 \n"
        L"Ю12 00 77996395 255239255255 0362:)";

    auto msg = Asoup::Message::parse(a_11260Str, boost::gregorian::date(2020,8,26), Asoup::AsoupOperationMode::LDZ, true);
    CPPUNIT_ASSERT(msg);
    AsoupPtr a_11260Arrival = AsoupEvent::Create(*msg, topology.get(), nullptr, nullptr, BadgeE(), Asoup::AsoupOperationMode::LDZ);
    CPPUNIT_ASSERT(a_11260Arrival);
    al.AddAsoupEvent(a_11260Arrival);
    CPPUNIT_ASSERT(al.FindEvent(*a_11260Arrival));

    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 5 );
    CPPUNIT_ASSERT( a_11260Arrival && !al.IsServed(a_11260Arrival) );
    auto vEvents = path->GetAllEvents();
    auto it11320Arrival = next( vEvents.begin(), 42 );
    auto it11320Departure = next( vEvents.begin(), 43 );
    auto it11260 = next( vEvents.begin(), 52 );
    // увязка АСОУП
    auto happenAccess = m_happenLayer->GetWriteAccess();
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    AsoupLinker linkerStrict(happenAccess, al.GetWriteAccess(), context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_11260Arrival, std::make_pair(*it11260, path)));
    CPPUNIT_ASSERT(al.IsServed(a_11260Arrival));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    auto gis1 = next( gis.cbegin(), 1 );
    CPPUNIT_ASSERT( gis1->first == (*it11320Departure)->GetTime() && 
        gis1->second->GetNumber().getNumber() == a_11260Arrival->GetNumber() );

}

void TC_AsoupToSpot::LinkAsoupByTime_6071()
{
	UtUserChart m_chart;
	auto& hl = m_chart.getHappenLayer();
	auto& al = m_chart.getAsoupLayer();

    PathPtr path = createPath( hl,
        L"<HemPath>"
        L"<SpotEvent create_time='20201124T200100Z' name='Departure' Bdg='ASOUP 1042_1p[11001]' parknum='2' waynum='5' num='2721' index='' />"
        L"<SpotEvent create_time='20201124T200435Z' name='Transition' Bdg='?[11002]' />"
        L"<SpotEvent create_time='20201124T200938Z' name='Transition' Bdg='4/6SP:4+[11451]'>"
        L"<rwcoord picketing1_val='387~399' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201124T201754Z' name='Transition' Bdg='5SP[11445]'>"
        L"<rwcoord picketing1_val='382~660' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20201124T202923Z' name='Transition' Bdg='1C[11443]' waynum='1' parknum='1' optCode='11442:11443' />"
        L"<SpotEvent create_time='20201124T203633Z' name='Transition' Bdg='1AC[11442]' waynum='1' parknum='1' optCode='11442:11446' />"
        L"<SpotEvent create_time='20201124T204947Z' name='Transition' Bdg='1C[11446]' waynum='1' parknum='1' optCode='11432:11446' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( path && path->GetEventsCount()==7 );
    auto vEvents = path->GetAllEvents();
    auto itDeparture11001 = vEvents.begin();


	auto a_InfoChanging11001 = createAsoup(al, topology, 
        L"(:1042 909/000+11001 2821 1655 071 0862 02 11420 24 11 22 25 02/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 056 05123 03494 252 53009858 73923823 000 000 00 061 002.70 061 000 000 73 061 000 000 90 000 002 000\n"
        L"Ю4 08620 2 08 061 002.70 061 000 73 061 000 90 000 002 :)",
		boost::gregorian::date(2020, 11, 24)
        );

    auto a_Departure11001 = createAsoup(al, topology,
        L"(:1042 909/000+11001 2721 1655 071 0862 03 11420 24 11 23 01 02/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 1 056 05123 03494 252 53009858 73923823 000 000 00 061 002.70 061 000 000 73 061 000 000 90 000 002 000\n"
        L"Ю3 575 00034251 1 21 31 0000 00000 CIBULSKIS   \n"
        L"Ю3 575 00034252 9\n"
        L"Ю4 08620 2 08 061 002.70 061 000 73 061 000 90 000 002 \n"
        L"Ю12 00 53009858 1 000 08620 42103 2428 005 00 00 00 00 00 0330 12 90 5917 04 167 08620 00000 08620 08 16550 43 00000 0000 026 1 1080 0      144 000 00000000\n"
        L"Ю12 00 53009783 255255255255 \n"
        L"Ю12 00 74722315 129225124049 062082202210630892800102722070073008603165503494021165561132032\n"
        L"Ю12 00 74753161 191239255245 06102700     000\n"
        L"Ю12 00 73038887 191239254127 0550232000000000\n"
        L"Ю12 00 74755158 191239254127 0630270165503494\n"
        L"Ю12 00 74744053 191239254127 0590272000000000\n"
        L"Ю12 00 72021959 191237255255 05602500720\n"
        L"Ю12 00 74873068 255237255255 02640748\n"
        L"Ю12 00 73961948 191237254127 06202670732165503494\n"
        L"Ю12 00 73046955 191237255255 05502320730\n"
        L"Ю12 00 74940255 191237255255 06202690732\n"
        L"Ю12 00 73045726 191237255255 05602330730\n"
        L"Ю12 00 73058984 191239255255 0590240\n"
        L"Ю12 00 74752841 191239255255 0630271\n"
        L"Ю12 00 73063612 191239255255 0580232\n"
        L"Ю12 00 74811829 255237255255 02670748\n"
        L"Ю12 00 73048324 191237255255 05302320730\n"
        L"Ю12 00 73058976 191255255255 054\n"
        L"Ю12 00 72545874 191237255255 05602500720\n"
        L"Ю12 00 74869843 191237255255 05202640748\n"
        L"Ю12 00 73035669 255237255255 02300730\n"
        L"Ю12 00 74752833 191239255255 0600270\n"
        L"Ю12 00 74870247 191237255255 05602670748\n"
        L"Ю12 00 74805227 191255255255 055\n"
        L"Ю12 00 73997488 191237254127 06002710732000000000\n"
        L"Ю12 00 74889676 191237254127 05402660748165503494\n"
        L"Ю12 00 73962011 191237254127 06002690732000000000\n"
        L"Ю12 00 73960320 191255255255 058\n"
        L"Ю12 00 73521288 191237255255 06102700730\n"
        L"Ю12 00 74945205 191237255255 06302670732\n"
        L"Ю12 00 74754839 191237255255 06502700730\n"
        L"Ю12 00 73049967 191239254127 0520232165503494\n"
        L"Ю12 00 72046212 191237255255 05502510720\n"
        L"Ю12 00 73070336 191237255255 05402320730\n"
        L"Ю12 00 74872938 191237255255 05702630748\n"
        L"Ю12 00 73064909 191237255255 05102320730\n"
        L"Ю12 00 74942814 191237254127 06102700732000000000\n"
        L"Ю12 00 72031016 191237255255 05502500720\n"
        L"Ю12 00 74805508 255237255255 02640748\n"
        L"Ю12 00 74747163 191237254127 06202710730165503494\n"
        L"Ю12 00 72023369 191237255255 05502460720\n"
        L"Ю12 00 74946559 191237255255 06302660732\n"
        L"Ю12 00 73412157 191237255255 06002670730\n"
        L"Ю12 00 74755133 191239255255 0630270\n"
        L"Ю12 00 72036551 191237255255 05502500720\n"
        L"Ю12 00 74813270 191237255255 05802630748\n"
        L"Ю12 00 74705443 191237255255 05202320730\n"
        L"Ю12 00 74812785 191237255255 05402700748\n"
        L"Ю12 00 74891433 191255255255 050\n"
        L"Ю12 00 74872920 191239255255 0530264\n"
        L"Ю12 00 74890021 191239255255 0540266\n"
        L"Ю12 00 73725541 255237255255 02320730\n"
        L"Ю12 00 74946476 191237255255 06102660732\n"
        L"Ю12 00 74940677 255239255255 0269\n"
        L"Ю12 00 73051344 191237255255 05402450730\n"
        L"Ю12 00 73062648 191239255255 0530232\n"
        L"Ю12 00 73043432 191239255255 0550240\n"
        L"Ю12 00 73958506 191237255255 06102550732\n"
        L"Ю12 00 73056954 191237255255 05702320730\n"
        L"Ю12 00 74723875 191239255255 0620270\n"
        L"Ю12 00 74873332 191237255255 05302620748\n"
        L"Ю12 00 73923823 191237255127 060025507320021:)",
		boost::gregorian::date(2020, 11, 24)
        );

    CPPUNIT_ASSERT(!al.IsServed(a_Departure11001));
    CPPUNIT_ASSERT(!al.IsServed(a_InfoChanging11001));

	{
    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, time_from_iso("20201124T200100"));
    AsoupLinker linkerStrict(hl.GetWriteAccess(), al.GetWriteAccess(), context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_Departure11001, std::make_pair(*itDeparture11001, path)));
    CPPUNIT_ASSERT(al.IsServed(a_Departure11001));
	}

    CPPUNIT_ASSERT(al.IsServed(a_InfoChanging11001));

    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto frg = gis.front();
    CPPUNIT_ASSERT( frg.first == (*itDeparture11001)->GetTime() && 
        frg.second->GetNumber().getNumber() == a_Departure11001->GetNumber() );

}

void TC_AsoupToSpot::IdentifyWithSaveCrewInfo_6139()
{
    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20210224T020500Z' name='Departure' Bdg='ASOUP 1042_5p9w[09890]' waynum='9' parknum='5' index='0982-016-3100' num='V2408' length='57' weight='2421' through='Y'>"
        L"<Locomotive Series='2М62УМ' NumLoc='116' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='M-SMIRNOVS' Tim_Beg='2021-02-24 02:00' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210224T021954Z' name='Transition' Bdg='?[09859]' />"
        L"<SpotEvent create_time='20210224T022453Z' name='Transition' Bdg='?p2w1[09840]' waynum='1' parknum='2' />"
        L"<SpotEvent create_time='20210224T023948Z' name='Transition' Bdg='1C[09813]' waynum='1' parknum='1' optCode='09812:09813' />"
        L"<SpotEvent create_time='20210224T024833Z' name='Transition' Bdg='1C[09812]' waynum='1' parknum='1' optCode='09810:09812' />"
        L"<SpotEvent create_time='20210224T030627Z' name='Transition' Bdg='1C[09810]' waynum='1' parknum='1' optCode='09803:09810' />"
        L"<SpotEvent create_time='20210224T031715Z' name='Transition' Bdg='1C[09803]' waynum='1' parknum='1' optCode='09802:09803' />"
        L"<SpotEvent create_time='20210224T032756Z' name='Transition' Bdg='1C[09802]' waynum='1' parknum='1' optCode='09801:09802' />"
        L"<SpotEvent create_time='20210224T034119Z' name='Transition' Bdg='1C[09801]' waynum='1' parknum='1' optCode='09800:09801' />"
        L"<SpotEvent create_time='20210224T035628Z' name='Transition' Bdg='2C[09800]' waynum='2' parknum='1' optCode='09790:09800' />"
        L"<SpotEvent create_time='20210224T040543Z' name='Transition' Bdg='2C[09790]' waynum='2' parknum='1' optCode='09780:09790' />"
        L"<SpotEvent create_time='20210224T041551Z' name='Transition' Bdg='1C[09780]' waynum='1' parknum='1' optCode='09751:09780' />"
        L"<SpotEvent create_time='20210224T043214Z' name='Transition' Bdg='1C[09751]' waynum='1' parknum='1' optCode='09750:09751' />"
        L"<SpotEvent create_time='20210224T044340Z' name='Transition' Bdg='3C[09750]' waynum='3' parknum='1' optCode='09750:09772' />"
        L"<SpotEvent create_time='20210224T050400Z' name='Arrival' Bdg='2C[09772]' waynum='2' parknum='1' />"
        L"<SpotEvent create_time='20210224T051327Z' name='Departure' Bdg='2C[09772]' waynum='2' parknum='1' optCode='09764:09772' />"
        L"<SpotEvent create_time='20210224T053335Z' name='Transition' Bdg='1C[09764]' waynum='1' parknum='1' optCode='09180:09764' />"
        L"<SpotEvent create_time='20210224T060024Z' name='Arrival' Bdg='9C[09180]' waynum='9' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( path && path->GetEventsCount()==18 );
    auto vEvents = path->GetAllEvents();
    auto h_Arrival9180 = vEvents.back();
    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto frg = gis.front();
    Locomotive loc;
    CPPUNIT_ASSERT( frg.second && frg.second->get_head_locomotive(loc) );
    CPPUNIT_ASSERT( !loc.get_crew().empty() );

    UtLayer<AsoupLayer> al;
    al.createPath(
        L"<AsoupEvent create_time='20210224T060100Z' name='Arrival' Bdg='ASOUP 1042_1p9w[09180]' index='0982-016-3100' num='2408' length='57' weight='2421' through='Y' waynum='9' parknum='1' dirFrom='09820' adjFrom='09764' ><![CDATA[(:1042 909/000+09180 2408 0982 016 3100 01 09820 24 02 08 01 01/09 2 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 057 02421 00000 264 77837995 77637296 000 000 00 000 066.70 000 066 000\n"
        L"Ю3 583 00001161 1 00 00 0000 00000 0           \n"
        L"Ю3 583 00001162 9\n"
        L"Ю4 07792 2 24 000 066.70 000 066 \n"
        L"Ю12 00 77837995 1 000 27360 48816 6987 357 01 00 00 00 00 0382 12 70 0769 04 086 31000 00000 07792 24 09820 18 00000 0000 020 1 1250 09826  128 000 00000000\n"
        L"Ю12 00 77831998 255239255255 0381\n"
        L"Ю12 00 76660430 255239255255 0378\n"
        L"Ю12 00 77720498 255239255255 0372\n"
        L"Ю12 00 76657618 255239255255 0373\n"
        L"Ю12 00 77970697 255239255255 0361\n"
        L"Ю12 00 77934891 255239255255 0362\n"
        L"Ю12 00 58267790 255237255255 03465080\n"
        L"Ю12 00 50832740 255239255255 0368\n"
        L"Ю12 00 77634699 255237255255 03720769\n"
        L"Ю12 00 58237710 255237255255 03475080\n"
        L"Ю12 00 78068392 255237255255 03870769\n"
        L"Ю12 00 77997799 255239255255 0363\n"
        L"Ю12 00 58260654 255237255255 03455080\n"
        L"Ю12 00 78068095 255237255255 03870769\n"
        L"Ю12 00 76647080 255239255255 0370\n"
        L"Ю12 00 50823145 255237255255 03675080\n"
        L"Ю12 00 58267618 255239255255 0346\n"
        L"Ю12 00 78068293 251237255255 45703880769\n"
        L"Ю12 00 76653252 255239255255 0372\n"
        L"Ю12 00 50823343 255237255255 03685080\n"
        L"Ю12 00 77719193 255237255255 03720769\n"
        L"Ю12 00 76644475 255255255255 \n"
        L"Ю12 00 77631893 255239255255 0371\n"
        L"Ю12 00 76641844 255239255255 0316\n"
        L"Ю12 00 77989994 255239255255 0361\n"
        L"Ю12 00 76653237 255239255255 0374\n"
        L"Ю12 00 77997195 255239255255 0362\n"
        L"Ю12 00 77298891 255239255255 0386\n"
        L"Ю12 00 77718690 255239255255 0372\n"
        L"Ю12 00 76644244 255239255255 0369\n"
        L"Ю12 00 77945996 255239255255 0362\n"
        L"Ю12 00 57878365 255237255255 03685780\n"
        L"Ю12 00 77996999 255237255255 03630769\n"
        L"Ю12 00 77550697 255239255255 0382\n"
        L"Ю12 00 78067899 255239255255 0388\n"
        L"Ю12 00 57878704 251237255255 55703685780\n"
        L"Ю12 00 76673771 255237255255 03800769\n"
        L"Ю12 00 58260951 255237255255 03465080\n"
        L"Ю12 00 76641562 255237255255 03720769\n"
        L"Ю12 00 76657600 255239255255 0374\n"
        L"Ю12 00 58260548 255237255255 03465080\n"
        L"Ю12 00 77778892 255237255255 03820769\n"
        L"Ю12 00 77965796 255239255255 0361\n"
        L"Ю12 00 76642032 255239255255 0316\n"
        L"Ю12 00 76660364 251239255255 3570338\n"
        L"Ю12 00 77629798 255239255255 0373\n"
        L"Ю12 00 77239291 255239255255 0374\n"
        L"Ю12 00 77781896 255239255255 0382\n"
        L"Ю12 00 77630093 255239255255 0373\n"
        L"Ю12 00 77971596 255239255255 0361\n"
        L"Ю12 00 78066891 255239255255 0387\n"
        L"Ю12 00 77967792 255239255255 0360\n"
        L"Ю12 00 57715666 255237255255 03495080\n"
        L"Ю12 00 77552792 251237255255 45703820769\n"
        L"Ю12 00 77996692 255239255255 0361\n"
        L"Ю12 00 77632891 255239255255 0373\n"
        L"Ю12 00 77636991 255239255255 0372\n"
        L"Ю12 00 76641927 255239255255 0316\n"
        L"Ю12 00 76653336 255239255255 0369\n"
        L"Ю12 00 76647072 255239255255 0371\n"
        L"Ю12 00 78063690 255239255255 0387\n"
        L"Ю12 00 76646900 255239255255 0369\n"
        L"Ю12 00 77631497 255239255255 0371\n"
        L"Ю12 00 76653344 255239255255 0369\n"
        L"Ю12 00 77637296 255239255255 0373:)]]><Locomotive Series='2М62УМ' NumLoc='116' Consec='1' CarrierCode='4'>"
        L"<Crew />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto a_Arrival9180 = pathAsoupList.front();
    CPPUNIT_ASSERT(!al.IsServed(a_Arrival9180));
    CPPUNIT_ASSERT( a_Arrival9180->GetDescr() && a_Arrival9180->GetDescr()->get_head_locomotive(loc) && loc.get_crew().empty() );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = al.GetWriteAccess();
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_Arrival9180, std::make_pair(h_Arrival9180, path)));
    CPPUNIT_ASSERT(al.IsServed(a_Arrival9180));

    gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    frg = gis.front();
    CPPUNIT_ASSERT( frg.second && frg.second->get_head_locomotive(loc) );
    CPPUNIT_ASSERT( !loc.get_crew().empty() );

}


void TC_AsoupToSpot::IdentifyLaterAsoup_6149()
{
	UtUserChart m_chart;
	auto& hl = m_chart.getHappenLayer();
	auto& al = m_chart.getAsoupLayer();

	PathPtr path = createPath( hl,
        L"<HemPath>"
        L"<SpotEvent create_time='20210129T123750Z' name='Form' Bdg='3C[09230]' waynum='3' parknum='1' optCode='09230:09240' index='1131-087-0987' num='2487' length='57' weight='4766' net_weight='3530' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3425' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MATICHAKS' Tim_Beg='2021-01-29 10:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210129T123750Z' name='Departure' Bdg='3C[09230]' waynum='3' parknum='1' optCode='09230:09240' index='1131-087-0987' num='J2491' length='57' weight='4766' net_weight='3530' through='Y'>"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3425' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MATICHAKS' Tim_Beg='2021-01-29 10:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210129T125511Z' name='Transition' Bdg='1C[09240]' waynum='1' parknum='1' optCode='09240:09241' index='1131-087-0987' num='J2491' length='57' weight='4766' net_weight='3530' through='Y'>"
        L"<feat_texts typeinfo='Т' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3425' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='MATICHAKS' Tim_Beg='2021-01-29 10:30' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20210129T130401Z' name='Transition' Bdg='1C[09241]' waynum='1' parknum='1' optCode='09241:09251' />"
        L"<SpotEvent create_time='20210129T131318Z' name='Transition' Bdg='1C[09251]' waynum='1' parknum='1' optCode='09181:09251' />"
        L"<SpotEvent create_time='20210129T132941Z' name='Transition' Bdg='3AC[09181]' waynum='3' parknum='1' optCode='09180' />"
        L"<SpotEvent create_time='20210129T133541Z' name='Arrival' Bdg='5C[09180]' waynum='5' parknum='1' index='' num='9505' heavy='Y' />"
        L"<SpotEvent create_time='20210129T141728Z' name='Departure' Bdg='5C[09180]' waynum='5' parknum='1' />"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( path && path->GetEventsCount()==8 );
    {
        auto events = path->GetAllEvents();
        // добавляем данные подвижной единицы 
        std::wstring tdrStr = L"<Info>"
        L"<TrainDescr index='1131-087-0987' num='J2491' length='57' weight='4766' net_weight='3530' through='Y'>"
            L"<Locomotive Series='2ТЭ10М' NumLoc='3425' Consec='1' CarrierCode='4'>"
            L"<Crew EngineDriver='MATICHAKS' Tim_Beg='2021-01-29 10:30' />"
            L"</Locomotive>"
            L"</TrainDescr>"
            L"<TrainDescr index='1131-087-0987' num='J2491' length='57' weight='4766' net_weight='3530' through='Y'>"
            L"<feat_texts typeinfo='Т' />"
            L"<Locomotive Series='2ТЭ10М' NumLoc='3425' Consec='1' CarrierCode='4'>"
            L"<Crew EngineDriver='MATICHAKS' Tim_Beg='2021-01-29 10:30' />"
            L"</Locomotive>"
            L"</TrainDescr>"
            L"<TrainDescr index='' num='9505' heavy='Y' />"
            L"</Info>";
        attic::a_document adoc;
        adoc.load_wide(tdrStr.c_str());

        attic::a_node tdrNode = adoc.document_element().child("TrainDescr"); 
        CPPUNIT_ASSERT ( tdrNode );
        TrainDescr td(tdrNode);
        path->SetInfoToSpot( *next(events.begin(), 1), td );
        tdrNode = tdrNode.next_sibling("TrainDescr");
        CPPUNIT_ASSERT ( tdrNode );
        td = TrainDescr(tdrNode);
        path->SetInfoToSpot( *next(events.begin(), 2), td );
        tdrNode = tdrNode.next_sibling("TrainDescr");
        td = TrainDescr(tdrNode);
        path->SetInfoToSpot( *next(events.begin(), 6), td );
    }
    
    auto vEvents = path->GetAllEvents();
    auto h_Departure9180 = vEvents.back();
    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 4 );
    auto frg = gis.back();
    CPPUNIT_ASSERT( frg.second && frg.second->GetNumber().getNumber()==9505 );

    createAsoup(al, topology, 
        L"(:1042 909/000+09180 2491 1131 087 0987 01 09181 29 01 16 36 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04766 03530 212 91118034 95434478 000 000 00 053 000.90 053 000 000 95 053 000 000\n"
        L"Ю3 575 00034251 1 10 30 0000 00000 MATICHAKS   \n"
        L"Ю3 575 00034252 9\n"
        L"Ю4 00000 1 01 053 000.90 053 000 95 053 000 \n"
        L"Ю4 09870 1 01 053 000.90 053 000 95 053 000 \n"
        L"Ю12 00 91118034 1 067 09870 54223 5552 060 02 00 00 00 00 0232 30 95 0950 04 106 09870 09860 00000 01 11310 38 00000 0000 020 0 0000 58147- 132 000 00000000\n"
        L"Ю12 00 95903118 171239255247 0665420730002290     \n"
        L"Ю12 00 95919239 191239255255 0650230\n"
        L"Ю12 00 95952958 191239255255 0640229\n"
        L"Ю12 00 95947982 189239127255 063040235102\n"
        L"Ю12 00 95927448 255239255255 0234\n"
        L"Ю12 00 95846135 187239127183 0615000237106025APEHДA\n"
        L"Ю12 00 95026373 189239255255 069020232\n"
		L"Ю12 00 95758801 187255255183 0723000200     \n"
		L"Ю12 00 55016232 189237255255 0670402335935\n"
        L"Ю12 00 95286704 185237255255 0624000202260950\n"
        L"Ю12 00 91122275 253239127255 040235102\n"
        L"Ю12 00 95661328 185239127255 075300020232106\n"
        L"Ю12 00 95296463 189239255255 058040225\n"
        L"Ю12 00 95026514 185239255183 070500020231025APEHДA\n"
        L"Ю12 00 95699310 189239255255 065040235\n"
        L"Ю12 00 95419370 185255255183 069300020200     \n"
        L"Ю12 00 95429569 255255255255 \n"
        L"Ю12 00 95341202 191239255255 0650234\n"
        L"Ю12 00 59453548 189237255255 0670402335935\n"
        L"Ю12 00 95200663 189237255255 0690202350950\n"
        L"Ю12 00 95412623 191255255255 070\n"
        L"Ю12 00 95315636 191239255255 0670233\n"
        L"Ю12 00 95361101 191239255255 0700235\n"
        L"Ю12 00 95315941 191239255255 0650233\n"
        L"Ю12 00 95971586 191239255255 0640229\n"
        L"Ю12 00 95472767 191239255255 0720238\n"
        L"Ю12 00 95071858 187239255255 0594000231\n"
        L"Ю12 00 95189635 191239255255 0600232\n"
        L"Ю12 00 95376620 191239255255 0690235\n"
        L"Ю12 00 95378774 255255255255 \n"
        L"Ю12 00 95911079 189239127255 061040234102\n"
        L"Ю12 00 95894358 189239127255 063020229106\n"
        L"Ю12 00 95971347 251239255255 4400230\n"
        L"Ю12 00 95203915 171239255255 069542235600233\n"
        L"Ю12 00 91101360 191239255255 0750232\n"
        L"Ю12 00 95359147 191239255255 0700235\n"
        L"Ю12 00 95945051 191239255255 0750232\n"
        L"Ю12 00 95216081 191239255255 0700233\n"
        L"Ю12 00 59897017 185237255255 0650600802365935\n"
        L"Ю12 00 58566712 191255255255 064\n"
        L"Ю12 00 95379178 169237255255 070542074000502350950\n"
        L"Ю12 00 95419560 255255255255 \n"
        L"Ю12 00 95343547 189239255255 064020233\n"
        L"Ю12 00 95341657 255255255255 \n"
        L"Ю12 00 95180964 189239255255 070050235\n"
        L"Ю12 00 95884912 253239255255 020232\n"
        L"Ю12 00 95343521 187239255255 0643000234\n"
        L"Ю12 00 95323937 255239255255 0233\n"
        L"Ю12 00 95418521 189239255255 069050235\n"
        L"Ю12 00 95324448 189239255255 063020234\n"
        L"Ю12 00 59826685 189237255255 0650402335935\n"
        L"Ю12 00 95434478 189237255255 0690502350950:)",
		boost::gregorian::date(2021, 01, 29)
        );

    createAsoup(al, topology, 
        L"(:1042 909/000+09180 2491 1131 087 0987 02 09860 29 01 17 15 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04766 03530 212 91118034 95434478 000 000 00 053 000.90 053 000 000 95 053 000 000\n"
        L"Ю3 575 00034251 1 10 30 0000 00000 MATICHAKS   \n"
        L"Ю3 575 00034252 9\n"
        L"Ю4 00000 1 01 053 000.90 053 000 95 053 000 \n"
        L"Ю4 09870 1 01 053 000.90 053 000 95 053 000 :)",
		boost::gregorian::date(2021, 01, 29)
        );

    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
    for ( auto a : pathAsoupList )
    {
        CPPUNIT_ASSERT(!al.IsServed(a));
    }

    auto a_Departure9180 = createAsoup(al, topology, 
        L"(:1042 909/000+09180 9505 1131 087 0987 03 09860 29 01 17 17 01/05 0 0/00 00 0\n"
        L"Ю2 0  T 00 00 00 0000 0 0 057 04766 03530 212 91118034 95434478 000 000 00 053 000.90 053 000 000 95 053 000 000\n"
        L"Ю3 575 00034251 1 10 30 0000 00000 MATICHAKS   \n"
        L"Ю3 575 00034252 9\n"
        L"Ю4 00000 1 01 053 000.90 053 000 95 053 000 \n"
        L"Ю4 09870 1 01 053 000.90 053 000 95 053 000 \n"
        L"Ю12 00 91118034 1 067 09870 54223 5552 060 02 00 00 00 00 0232 30 95 0950 04 106 09870 09860 00000 01 11310 38 00000 0000 020 0 0000 58147- 132 000 00000000\n"
        L"Ю12 00 95903118 171239255247 0665420730002290     \n"
        L"Ю12 00 95919239 191239255255 0650230\n"
        L"Ю12 00 95952958 191239255255 0640229\n"
        L"Ю12 00 95947982 189239127255 063040235102\n"
        L"Ю12 00 95927448 255239255255 0234\n"
        L"Ю12 00 95846135 187239127183 0615000237106025APEHДA\n"
        L"Ю12 00 95026373 189239255255 069020232\n"
        L"Ю12 00 95758801 187255255183 0723000200     \n"
        L"Ю12 00 55016232 189237255255 0670402335935\n"
        L"Ю12 00 95286704 185237255255 0624000202260950\n"
        L"Ю12 00 91122275 253239127255 040235102\n"
        L"Ю12 00 95661328 185239127255 075300020232106\n"
        L"Ю12 00 95296463 189239255255 058040225\n"
        L"Ю12 00 95026514 185239255183 070500020231025APEHДA\n"
        L"Ю12 00 95699310 189239255255 065040235\n"
        L"Ю12 00 95419370 185255255183 069300020200     \n"
        L"Ю12 00 95429569 255255255255 \n"
        L"Ю12 00 95341202 191239255255 0650234\n"
        L"Ю12 00 59453548 189237255255 0670402335935\n"
        L"Ю12 00 95200663 189237255255 0690202350950\n"
        L"Ю12 00 95412623 191255255255 070\n"
        L"Ю12 00 95315636 191239255255 0670233\n"
        L"Ю12 00 95361101 191239255255 0700235\n"
        L"Ю12 00 95315941 191239255255 0650233\n"
        L"Ю12 00 95971586 191239255255 0640229\n"
        L"Ю12 00 95472767 191239255255 0720238\n"
        L"Ю12 00 95071858 187239255255 0594000231\n"
        L"Ю12 00 95189635 191239255255 0600232\n"
        L"Ю12 00 95376620 191239255255 0690235\n"
        L"Ю12 00 95378774 255255255255 \n"
        L"Ю12 00 95911079 189239127255 061040234102\n"
        L"Ю12 00 95894358 189239127255 063020229106\n"
        L"Ю12 00 95971347 251239255255 4400230\n"
        L"Ю12 00 95203915 171239255255 069542235600233\n"
        L"Ю12 00 91101360 191239255255 0750232\n"
        L"Ю12 00 95359147 191239255255 0700235\n"
        L"Ю12 00 95945051 191239255255 0750232\n"
        L"Ю12 00 95216081 191239255255 0700233\n"
        L"Ю12 00 59897017 185237255255 0650600802365935\n"
        L"Ю12 00 58566712 191255255255 064\n"
        L"Ю12 00 95379178 169237255255 070542074000502350950\n"
        L"Ю12 00 95419560 255255255255 \n"
        L"Ю12 00 95343547 189239255255 064020233\n"
        L"Ю12 00 95341657 255255255255 \n"
        L"Ю12 00 95180964 189239255255 070050235\n"
        L"Ю12 00 95884912 253239255255 020232\n"
        L"Ю12 00 95343521 187239255255 0643000234\n"
        L"Ю12 00 95323937 255239255255 0233\n"
        L"Ю12 00 95418521 189239255255 069050235\n"
        L"Ю12 00 95324448 189239255255 063020234\n"
        L"Ю12 00 59826685 189237255255 0650402335935\n"
        L"Ю12 00 95434478 189237255255 0690502350950:)",
		boost::gregorian::date(2021, 01, 29)
        );

    pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 3 );

	{
	Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = al.GetWriteAccess();
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    path = linkerStrict.identify(a_Departure9180, std::make_pair(h_Departure9180, path));
    CPPUNIT_ASSERT( path );
	}

	CPPUNIT_ASSERT(al.IsServed(a_Departure9180));

	LinkFreeAsoup(a_Departure9180, m_chart, time_from_iso("20210129T142000Z"));

    for ( auto a : pathAsoupList )
    {
        CPPUNIT_ASSERT(al.IsServed(a));
    }

    gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 5 );
    frg = gis.back();
    CPPUNIT_ASSERT( frg.second && frg.second->GetNumber().getNumber()==9505 && frg.first==vEvents.back()->GetTime() );
}

void TC_AsoupToSpot::IdentifyWithChangeLitersDTN_5208()
{
    PathPtr path = createPath( *m_happenLayer,
        L"<HemPath>"
        L"<SpotEvent create_time='20190320T041047Z' name='Form' Bdg='10C[11310]' waynum='10' parknum='1' index='1131-061-0918' num='2411' length='58' weight='4621' net_weight='3028' through='Y'>"
        L"<feat_texts typeinfo='ДТ' />"
        L"<Locomotive Series='2ТЭ10М' NumLoc='3422' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GOLOVNJOVS' Tim_Beg='2019-03-20 04:39' />"
        L"</Locomotive>"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190320T041047Z' name='Departure' Bdg='10C[11310]' waynum='10' parknum='1' />"
        L"<SpotEvent create_time='20190320T041706Z' name='Transition' Bdg='701SP[11321]'>"
        L"<rwcoord picketing1_val='222~474' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent create_time='20190320T043609Z' name='Transition' Bdg='1C[11311]' waynum='1' parknum='1' optCode='11311:11380' />"
        L"<SpotEvent create_time='20190320T045732Z' name='Transition' Bdg='1C[11380]' waynum='1' parknum='1' optCode='11380:11381' />"
        L"<SpotEvent create_time='20190320T051659Z' name='Arrival' Bdg='3C[11381]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190320T052212Z' name='Departure' Bdg='3C[11381]' waynum='3' parknum='1' />"
        L"<SpotEvent create_time='20190320T053557Z' name='Transition' Bdg='1C[11390]' waynum='1' parknum='1' optCode='11390:11391' />"
        L"<SpotEvent create_time='20190320T055810Z' name='Transition' Bdg='1C[11391]' waynum='1' parknum='1' optCode='11391:11401' />"
        L"<SpotEvent create_time='20190320T061155Z' name='Transition' Bdg='1C[11401]' waynum='1' parknum='1' optCode='11401:11412' />"
        L"<SpotEvent create_time='20190320T063231Z' name='Transition' Bdg='1C[11412]' waynum='1' parknum='1' optCode='11412:11420' />"
        L"<SpotEvent create_time='20190320T065926Z' name='Arrival' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190320T075800Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' />"
        L"<SpotEvent create_time='20190320T081028Z' name='Span_move' Bdg='DK6C[09190:11420]' waynum='1'>"
        L"<rwcoord picketing1_val='295~800' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='296~715' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        );
    CPPUNIT_ASSERT( path && path->GetEventsCount()==14 );
    auto vEvents = path->GetAllEvents();
    auto h_Departure11420 = std::prev(vEvents.end(), 2);
    auto gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 1 );
    auto frg = gis.front();
    CPPUNIT_ASSERT( frg.second );
    TrainSpecialNotes notes = frg.second->GetNotes();
    CPPUNIT_ASSERT( notes.HaveLong() && notes.HaveHeavy() );

    UtLayer<AsoupLayer> al;
    al.createPath(
        L"<AsoupEvent create_time='20190320T075700Z' name='Departure' Bdg='ASOUP 1042_1p4w[11420]' index='1131-061-0918' num='2411' length='56' weight='4449' net_weight='2905' through='Y' waynum='4' parknum='1' dirTo='09180' adjTo='09190' ><![CDATA[(:1042 909/000+11420 2411 1131 061 0918 03 09180 20 03 09 57 04/04 0 0/00 00 0\n"
        L"Ю2 0 00 00 00 00 0000 0 0 056 04449 02905 220 42320523 50532282 000 000 00 055 000.40 005 000 000 60 011 000 000 70 025 000 000 73 001 000 000 90 014 000 000 95 004 000 000 96 010 000 000\n"
        L"Ю3 575 00034221 1 04 39 0000 00000 GOLOVNJOVS  \n"
        L"Ю3 575 00034222 9\n"
        L"Ю4 09170 2 10 017 000.70 007 000 73 001 000 90 010 000 96 010 000 \n"
        L"Ю4 00000 1 01 038 000.40 005 000 60 011 000 70 018 000 90 004 000 95 004 000 \n"
        L"Ю4 09330 1 01 006 000.40 005 000 60 001 000 \n"
        L"Ю4 09170 1 01 001 000.60 001 000 \n"
        L"Ю4 09240 1 01 005 000.60 004 000 90 001 000 95 001 000 \n"
        L"Ю4 09340 1 01 004 000.60 004 000 \n"
        L"Ю4 09790 1 01 001 000.60 001 000 \n"
        L"Ю4 09150 1 01 018 000.70 018 000 \n"
        L"Ю4 09290 1 01 003 000.90 003 000 95 003 000 \n"
        L"Ю12 00 42320523 1 060 09330 08118 5967 400 00 00 00 00 00 0262 30 40 0404 04 105 09180 09330 00000 01 09180 24 07000 3085 026 0 0000 0      132 064 00000000\n"
        L"Ю12 00 54820931 255237255255 02575406\n"
        L"Ю12 00 54832597 255255255255 \n"
        L"Ю12 00 54821194 255239255255 0260\n"
        L"Ю12 00 54824180 255239255255 0257\n"
        L"Ю12 00 51833093 129225064037 066100002140315003600102672070570008612550091701011310030452094030209140OXP   000\n"
        L"Ю12 00 50358191 191239255255 0590278\n"
        L"Ю12 00 57211260 191239255255 0660265\n"
        L"Ю12 00 51097855 191239255255 0590235\n"
        L"Ю12 00 51114684 191239255255 0670247\n"
        L"Ю12 00 75031385 191237255255 06602600732\n"
        L"Ю12 00 57031056 255237255255 02675700\n"
        L"Ю12 00 76694272 131229195111 0400915022609826545703673007690915000000010915067430000\n"
        L"Ю12 00 76691682 255239255255 0352\n"
        L"Ю12 00 55206619 191237255255 04203535080\n"
        L"Ю12 00 58239096 171239254127 039226263570367811802157\n"
        L"Ю12 00 58256553 191239255255 0340376\n"
        L"Ю12 00 58241092 191239255255 0380367\n"
        L"Ю12 00 55089254 129233088119 060093404330458483000002336056001000986009180172264052910     \n"
        L"Ю12 00 62878806 255237255255 02500600\n"
        L"Ю12 00 56940604 251237255255 40002435600\n"
        L"Ю12 00 58439316 255239255255 0242\n"
        L"Ю12 00 56161714 219239223255 09330000023809330\n"
        L"Ю12 00 55146112 135239218119 0630917043304767902420917011310302504862COБCTB\n"
        L"Ю12 00 54100870 135239218247 067097904361911440243098200918030260ПГK   \n"
        L"Ю12 00 94821600 131161064103 0591001000300662308502024020960946141125500917010113104269570130391400     \n"
        L"Ю12 00 94832870 255239255255 0230\n"
        L"Ю12 00 94825387 255239255255 0240\n"
        L"Ю12 00 94212859 187141127255 058080010101950942105\n"
        L"Ю12 00 94852050 251141127255 085020002400946141\n"
        L"Ю12 00 94397288 187237126255 0590800210094210566780\n"
        L"Ю12 00 94453545 191239255255 0580196\n"
        L"Ю12 00 62880380 131161068111 0600924043304584850000025030600600100092400000001172264052910000\n"
        L"Ю12 00 56578289 255237255255 02425600\n"
        L"Ю12 00 63985774 163237254119 06543302606208002440600637202843CУЛЬФT\n"
        L"Ю12 00 63605505 255255255255 \n"
        L"Ю12 00 94784246 129161068103 0581001000300662308502020240209609461411255009170104269570130391400     \n"
        L"Ю12 00 94832052 255255255255 \n"
        L"Ю12 00 94785219 191255255255 059\n"
        L"Ю12 00 59891614 129161068111 055092405422358480600400023430955935106092400000001386130044940000\n"
        L"Ю12 00 95747408 129237219247 0620929054223590036002023209500986009180APEHДA\n"
        L"Ю12 00 95737979 191255255255 061\n"
        L"Ю12 00 95742417 255255255255 \n"
        L"Ю12 00 58216367 129233088119 03309150226268265557010369705080086091500915003796101684OXP   \n"
        L"Ю12 00 58187873 191239255255 0340347\n"
        L"Ю12 00 58168030 255239255255 0346\n"
        L"Ю12 00 58184789 191239255255 0370367\n"
        L"Ю12 00 58188087 191239255255 0340347\n"
        L"Ю12 00 76631456 187237254127 03735703510769811802157\n"
        L"Ю12 00 58170556 191237255255 03303465080\n"
        L"Ю12 00 58247032 251255255255 557\n"
        L"Ю12 00 50517416 255255255255 \n"
        L"Ю12 00 58245549 187239254127 0383570367761609376\n"
        L"Ю12 00 50898238 191255255255 037\n"
        L"Ю12 00 50532282 187239255255 0360570371:)]]><Locomotive Series='2ТЭ10М' NumLoc='3422' Consec='1' CarrierCode='4'>"
        L"<Crew EngineDriver='GOLOVNJOVS' Tim_Beg='2019-03-20 04:39' />"
        L"</Locomotive>"
        L"</AsoupEvent>"
        );

    auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
    CPPUNIT_ASSERT( pathAsoupList.size() == 1 );
    auto a_Departure11420 = pathAsoupList.front();
    CPPUNIT_ASSERT(!al.IsServed(a_Departure11420));
    CPPUNIT_ASSERT( a_Departure11420->GetDescr() && a_Departure11420->GetDescr()->GetNotes()==TrainSpecialNotes() );

    Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
    auto happenAccess = m_happenLayer->GetWriteAccess();
    auto asoupAccess = al.GetWriteAccess();
    AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
    CPPUNIT_ASSERT(linkerStrict.identify(a_Departure11420, std::make_pair(*h_Departure11420, path)));
    CPPUNIT_ASSERT(al.IsServed(a_Departure11420));

    gis = path->GetInfoSpots();
    CPPUNIT_ASSERT( gis.size() == 2 );
    frg = gis.back();
    CPPUNIT_ASSERT( frg.second );
    notes = frg.second->GetNotes();
    CPPUNIT_ASSERT( notes == TrainSpecialNotes() );
}


void TC_AsoupToSpot::LinkInfoChangingAndDepartureDifNumber_7050()
{
	PathPtr path = createPath( *m_happenLayer,
		L"<HemPath>"
		L"<SpotEvent create_time='20220124T061710Z' name='Form' Bdg='4C[11420]' waynum='4' parknum='1' />"
		L"<SpotEvent create_time='20220124T061710Z' name='Departure' Bdg='4C[11420]' waynum='4' parknum='1' />"
		L"<SpotEvent create_time='20220124T062019Z' name='Station_exit' Bdg='PJP[11420]'>"
		L"<rwcoord picketing1_val='300~341' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"<SpotEvent create_time='20220124T062019Z' name='Span_move' Bdg='DK11C[09190:11420]' waynum='1'>"
		L"<rwcoord picketing1_val='299~550' picketing1_comm='Вентспилс' />"
		L"<rwcoord picketing1_val='300~341' picketing1_comm='Вентспилс' />"
		L"</SpotEvent>"
		L"</HemPath>"
		);

	UtLayer<AsoupLayer> al;
	auto a_departure11420 = createAsoup(al, topology, 
		L"(:1042 909/000+11420 2481 1131 370 0987 03 09180 24 01 08 17 04/04 0 0/00 00 0\n"
		L"Ю2 0  T 00 00 00 0000 0 0 056 04731 03496 212 91101550 95824280 000 000 00 053 000.90 053 000 000 95 053 000 000\n"
		L"Ю3 583 00002661 1 01 18 0000 00000 M-MICKEVICS \n"
		L"Ю3 583 00002662 9\n"
		L"Ю4 00000 1 01 053 000.90 053 000 95 053 000 \n"
		L"Ю4 09870 1 01 053 000.90 053 000 95 053 000 \n"
		L"Ю12 00 91101550 1 074 09870 54207 5552 300 02 00 00 00 00 0232 30 95 0950 04 106 09870 09860 00000 01 11310 38 60630 5856 020 0 0000 0      132 000 00000000\n"
		L"Ю12 00 95380572 191239255127 06402350000\n"
		L"Ю12 00 95234829 191239255127 06702265856\n"
		L"Ю12 00 95434478 191239255127 07002350000\n"
		L"Ю12 00 98210800 185255126127 05740003102632507835\n"
		L"Ю12 00 98210792 191255255255 056\n"
		L"Ю12 00 98209810 191255255255 055\n"
		L"Ю12 00 91113027 185239127255 066500020232106\n"
		L"Ю12 00 95316832 191239255255 0600233\n"
		L"Ю12 00 95945432 191239255255 0650232\n"
		L"Ю12 00 95801296 255255255255 \n"
		L"Ю12 00 95174389 191239255255 0670235\n"
		L"Ю12 00 98210784 189255127255 05404102\n"
		L"Ю12 00 98210834 189255255255 05603\n"
		L"Ю12 00 91119321 185239127255 062300020232106\n"
		L"Ю12 00 95619094 191255255255 066\n"
		L"Ю12 00 95636593 255255255255 \n"
		L"Ю12 00 95677951 191255255255 065\n"
		L"Ю12 00 95909834 171239254127 070542235600235651506029\n"
		L"Ю12 00 95750865 255255255255 \n"
		L"Ю12 00 95751673 255255255255 \n"
		L"Ю12 00 91118166 171239254127 071542074000232596105108\n"
		L"Ю12 00 91119404 191255255255 075\n"
		L"Ю12 00 95762415 191255255255 072\n"
		L"Ю12 00 59771998 169237254119 06254223560040236593559080442500000 \n"
		L"Ю12 00 59946699 191239255255 0530223\n"
		L"Ю12 00 59946970 191255255255 057\n"
		L"Ю12 00 95150736 189237255255 0640202320950\n"
		L"Ю12 00 95483863 191239255255 0670238\n"
		L"Ю12 00 95493524 191255255255 069\n"
		L"Ю12 00 59098541 191237255255 06302345935\n"
		L"Ю12 00 95442380 171237254119 06454207300023209505824034660     \n"
		L"Ю12 00 95965752 191255255247 065-     \n"
		L"Ю12 00 95780698 191255255255 068\n"
		L"Ю12 00 95920153 255255255255 \n"
		L"Ю12 00 95944732 191255255255 063\n"
		L"Ю12 00 95661427 187255254119 0714005985036550     \n"
		L"Ю12 00 95899886 191255255255 070\n"
		L"Ю12 00 95884789 191255255255 071\n"
		L"Ю12 00 95320701 191239255255 0670233\n"
		L"Ю12 00 59097519 191237255255 06402355935\n"
		L"Ю12 00 98207772 185253126119 06530004095010260160977023/12 \n"
		L"Ю12 00 98207806 191255255255 066\n"
		L"Ю12 00 98207822 191255255255 067\n"
		L"Ю12 00 95398855 189255127255 06802106\n"
		L"Ю12 00 95497038 191239255255 0710232\n"
		L"Ю12 00 95488383 191255255255 070\n"
		L"Ю12 00 98207749 189239127255 066040235102\n"
		L"Ю12 00 91121780 189239127255 069020232106\n"
		L"Ю12 00 95322178 255239255255 0233\n"
		L"Ю12 00 91117960 191239255255 0660232\n"
		L"Ю12 00 95822953 187255254119 0755005977042980     \n"
		L"Ю12 00 95824280 255255255255 :)",
		boost::gregorian::date(2022, 01, 24)
		);

	auto a_info11420 = createAsoup( al, topology, 
	L"(:1042 909/000+11420 2482 1131 370 0987 13 00000 24 01 07 43 04/04 0 0/00 00 0\n"
		L"Ю2 0  T 00 00 00 0000 0 0 056 04731 03496 212 91101550 95824280 000 000 00 053 000.90 053 000 000 95 053 000 000\n"
		L"Ю3 583 00002661 1 01 18 0000 00000 M-MICKEVICS \n"
		L"Ю3 583 00002662 9\n"
		L"Ю4 00000 1 01 053 000.90 053 000 95 053 000 \n"
		L"Ю4 09870 1 01 053 000.90 053 000 95 053 000 \n"
		L"Ю12 13 91101550 1 074 09870 54207 5552 300 02 00 00 00 00 0232 30 95 0950 04 106 09870 09860 00000 01 11310 38 60630 5856 020 0 0000 0      132 000 00000000\n"
		L"Ю12 13 95380572 191239255127 06402350000\n"
		L"Ю12 13 95234829 191239255127 06702265856\n"
		L"Ю12 13 95434478 191239255127 07002350000\n"
		L"Ю12 13 98210800 185255126127 05740003102632507835\n"
		L"Ю12 13 98210792 191255255255 056\n"
		L"Ю12 13 98209810 191255255255 055\n"
		L"Ю12 13 91113027 185239127255 066500020232106\n"
		L"Ю12 13 95316832 191239255255 0600233\n"
		L"Ю12 13 95945432 191239255255 0650232\n"
		L"Ю12 13 95801296 255255255255 \n"
		L"Ю12 13 95174389 191239255255 0670235\n"
		L"Ю12 13 98210784 189255127255 05404102\n"
		L"Ю12 13 98210834 189255255255 05603\n"
		L"Ю12 13 91119321 185239127255 062300020232106\n"
		L"Ю12 13 95619094 191255255255 066\n"
		L"Ю12 13 95636593 255255255255 \n"
		L"Ю12 13 95677951 191255255255 065\n"
		L"Ю12 13 95909834 171239254127 070542235600235651506029\n"
		L"Ю12 13 95750865 255255255255 \n"
		L"Ю12 13 95751673 255255255255 \n"
		L"Ю12 13 91118166 171239254127 071542074000232596105108\n"
		L"Ю12 13 91119404 191255255255 075\n"
		L"Ю12 13 95762415 191255255255 072\n"
		L"Ю12 13 59771998 169237254119 06254223560040236593559080442500000 \n"
		L"Ю12 13 59946699 191239255255 0530223\n"
		L"Ю12 13 59946970 191255255255 057\n"
		L"Ю12 13 95150736 189237255255 0640202320950\n"
		L"Ю12 13 95483863 191239255255 0670238\n"
		L"Ю12 13 95493524 191255255255 069\n"
		L"Ю12 13 59098541 191237255255 06302345935\n"
		L"Ю12 13 95442380 171237254119 06454207300023209505824034660     \n"
		L"Ю12 13 95965752 191255255247 065-     \n"
		L"Ю12 13 95780698 191255255255 068\n"
		L"Ю12 13 95920153 255255255255 \n"
		L"Ю12 13 95944732 191255255255 063\n"
		L"Ю12 13 95661427 187255254119 0714005985036550     \n"
		L"Ю12 13 95899886 191255255255 070\n"
		L"Ю12 13 95884789 191255255255 071\n"
		L"Ю12 13 95320701 191239255255 0670233\n"
		L"Ю12 13 59097519 191237255255 06402355935\n"
		L"Ю12 13 98207772 185253126119 06530004095010260160977023/12 \n"
		L"Ю12 13 98207806 191255255255 066\n"
		L"Ю12 13 98207822 191255255255 067\n"
		L"Ю12 13 95398855 189255127255 06802106\n"
		L"Ю12 13 95497038 191239255255 0710232\n"
		L"Ю12 13 95488383 191255255255 070\n"
		L"Ю12 13 98207749 189239127255 066040235102\n"
		L"Ю12 13 91121780 189239127255 069020232106\n"
		L"Ю12 13 95322178 255239255255 0233\n"
		L"Ю12 13 91117960 191239255255 0660232\n"
		L"Ю12 13 95822953 187255254119 0755005977042980     \n"
		L"Ю12 13 95824280 255255255255 :)",
		boost::gregorian::date(2020, 11, 24)
		);

	{
		auto readAccess = m_happenLayer->GetReadAccess();
		CPPUNIT_ASSERT_EQUAL(size_t(1), readAccess->getAllPathes().size());

		auto pathAsoupList = al.GetReadAccess()->UT_GetEvents();
		CPPUNIT_ASSERT( pathAsoupList.size() == 2 );
		for( auto asoup : pathAsoupList )
			CPPUNIT_ASSERT( !al.IsServed( asoup ) );
	}

	auto vEvents = path->GetAllEvents();
	auto h_Form11420 = *(vEvents.begin());
	CPPUNIT_ASSERT ( h_Form11420->GetCode()==HCode::FORM );
	auto h_Departure11420 = *(std::next(vEvents.begin(), 1));
	CPPUNIT_ASSERT ( h_Departure11420->GetCode()==HCode::DEPARTURE );

	Context context(sysChance, *esrGuide, *guessTransciever, uncontrolledStations, identifyCategory, nullptr, topology, nullptr, 0);
	{
		auto happenAccess = m_happenLayer->GetWriteAccess();
		auto asoupAccess = al.GetWriteAccess();
		AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
		CPPUNIT_ASSERT(linkerStrict.identify(a_departure11420, std::make_pair(h_Departure11420, path)));
		CPPUNIT_ASSERT(al.IsServed(a_departure11420));
		CPPUNIT_ASSERT(path->GetFragmentsChangeEvent().size()==1);
	}

	{
		auto happenAccess = m_happenLayer->GetWriteAccess();
		auto asoupAccess = al.GetWriteAccess();
		AsoupLinker linkerStrict(happenAccess, asoupAccess, context, StrictIdentifyPolicy(), nullptr);
		CPPUNIT_ASSERT(linkerStrict.identify(a_info11420, std::make_pair(h_Form11420, path)));
		CPPUNIT_ASSERT(al.IsServed(a_info11420));
		CPPUNIT_ASSERT(path->GetFragmentsChangeEvent().size()==2);
		Hem::EventAddress<SpotEvent> sea(*h_Form11420);
		AsoupLayer::ConstAsoupSet linkedAsoup = al.GetLinkedEvents(sea);
		CPPUNIT_ASSERT( linkedAsoup.size()==1 );
	}


}

