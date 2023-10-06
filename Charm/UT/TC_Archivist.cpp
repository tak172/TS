#include "stdafx.h"

#include <numeric>

#include "../BlackBox/bbx_BlackBox.h"
#include "../BlackBox/bbx_Page.h"
#include "../Docker/dk_docker.h"
#include "../Fund/ActorConnection.h"
#include "../Fund/BbxConst.h"
#include "../Fund/F2A_BbxOption.h"
#include "../helpful/Dictum.h"
#include "../helpful/Log.h"
#include "../helpful/EsrGuide.h"
#include "../helpful/DistrictGuide.h"
#include "../helpful/SpanTravelTime.h"
#include "../helpful/StatWorkModeGuide.h"
#include "TC_Archivist.h"
#include "UtHelpfulBlackBox.h"
#include "UtGateway.h"
#include "../Fund/Archivist.h"
#include "../Fund/Lexicon.h"
#include "../TeleControl/KeyAccumulator.h"
#include "../TeleControl/StationControl.h"
#include "../StatBuilder/LogicCommand.h"
#include "../TeleControl/RegionControl.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Archivist );

namespace bt = boost::posix_time;
namespace btth = boost::this_thread;

const bt::time_duration my_bbx_delay = bt::milliseconds( 30 );

void TC_Archivist::setUp()
{
    Bbx::Location::clearFolderCache();
    prev_bbx_delay = Bbx::Writer::getCacheDeviateDelay();
    Bbx::Writer::setCacheDeviateDelay(my_bbx_delay);
    fix_moment = PrepareBlackBox(bbxLocation);

    // Создать докеров - внутреннего и внешнего
    Network::Identifier dk_int("arc_Docker");
    Network::Identifier dk_ext("ext_Docker");
    int_docker.reset( new Network::Docker(dk_int, 30111 ) );
    int_docker->add_addressee(Network::addressee("localhost", 30222));
	CPPUNIT_ASSERT(int_docker->start());
    if (int_docker->is_dead())
        CPPUNIT_FAIL("int_docker is dead");

    ext_docker.reset( new Network::Docker(dk_ext, 30222 ) );
	CPPUNIT_ASSERT(ext_docker->start());
    if (ext_docker->is_dead())
        CPPUNIT_FAIL("ext_docker is dead");

    int_channelId = 0;
    auto fence=GetTickCount()+1000;
    for( ; fence > GetTickCount(); btth::sleep( bt::milliseconds(1) ) )
    {
        std::vector<unsigned> vId = int_docker->get_new_channels();
        if ( !vId.empty() )
        {
            int_channelId = vId.front();
            break;
        }
    }
    CPPUNIT_ASSERT( int_channelId!=0 );
    actorConnection.reset( new ActorConnection( bbxLocation, *int_docker, int_channelId, [this](){return time(nullptr);} ) );
}

void TC_Archivist::tearDown()
{
    actorConnection.reset();
    ext_docker->shutdown();
    int_docker->shutdown();
    ext_docker.reset();
    int_docker.reset();
    DeleteBlackBox(bbxLocation);
    Bbx::Writer::setCacheDeviateDelay(prev_bbx_delay);
}

