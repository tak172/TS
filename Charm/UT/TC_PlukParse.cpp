#include "stdafx.h"
#include "TC_PlukParse.h"
#include "TC_PortablePgSQL.h"
#include "../Pluk/PlukDataBase.h"
#define  UNITTEST_CHECK_BUG
#include "../Pluk/parse_change.h"
#undef UNITTEST_CHECK_BUG
#include "../Pluk/pg_query.h"
#include "../helpful/Attic.h"
#include "../helpful/Log.h"
#include "../Hem/XmlStrConst.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PlukParse );

// тестовый разборщик изменений
class UT_parse_change : public parse_change
{
public:
    UT_parse_change(db_query_ptr _db_query, cache_ptr _cache)
        : parse_change(_db_query,_cache)
    {}
    // специальная реакция на ошибку (для обнаружения в тестах)
    void handle_error(const std::string& /*msg*/) override
    {
        CPPUNIT_FAIL("UT_parse_change: error occured in DB");
    }
};

// тестовая БД
class UT_PlukDataBase : public PlukDataBase
{
public:
    UT_PlukDataBase(std::string stringConnect)
        : PlukDataBase(stringConnect)
    {}
    // создать тестовый разборщик изменений
    std::shared_ptr<parse_change> buildParseChange(std::shared_ptr<db::pg_query> db_query) override
    {
        return std::make_shared<UT_parse_change>(db_query, cache);
    }
};


void TC_PlukParse::CreateService()
{
	CPPUNIT_ASSERT_NO_THROW(GetPlukDataBase(LocalPostgresInstance::GetStringConnect()));
}	

void TC_PlukParse::ParseHem()
{
    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::string hem_first = 
        "<?xml version='1.0'?>"
        "<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        "<AutoChanges><HappenLayer>"
        "<Trio>"
        "  <Body create_time='20170117T073659Z' name='Form'      Bdg='P5P[09000:09420]' waynum='1'/>"
        "  <Body create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></AutoChanges>"
        "<UserChanges><HappenLayer>"
        "<Trio>"
        "  <Body create_time='20170117T073659Z' name='Form'      Bdg='P5P[09000:09420]' waynum='1'/>"
        "  <Body create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></UserChanges>"
        "</SoxPostUpdChange>";
	std::string hem_second = 
        "<?xml version='1.0'?>"
        "<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        "<AutoChanges><HappenLayer>"
        "<Trio>"
        "  <Pre  create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1'/>"
        "  <Post create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Body create_time='20170117T073713Z' name='Span_move' Bdg='N29P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></AutoChanges>"
        "<UserChanges><HappenLayer>"
        "<Trio>"
        "  <Pre  create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Post create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Body create_time='20170117T073713Z' name='Span_move' Bdg='N29P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></UserChanges>"
        "</SoxPostUpdChange>";
	std::string hem_incorrect = 
        "<?xml version='1.0'?>"
        "<SoxPostUpdChange increment='Y' presently='20170117T073648Z'>"
        "<AutoChanges><HappenLayer>"
        "<Trio>"
        "  <Pre  create_time='20170117T073659Z' name='Span_move' Bdg='' waynum='1'/>"
        "  <Post create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Body create_time='20170117T073713Z' name='Span_move' Bdg='N29P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></AutoChanges>"
        "<UserChanges><HappenLayer>"
        "<Trio>"
        "  <Pre  create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Post create_time='20170117T073659Z' name='Span_move' Bdg='P7P[09000:09420]' waynum='1' cover='Y'/>"
        "  <Body create_time='20170117T073713Z' name='Span_move' Bdg='N29P[09000:09420]' waynum='1'/>"
        "</Trio>"
        "</HappenLayer></UserChanges>"
        "</SoxPostUpdChange>";
 	CPPUNIT_ASSERT(plukDB->writeChanges(hem_first));
 	CPPUNIT_ASSERT(plukDB->writeChanges(hem_second));
	CPPUNIT_ASSERT(plukDB->writeChanges(hem_incorrect));
}

