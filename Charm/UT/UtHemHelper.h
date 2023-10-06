#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Hem/Appliers/aePusherHappen.h"
#include "../Hem/GriffinUtility.h"
#include "../Hem/FutureLayer.h"
#include "../Hem/IdentifyCategory.h"

class UtIdentifyCategoryList : public Hem::IdentifyCategoryList
{
public:
    UtIdentifyCategoryList();
};

template< class SomeLayer >
class UtLayer : public SomeLayer
{
public:
    static UtLayer<SomeLayer>& upgrade( SomeLayer& someLayer )
    {
        return reinterpret_cast< UtLayer<SomeLayer>& >(someLayer);
    }
    SomeLayer&       base()       { return *this; }
    const SomeLayer& base() const { return *this; }

    bool exist_path_size( size_t count );
    size_t path_count();
    bool exist_event( const std::string& cond );
    bool exist_series( const std::string& attr, const std::string& value );
    bool exist_series2( std::pair<const std::string&,const std::string&> node_attr, const std::string& value );
    std::string get_series( const std::string& attr );

    template<unsigned N> void createPath( const std::shared_ptr<const SpotEvent>(&arr)[N] );
    template<unsigned N> void createPath( const SpotEvent(&arr)[N] );
    void createPath( const std::wstring& text );

    std::shared_ptr<attic::a_document> result();

    friend attic::a_node operator<< ( attic::a_node parent_node, const UtLayer<SomeLayer>& some_layer )
    {
        parent_node << some_layer.base();
        return parent_node;
    }

    friend const attic::a_node operator>> ( attic::a_node parent_node, UtLayer<SomeLayer>& some_layer )
    {
        parent_node >> some_layer.base();
        return parent_node;
    }

private:
    bool exist_pack( std::function<std::string(attic::a_node)> reader, const std::string& value );
    std::vector<std::string> read_pack( std::function<std::string(attic::a_node)> reader );
    std::vector<size_t> path_sizes();
    size_t path_count_impl( const std::string& xpath_selector );
    void createPath_impl( const std::shared_ptr<const SpotEvent>* first, size_t count );
};

template< class L >
inline bool UtLayer<L>::exist_path_size(size_t count)
{
    auto sizes = path_sizes();
    return sizes.end() != std::find( sizes.begin(), sizes.end(), count );
}

template< class L >
inline bool UtLayer<L>::exist_series( const std::string& attr, const std::string& draft_value )
{
    auto get_attr = [&attr]( attic::a_node spot ) {
        return std::string( spot.attribute( attr ).as_string() );
    };
    return exist_pack(get_attr, draft_value);
}

template< class L >
inline std::string UtLayer<L>::get_series( const std::string& attr )
{
    auto get_attr = [&attr]( attic::a_node spot ) {
        return std::string( spot.attribute( attr ).as_string() );
    };
    auto sum = read_pack( get_attr );
    std::string res;
    if ( !sum.empty() )
    {
        std::swap( res, sum.front() );
        sum.erase( sum.begin() );
        for( auto& s : sum )
            res += "; " + s;
    }
    return res;
}

template< class L >
template<unsigned N>
inline void UtLayer<L>::createPath( const std::shared_ptr<const SpotEvent>(&arr)[N] )
{
    createPath_impl( arr, N );
}

template< class L >
template<unsigned N>
inline void UtLayer<L>::createPath( const SpotEvent(&arr)[N] )
{
    std::shared_ptr<const SpotEvent> temp[N];
    for( unsigned i = 0; i<N; ++i )
        temp[i] = std::make_shared<const SpotEvent>( arr[i] );

    createPath(temp);
}

template< class L >
inline std::shared_ptr<attic::a_document> UtLayer<L>::result()
{
    TakeChanges( attic::a_node() );
    std::shared_ptr<attic::a_document> pdoc = std::make_shared<attic::a_document>("test");
    pdoc->document_element() << (const L&)(*this);
    return pdoc;
}

template<>
inline std::shared_ptr<attic::a_document> UtLayer<HappenLayer>::result()
{
    TakeChanges(attic::a_node());
    std::shared_ptr<attic::a_document> pdoc = std::make_shared<attic::a_document>("test");
    Serialize(pdoc->document_element());
    return pdoc;
}

template<>
inline std::shared_ptr<attic::a_document> UtLayer<FutureLayer>::result()
{
    TakeChanges(attic::a_node());
    std::shared_ptr<attic::a_document> pdoc = std::make_shared<attic::a_document>("test");
    Serialize(pdoc->document_element());
    return pdoc;
}

template< class L >
bool UtLayer<L>::exist_series2( std::pair<const std::string&,const std::string&> node_attr, const std::string& draft_value )
{
    auto get_subnode_attr = [&node_attr]( attic::a_node spot ) -> std::string {
        return std::string( spot.child(node_attr.first).attribute( node_attr.second ).as_string() );
    };

    return exist_pack( get_subnode_attr, draft_value );
}

template< class T > T createEvent( const std::wstring& text )
{
    attic::a_document doc;
    bool succ = doc.load_wide( text );
    CPPUNIT_ASSERT( succ );
    T ev;
    doc.document_element() >> ev;
    return ev;
}
template<>
inline SpotEvent createEvent<SpotEvent>( const std::wstring& text )
{
    attic::a_document doc;
    bool succ = doc.load_wide( text );
    CPPUNIT_ASSERT( succ );
    SpotEvent ev( doc.document_element() );
    return ev;
}
// лишь объявление. Потребовалась лишь одна специализация (см.ниже)
template< class T > std::shared_ptr<T> createEventPtr( const std::wstring& text );
template<>
inline std::shared_ptr<SpotEvent> createEventPtr<SpotEvent>( const std::wstring& text )
{
    attic::a_document doc;
    bool succ = doc.load_wide( text );
    CPPUNIT_ASSERT( succ );
    return std::make_shared<SpotEvent>( doc.document_element() );
}

namespace Gui { class Layer; };

template < class SomeChart >
class UtChart : public SomeChart
{
public:
    static UtChart<SomeChart>& upgrade( SomeChart& someChart )
    {
        return reinterpret_cast< UtChart<SomeChart>& >(someChart);
    }
    UtLayer<HappenLayer>& getHappenLayer()
    {
        return UtLayer<HappenLayer>::upgrade( SomeChart::getHappenLayer() );
    }
    UtLayer<Gui::Layer>& getGuiLayer()
    {
        return UtLayer<Gui::Layer>::upgrade( SomeChart::getGuiLayer() );
    }
};

std::vector<HemHelpful::SpotEventPtr> loaderSpot( const wchar_t* text );
