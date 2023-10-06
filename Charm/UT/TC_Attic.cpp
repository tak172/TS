#include "stdafx.h"

#include "TC_Attic.h"
#include "../helpful/Attic.h"
#include "../helpful/Time_Iso.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Attic );

void TC_Attic::attr_time()
{
    size_t HOUR = 60*60;
    time_t t=time(nullptr);
    t -= t%HOUR;
    t += 13*60;
    attic::a_document xreq("testAttr");
    attic::a_node root = xreq.document_element();
    CPPUNIT_ASSERT( root.append_attribute("moment").set_value(t) );
    time_t res=root.attribute("moment").as_time();
    CPPUNIT_ASSERT(t==res);

    std::string flash_text = "20150101T000000Z";
    root.brief_attribute("flash", flash_text );
    time_t flash =root.attribute("flash").as_time();
    root.brief_attribute("flash", flash );
    CPPUNIT_ASSERT( flash_text == root.attribute("flash").as_string() );
}

void TC_Attic::attr_time_2038()
{
    attic::a_document xreq("testAttr");
    attic::a_node root = xreq.document_element();
    // ошибка преобразования моментов после 2038-01-19 03:14:07 UTC
    root.brief_attribute("alfa", "20380119T031400Z" );
    root.brief_attribute("beta", "20380119T031407Z" );
    root.brief_attribute("more", "29991111T111111Z" );
    time_t alfa = root.attribute("alfa").as_time();
    time_t beta = root.attribute("beta").as_time();
    time_t more = root.attribute("more").as_time();
    CPPUNIT_ASSERT( 0 < alfa );
    CPPUNIT_ASSERT( alfa < beta );
    CPPUNIT_ASSERT( beta < more );
}

void TC_Attic::attr_char()
{
    char ch = '&';
    attic::a_document xreq("test");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("a").set_value(ch) );
    char res=n.attribute("a").as_char();
    CPPUNIT_ASSERT(ch==res);
}

void TC_Attic::attr_wchar()
{
    attic::a_document xreq("test");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("exist").set_value(L"ю") );
    CPPUNIT_ASSERT( L'ю' == n.attribute("exist").as_wchar() );
    CPPUNIT_ASSERT( 0 == n.attribute("notfound").as_wchar() );
}

void TC_Attic::attr_int_zero()
{
    attic::a_document xreq("testAttr");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("i0").set_value( 0 ) );
    CPPUNIT_ASSERT( n.attribute("i0").as_int() == 0 );
    CPPUNIT_ASSERT( n.attribute("i0").as_string() == std::string("0") );
}

void TC_Attic::attr_ulong()
{
    unsigned long ul = 0x12345678;
    attic::a_document xreq("testAttr");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("ul").set_value(ul) );
    unsigned long res=n.attribute("ul").as_ulong();
    CPPUNIT_ASSERT(ul==res);
}

void TC_Attic::attr_uuid()
{
    UUID_t id("709e682c-2292-4b86-bb36-36c744c40d4c");
    attic::a_document xreq("testAttr");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("set").set_value(id) );
    UUID_t tx = n.attribute("set").as_uuid();
    CPPUNIT_ASSERT( id == tx );
    CPPUNIT_ASSERT( n.append_attribute("zero").set_value( UUID_t() ) );
    UUID_t zr = n.attribute("zero").as_uuid();
    CPPUNIT_ASSERT( zr == UUID_t() );
}


void TC_Attic::attr_string_ref()
{
    attic::a_document xreq("test");
    attic::a_node n=xreq.document_element();
    CPPUNIT_ASSERT( n.append_attribute("AA").set_value("some string") );
    CPPUNIT_ASSERT( n.append_attribute("BB").set_value("some string") );
    // возвращаемые указатели конечно показывают в разные места
    CPPUNIT_ASSERT( n.attribute("AA").as_string() != n.attribute("BB").as_string() );
    // однако сравнение строк работает
    CPPUNIT_ASSERT( n.attribute("AA").as_string_ref() == n.attribute("BB").as_string_ref() );
    // можно делать строки для сравнения, но это менее удобно
    CPPUNIT_ASSERT( std::string(n.attribute("AA").as_string()) == std::string(n.attribute("BB").as_string()) );
}