void TC_Archivist::Positioning()
{
    CPPUNIT_ASSERT_MESSAGE( "Не была выполнена синхронизация", Synchronization() );
    std::vector<time_t> checkPoints;
    checkPoints.push_back(fix_moment + c_BbxBegin); // в первой части [c_BbxBegin..LACUNA_BEG-1] 
    checkPoints.push_back(fix_moment + c_BbxEnd); // во второй части [LACUNA_END..c_BbxEnd] 
    checkPoints.push_back(fix_moment + 190);
    checkPoints.push_back(fix_moment + c_LacunaAft);
    checkPoints.push_back(fix_moment + 210);
    checkPoints.push_back(fix_moment + 220);
    checkPoints.push_back(fix_moment + 260);
    checkPoints.push_back(fix_moment + 333); // в лакуне

    for (time_t moment : checkPoints)
    {
        // перематываемся на точку
        SendRewindTo(moment);
        // прием ответов
        std::vector<PXDOC> rcv=ReceiveRewindResponse();
        // проверка правильных ответов
        F2A_BBX_OPTION opt;
        if ( !rcv.empty() )
        {
            attic::a_node xOpt = rcv.back()->child( BBX_ROOT() ).child( BBX_OPT() );
            xOpt >> opt;
        }
        CPPUNIT_ASSERT(StampWasReached(rcv, moment));
        const time_t delta = moment - fix_moment;
        if ( c_BbxBegin <= delta && delta < c_LacunaAft )
        {
            CPPUNIT_ASSERT( !opt.getLacuna().exist() );
        }
        else if ( c_LacunaAft <= delta && delta < c_LacunaEnd )
        {
            CPPUNIT_ASSERT( opt.getLacuna().exist() );
            CPPUNIT_ASSERT_EQUAL( fix_moment + c_LacunaAft+1, opt.getLacuna().from() );
            CPPUNIT_ASSERT_EQUAL( fix_moment + c_LacunaEnd-1, opt.getLacuna().to() );
        }
        else if ( c_LacunaEnd <= delta && delta <= c_BbxEnd )
        {
            CPPUNIT_ASSERT( !opt.getLacuna().exist() );
        }
    }
}

void TC_Archivist::EqualMoments()
{
    std::vector<PXDOC> rcv;
    CPPUNIT_ASSERT_MESSAGE( "Выполнена синхронизация", Synchronization() );
    time_t m = fix_moment;
    rcv = GetRewindData(m+422);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+422));
    CPPUNIT_ASSERT(ContainsData(rcv));
    // проверим, что от последней записи 422 до момента 423 никаких данных нет
    rcv = GetRewindData(m+423);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+423));
    CPPUNIT_ASSERT(DoesNotContainData(rcv));
    // вернемся и проверим, что 
    // --- от 423 до 422 никаких данных нет, 
    // --- от 422 до 421 есть несколько данных
    // разгон
    rcv = GetRewindData(m+424);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+424));
    CPPUNIT_ASSERT(DoesNotContainData(rcv));
    rcv = GetRewindData(m+423);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+423));
    CPPUNIT_ASSERT(DoesNotContainData(rcv));
    // проверка
    rcv = GetRewindData(m+422);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+422));
    CPPUNIT_ASSERT(DoesNotContainData(rcv));
    // проверка
    rcv = GetRewindData(m+421);
    CPPUNIT_ASSERT(StampWasReached(rcv,m+421));
    CPPUNIT_ASSERT(ContainsData(rcv));
}

void TC_Archivist::SendRewindTo(time_t where)
{
    attic::a_document xreq;
    xreq.set_document_element(XML_A2F_BT_MOVE).append_attribute("start_moment").set_value(where);
    CPPUNIT_ASSERT(ext_docker->send( xreq.to_str() ));
}

void TC_Archivist::SendSpeed( int _rapid_percent )
{
    attic::a_document xreq;
    xreq.set_document_element(XML_A2F_BT_PLAY).append_attribute("rapid_percent").set_value(_rapid_percent);
    CPPUNIT_ASSERT(ext_docker->send( xreq.to_str() ));
}

void TC_Archivist::SendPause()
{
    SendSpeed(0);
}

void TC_Archivist::SendRealSpeed( time_t from )
{
    attic::a_document xreq;
    xreq.set_document_element( XML_A2F_BT_PLAY_REAL_TIME ).append_attribute("from").set_value(from);
    CPPUNIT_ASSERT(ext_docker->send( xreq.to_str() ));
}