void TC_PlukParse::ParseAsoup()
{
	auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::string asoup_correct = 
        "<?xml version='1.0'?><SoxPostUpdChange increment='Y' presently='20170117T073705Z'>"
        "<AutoChanges><AsoupLayer><Trio layer='asoup'>"
        "<Body create_time='20170117T073600Z' name='Arrival' Bdg='ASOUP 1042[11760]' index='1116-078-1176' num='8501' length='4' weight='124' util='Y' waynum='5' parknum='1' "
        "dirFrom='11750' adjFrom='11750'>"
        "<![CDATA[(:1042 909/000+11760 8501 1116 078 1176 01 11750 17 01 09 36 01/05 1 0/00 00 0 2 0 00 00 00 00 0000 0 0 004 00124 00000 012 19185362 19151257 003 000 00 000 000 12 00 19185362 1 000 11310 42100 7755 005 00 00 00 00 00 0537 50 10 0580 04 130 11760 11420 00000 02 11310 43 00000 0000 000 0 0000 80/25  128 000 00000000 12 00 19151240 251237127247 000035005561050      12 00 19151257 255255255255 :)]]>"
        "</Body></Trio></AsoupLayer></AutoChanges>"
        "<UserChanges><AsoupLayer><Trio layer='asoup'>"
        "<Body create_time='20170117T073600Z' name='Arrival' Bdg='ASOUP 1042[11760]' index='1116-078-1176' num='8501' length='4' weight='124' util='Y' waynum='5' parknum='1' "
        "dirFrom='11750' adjFrom='11750' linkBadge='1C&apos;2[11760]' linkTime='20170117T073600Z' linkCode='Transition'>"
        "<![CDATA[(:1042 909/000+11760 8501 1116 078 1176 01 11750 17 01 09 36 01/05 1 0/00 00 0 2 0 00 00 00 00 0000 0 0 004 00124 00000 012 19185362 19151257 003 000 00 000 000 12 00 19185362 1 000 11310 42100 7755 005 00 00 00 00 00 0537 50 10 0580 04 130 11760 11420 00000 02 11310 43 00000 0000 000 0 0000 80/25  128 000 00000000 12 00 19151240 251237127247 000035005561050      12 00 19151257 255255255255 :)]]>"
        "</Body></Trio></AsoupLayer></UserChanges></SoxPostUpdChange>";
	std::string asoup_incorrect = "<?xml version=\"1.0\"?><SoxPostUpdChange increment=\"Y\" presently=\"20170117T073705Z\"><AutoChanges><AsoupLayer><Trio layer=\"asoup\"><Body  name=\"Arrival\" Bdg=\"ASOUP 1042[11760]\" index=\"1116-078-1176\" num=\"8501\" length=\"4\" weight=\"124\" util=\"Y\" waynum=\"5\" parknum=\"1\" dirFrom=\"11750\" adjFrom=\"11750\"></Body></Trio></AsoupLayer></AutoChanges><UserChanges><AsoupLayer><Trio layer=\"asoup\"><Body create_time=\"20170117T073600Z\" name=\"Arrival\" Bdg=\"ASOUP 1042[11760]\" index=\"1116-078-1176\" num=\"8501\" length=\"4\" weight=\"124\" util=\"Y\" waynum=\"5\" parknum=\"1\" dirFrom=\"11750\" adjFrom=\"11750\"></Body></Trio></AsoupLayer></UserChanges></SoxPostUpdChange>";
	CPPUNIT_ASSERT(plukDB->writeChanges(asoup_correct));
	CPPUNIT_ASSERT(plukDB->writeChanges(asoup_incorrect));
}