// проверяется ввод-вывод предопределенных сущностей XML
void TC_Attic::entity()
{
    const char * example =
        "<Test quot=\"&quot;\" amp=\"&amp;\" apos=\"&apos;\" lt=\"&lt;\" gt=\"&gt;\" />";
    attic::a_document doc;
    doc.load_utf8( example );
    attic::a_node n=doc.document_element();
    CPPUNIT_ASSERT( '\"' == n.attribute("quot").as_wstring()[0] );
    CPPUNIT_ASSERT( '&' == n.attribute("amp").as_wstring()[0] );
    CPPUNIT_ASSERT( '\'' == n.attribute("apos").as_wstring()[0] );
    CPPUNIT_ASSERT( '<' == n.attribute("lt").as_wstring()[0] );
    CPPUNIT_ASSERT( '>' == n.attribute("gt").as_wstring()[0] );
    CPPUNIT_ASSERT( doc.to_str() == example );
}

// проверяется компактный и красивый вывод XML
void TC_Attic::pretty()
{
    const char * dirty =
        "\n\n\n\n\n                   <Test>   <First "
        "                    Id='123' Stan='***' >"
        "    <Object Put='+' Napr='++' />   <Speed Gruz='25' Pass='25' Elec='25' />"
        "  </First> "
        "               \n\n\n   "
        "  <Second                         Id='999'        >"
        "  </Second> "
        "       "
        "               \n\n\n        </Test>";

    const wchar_t pretty[] =
        L"<Test>"                                             L"\n"
        L"  <First Id=\"123\" Stan=\"***\">"                  L"\n"
        L"    <Object Put=\"+\" Napr=\"++\" />"               L"\n"
        L"    <Speed Gruz=\"25\" Pass=\"25\" Elec=\"25\" />"  L"\n"
        L"  </First>"                                         L"\n"
        L"  <Second Id=\"999\" />"                            L"\n"
        L"</Test>"                                            L"\n";

    const char plain[] =
        "<Test>"                                             
        "<First Id=\"123\" Stan=\"***\">"                  
        "<Object Put=\"+\" Napr=\"++\" />"               
        "<Speed Gruz=\"25\" Pass=\"25\" Elec=\"25\" />"
        "</First>"                                       
        "<Second Id=\"999\" />"                          
        "</Test>";

    attic::a_document doc;
    doc.load_utf8( dirty );
    CPPUNIT_ASSERT( doc.to_str() == plain );
    CPPUNIT_ASSERT( doc.pretty_wstr() == pretty );
}

void TC_Attic::declaration()
{
    using namespace std;
    string etalon = "<?xml version=\"1.0\" encoding=\"UTF-8\" standalone=\"yes\"?><!--bla bla bla--><xxx why=\"since\" />";
    string et1251 = "<?xml version=\"1.0\" encoding=\"windows-1251\" standalone=\"yes\"?><!--bla bla bla--><xxx why=\"since\" />";
    // создаем документ с декларацией
	attic::a_document doc;
	doc.append_comment(L"bla bla bla");
	doc.set_document_element("xxx").append_attribute("why").set_value("since");
	doc.append_declaration();
    // совпадение с эталоном
	CPPUNIT_ASSERT( doc.to_str() == etalon );
    // повторная декларация изменяет существующую декларацию
	doc.append_declaration( 1251 );
    CPPUNIT_ASSERT( doc.to_str() == et1251 );
}