std::vector<TC_Archivist::PXDOC> TC_Archivist::ReceiveRewindResponse()
{
    bt::time_duration wait_interval = my_bbx_delay + bt::milliseconds( 30+100 ); // ЧЯ + архивист + чуть-чуть
    std::vector<PXDOC> rcv;
    bool completed = false;
    
    for ( bt::ptime bound = bt::microsec_clock::universal_time() + wait_interval;
          !completed && bt::microsec_clock::universal_time() < bound;
          btth::sleep(bt::milliseconds(1)) )
    {
        actorConnection->regular_io(*int_docker, int_channelId);
        if ( !ext_docker->empty() )
        {
            PXDOC in_doc( receive_xml() );
            attic::a_node xroot = in_doc->document_element();
            if ( xroot.name_is( BBX_ROOT() ) )
            {
                rcv.push_back(in_doc);
                F2A_BBX_OPTION pr;
                xroot.child( BBX_OPT() ) >> pr;                
                if ( pr.getFin() )
                {
                    completed = true;
                    DebugLog( L"TC_Archivist::ReceiveRewindResponse readed -- moment=%d fin=%s", pr.getMoment(), pr.getFin()? L"Y":L"n" );
                }
				else if ( pr.getEof() )
				{
					completed = true;
					DebugLog( L"TC_Archivist::ReceiveRewindResponse readed -- moment=%d eof=%s", pr.getMoment(), pr.getEof()? L"Y":L"n" );
				}
            }
            else
            {
                DebugLog( L"TC_Archivist::ReceiveRewindResponse readed -- '%s'", FromUtf8(xroot.name()) );
            }
        }
    }
    if ( !completed )
        DebugLog( L"TC_Archivist::ReceiveRewindResponse fin=n\n" );
    return rcv;
}

// синхронизация заказчика и исполнителя
// останавливаем и убеждаемся в полной остановке
bool TC_Archivist::Synchronization()
{
    SendPause();
    ReceiveRewindResponse();
    if ( !SynchronizationOneMoment(fix_moment+c_LacunaEnd) )
        return false;
    if ( !SynchronizationOneMoment(fix_moment+c_BbxBegin) )
        return false;
    if ( !SynchronizationOneMoment(fix_moment+c_LacunaAft) )
        return false;
    return true;
}

bool TC_Archivist::StampWasReached( const std::vector<PXDOC> &rcv, time_t need_mom ) const
{
    if ( !rcv.empty() )
    {
        F2A_BBX_OPTION opt;
        attic::a_node xOpt = rcv.back()->child( BBX_ROOT() ).child( BBX_OPT() );
        xOpt >> opt;

        return ( opt.getFin() && opt.getMoment()==need_mom );
    }
    else
        return false;

}

bool TC_Archivist::DoesNotContainData( const std::vector<PXDOC> &rcv ) const
{
    return !ContainsData( rcv );
}

// наличие реальных данных, а не только опций
bool TC_Archivist::ContainsData( const std::vector<PXDOC> &rcv ) const
{
    for( const PXDOC& it : rcv )
    {
        if ( it->child( BBX_ROOT() ).child( BBX_LOS() ) )
            return true;
    }
    return false;
}

std::vector<TC_Archivist::PXDOC> TC_Archivist::GetRewindData( time_t mom )
{
    SendRewindTo(mom);
    return ReceiveRewindResponse();
}

void TC_Archivist::Content()
{
    CPPUNIT_ASSERT_MESSAGE( "Выполнена синхронизация", Synchronization() );
    time_t moment = fix_moment;

    {
        // fix_moment достигнуть невозможно
        std::vector<PXDOC> rcv = GetRewindData(moment);
        CPPUNIT_ASSERT(!StampWasReached(rcv, moment));
    }

    SendRewindTo(fix_moment+522);

    bool completed=false;
    for( DWORD bound = GetTickCount()+1000; !completed && GetTickCount()<bound; /*none*/  )
    {
        actorConnection->regular_io(*int_docker, int_channelId);
        if ( ext_docker->empty() )
            btth::sleep( bt::milliseconds( 1 ) );
        else
        {
            boost::scoped_ptr<attic::a_document> in_doc( receive_xml() );
            attic::a_node xroot = in_doc->document_element();
            CPPUNIT_ASSERT( xroot.name_is( BBX_ROOT() ) );
            {
                F2A_BBX_OPTION pr;
                xroot.child( BBX_OPT() ) >> pr;                

                if (!pr.getFin())
                {
                    attic::a_attribute aCont = xroot.child( BBX_LOS() ).attribute(CONTENT_ATTR);
                    switch( pr.getType() )
                    {
                    case Bbx::RecordType::Increment:
                        CPPUNIT_ASSERT( aCont.value() == CONTENT_INC );
                        break;
                    case Bbx::RecordType::Reference:
                        CPPUNIT_ASSERT( aCont.value() == CONTENT_SUP );
                        break;
                    case Bbx::RecordType::IncomingPackage:
                        CPPUNIT_ASSERT( aCont.value() == CONTENT_PACKAGE_IN );
                        break;
                    case Bbx::RecordType::OutboxPackage:
                        CPPUNIT_ASSERT( aCont.value() == CONTENT_PACKAGE_OUT );
                        break;
                    default:
                        CPPUNIT_FAIL("Unknown type");
                        break;
                    }
                }
                else
                    completed = true;
            }
        }
    }
}