void TC_PlukParse::ParseBusy()
{
	auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::string busy_first = "<?xml version=\"1.0\"?><SoxPostUpdChange increment=\"Y\" presently=\"20170117T073648Z\"><TerraChanges><BusyLayer><Trio layer=\"busy\"><Body create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /></Trio></BusyLayer></TerraChanges></SoxPostUpdChange>";
	std::string busy_second = "<?xml version=\"1.0\"?><SoxPostUpdChange increment=\"Y\" presently=\"20170117T073648Z\"><TerraChanges><BusyLayer><Trio layer=\"busy\"><Pre create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /><Post create_time=\"20170117T073647Z\" name=\"WayOccupy\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" cover=\"Y\" /><Body create_time=\"20170117T073702Z\" name=\"WayEmpty\" Bdg=\"1C[09190]\" waynum=\"1\" parknum=\"1\" /></Trio></BusyLayer></TerraChanges></SoxPostUpdChange>";
	std::string busy_incorrect = "<?xml version=\"1.0\"?><SoxPostUpdChange increment=\"Y\" presently=\"20170117T073648Z\"><TerraChanges><BusyLayer><Trio layer=\"busy\"><Body create_time=\"20170117T073647Z\" name=\"\" Bdg=\"\"/></Trio></BusyLayer></TerraChanges></SoxPostUpdChange>";
	CPPUNIT_ASSERT(plukDB->writeChanges(busy_first));
	CPPUNIT_ASSERT(plukDB->writeChanges(busy_second));
	CPPUNIT_ASSERT(plukDB->writeChanges(busy_incorrect));
}

void TC_PlukParse::ParseSpeedLimit() 
{
	auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::wstring limit_first = L"<SoxPostUpdChange increment=\"Y\" presently=\"20170822T145959Z\"><UserChanges><LimitLayer><Trio layer=\"limit\"><Body Id=\"i3\" esr=\"09360:11760\" FullSpan=\"Y\" Sudden=\"Y\" StartTime=\"20170822T120100Z\" Comment=\"непредвиденное предупреждение на весь перегон на все пути со временем окончания с конкретной причиной\" Reason=\"13653\" SpeedPass=\"11\" SpeedGoods=\"22\" SpeedSuburban=\"33\"><picketing_start picketing1_val=\"94~600\" picketing1_comm=\"Рига-Зилупе\" /><picketing_stop picketing1_val=\"111~500\" picketing1_comm=\"Рига-Зилупе\" /></Body></Trio></LimitLayer></UserChanges></SoxPostUpdChange>";
	std::wstring limit_second = L"<SoxPostUpdChange increment=\"Y\" presently=\"20170822T145959Z\"><UserChanges><LimitLayer><Trio layer=\"limit\"><Pre Id=\"i3\" esr=\"09360:11760\" FullSpan=\"Y\" Sudden=\"Y\" StartTime=\"20170822T120100Z\" StopTime=\"20170822T131200Z\" Comment=\"непредвиденное предупреждение на весь перегон на все пути со временем окончания с конкретной причиной\" Reason=\"13653\" SpeedPass=\"11\" SpeedGoods=\"22\" SpeedSuburban=\"33\" cover=\"Y\"><picketing_start picketing1_val=\"94~600\" picketing1_comm=\"Рига-Зилупе\" /><picketing_stop picketing1_val=\"111~500\" picketing1_comm=\"Рига-Зилупе\" /></Pre><Body Id=\"i3\" esr=\"09360:11760\" waynum=\"1\" FullSpan=\"Y\" Sudden=\"Y\" StartTime=\"20170822T120100Z\" StopTime=\"20170822T135500Z\" Comment=\"непредвиденное предупреждение на весь перегон на все пути со временем окончания с конкретной причиной&#13;&#10;UPD: только первый путь перегона\" Reason=\"13653\" SpeedPass=\"11\" SpeedGoods=\"22\" SpeedSuburban=\"33\"><picketing_start picketing1_val=\"94~600\" picketing1_comm=\"Рига-Зилупе\" /><picketing_stop picketing1_val=\"111~500\" picketing1_comm=\"Рига-Зилупе\" /></Body></Trio></LimitLayer></UserChanges></SoxPostUpdChange>";
	attic::a_document doc;
	doc.load_wide(limit_first);
	CPPUNIT_ASSERT(plukDB->writeChanges(doc.to_str()));
	doc.load_wide(limit_second);
	CPPUNIT_ASSERT(plukDB->writeChanges(doc.to_str()));
}