// добавление комментариев к документу
void TC_Attic::doc_comment()
{
    attic::a_document doc;

    doc.append_comment(L"one");
    CPPUNIT_ASSERT( doc.to_str() == "<!--one-->" );

    doc.append_comment(L"two");
    CPPUNIT_ASSERT( doc.to_str() == "<!--one--><!--two-->" );

    doc.set_document_element("main");
    doc.append_comment(L"three");
    CPPUNIT_ASSERT( doc.to_str() == "<!--one--><!--two--><main /><!--three-->" );
}

// проверяется обрезание свободного места вокруг plain character data
// Красивый вывод XML добавляет пробелы и переводы строк,
// при повторной загрузке pcdata должны совпадать
void TC_Attic::trim_ws_around_pcdata()
{
    attic::a_document doc;
    std::wstring w1,w2;
    // документ без вложенных узлов
    doc.load_utf8("<Some>  \n\n &gt;&lt; \n\n\n\n </Some>");
    w1 = doc.pretty_wstr();
    doc.load_wide(w1);
    w2 = doc.pretty_wstr();
    CPPUNIT_ASSERT( w1==w2 );

    // документ с вложенными узлами
    doc.load_utf8("<Kind>   &gt; \n\n\n\n  <Sub> lower <Sub/>  </Kind>");
    w1 = doc.pretty_wstr();
    doc.load_wide(w1);
    w2 = doc.pretty_wstr();
    CPPUNIT_ASSERT( w1==w2 );
}

// сравнение узлов и атрибутов xml на равенство
void TC_Attic::identical()
{
    attic::a_document doc("r2d2");
    {
        auto ch1 = doc.document_element().append_child("Morn");
        ch1.brief_attribute("Parrot", 38 );
        ch1.brief_attribute("Monkey", "5" );
        ch1.append_child("Granny").brief_attribute("patty","hurrah");

        auto ch2 = doc.document_element().append_child("TWILIGHT");
        ch2.append_child("Granny").brief_attribute("PIZZA","hurrah");
        ch2.brief_attribute("Parrot", "38" );
        ch2.brief_attribute("Monkey", 5 );

        CPPUNIT_ASSERT( ch1 != ch2 );
    }
    // сейчас не равны
    auto m = doc.document_element().child("Morn");
    auto t = doc.document_element().child("TWILIGHT");
    CPPUNIT_ASSERT( !attic::identical(m,t) );
    CPPUNIT_ASSERT( m.to_str() != t.to_str() );
    // после двух правок
    t.first_child().first_attribute().set_name("patty");
    t.set_name("Morn");
    // стали равны
    CPPUNIT_ASSERT( attic::identical(m,t) );
    CPPUNIT_ASSERT( m.to_str() == t.to_str() );
    CPPUNIT_ASSERT( m != t ); // хотя это разные узлы документа
    // добавить безымянные узлы с разными значениями
    auto mPC = m.append_text("mmm");
    auto tPC = t.append_text("ttt");
    CPPUNIT_ASSERT( !attic::identical(m,t) );
    // проверить с одинаковыми значениями
    mPC.set_value("xxx");
    tPC.set_value("xxx");
    CPPUNIT_ASSERT( attic::identical(m,t) );
}

// сравнение узлов и атрибутов xml на порядок
void TC_Attic::isLess()
{
    attic::a_document doc("r2d2");
    {
        auto ch1 = doc.document_element().append_child("Alfa");
        ch1.brief_attribute("Flag", "5" );
        ch1.append_child("Goga").brief_attribute("Cool","yes");

        auto ch2 = doc.document_element().append_child("Beta");
        ch2.append_child("Zuzu").brief_attribute("Hot","yes");
        ch2.brief_attribute("Flag", 5 );

        CPPUNIT_ASSERT( ch1 != ch2 );
    }
    //
    auto one = doc.document_element().child("Alfa");
    auto two = doc.document_element().child("Beta");
    CPPUNIT_ASSERT( attic::isLess(one,two) );
    CPPUNIT_ASSERT( !attic::isLess(two,one) );
    CPPUNIT_ASSERT( one.to_str() != two.to_str() );
    // после правок
    two.first_child().set_name("Goga");
    two.first_child().first_attribute().set_name("Cool");
    two.set_name("Alfa");
    // стали равны
    CPPUNIT_ASSERT( one.to_str() == two.to_str() );
    CPPUNIT_ASSERT( !attic::isLess(one,two) );
    CPPUNIT_ASSERT( !attic::isLess(two,one) );
    CPPUNIT_ASSERT( one != two ); // хотя это разные узлы документа
}