void TC_Archivist::LacunaDetection()
{
    CPPUNIT_ASSERT_MESSAGE( "Выполнена синхронизация", Synchronization() );
    time_t moment = fix_moment;

    {
        std::vector<PXDOC> rcv = GetRewindData(moment);
        CPPUNIT_ASSERT(!StampWasReached(rcv, moment));
    }

    // проверочные моменты
    int moms[] = {
        // вперед и в лакуну
        c_LacunaAft-1,
        c_LacunaAft,
        c_LacunaAft+1,
        c_LacunaAft+2,
        // вперед уже из лакуны
        c_LacunaEnd-2,
        c_LacunaEnd-1,
        c_LacunaEnd,
        c_LacunaEnd+1,
        // назад и в лакуну
        c_LacunaEnd+2,
        c_LacunaEnd+1,
        c_LacunaEnd,
        c_LacunaEnd-1,
        c_LacunaEnd-2,
        c_LacunaEnd-3,
        // назад уже из лакуны
        c_LacunaAft+2,
        c_LacunaAft+1,
        c_LacunaAft+0,
        c_LacunaAft-1,
        c_LacunaAft-2
    };

    for( size_t j=0; j<size_array(moms); j++ )
    {
        SendRewindTo( fix_moment + moms[j] );

        bool completed=false;
        for( DWORD bound = GetTickCount()+1000; !completed && GetTickCount()<bound; /*none*/  )
        {
            actorConnection->regular_io(*int_docker, int_channelId);
            if ( ext_docker->empty() )
                btth::sleep( bt::milliseconds( 1 ) );
            else
            {
                boost::scoped_ptr<attic::a_document> in_doc( receive_xml() );
                attic::a_node xroot = in_doc->document_element();
                CPPUNIT_ASSERT( xroot.name_is( BBX_ROOT() ) );
                F2A_BBX_OPTION pr;
                xroot.child( BBX_OPT() ) >> pr;                
                if (pr.getFin())
                {
                    DebugLog( L"in moment=%d bbx say %s\n", moms[j], pr.getLacuna().to_wstr(moms[j]) );
                    if ( fix_moment+c_LacunaAft<pr.getMoment() && pr.getMoment()<fix_moment+c_LacunaEnd )
                    {
                        CPPUNIT_ASSERT( pr.getLacuna().from() == fix_moment+c_LacunaAft+1 );
                        CPPUNIT_ASSERT( pr.getLacuna().to()   == fix_moment+c_LacunaEnd-1 );
                    }
                    completed = true;
                }
            }
        }
    }
}

// проверка получения последовательности при непрерывном воспроизведении
void TC_Archivist::PlayingSequence()
{
    CPPUNIT_ASSERT_MESSAGE( "Выполнена синхронизация", Synchronization() );
    const int SLOW_SPEED = 200;
    const int FAST_SPEED = 400;

    // простой случай
    CPPUNIT_ASSERT( playInterval(c_BbxBegin+0,c_BbxBegin+5, SLOW_SPEED) );

    // назад и в лакуну
    CPPUNIT_ASSERT( playInterval(c_LacunaEnd+5, c_LacunaEnd-3, FAST_SPEED) );
    // назад уже из лакуны
    CPPUNIT_ASSERT( playInterval(c_LacunaAft+3, c_LacunaAft-3, FAST_SPEED) );
    // вперед уже из лакуны
    CPPUNIT_ASSERT( playInterval(c_LacunaEnd-2, c_LacunaEnd+5, FAST_SPEED) );
    // вперед и в лакуну
    CPPUNIT_ASSERT( playInterval(c_LacunaAft-4, c_LacunaAft+4, FAST_SPEED) );
}

