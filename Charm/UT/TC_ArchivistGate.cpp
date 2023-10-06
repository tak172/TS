#include "stdafx.h"
#include "TC_ArchivistGate.h"
#include "UtHelpfulBlackBox.h"
#include "../Fund/F2A_BbxOption.h"
#include "../helpful/Log.h"
#include "../helpful/StrToTime.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ArchivistGate );

const DWORD c_TimeOutMs = 1000;

void TC_ArchivistGate::setUp()
{
    auto tuple = std::make_tuple(ref_records, inc_records, incoming_packages, outbox_packages);
    baseMoment = PrepareBlackBox(blackBoxLocation, &tuple);
    ref_records = std::get<0>(tuple);
    inc_records = std::get<1>(tuple);
    incoming_packages = std::get<2>(tuple);
    outbox_packages = std::get<3>(tuple);
    gateway = std::make_shared<UtGateway>();
}

void TC_ArchivistGate::tearDown()
{
    archivist.reset();
    gateway->PopAll();
    gateway.reset();
    DeleteBlackBox(blackBoxLocation);
}

void TC_ArchivistGate::DefaultArchivistInitialization()
{
    archivist.reset( new Archivist( gateway, blackBoxLocation, []() { return time( nullptr ); } ) );
    archivist->set_speed(0);
}

void TC_ArchivistGate::AbsoluteArchivistInitialization()
{
    archivist.reset( new Archivist( gateway, blackBoxLocation, []() {return time( nullptr ); } ) );

	if (!archivist->opened_bbx() 
        || (gateway->needAsync() && archivist->is_dead()))
	{
		WriteLog(L"Archivist for <%s> gateway failed to start to read all from %s",
			gateway->description(),
			UTC_to_wString(baseMoment, DTO_YMD_HMS));
	}
	else
	{
		archivist->set_absolute_speed( baseMoment );
	}
}

void TC_ArchivistGate::Wipe( UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek )
{
    read.clear();
    seek.clear();
}

void TC_ArchivistGate::Wipe(UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek, UtGateway::vecDocAndOpt& data)
{
    read.clear();
    seek.clear();
    data.clear();
}

bool TC_ArchivistGate::RewindAndReceiveResponse(time_t stamp, UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek)
{
    archivist->rewind_to(stamp);
    archivist->regular();
    return ReceiveRewindResponse(stamp,read, seek);
}

bool TC_ArchivistGate::ReceiveRewindResponse(time_t stamp, UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek)
{
	CPPUNIT_ASSERT(read.empty());
	CPPUNIT_ASSERT(seek.empty());

    archivist->regular();
    const size_t LOOP = (inc_records+ref_records)*3;
    for( size_t i=0; i<LOOP && !MomReached(stamp,seek); i++ )
	{
        Receive(read,seek);
	}

	return MomReached(stamp,seek);
}

bool TC_ArchivistGate::ContainsData(UtGateway::vecDocAndOpt& read) const
{
    return !read.empty();
}

bool TC_ArchivistGate::DoesNotContainData(UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek) const
{
    return (1 == seek.size()) && (read.empty());
}

bool TC_ArchivistGate::StampWasReached(const UtGateway::vecDocAndOpt& seek, time_t stamp)
{
    if (seek.empty())
        return false;
	else
	{
		F2A_BBX_OPTION opt = seek.back().second;
        // Результат обязан быть последним пакетом
        CPPUNIT_ASSERT(opt.getFin());
        // Заказанное время должно совпадать с переданным штампом
        return opt.getMoment() == stamp;
    }
}

void TC_ArchivistGate::Positioning()
{
    DefaultArchivistInitialization();
    vector<time_t> chechPoints;
    chechPoints.push_back(baseMoment + c_BbxBegin); // в первой части [c_BbxBegin..LACUNA_BEG-1] 
    chechPoints.push_back(baseMoment + c_BbxEnd); // во второй части [LACUNA_END..c_BbxEnd] 
    chechPoints.push_back(baseMoment + 190);
    chechPoints.push_back(baseMoment + c_LacunaAft);
    chechPoints.push_back(baseMoment + 210);
    chechPoints.push_back(baseMoment + 220);
    chechPoints.push_back(baseMoment + 260);
    chechPoints.push_back(baseMoment + 333); // в лакуне

    for(time_t moment : chechPoints)
    {
        UtGateway::vecDocAndOpt readResults;
        UtGateway::vecDocAndOpt seekResults;

		DebugLog( L"TC_ArchivistGate::Positioning moment=%d", moment );
        CPPUNIT_ASSERT(RewindAndReceiveResponse(moment, readResults, seekResults));
        CPPUNIT_ASSERT(StampWasReached(seekResults, moment));

        const time_t delta = moment - baseMoment;
        F2A_BBX_OPTION opt = seekResults.back().second;
        if ( c_BbxBegin <= delta && delta < c_LacunaAft )
        {
            CPPUNIT_ASSERT( !opt.getLacuna().exist() );
        }
        else if ( c_LacunaAft < delta && delta < c_LacunaEnd )
        {
            CPPUNIT_ASSERT( opt.getLacuna().exist() );
            CPPUNIT_ASSERT_EQUAL( baseMoment + c_LacunaAft+1, opt.getLacuna().from() );
            CPPUNIT_ASSERT_EQUAL( baseMoment + c_LacunaEnd-1, opt.getLacuna().to() );
        }
        else if ( c_LacunaEnd <= delta && delta <= c_BbxEnd )
        {
            CPPUNIT_ASSERT( !opt.getLacuna().exist() );
        }       
    }
}