void TC_Attic::encoding()
{
    using namespace std;
    struct Two
    {
        int code;
        wstring wtext;
    };
    Two example[] =
    {
        { CODEPAGE_UTF8,
        L"<?xml version='1.0' standalone='yes'?>"
        L"<Text comment='Кириллица' />"
        },
        { CODEPAGE_UTF8,
        L"<?xml version='1.0' encoding='UTF-8' standalone='yes'?>"
        L"<Text comment='Кириллица' />"
        },
        { 1251,
        L"<?xml version='1.0' encoding='Windows-1251' standalone='yes'?>"
        L"<Text comment='Кириллица' />"
        },
        { 866,
        L"<?xml version='1.0' encoding='cp866' standalone='yes'?>"
        L"<Text comment='Кириллица' />"
        }
    };

    for( auto ex : example )
    {
        std::string raw = ToOtherCP( ex.wtext, ex.code );

        // создаем документ с декларацией
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_strange( raw ) );
        CPPUNIT_ASSERT( L"Кириллица" == doc.document_element().attribute("comment").as_wstring() );
    }
}

// Быстрая проверка на Xml-ность набора байт
void TC_Attic::xmllike()
{
    std::string s( ToUtf8(L"<Test><One>Даже<One/>русский</Test>") );

    // чистая строка считается XML-документом
    std::vector<char> v( s.begin(), s.end() );
    CPPUNIT_ASSERT( attic::Xmllike(v) );

    // документ выведем красиво, но он должен опознаваться как XML
    // хотя есть лишние пробелы и переводы строки
    attic::a_document doc;
    doc.load_wide( From1251(s) );
    std::string beautiful = "   \n" + doc.pretty_str();
    std::vector<char> v2( beautiful.begin(), beautiful.end() );
    CPPUNIT_ASSERT( attic::Xmllike(v2) );
}

/*
void TC_Attic::memoryUsage()
{
    attic::a_document doc("MyRoot");
    const size_t PASS = 10000;
    const size_t HOLD = 50;
    size_t curr = 0;
    attic::a_node node = doc.document_element();
    {
        size_t a, u;
        doc.memory_info( a, u );
    }

    for( size_t step=0; PASS>step; ++step )
    {
        attic::a_node ch = node.append_child( "tag"+std::to_string(step) );
        attic::a_attribute atr = ch.append_attribute( "atr" + std::to_string(step) );
        atr.set_value( std::string( step+100, char('a'+step) ) );
        ++curr;
        if ( curr > HOLD )
        {
            attic::a_node nn = node.first_child();
            for( size_t i=0; i < step % HOLD; ++i )
                nn = nn.next_sibling();
            if ( nn )
            {
                node.remove_child( nn );
                --curr;
            }
        }
        
        size_t a, u;
        doc.memory_info( a, u );
        std::wstring ws = L"Doc: allocated "+ std::to_wstring(a);
        ws += L" used " +std::to_wstring(u);
        ws += L" ratio " + std::to_wstring(u*100/a) + L"%\n";
#ifndef LINUX
        OutputDebugString(ws.c_str());
#else
        std::wcerr << ws << std::endl;
#endif // !LINUX
    }

    {
        attic::a_document doc2;
        doc2.reset(doc);
        size_t a, u;
        doc2.memory_info( a, u );
#ifndef LINUX
        OutputDebugString( (L"Doc2 allocated " + std::to_wstring(a) + L" used " +std::to_wstring(u) + L"\n").c_str() );
        OutputDebugString( (L"Doc2 string size " + std::to_wstring( doc2.to_str().size() ) + L"\n").c_str() );
#else
        std::wcerr << L"Doc2 allocated " + std::to_wstring(a) + L" used " + std::to_wstring(u) << std::endl;
        std::wcerr << L"Doc2 string size " + std::to_wstring(doc2.to_str().size()) << std::endl;
#endif // !LINUX
    }
}
*/