// проверка получения живых данных
void TC_Archivist::live_writer( bt::time_duration dur,
                                time_t first,
                                unsigned count )
{
    w_moms.clear();
    w_moms.reserve(count);
    {
        auto bw = Bbx::Writer::create(bbxLocation);
        *bw << Sup(first);
        auto point = bt::microsec_clock::universal_time();
        for( unsigned i=0; i<count; ++i )
        {
            *bw << Inc( first+i, 777-i );
            w_moms.push_back( bt::microsec_clock::universal_time() );
            point += dur;
            btth::sleep( point );
        }
    }
    auto after_point = bt::microsec_clock::universal_time() + bt::seconds(5);
    while( bt::microsec_clock::universal_time() < after_point && !btth::interruption_requested() )
        btth::sleep( bt::milliseconds(10) );
}

void TC_Archivist::PlayingLive()
{
    CPPUNIT_ASSERT_MESSAGE( "Синхронизация на конец ящика", SynchronizationOneMoment(fix_moment+c_BbxEnd) );

    SendRealSpeed(fix_moment+c_BbxEnd);
    const unsigned COUNT = 100;
    const bt::time_duration DURATION = bt::milliseconds(100);
    const time_t START = fix_moment+c_BbxEnd+7;
    std::vector<bt::ptime> r_moms;
    w_moms.reserve(COUNT);
    r_moms.resize(COUNT, bt::ptime() );
    boost::thread wt = boost::thread( boost::bind(&TC_Archivist::live_writer, this, DURATION, START, COUNT ) );
    
    // получить всё возможное
    for( auto bound = bt::microsec_clock::universal_time() + DURATION*COUNT + bt::seconds(1);
            bt::microsec_clock::universal_time() <= bound;
            /*none*/
       )
    {
        actorConnection->regular_io(*int_docker, int_channelId);
        if ( !ext_docker->empty() )
        {
            boost::scoped_ptr<attic::a_document> in_doc( receive_xml() );
            if ( in_doc->document_element().name_is( BBX_ROOT() ) )
            {
                F2A_BBX_OPTION pr;
                in_doc->document_element().child( BBX_OPT() ) >> pr;
                int k = static_cast<int>(pr.getMoment() - START);
                // учитываем только ожидаемые данные, пришедшие впервые
                if ( 777-k == in_doc->document_element().child( BBX_LOS() ).attribute("special").as_int() 
                    && 0<=k
                    && (unsigned)k < r_moms.size()
                    && bt::ptime() == r_moms[ k ]
                   )
                   {
                       r_moms[ k ] = bt::microsec_clock::universal_time();
                   }
            }
        }
    }
    wt.interrupt();
    wt.join();

    // было отправлено и получено всё
    CPPUNIT_ASSERT( COUNT==w_moms.size() );
    CPPUNIT_ASSERT( COUNT==r_moms.size() );
    // вычислить характеристики
    bt::time_duration sum   = bt::milliseconds(0);
    bt::time_duration lower = bt::hours(24);
    bt::time_duration upper = bt::milliseconds(0);
    long loIdx = 0;
    long upIdx = 0;
    for( unsigned k=0; k<r_moms.size(); ++k )
    {
        CPPUNIT_ASSERT( bt::ptime()!=r_moms[k] ); // каждый элемент должен быть получен

        bt::time_duration delta = r_moms[k] - w_moms[k];
        sum += delta;
        if ( delta<lower )
        {
            lower = delta;
            loIdx = k;
        }
        if ( upper<delta )
        {
            upper = delta;
            upIdx = k;
        }
    }
    bt::time_duration average = sum / COUNT;
    WriteLog( L"Total: average %d, minmax %d..%d (position [%d]..[%d])",
        average.total_milliseconds(), lower.total_milliseconds(), upper.total_milliseconds(), loIdx, upIdx );

    auto rower = []( std::wstring title, const std::vector<bt::time_duration>& x_dur ){
        std::wstring text = title;
        for( auto x : x_dur )
        {
            std::wstring val = std::to_wstring( x.total_milliseconds() );
            text += std::wstring( 7-val.size(),' ' ) + val;
        }
        return text;
    };
    // начало координат
    const auto left = w_moms.front();
    std::vector<bt::time_duration> temp;
    temp.resize(COUNT);
    // индексы
    for( size_t i=0; i<COUNT; ++i )
        temp[i] = bt::milliseconds(i);
    WriteLog( rower( L"Total:              []", temp ) );
    // точки записи
    for( size_t i=0; i<COUNT; ++i )
        temp[i] = w_moms[i] - left;
    WriteLog( rower( L"Total:     write point", temp ) );
    // точки чтения
    for( size_t i=0; i<COUNT; ++i )
        temp[i] = r_moms[i] - left;
    WriteLog( rower( L"Total:      read point", temp ) );
    // отклонения точек чтения
    for( size_t i=0; i<COUNT; ++i )
        temp[i] = r_moms[i] - w_moms[i];
    WriteLog( rower( L"Total:  read deviation", temp ) );

    // максимальная задержка ограничена (my_bbx_delay внутри ЧЯ и немного времени архивисту на чтение)
    auto const fence = my_bbx_delay + bt::milliseconds( 800 );
    WriteLog( L"Total: "+std::wstring( (upper<=fence)?L"":L" ***** error " ) );
    std::string msg = "upper="+std::to_string(upper.total_milliseconds()) + " < "+std::to_string(fence.total_milliseconds())+" required!";
//     if ( upper<=fence )
//         ;
//     else
//     {
//         WriteLog.set_sync_output();
//     }
    CPPUNIT_ASSERT_MESSAGE( msg, upper<=fence );
    // средняя поменьше
    CPPUNIT_ASSERT( average<=my_bbx_delay+bt::milliseconds(100) );
}