void TC_ArchivistGate::EqualMoments()
{
    DefaultArchivistInitialization();
    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 411, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 411));
    CPPUNIT_ASSERT(ContainsData(read));
    Wipe(read, seek);

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 422, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 422));
    CPPUNIT_ASSERT(ContainsData(read));
    Wipe(read, seek);

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 423, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 423));
    CPPUNIT_ASSERT(DoesNotContainData(read, seek));
    Wipe(read, seek);

    // вернемся и проверим, что 
    // --- от 423 до 422 никаких данных нет, 
    // --- от 422 до 421 есть несколько данных
    // разгон
    
    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 424, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 424));
    CPPUNIT_ASSERT(DoesNotContainData(read, seek));
    Wipe(read, seek);

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 423, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 423));
    CPPUNIT_ASSERT(DoesNotContainData(read, seek));
    Wipe(read, seek);

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 422, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 422));
    CPPUNIT_ASSERT(DoesNotContainData(read, seek));
    Wipe(read, seek);

    CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + 421, read, seek));
    CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + 421));
    CPPUNIT_ASSERT(ContainsData(read));
    Wipe(read, seek);
}

void TC_ArchivistGate::CheckLacunaDetectionInVector(UtGateway::vecDocAndOpt& vec, time_t stamp)
{
    while ( !vec.empty() )
    {
        F2A_BBX_OPTION opt = vec.front().second;
        vec.erase(vec.begin());
        if (opt.getFin())
        {
            DebugLog( L"in moment=%d bbx say %s\n", stamp, opt.getLacuna().to_wstr(stamp) );
            if (baseMoment + c_LacunaAft+1 <= opt.getMoment() && opt.getMoment() < baseMoment + c_LacunaEnd)
            {
                CPPUNIT_ASSERT(opt.getLacuna().from() == baseMoment + c_LacunaAft+1);
                CPPUNIT_ASSERT(opt.getLacuna().to()   == baseMoment + c_LacunaEnd-1);
            }
        }
    }
}

void TC_ArchivistGate::LacunaDetection()
{
    DefaultArchivistInitialization();
    // проверочные моменты
    int moments[] = {
        // вперед и в лакуну
        c_LacunaAft - 1,
        c_LacunaAft - 0,
        c_LacunaAft + 1,
        c_LacunaAft + 2,
        // вперед уже из лакуны
        c_LacunaEnd - 2,
        c_LacunaEnd - 1,
        c_LacunaEnd,
        c_LacunaEnd + 1,
        // назад и в лакуну
        c_LacunaEnd + 2,
        c_LacunaEnd + 1,
        c_LacunaEnd,
        c_LacunaEnd - 1,
        c_LacunaEnd - 2,
        c_LacunaEnd - 3,
        // назад уже из лакуны
        c_LacunaAft + 2,
        c_LacunaAft + 1,
        c_LacunaAft - 0,
        c_LacunaAft - 1,
        c_LacunaAft - 2
    };

    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;

    for( auto iMoment : moments )
    {
        CPPUNIT_ASSERT(RewindAndReceiveResponse(baseMoment + iMoment, read, seek));
        CPPUNIT_ASSERT(StampWasReached(seek, baseMoment + iMoment ));

        CheckLacunaDetectionInVector(seek, iMoment );
        CheckLacunaDetectionInVector(read, iMoment );
        Wipe(read, seek);
    }
}