void TC_PlukParse::ParseForbid()
{
	auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::wstring forbid_first  =
        L"<SoxPostUpdChange increment='Y' presently='20170822T145959Z'>"
        L"<UserChanges><ForbidLayer>"
        L"<Trio layer='forbid'>"
        L"  <Body Id='i1' esr='11272:11273' FullSpan='Y' StartTime='20170822T120100Z' StopTime='20170822T130800Z' Comment='two-way span&#13;&#10;default forbid' Reason='1601' inner_name='srvWay' RightAB='Y'><picketing_start picketing1_val='445~800' picketing1_comm='Россия' /><picketing_stop picketing1_val='455~500' picketing1_comm='Россия' /></Body>"
        L"</Trio>"
        L"</ForbidLayer></UserChanges></SoxPostUpdChange>";
	std::wstring forbid_second =
        L"<SoxPostUpdChange increment='Y' presently='20170822T145959Z'>"
        L"<UserChanges><ForbidLayer>"
        L"<Trio layer='forbid'>"
        L"  <Pre Id='i1' esr='11272:11273' waynum='2' StartTime='20170822T120100Z' StopTime='20170822T130800Z' Comment='two-way span&#13;&#10;только по второму пути (было все)&#13;&#10;работы на 450.3-450.7&#13;&#10;время как планируемое, так и фактическое&#13;&#10;ещё и причина указана' inner_name='srvWay' Show_Overtime='Y' RightAB='Y' cover='Y'><picketing_start picketing1_val='449~200' picketing1_comm='Россия' /><picketing_stop picketing1_val='449~700' picketing1_comm='Россия' /></Pre>"
        L"  <Body Id='i1' esr='11272:11273' waynum='2' FullSpan='Y' StartTime='20170822T120100Z' StopTime='20170822T130800Z' Fact_Start='20170822T121000Z' Fact_Stop='20170822T140000Z' Comment='two-way span&#13;&#10;флаги автоблокировки изменены&#13;&#10;только по второму пути (было все)&#13;&#10;работы опять на всем перегоне&#13;&#10;фактическое удалено, а плановое пеменялось&#13;&#10;' Reason='1601' inner_name='srvWay' WrongAB='Y'><picketing_start picketing1_val='445~800' picketing1_comm='Россия' /><picketing_stop picketing1_val='455~500' picketing1_comm='Россия' /></Body>"
        L"</Trio>"
        L"</ForbidLayer></UserChanges></SoxPostUpdChange>";
    std::wstring forbid_three  = 
        L"<SoxPostUpdChange increment='Y' presently='20170822T145959Z'>"
        L"<UserChanges><ForbidLayer>"
        L"<Trio layer='forbid'>"
        L"  <Body Id='i1185' esr='11042:16169' waynum='1' FullSpan='Y' StartTime='20170826T051200Z' StopTime='20170826T083900Z' Fact_Start='20190426T051200Z' Fact_Stop='20190426T083900Z' Reason='2816724' inner_name='srvWay' moveModeInRight='autoblock' RightAB='Y' moveModeInWrong='phone'>"
        L"    <picketing_start picketing1_val='460~400' picketing1_comm='Вентспилс' />"
        L"    <picketing_stop  picketing1_val='470~1000' picketing1_comm='Вентспилс' />"
        L"  </Body>"
        L"</Trio>"
        L"</ForbidLayer></UserChanges></SoxPostUpdChange>";

    std::vector<std::string> param;
    param.push_back(ForbidLayer_xAttr);

    {
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_wide(forbid_first) );
        CPPUNIT_ASSERT( plukDB->writeChanges(doc.to_str()));
        std::string res = plukDB->getUserChart("20170801T000000Z", "20170901T000000Z", param);
        CPPUNIT_ASSERT( doc.load_utf8( res ) );
        OutputDebugString( ( doc.pretty_wstr() + L"\n\n").c_str() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent").size() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent[ @StartTime ]").size() );
        CPPUNIT_ASSERT( 0 == doc.select_nodes("//ForbidEvent[ @Fact_Start ]").size() );
    }

    {
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_wide(forbid_second) );
        CPPUNIT_ASSERT( plukDB->writeChanges(doc.to_str()));
        std::string res = plukDB->getUserChart("20170801T000000Z", "20170901T000000Z", param);
        CPPUNIT_ASSERT( doc.load_utf8( res ) );
        OutputDebugString( ( doc.pretty_wstr() + L"\n\n").c_str() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent").size() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent[ @Fact_Start ]").size() );
    }

    {
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_wide(forbid_three) );
        CPPUNIT_ASSERT( plukDB->writeChanges(doc.to_str()) );
        std::string res = plukDB->getUserChart("20170801T000000Z", "20170901T000000Z", param);
        CPPUNIT_ASSERT( doc.load_utf8( res ) );
        OutputDebugString( ( doc.pretty_wstr() + L"\n\n").c_str() );
        CPPUNIT_ASSERT( 2 == doc.select_nodes("//ForbidEvent").size() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent[ @Id='i1' ]").size() );
        CPPUNIT_ASSERT( 1 == doc.select_nodes("//ForbidEvent[ @Id='i1185' ]").size() );
    }
}