bool CheckContent(UtGateway::DocAndOpt data, std::string expected)
{
    attic::a_document in_doc;
    in_doc.load_utf8( data.first );
    attic::a_node xroot = in_doc.document_element();
    CPPUNIT_ASSERT(xroot.name_is(BBX_ROOT()));
    attic::a_attribute aCont = xroot.child(BBX_LOS()).attribute(CONTENT_ATTR);
    return aCont.value() == expected;
}

void TC_Archivist::SkipPackages()
{
    {
        std::shared_ptr<UtGateway> gateway = std::make_shared<UtGateway>();
        Archivist archivist( gateway, bbxLocation, []() {return time( nullptr ); } );

        CPPUNIT_ASSERT(archivist.opened_bbx());
        archivist.rewind_to(fix_moment);
        archivist.set_speed(10000);

        auto now = std::chrono::system_clock::now();
        while (std::chrono::system_clock::now() < now + std::chrono::seconds(2) ) // 1.05 необходимо + 0.95 запас
        {
            archivist.regular();
        }

        UtGateway::vecDocAndOpt readed;
        gateway->PopReadedMessages(readed);

        CPPUNIT_ASSERT(readed.size() > 5u);
        CPPUNIT_ASSERT(CheckContent(readed[0], CONTENT_SUP));
        CPPUNIT_ASSERT(CheckContent(readed[1], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[2], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[3], CONTENT_PACKAGE_IN));
        CPPUNIT_ASSERT(CheckContent(readed[4], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[5], CONTENT_INC));
    }

    {
        std::shared_ptr<UtGateway> gateway= std::make_shared<UtGateway>();
        Archivist archivist( gateway, bbxLocation, []() {return time( nullptr ); } );

        CPPUNIT_ASSERT(archivist.opened_bbx());
        archivist.rewind_to(fix_moment + c_LacunaAft);
        archivist.set_speed(-10000);

        auto now = std::chrono::system_clock::now();
        while( std::chrono::system_clock::now() < now + std::chrono::seconds(2) ) // 0.89 необходимо + 1.11 запас
        {
            archivist.regular();
        }

        UtGateway::vecDocAndOpt readed;
        gateway->PopReadedMessages(readed);

        CPPUNIT_ASSERT(readed.size() > 4u);
        CPPUNIT_ASSERT(CheckContent(readed[0], CONTENT_SUP));
        CPPUNIT_ASSERT(CheckContent(readed[1], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[2], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[3], CONTENT_INC));
        CPPUNIT_ASSERT(CheckContent(readed[4], CONTENT_INC));
    }
}