void TC_ArchivistGate::Content()
{
    DefaultArchivistInitialization();
    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;

    CPPUNIT_ASSERT( !RewindAndReceiveResponse(baseMoment + c_BbxEnd+22, read, seek) );
    CPPUNIT_ASSERT( !StampWasReached(seek, baseMoment + c_BbxEnd+22 ));
    CPPUNIT_ASSERT(  StampWasReached(seek, baseMoment + c_BbxEnd ));

    for( const UtGateway::DocAndOpt& it : read )
    {
        F2A_BBX_OPTION opt;
		opt = it.second;
        
		const string& strDoc = it.first;
        vector<char> vec(strDoc.begin(), strDoc.end());
        attic::a_document doc;
        doc.load_utf8(vec);
        attic::a_node xroot = doc.document_element();
        CPPUNIT_ASSERT( xroot.name_is( BBX_ROOT() ) );

        if (!opt.getFin())
        {
            attic::a_attribute aCont = xroot.child( BBX_LOS() ).attribute(CONTENT_ATTR);
            switch( opt.getType() )
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
    }
}

// проверка получения последовательности при непрерывном воспроизведении
void TC_ArchivistGate::PlayingSequence()
{
    DefaultArchivistInitialization();
    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;

    const int SLOW_SPEED = 200;
    const int FAST_SPEED = 400;

    // назад и в лакуну
    CPPUNIT_ASSERT( playInterval(c_LacunaEnd+5, c_LacunaEnd-3, FAST_SPEED) );
    // простой случай
    CPPUNIT_ASSERT( playInterval(c_BbxBegin+0,c_BbxBegin+5, SLOW_SPEED) );

    // назад уже из лакуны
    CPPUNIT_ASSERT( playInterval(c_LacunaAft+3, c_LacunaAft-3, FAST_SPEED) );
    // вперед уже из лакуны
    CPPUNIT_ASSERT( playInterval(c_LacunaEnd-2, c_LacunaEnd+5, FAST_SPEED) );
    // вперед и в лакуну
    CPPUNIT_ASSERT( playInterval(c_LacunaAft-4, c_LacunaAft+4, FAST_SPEED) );
}

bool TC_ArchivistGate::playInterval( int from, int to, int speed_scale )
{
    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;
    //DebugLog( "---------------------\n" );
    //DebugLog( "playing content %d..%d\n", from, to );
    archivist->rewind_to(baseMoment + from);
    const int sign = (from<to? +1:-1);
    archivist->set_speed( sign*abs(speed_scale) );

    int last_wait = 0;
    for( ; from!=to; boost::this_thread::yield()  )
    {
        if ( last_wait!=from )
        {
            last_wait = from;
            //DebugLog( "ожидается = %d\n", from );
        }
        for( Receive(read,seek); !seek.empty(); seek.erase(seek.begin()) )
        {
            F2A_BBX_OPTION pr = seek.front().second;
            if (pr.getFin())
            {
                int m = int(pr.getMoment()-baseMoment);
                wstring s = pr.getLacuna().to_wstr(baseMoment+from);
                if ( baseMoment+from == pr.getMoment() )
                {
                    //DebugLog( L"получено  = %d %s\n", m, s );
                    from += sign;
                }
                else
                    DebugLog( L"**ошибка**= %d %s\n", m, s );
            }
        }
    }
    return from==to;
}

void TC_ArchivistGate::ReadAll()
{
    // Инициализация, подобная инициализации Архивистов для Guess'а
	AbsoluteArchivistInitialization();
	UtGateway::vecDocAndOpt read;
	UtGateway::vecDocAndOpt seek;

    const size_t LOOP = (inc_records + ref_records) * 2;
	for( size_t i=0; i<LOOP && seek.empty(); i++ )
	{
        Receive(read, seek);
	}

    CPPUNIT_ASSERT(1 + inc_records == read.size());
    CPPUNIT_ASSERT(1u == size32(seek));
    Wipe(read, seek);
}

void TC_ArchivistGate::ReadLiveExpansion()
{
    // Инициализация, подобная инициализации Архивистов для Guess'а
    AbsoluteArchivistInitialization();
    UtGateway::vecDocAndOpt read;
    UtGateway::vecDocAndOpt seek;

    // вычитывание существующего ящика целиком
    for( const DWORD border = GetTickCount()+c_TimeOutMs; GetTickCount()<border && seek.empty(); /*none*/ )
    {
        Receive(read, seek);
    }
    CPPUNIT_ASSERT(1 + inc_records == read.size());
    CPPUNIT_ASSERT_EQUAL(1u, size32(seek));
    Wipe(read, seek);

    // Продолжение ящика и чтение на ходу
    {
        using namespace boost::posix_time;

        auto ext = Bbx::Writer::create(blackBoxLocation);
        archivist->regular();
        *ext << Sup(baseMoment + c_BbxEnd+3 )
            << Inc(baseMoment + c_BbxEnd+4 )
            << Inc(baseMoment + c_BbxEnd+4 )
            << Inc(baseMoment + c_BbxEnd+7 );

        CPPUNIT_ASSERT(!ext->isDead());

        // вычитывание появившихся новых данных
        for( const DWORD border = GetTickCount()+c_TimeOutMs; GetTickCount()<border; /*none*/ )
        {
            Receive(read, seek);
        }
        // Проверим, что новые данные считаны (референс после лакуны + 3инкремента)
        CPPUNIT_ASSERT( read.size()==1+3 );
        CPPUNIT_ASSERT( !seek.empty() );
        Wipe(read, seek);
    }
}

bool TC_ArchivistGate::MomReached( const time_t stamp, const UtGateway::vecDocAndOpt &seek ) const
{
	return !seek.empty() && seek.back().second.getMoment()==stamp;
}


void TC_ArchivistGate::Receive( UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek )
{
    archivist->regular();
    gateway->PopReadedMessages(read);
    gateway->PopSeekingResults(seek);
}