void TC_PlukParse::ParseMark()
{
	auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());
	std::wstring mark_first = L"<SoxPostUpdChange increment=\"Y\" presently=\"20170822T145959Z\"><UserChanges><MarkLayer><Trio layer=\"mark\"><Pre Id=\"i1\" esr=\"11273:11330\" create_time=\"20170822T134300Z\" Comment=\"Пометка &quot;толчок в пути&quot;\" inner_name=\"markPushWay\" cover=\"Y\"><picketing picketing1_val=\"461~59\" picketing1_comm=\"Россия\" /></Pre></Trio></MarkLayer></UserChanges></SoxPostUpdChange>";
	std::wstring mark_second = L"<SoxPostUpdChange increment=\"Y\" presently=\"20170822T145959Z\"><UserChanges><MarkLayer><Trio layer=\"mark\"><Pre Id=\"i1\" esr=\"11273:11330\" create_time=\"20170822T130700Z\" Comment=\"Пометка &quot;толчок в пути&quot;\" inner_name=\"markPushWay\" cover=\"Y\"><picketing picketing1_val=\"458~535\" picketing1_comm=\"Россия\" /></Pre><Body Id=\"i1\" esr=\"11273:11330\" create_time=\"20170822T134300Z\" Comment=\"Пометка &quot;толчок в пути&quot;\" inner_name=\"markPushWay\"><picketing picketing1_val=\"461~59\" picketing1_comm=\"Россия\" /></Body></Trio></MarkLayer></UserChanges></SoxPostUpdChange>";
	attic::a_document doc;
	doc.load_wide(mark_first);
	CPPUNIT_ASSERT(plukDB->writeChanges(doc.to_str()));
	doc.load_wide(mark_second);
	CPPUNIT_ASSERT(plukDB->writeChanges(doc.to_str()));
	std::vector<std::string> param;
	param.push_back(MarkLayer_xAttr);
	std::string result = plukDB->getUserChart("20170801T000000Z", "20170901T000000Z", param);
	CPPUNIT_ASSERT(!result.empty());
}