attic::a_document* TC_Archivist::receive_xml()
{
    attic::a_document* doc = new attic::a_document;
    // просто принять документ
    doc->load_utf8( *ext_docker->receive() );
    // сообщить, что можно присылать ещё
    CPPUNIT_ASSERT_MESSAGE("document NOT received", *doc);
    attic::a_document xdoc;
    attic::a_node xnode = xdoc.set_document_element( XML_A2F_OVERSTOCK );
    xnode.brief_attribute( XML_A2F_OVERSTOCK_ATTR, 1 );
    xnode.ensure_attribute( XML_A2F_TYPE ).set_value( BBX_ROOT() );
    CPPUNIT_ASSERT(ext_docker->send( xdoc.to_str() ));
    // 
    return doc;
}

bool TC_Archivist::playInterval( int from, int to, int speed_scale )
{
    CPPUNIT_ASSERT_MESSAGE( "Не была выполнена синхронизация", Synchronization() );
    DebugLog( L"---------------------\n" );
    DebugLog( L"playing content %d..%d\n", from, to );
    SendRewindTo( fix_moment + from );
    actorConnection->regular_io(*int_docker, int_channelId);
    const int sign = (from<to? +1:-1);
    SendSpeed( sign*abs(speed_scale) );
    actorConnection->regular_io(*int_docker, int_channelId);

    int timeout = (to-from+1)*sign*2000;
    int last_wait = 0;
    for( DWORD bound = GetTickCount()+timeout; GetTickCount()<bound && from!=to; /*none*/  )
    {
        actorConnection->regular_io(*int_docker, int_channelId);
        if ( last_wait!=from )
        {
            last_wait = from;
            //DebugLog( "ожидается = %d\n", from );
        }
        if ( ext_docker->empty() )
            btth::sleep( bt::milliseconds( 1 ) );
        else
        {
            boost::scoped_ptr<attic::a_document> in_doc( receive_xml() );
            attic::a_node xroot = in_doc->document_element();
            CPPUNIT_ASSERT( xroot.name_is( BBX_ROOT() ) );
            F2A_BBX_OPTION pr;
            xroot.child( BBX_OPT() ) >> pr;                
            if (pr.getFin())
            {
                int m = int(pr.getMoment()-fix_moment);
                std::wstring s = pr.getLacuna().to_wstr(fix_moment+from);
                //DebugLog( "получено  = %d %s\n", m, s );
                if ( fix_moment+from == pr.getMoment() )
                {
                    //DebugLog( L"учтено    = %d %s\n", m, s );
                    from += sign;
                }
                else
                {
                    DebugLog( L"**ошибка**= %d %s\n", m, s );
                    CPPUNIT_FAIL( "нарушена последовательность" );
                }
            }
        }
    }
    return from==to;
}

bool TC_Archivist::SynchronizationOneMoment( const time_t checked_moment )
{
    SendRewindTo(checked_moment);
    actorConnection->regular_io(*int_docker, int_channelId);
    unsigned pass_count = 0;
    for( DWORD giga=GetTickCount()+1000; GetTickCount()<giga; btth::sleep( bt::milliseconds( 1 ) ) )
    {
        pass_count++;
        actorConnection->regular_io(*int_docker, int_channelId);
        std::vector<PXDOC> rcv = ReceiveRewindResponse();
        if ( StampWasReached(rcv, checked_moment) )
            return true;
    }
    CPPUNIT_ASSERT_EQUAL_MESSAGE( "Sync error! Actual show pass count.", checked_moment, time_t(pass_count) ); // это FAIL, но с выводом ожидаемого значения
    return false;
}