static std::shared_ptr<attic::a_document> single_write( std::string from, std::string to, size_t count )
{
    std::shared_ptr<attic::a_document> pdoc( new attic::a_document("ParaData") ); 
    attic::a_node root = pdoc->document_element();
    root.brief_attribute("left", from);
    root.brief_attribute("right",to);
    time_t left = root.attribute("left").as_time();
    time_t right = root.attribute("right").as_time();
    root.brief_attribute( "leftX", left );
    root.brief_attribute( "rightX", right );
    CPPUNIT_ASSERT( from == root.attribute("leftX").as_string() );
    CPPUNIT_ASSERT( to   == root.attribute("rightX").as_string() );
    time_t delta = (right-left)/count;
    for( time_t i = left; i < right; i += delta )
    {
        root.append_child("Item").brief_attribute("time", i );
    }
    return pdoc;
}

static size_t parallel_write( std::string from, std::string to, size_t count )
{
    auto putter = [&from,&to,&count]() -> std::string
    {
        auto pdoc = single_write( from, to, count );
        return pdoc->to_str();
    };

    auto f1 = std::async( std::launch::async, putter );
    auto f2 = std::async( std::launch::async, putter );
    auto f3 = std::async( std::launch::async, putter );

    std::string s1 = f1.get();
    std::string s2 = f2.get();
    std::string s3 = f3.get();

    size_t err = 0;
    if ( s1.size() != s2.size() )
        err = 12000;
    else if ( s2.size() != s3.size() )
        err = 23000;
    else
    {
        for( auto i1 = s1.cbegin(), i2 = s2.cbegin(), i3 = s3.cbegin(); s1.cend() != i1; ++i1, ++i2, ++i3 )
        {
            if ( *i1 != *i2 || *i2 != *i3 )
                ++err;
        }
    }
    return err;
}

static size_t parallel_read( const attic::a_document& doc )
{
    auto getter = [&doc]() -> std::vector<time_t>
    {
        std::vector<time_t> result;
        for( attic::a_node n : doc.document_element().children("Item") )
            result.push_back( n.attribute("time").as_time() );
        return result;
    };

    auto f1 = std::async( std::launch::async, getter );
    auto f2 = std::async( std::launch::async, getter );
    auto f3 = std::async( std::launch::async, getter );

    std::vector<time_t> r1 = f1.get();
    std::vector<time_t> r2 = f2.get();
    std::vector<time_t> r3 = f3.get();

    size_t err = 0;
    if ( r1.empty() )
    {
        ++err;
    }
    else
    {
        for( auto it = r1.cbegin(), itE = std::prev( r1.cend() ); itE != it; ++it )
        {
            if ( *it < *std::next(it) )
                ;
            else
                ++err;
        }
    }
    for( auto it1 = r1.cbegin(), it2 = r2.cbegin(), it3 = r3.cbegin(); r1.cend() != it1; ++it1, ++it2, ++it3 )
    {
        if ( *it1 != *it2 || *it2 != *it3 )
            ++err;
    }
    return err;
}

