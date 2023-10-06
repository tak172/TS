#include "stdafx.h"

#include "UtHemHelper.h"
#include "../Hem/BusyLayer.h"
#include "../Hem/LimitLayer.h"
#include "../Hem/NoteLayer.h"
#include "../Hem/ForbidLayer.h"
#include "../Hem/GuiLayer.h"

void UtLayer<HappenLayer>::createPath_impl( const std::shared_ptr<const SpotEvent>* arr, size_t count )
{
    const UtIdentifyCategoryList identifyCategory;
    TrioSpot trio;
    trio.body( arr, arr+count );
    Hem::aePusherUserHappen pusher( GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT_EQUAL(trio.body().size(), sz);

    TakeChanges( attic::a_node() );
}

void UtLayer<HappenLayer>::createPath( const std::wstring& text )
{
    const UtIdentifyCategoryList identifyCategory;
    attic::a_document sourceDoc;
    CPPUNIT_ASSERT(sourceDoc.load_wide(text));

    HappenLayer::EventsList vEv;

    for (attic::a_node node : sourceDoc.document_element().children() )
    {
        HemHelpful::SpotEventPtr complexSpot = std::make_shared<SpotEvent>(node);
        vEv.push_back(complexSpot);
    }
    CPPUNIT_ASSERT( !vEv.empty() );
    TrioSpot trio;
    trio.body( vEv.begin(), vEv.end() );
    Hem::aePusherUserHappen pusher( this->GetWriteAccess(), trio, identifyCategory );
    pusher.Action();
    auto sz = pusher.AppliedEvents().size();
    CPPUNIT_ASSERT_EQUAL(trio.body().size(), sz);

    this->TakeChanges( attic::a_node() );
}

template<>
std::vector<size_t> UtLayer<HappenLayer>::path_sizes()
{
    auto resdoc = result();
    auto paths = resdoc->select_nodes( "//HemPath" );
    std::vector<size_t> res;
    for( auto path : paths )
    {
        size_t count=0;
        for( attic::a_node spot : path.node().children("SpotEvent") )
        {
            ++count;
        }
        res.push_back( count );
    }
    return res;
}

void UtLayer<FutureLayer>::createPath( const std::wstring& text )
{
    attic::a_document sourceDoc;
    CPPUNIT_ASSERT(sourceDoc.load_wide(text));
    std::vector<FixableEvent> events;
    for (attic::a_node node : sourceDoc.document_element().children() )
        events.emplace_back(node);
    auto firstEventPtr = std::make_shared<FixableEvent>( events.front() );
    {
        // блок необходим для освобождения WriteAccess
        auto futureWriteAccess = this->GetWriteAccess();
        bool created = Hem::CreatePath<FixableEvent>( futureWriteAccess, firstEventPtr );
        CPPUNIT_ASSERT( created );
        std::shared_ptr<Hem::FixablePath> pathPtr;
        pathPtr = futureWriteAccess->getPath( firstEventPtr );
        CPPUNIT_ASSERT( pathPtr );

        auto iFirst = events.begin()+1;
        auto iLast  = events.end();
        std::for_each( events.begin()+1, events.end(),
            [&futureWriteAccess,pathPtr]( const FixableEvent & event ){
                bool pushed = futureWriteAccess->pushBack( std::make_shared<FixableEvent>( event ), pathPtr );
                CPPUNIT_ASSERT( pushed );
        } );
    }
    this->TakeChanges( attic::a_node() );
}

template<>
std::vector<size_t> UtLayer<FutureLayer>::path_sizes()
{
    auto resdoc = result();
    auto paths = resdoc->select_nodes( "//HemPath" );
    std::vector<size_t> res;
    for( auto path : paths )
    {
        size_t count=0;
        for( attic::a_node spot : path.node().children("SpotEvent") )
        {
            ++count;
        }
        res.push_back( count );
    }
    return res;
}

template<> size_t UtLayer<HappenLayer>::path_count()
{
    return path_count_impl( "//HemPath" );
}

template<> size_t UtLayer<AsoupLayer>::path_count()
{
    return path_count_impl( "//AsoupEvent" );
}

template<> size_t UtLayer<FutureLayer>::path_count()
{
    return path_count_impl( "//HemPath" );
}

template<> size_t UtLayer<Note::Layer>::path_count()
{
    return path_count_impl( "//MarkEvent" );
}

template<> size_t UtLayer<Limit::Layer>::path_count()
{
    return path_count_impl( "//LimitEvent" );
}

template<> size_t UtLayer<Forbid::Layer>::path_count()
{
    return path_count_impl( "//ForbidEvent" );
}

template<> size_t UtLayer<Busy::Layer>::path_count()
{
    return path_count_impl( "//BusyPath" );
}

template<> size_t UtLayer<Gui::Layer>::path_count()
{
	return path_count_impl( "//GuiEvent" );
}

template< class L >
size_t UtLayer<L>::path_count_impl( const std::string& xpath_selector )
{
    return result()->select_nodes( xpath_selector.c_str() ).size();
}

template<>
bool UtLayer<Limit::Layer>::exist_event( const std::string& cond )
{
    std::string ss = "//LimitEvent";
    if ( !cond.empty() )
        ss += "[" + cond + "]";
    return !result()->select_nodes( ss.c_str() ).empty();
}

template<>
bool UtLayer<Forbid::Layer>::exist_event( const std::string& cond )
{
    std::string ss = "//ForbidEvent";
    if ( !cond.empty() )
        ss += "[" + cond + "]";
    return !result()->select_nodes( ss.c_str() ).empty();
}

template<>
bool UtLayer<Note::Layer>::exist_event( const std::string& cond )
{
    std::string ss = "//MarkEvent";
    if ( !cond.empty() )
        ss += "[" + cond + "]";
    return !result()->select_nodes( ss.c_str() ).empty();
}

template<>
bool UtLayer<Busy::Layer>::exist_event( const std::string& cond )
{
    std::string ss = "//BusyEvent";
    if ( !cond.empty() )
        ss += "[" + cond + "]";
    return !result()->select_nodes( ss.c_str() ).empty();
}

template<>
bool UtLayer<HappenLayer>::exist_event( const std::string& cond )
{
    std::string ss = "//HemPath/SpotEvent";
    if ( !cond.empty() )
        ss += " [ " + cond + " ]";
    auto rr = result();
    auto qq = rr->pretty_wstr();
    return !qq.empty() && !rr->select_nodes( ss.c_str() ).empty();
}

template<>
bool UtLayer<Gui::Layer>::exist_event( const std::string& cond )
{
	std::string ss = "//GuiEvent";
	if ( !cond.empty() )
		ss += "[" + cond + "]";
	return !result()->select_nodes( ss.c_str() ).empty();
}

template<>
std::vector<std::string> UtLayer<HappenLayer>::read_pack( std::function<std::string(attic::a_node)> reader )
{
    auto resdoc = result();
    auto paths = resdoc->select_nodes( "//HemPath" );
    const std::string DASH("-");
    std::vector<std::string> res;
    for( auto path : paths )
    {
        std::string sum;
        for( attic::a_node spot = path.node().child("SpotEvent"); spot; spot = spot.next_sibling("SpotEvent") )
        {
            std::string temp( reader(spot) );
            sum += temp.empty()? std::string("-") : temp;
            sum += ' ';
        }
        res.push_back( sum );
    }
    return res;
}

static std::string make_value(const std::string& draft_value)
{
    std::string value=draft_value;
    char SPACE = ' ';
    const std::string TWOSPACE(2,SPACE);
    while( !value.empty() && SPACE == value.front() )
        value.erase( value.begin() );
    for( auto pos = value.find(TWOSPACE); std::string::npos != pos; pos = value.find(TWOSPACE,pos) )
        value.erase(pos,1);
    if ( value.back() != SPACE )
        value.push_back( SPACE );
    return value;
}

template<>
bool UtLayer<HappenLayer>::exist_pack( std::function<std::string(attic::a_node)> reader, const std::string& draft_value )
{
    auto sum = read_pack( reader );
    std::string value = make_value( draft_value );
    return sum.end() != std::find( sum.begin(), sum.end(), value );
}

template<>
bool UtLayer<FutureLayer>::exist_pack( std::function<std::string(attic::a_node)> reader, const std::string& draft_value )
{
    std::string value = make_value( draft_value );
    auto sum = read_pack( reader );
    return sum.end() != std::find( sum.begin(), sum.end(), value );
}

template<>
std::vector<std::string> UtLayer<FutureLayer>::read_pack( std::function<std::string(attic::a_node)> reader )
{
    auto resdoc = result();
    auto paths = resdoc->select_nodes( "//HemPath" );
    const std::string DASH("-");
    std::vector<std::string> res;
    for( auto path : paths )
    {
        std::string sum;
        for( attic::a_node spot = path.node().child("SpotEvent"); spot; spot = spot.next_sibling("SpotEvent") )
        {
            std::string temp( reader(spot) );
            sum += temp.empty()? std::string("-") : temp;
            sum += ' ';
        }
        res.push_back( sum );
    }
    return res;
}

void UtLayer<AsoupLayer>::createPath( const std::wstring& text )
{
    auto before = this->path_count();

    attic::a_document sourceDoc;
    if ( sourceDoc.load_wide( text ) )
    {
        Hem::AsoupServedData asoupData( sourceDoc.document_element() );
        this->PushAsoupData(asoupData);
    }
    this->TakeChanges( attic::a_node() );
    auto after = this->path_count();
    CPPUNIT_ASSERT( before+1 == after );
}

template<>
std::vector<std::string> UtLayer<Note::Layer>::read_pack( std::function<std::string(attic::a_node)> reader )
{
    auto resdoc = result();
    auto marks = resdoc->select_nodes( "//MarkEvent" );
    const std::string DASH("-");
    std::vector<std::string> res;
    for( auto mark : marks )
    {
        std::string sum;
        std::string temp( reader(mark.node()) );
        sum += temp.empty()? std::string("-") : temp;
        sum += ' ';
        res.push_back( sum );
    }
    return res;
}

template<>
bool UtLayer<Note::Layer>::exist_pack( std::function<std::string(attic::a_node)> reader, const std::string& draft_value )
{
    auto sum = read_pack( reader );
    std::string value = make_value( draft_value );
    return sum.end() != std::find( sum.begin(), sum.end(), value );
}

UtIdentifyCategoryList::UtIdentifyCategoryList() :
    Hem::IdentifyCategoryList()
{
    std::wstring idCategoryStr = 
        L"<Category>"
        L"<Link numbers='1-698'/>"
        L"<Link numbers='701-898, 6001-7998' />"
        L"<Link numbers='1001-3998, 9001-9798' />"
        L"<Link numbers='4001-4998' />"
        L"<Link numbers='5001-5998' />"
        L"<Link numbers='8001-8048' />"
        L"<Link numbers='8051-8098' />"
        L"<Link numbers='8101-8998' />"

        L"<Tear numbers='' departureNumbers='4001-4998' />"
        L"<Tear numbers='1000-2998' departureNumbers='8000-8998' />"
        L"</Category>";

    attic::a_document adoc;
    adoc.load_wide( idCategoryStr );

    deserialize(adoc.document_element());
}

std::vector<HemHelpful::SpotEventPtr> loaderSpot( const wchar_t* text )
{
	attic::a_document doc;
	CPPUNIT_ASSERT( doc.load_wide(text) );
	std::vector<HemHelpful::SpotEventPtr> result;
	for (attic::a_node node : doc.document_element().children() )
	{
		HemHelpful::SpotEventPtr complexSpot = std::make_shared<SpotEvent>(node);
		result.push_back(complexSpot);
	}
	return result;
}