// Вспомогательный класс для повторной прокрутки
// логов плюка и записи их в БД
class MsgReader
{
public:
    MsgReader();
    ~MsgReader();
    bool get( attic::a_document& msg );
    std::wstring folder() const;
private:
    std::vector<std::ifstream> m_inlog;
    std::string   m_buffer;
    attic::a_document m_pathFromHef;
    std::vector<std::string> m_importantPiece;

    bool getAny( attic::a_document& msg );
    bool load( const std::string& accum, attic::a_document& msg );
    void removeExcess( attic::a_document& msg );
    bool isStart( boost::string_ref s ) const;
    bool isImportant( attic::a_document& msg ) const;
    bool containsPieces( boost::string_ref text ) const;
    attic::a_node hemCh( attic::a_document& msg ) const;
    static std::string originForCompare( attic::a_node n );
};

MsgReader::MsgReader()
{
    const std::wstring f = folder();
    m_inlog.emplace_back( std::ifstream( f+L"pluk.cut_log.2020-02-07", std::ios_base::in | std::ios_base::binary ) );
    m_inlog.emplace_back( std::ifstream( f+L"pluk.cut_log.2020-02-08", std::ios_base::in | std::ios_base::binary ) );
    auto h = m_pathFromHef.load_file( f+L"20020718_mixAuto.hef" );
    CPPUNIT_ASSERT( h );

    std::string query = "//AutoChart/HappenLayer/HemPath/SpotEvent[@create_time!=0 and @name!=0 and @Bdg!=0]";
    auto vNodes = m_pathFromHef.select_nodes( query.c_str() );
    for ( auto it : vNodes )
    {
        attic::a_node n = it.node();
        std::string temp = n.to_str();
        boost::string_ref s(temp);
        auto from = s.find("create_time");
        CPPUNIT_ASSERT( s.npos != from );
        auto to = s.find("Bdg");
        CPPUNIT_ASSERT( s.npos != to );
        while( to < s.size() && ' ' != s[to] )
            ++to;

        m_importantPiece.push_back( s.substr(from, to-from).to_string() );
    }
}

MsgReader::~MsgReader()
{}

bool MsgReader::get( attic::a_document& msg )
{
    while( true )
    {
        if ( !getAny( msg ) )
            return false;
        removeExcess(msg);
        if ( isImportant(msg) )
            return true;
    }
    return false;
}

std::wstring MsgReader::folder() const
{
    return L"E:/WorkCopy.all/Sample/logics/#5664/Анализ лога плюка/";
}

bool MsgReader::getAny( attic::a_document& msg )
{
    std::string accum = m_buffer;
    for( ; !m_inlog.empty(); m_inlog.erase( m_inlog.begin() ) )
    {
        auto& il = m_inlog.front();
        while( getline( il, m_buffer ) )
        {
            if ( !isStart(m_buffer) )
            {
                // не начало сообщения - продолжить накопление
                accum += m_buffer;
                m_buffer.clear();
            }
            else if ( !load( accum, msg ) )
            {
                // сообщение не загрузить - отбросить такое сообщение и продолжить работу
                accum = m_buffer;
                m_buffer.clear();
            }
            else
            {
                // загрузка успешна - вернуть результат
                return true;
            }
        }
        // файл окончен 
        if ( !load( accum, msg ) )
        {
            // накопленное сообщение не загрузить - отбросить такое сообщение и продолжить работу
            accum = m_buffer;
            m_buffer.clear();
        }
        else
        {
            // загрузка успешна - вернуть результат
            return true;
        }
    }
    // все закончено - результата нет
    return false;
}

bool MsgReader::load(const std::string& accum, attic::a_document& msg)
{
    boost::string_ref s(accum);
    const std::string pfx = "<body>";
    const std::string sfx = "</body>";
    OutputDebugString( FromUtf8( s.substr(0,31).to_string()+"\n" ).c_str() );
    auto pos = s.find(pfx);
    if ( s.npos != pos )
    {
        s.remove_prefix( pos+pfx.size() );
        auto epos = s.find(sfx);
        if ( s.npos != epos )
        {
            s.remove_suffix( s.size() - epos );
            return msg.load_utf8( s );
        }
    }
    // остальные случаи
    return false;
}