// тест на параллельное чтение атрибута времени из XML
void TC_Attic::async_time_attribute_read()
{
    const size_t item_count = 365*24;
    const size_t loop_count = 32; 
    auto pdoc = single_write( "20150101T000000Z", "20160101T000000Z", item_count );
 
    std::vector<size_t> err;
    for( unsigned int loop=0; loop < loop_count; loop++ )
    {
        err.push_back( parallel_read( *pdoc ) );
        //OutputDebugString( ( std::to_wstring(loop) + L"").c_str() );
    }
    //OutputDebugString( L"\n" );
    std::sort( err.begin(), err.end() );
//     OutputDebugString( (L" Repeat read " + std::to_wstring(loop_count) + L", ").c_str() );
//     OutputDebugString( (L" Items " + std::to_wstring(item_count) + L", ").c_str() );
//     OutputDebugString( (L" Errors min=" + std::to_wstring(err.front()) + L" ").c_str() );
//     OutputDebugString( (L" median=" + std::to_wstring(err[err.size()/2]) + L" ").c_str() );
//     OutputDebugString( (L" max=" + std::to_wstring(err.back()) + L" \n").c_str() );
    CPPUNIT_ASSERT( err.back() == 0 );
}

// тест на параллельную запись атрибута времени в XML
void TC_Attic::async_time_attribute_write()
{
    const size_t item_count = 365*24;
    const size_t loop_count = 32; 
    std::vector<size_t> err;
    // проверка записи
    {
//         auto ods = []( const std::wstring& ws ){
//             OutputDebugString( ws.c_str() );
//         };
//         FotoFinish ff( __FUNCTIONW__, ods, 5 );
        for(unsigned int loop=0; loop < loop_count; loop++ )
        {
            err.push_back( parallel_write( "20150101T000000Z", "20160101T000000Z", item_count ) );
//             OutputDebugString( std::to_wstring(loop).c_str() );
        }
//         OutputDebugString( L"\n" );
    }
    std::sort( err.begin(), err.end() );
//     OutputDebugString( (L" Repeat write " + std::to_wstring(loop_count) + L", ").c_str() );
//     OutputDebugString( (L" Items " + std::to_wstring(item_count) + L", ").c_str() );
//     OutputDebugString( (L" Errors min=" + std::to_wstring(err.front()) + L" ").c_str() );
//     OutputDebugString( (L" median=" + std::to_wstring(err[err.size()/2]) + L" ").c_str() );
//     OutputDebugString( (L" max=" + std::to_wstring(err.back()) + L" \n").c_str() );
    CPPUNIT_ASSERT( err.back() == 0 );
}

// проверка на слияние последовательных элементов PCData
void TC_Attic::adjacentPCData()
{
    attic::a_document docA("d");
    attic::a_document docB("d");
    attic::a_node nA = docA.document_element().append_child("ch");
    attic::a_node nB = docB.document_element().append_child("ch");
    auto a1 = nA.append_text("alfa");
    auto a2 = nA.append_text("beta");
    /*auto b12 =*/ nB.append_text("alfabeta");
    std::string txtA = docA.to_str();
    std::string txtB = docB.to_str();
    CPPUNIT_ASSERT( txtA == txtB );
    CPPUNIT_ASSERT( attic::identical( attic::a_node( docA ), attic::a_node( docB ) ) );
    CPPUNIT_ASSERT( a1 == a2 );
}

void TC_Attic::exact_text()
{
    const std::string sample = "11]]>  \n22<![CDATA[33   \n\n";

    std::string row;
    {
        attic::a_document doc("ex");
        attic::a_node node1 = doc.document_element().append_child("x");
        attic::a_node node2 = doc.document_element().append_child("y");

        node1.append_text( sample );
        node2.append_exact_text(sample);

        row = doc.to_str();
    }
    {
        attic::a_document doc;
        CPPUNIT_ASSERT( doc.load_utf8(row) );

        attic::a_node node1 = doc.document_element().child("x");
        attic::a_node node2 = doc.document_element().child("y");

        std::string like = node1.text_as_string();
        std::string exact = node2.text_as_string();

        CPPUNIT_ASSERT( sample != like );
        CPPUNIT_ASSERT( sample == exact );
    }
}