// Удалить все изменения, кроме AutoChanges
void MsgReader::removeExcess( attic::a_document& msg )
{
    bool more = true;
    while( more )
    {
        more = false;
        attic::a_node hemChanges = hemCh(msg);
        for( attic::a_node someCh : hemChanges.children() )
        {
            if ( !someCh.name_is("AutoChanges") )
            {
                more = hemChanges.remove_child( someCh );
                break;
            }
        }
    }
    more = true;
    while( more )
    {
        more = false;
        attic::a_node hl = hemCh(msg).child("AutoChanges").child("HappenLayer");
        for( attic::a_node trio : hl.children("Trio") )
        {
            if ( !containsPieces( trio.to_str() ) )
            {
                hl.remove_child( trio );
                more = true;
                break;
            }
        }
    }
}

bool MsgReader::isStart( boost::string_ref s ) const
{
    const size_t TYPE_POSITION = 22;
    if ( s.size()<=TYPE_POSITION )
        return false;
    else
    {
        auto head = s.substr(22,33);
        return head.npos != head.find(" [DEBUG] ") ||
               head.npos != head.find(" [INFO] ");
    }
}

bool MsgReader::isImportant( attic::a_document& msg ) const
{
    attic::a_node hcNode = hemCh(msg);
    std::string plainHL = hcNode.child("AutoChanges").child("HappenLayer").to_str();
    return containsPieces( plainHL );
}

bool MsgReader::containsPieces( boost::string_ref text ) const
{
    for( auto& piece : m_importantPiece )
    {
        if ( text.npos != text.find(piece) )
            return true;
    }
    return false;
}

attic::a_node MsgReader::hemCh( attic::a_document& msg ) const
{
    attic::a_node hemChanges = msg.document_element();
    if ( hemChanges.name_is("HemChanges") )
        return hemChanges;
    else
        return attic::a_node();
}

void TC_PlukParse::ReplayLog()
{
    auto plukDB = GetPlukDataBase(LocalPostgresInstance::GetStringConnect());

    MsgReader reader;
    std::ofstream dbg( reader.folder() + L"Debug.log",
                         std::ios_base::out | std::ios_base::binary | std::ios_base::trunc );

    std::vector<std::string> param;
    param.push_back(HappenLayer_xAttr);

    attic::a_document msg;
    unsigned seq=0;
    while( reader.get( msg ) )
    {
        if ( seq >= 2 )
            OutputDebugString(L"");
        dbg << "Changes[" << seq << "]:.................................\n";
        dbg << msg.pretty_str() << "\n";
        CPPUNIT_ASSERT( plukDB->writeChanges( msg.to_str() ) );

        std::string result = plukDB->getAutoChart("20200207T180000Z", "20200208T030000Z", param);
        attic::a_document dd;
        CPPUNIT_ASSERT( dd.load_utf8( result ) );
        dbg << "From DB[" << seq << "]:- - - - - - - - - - - - - - - - - -\n"
            << dd.pretty_str() << "\n\n";
        seq++;
    }
}

std::shared_ptr<PlukDataBase> TC_PlukParse::GetPlukDataBase(const std::string& stringConnect)
{
	auto plukDB = std::make_shared<UT_PlukDataBase>(stringConnect);
	const int max_wait = 5;
	int wait_count = 0;
	while (!plukDB->isConnect() && wait_count < max_wait) {
		++wait_count;
		std::this_thread::sleep_for(std::chrono::seconds(1));
	}
	if (wait_count == max_wait)
		throw std::runtime_error("Error connect to: " + stringConnect);
	else 
		return plukDB;
}

void TC_PlukParse::setUp()
{
	LocalPostgresInstance::Start();
}

void TC_PlukParse::tearDown() 
{
	LocalPostgresInstance::Stop();
}
