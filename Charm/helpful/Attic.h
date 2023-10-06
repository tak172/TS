#pragma once

#ifndef _ATTIC_H_
#define _ATTIC_H_

#include <boost/static_assert.hpp>
#include <boost/utility/string_ref_fwd.hpp>
//#include "PugiXML/pugixml.hpp"
#include "../../AdoptTools/PugiXML/pugixml.hpp"
#include "../helpful/Utf8.h"
#include "../helpful/Uuid.h"
#include <map>
#include <vector>
#include <time.h>

namespace boost { namespace posix_time { class ptime; class time_duration; } };
namespace attic
{
    class a_node;
    class a_attribute;
    class a_document;
    class a_parse_result;
    class a_text;

    // класс атрибута в структуре XML
    class a_attribute : public pugi::xml_attribute
    {
    public:
        typedef pugi::xml_attribute PARENT;
        // конструктор
        a_attribute(const pugi::xml_attribute& _par)
            : pugi::xml_attribute(_par)
        {};
        // чтение
        bool name_is( const std::string& if_aname ) const;
        
        int  as_def_int(int defvalue) const;
        bool as_def_bool( bool defvalue ) const;
        std::wstring as_def_wstring( const wchar_t* defvalue ) const;

        std::wstring as_wstring() const;
        time_t as_time() const;
        char as_char() const;
		wchar_t as_wchar() const;
        long as_long() const;
        unsigned long as_ulong() const;
        unsigned short as_ushort() const;
        UUID_t as_uuid() const;
        // перекрытие родительских методов из-за наличия там параметров по умолчанию
        const char* as_string() const { return PARENT::as_string(); }
        boost::string_ref as_string_ref() const { return PARENT::as_string(); }
        int as_int( int def = 0 ) const            { return PARENT::as_int(def);    }
        unsigned int as_uint() const  { return PARENT::as_uint();   }
        double as_double() const      { return PARENT::as_double(); }
        float as_float() const        { return PARENT::as_float();  }
        bool as_bool( bool def = false ) const { return PARENT::as_bool( def ); }
        // запись
        bool set_value( bool avalue );
        bool set_value( char avalue);
        bool set_value( int avalue);
        bool set_value( unsigned int avalue);
        bool set_value( unsigned long avalue);
#ifdef _WIN64
        bool set_value( size_t avalue);
#endif
        bool set_value( time_t avalue);
        bool set_value( double avalue);
        bool set_value( const char* avalue);
        bool set_value( const std::string& avalue );
        bool set_value( const wchar_t* avalue );
        bool set_value( const std::wstring& avalue );
        bool set_value( const boost::posix_time::ptime& avalue);
        bool set_value( const boost::posix_time::time_duration& avalue);
        bool set_value( const UUID_t& value );
    };

    // Range-based for loop support
    // Основное назначение структуры - хранить shared_ptr на копию строки
    template <typename It> class a_object_range : public pugi::xml_object_range<It>
    {
    public:
        a_object_range( const pugi::xml_object_range<It>& ran_, std::shared_ptr<std::string> hold_ )
            : pugi::xml_object_range<It>(ran_), hold(hold_)
        {}
        std::shared_ptr<std::string> hold;
    };

    // класс узла в структуре XML
    class a_node : public pugi::xml_node
    {
    public:
        typedef pugi::xml_node PARENT;
        a_node()
        {};
        a_node(const pugi::xml_node& _par)
            : pugi::xml_node(_par)
        {};
        bool name_is( const std::string& if_aname ) const;
        // очевидные упрощения
        bool set_name(const std::string& cname);
        a_node child(const char* cname) const;
        a_node child(const std::string& cname) const;
        pugi::xml_object_range<pugi::xml_node_iterator> children() const;
        a_object_range<pugi::xml_named_node_iterator> children(const std::string& name) const;
        a_node first_child() const;
        a_attribute append_attribute(const std::string& name);
        a_attribute attribute(const std::string& cname) const;
        pugi::xml_object_range<pugi::xml_attribute_iterator> attributes() const;
        a_node append_child( const std::string& cname );
        void   append_comment( const std::wstring& comment );
        a_node append_text( const std::string& text );
        a_node append_text( const std::wstring& text );
        a_node append_text( int text );
        a_node append_exact_text( boost::string_ref content ); ///< добавление нода с точным значением текста
        a_node append_copy(const a_node& proto);
        using PARENT::append_buffer;
        a_parse_result append_buffer( boost::string_ref contents );
        a_node insert_child_after(const std::string& cname, const a_node& node);
        a_node insert_move_before(const a_node& moved, const a_node& node);
        a_node next_sibling( const std::string& cname ) const;
        a_node next_sibling() const;
        bool remove_child( const char * cname );
        bool remove_child( const a_node& ch );
        bool remove_child( const std::string& cname );
        bool remove_attribute( const char * aname );
        bool remove_attribute( const std::string& aname );
        bool remove_attribute( const a_attribute& aatr );
        // получение элемента (если надо - с созданием)
        a_node ensure_child(const char* cname);
        a_node ensure_child(const std::string& cname);
        // получение атрибута (если надо - с созданием)
        a_attribute ensure_attribute(const char * aname);
        a_attribute ensure_attribute(const std::string& aname);
        // короткая запись атрибута
        template<class T>
        void brief_attribute(const char * aname, const T& avalue);
        template<class T>
        void brief_attribute(const std::string& aname, const T& avalue)
        {
            brief_attribute(aname.c_str(), avalue);
        }
        // Сериализация
        std::string to_str() const;
        // Красивая печать в строку
        std::wstring pretty_wstr() const;
        std::string pretty_str( unsigned depth = 0 ) const;
        // Текстовое содержание
        std::string text_as_string() const;
        unsigned text_as_uint( unsigned def = 0 ) const;
        int      text_as_int( int def = 0 ) const;
    private:
        // Родительская печать закрыта
        void print() const;
        // Текстовое содержание закрыто
        a_text text() const;
    };
    // класс результата загрузки
    class a_parse_result : public pugi::xml_parse_result
    {
    public:
        a_parse_result()
        {};
        a_parse_result(const pugi::xml_parse_result& _par )
            : pugi::xml_parse_result(_par)
        {};
    };
    // просто синоним в данном пространстве имен
    typedef pugi::xpath_node_set a_xpath_node_set;
	typedef pugi::xpath_node a_xpath_node;
    // класс документа в структуре XML
    class a_document : public pugi::xml_document
    {
    public:
        typedef pugi::xml_document PARENT;
        a_document()
            : pugi::xml_document()
        {};
        explicit a_document(const std::string& root_tag)
            : pugi::xml_document()
        {
            set_document_element(root_tag);
        }
        // Removes all nodes, leaving the empty document
        void reset()
        {
            PARENT::reset();
        }
        // Removes all nodes, then copies the entire contents of the specified document
        void reset(const a_document& proto)
        {
            PARENT::reset( proto );
        }
    public:
        // очевидные упрощения
        operator a_node()
        {
            return (xml_node)*this;
        };
        a_node document_element() const
        {
            return PARENT::document_element();
        }
        a_node set_document_element(const std::string& root_name)
        {
            if ( !document_element() )
            {
                return PARENT::append_child(root_name.c_str());
            }
            else
            {
                a_node de = document_element();
                de.set_name( root_name.c_str() );
                return de;
            }
        }

        a_node child(const std::string& cname) const
        {
            return child( cname.c_str() );
        };
        a_node child(const char* cname) const
        {
            return PARENT::child( cname );
        };
        a_parse_result load_wide( boost::wstring_ref contents);
        a_parse_result load_utf8(const void* contents, size_t size)
        {
            const char * cont = reinterpret_cast<const char*>(contents);
            return load_utf8( boost::string_ref(cont, size) );
        }
        a_parse_result load_utf8(const std::vector<char>& vch)
        {
            if ( vch.empty() )
                return load_utf8( boost::string_ref() );
            else
                return load_utf8( boost::string_ref(&*vch.begin(), vch.size()) );
        }
        a_parse_result load_utf8(boost::string_ref s)
        {
            const void * p = s.empty()? nullptr : &*s.begin();
            return PARENT::load_buffer( p, s.size(), pugi::parse_full | pugi::parse_trim_pcdata);
        }
        a_parse_result load_file( const std::wstring& fname )
        {
            return PARENT::load_file( fname.c_str(),pugi::parse_full);
        }
        bool save_file( const std::string& fname ) const
        {
            const unsigned int fmt = pugi::format_indent | pugi::format_write_bom | pugi::format_save_file_text;
            return PARENT::save_file( fname.c_str(),"    ",fmt);
        }
        bool save_file( const std::wstring& fname ) const
        {
            const unsigned int fmt = pugi::format_indent | pugi::format_write_bom | pugi::format_save_file_text;
            return PARENT::save_file( fname.c_str(), "    ", fmt );
        }
        bool load_strange_file( const std::wstring& fname );
        bool load_strange( boost::string_ref contents );

        // Сериализация
        std::string to_str() const
        {
            return a_node(*this).to_str();
        }
        // Красивая печать в строку
        std::wstring pretty_wstr() const
        {
            return a_node(*this).pretty_wstr();
        }

        std::string pretty_str() const
        {
            return a_node(*this).pretty_str();
        }
        
        void append_declaration( unsigned encoding_cp = 0 ); // по умолчанию UTF-8
        void append_comment( const std::wstring& comm );
    private:
        // -- сокрытие и запреты --
        // Non-copyable semantics
        a_document(const a_document&);
        const a_document& operator=(const a_document&);
        // скрыть неоднозначности и вынудить
        // использовать только document_element()
        using PARENT::root;
        using PARENT::first_child;
        using PARENT::append_child;
        // запрет базового метода
        using PARENT::load_buffer;
        //
        std::string read_whole_file( const std::wstring &fname );
        std::string read_encoding( boost::string_ref s_all );
        unsigned detect_cp( const std::string& encval );
    };

    // класс текстового содержимого в структуре XML
    class a_text : public pugi::xml_text
    {
    public:
        typedef pugi::xml_text PARENT;
        a_text()
            : pugi::xml_text()
        {};
        a_text(const pugi::xml_text& _par)
            : pugi::xml_text(_par)
        {};

        bool empty() const { return PARENT::empty(); }
        const char* get() const { return PARENT::get(); }
        const char* as_string(const char* def = "") const { return PARENT::as_string(def); }
        int as_int(int def = 0) const { return PARENT::as_int(def); }
        unsigned int as_uint(unsigned int def = 0) const { return PARENT::as_uint(def); }
        double as_double(double def = 0) const { return PARENT::as_double(def); }
        float as_float(float def = 0) const { return PARENT::as_float(def); }
        bool as_bool(bool def = false) const { return PARENT::as_bool(def); }
        long long as_llong(long long def = 0) const { return PARENT::as_llong(def); }
        unsigned long long as_ullong(unsigned long long def = 0) const { return PARENT::as_ullong(def); }

        // Get the data node (node_pcdata or node_cdata) for this object
        a_node data() const { return PARENT::data(); }
    };

    //
    // Реализация a_attribute
    //
    inline bool a_attribute::name_is( const std::string& if_aname ) const
    {
        return ( name() == if_aname );
    }
    inline std::wstring a_attribute::as_def_wstring( const wchar_t* defvalue ) const
    {
        if ( *name() )
            return as_wstring();
        else
            return defvalue;
    }
    inline int a_attribute::as_def_int(int defvalue) const
    {
        if ( *name() )
            return as_int();
        else
            return defvalue;
    }
    inline bool a_attribute::as_def_bool(bool defvalue) const
    {
        if ( *name() )
            return as_bool();
        else
            return defvalue;
    }
    inline char a_attribute::as_char() const
    {
        return *value();
    }
    inline long a_attribute::as_long() const
    {
#ifndef LINUX
        BOOST_STATIC_ASSERT(sizeof(int) >= sizeof(long));
#endif // !LINUX
        return as_int();
    }
    inline unsigned long a_attribute::as_ulong() const
    {
#ifndef LINUX
        BOOST_STATIC_ASSERT(sizeof(unsigned int)>=sizeof(unsigned long));
#endif // !LINUX
        return as_uint();
    }
    inline unsigned short a_attribute::as_ushort() const
    {
#ifndef LINUX
        BOOST_STATIC_ASSERT(sizeof(unsigned int)>=sizeof(unsigned short));
#endif // !LINUX
        return static_cast<unsigned short>(as_uint());
    }

    inline bool a_attribute::set_value( const std::string& avalue )
    {
        return set_value( avalue.empty()? "" : avalue.c_str() );
    }
    inline bool a_attribute::set_value(const char* avalue)
    {
        return PARENT::set_value( avalue );
    }
    inline bool a_attribute::set_value(char avalue)
    {
        char temp[2]={avalue,0};
        return PARENT::set_value( temp );
    }
    inline bool a_attribute::set_value(int avalue)
    {
        return PARENT::set_value( avalue );
    }
    inline bool a_attribute::set_value(unsigned int avalue)
    {
        return PARENT::set_value( avalue );
    }
    inline bool a_attribute::set_value(unsigned long avalue)
    {
#ifndef LINUX
        BOOST_STATIC_ASSERT(sizeof(unsigned int)>=sizeof(unsigned long));
#endif // !LINUX
        return PARENT::set_value( static_cast<unsigned int>(avalue) );
    }
    inline bool a_attribute::set_value(double avalue)
    {
        return PARENT::set_value( avalue, -1 );
    }
    inline bool a_attribute::set_value( bool avalue )
    {
        return set_value( avalue? "Y":"" );
    }
    //
    // Реализация a_node
    //
    inline bool a_node::name_is( const std::string& if_aname ) const
    {
        return ( name() == if_aname );
    }
    // очевидные упрощения
    inline bool a_node::set_name(const std::string& cname)
    {
        return PARENT::set_name( cname.c_str() );
    }
    inline a_node a_node::child(const std::string& cname) const
    {
        return child( cname.c_str() );
    };
    inline pugi::xml_object_range<pugi::xml_node_iterator> a_node::children() const
    {
        return PARENT::children();
    }
    inline a_object_range<pugi::xml_named_node_iterator> a_node::children(const std::string& cname) const
    {
        // сначала копия параметра, причем именно shared для идентичности адреса
        std::shared_ptr<std::string> temp = std::make_shared<std::string>( cname );
        // затем строим диапазон, в котором сохраним копию т.к. она необходима для работы диапазона
        a_object_range<pugi::xml_named_node_iterator> res( PARENT::children( temp->c_str() ), temp );
        return res;
    }
    inline a_node a_node::child(const char * cname) const
    {
        return PARENT::child( cname );
    };
    inline a_node a_node::first_child() const
    {
        return PARENT::first_child();
    };
    inline a_attribute a_node::attribute(const std::string& cname) const
    {
        return PARENT::attribute( cname.c_str() );
    };
    inline a_attribute a_node::append_attribute(const std::string& cname)
    {
        return PARENT::append_attribute( cname.c_str() );
    };
    inline pugi::xml_object_range<pugi::xml_attribute_iterator> a_node::attributes() const
    {
        return PARENT::attributes();
    }
    inline a_node a_node::append_child( const std::string& cname )
    {
        return PARENT::append_child(cname.c_str());
    }
    inline a_node a_node::append_copy(const a_node& proto)
    {
        return PARENT::append_copy(proto);
    }
    inline a_node a_node::insert_child_after( const std::string& cname, const a_node& node )
    {
        return PARENT::insert_child_after( cname.c_str(), node );
    }
    inline a_node a_node::insert_move_before(const a_node& moved, const a_node& node)
    {
        return PARENT::insert_move_before( moved, node );
    }
    inline a_node a_node::next_sibling( const std::string& cname ) const
    {
        return PARENT::next_sibling(cname.c_str());
    }
    inline a_node a_node::next_sibling() const
    {
        return PARENT::next_sibling();
    }
    inline bool a_node::remove_child( const char * cname )
    {
        return PARENT::remove_child(cname);
    }
    inline bool a_node::remove_child( const a_node& ch )
    {
        return PARENT::remove_child(ch);
    }
    inline bool a_node::remove_child( const std::string& cname )
    {
        return PARENT::remove_child(cname.c_str());
    }
    inline bool a_node::remove_attribute( const char * aname )
    {
        return PARENT::remove_attribute(aname);
    }
    inline bool a_node::remove_attribute( const std::string& aname )
    {
        return PARENT::remove_attribute(aname.c_str());
    }
    inline bool a_node::remove_attribute( const a_attribute& aatr )
    {
        return PARENT::remove_attribute(aatr);
    }
    // получение элемента (если надо - с созданием)
    inline a_node a_node::ensure_child(const char* cname)
    {
        a_node n = child( cname );
        if ( !n )
            n = append_child( cname );
        return n;
    };
    inline a_node a_node::ensure_child(const std::string& cname)
    {
        return ensure_child( cname.c_str() );
    };
    // получение атрибута (если надо - с созданием)
    inline a_attribute a_node::ensure_attribute(const char * aname)
    {
        a_attribute a = attribute(aname);
        if ( !a )
            a = append_attribute(aname);
        return a;
    }
    inline a_attribute a_node::ensure_attribute(const std::string& aname)
    {
        return ensure_attribute( aname.c_str() );
    }
    // короткая запись атрибута
    typedef char * ptr_char;
    template<> 
    inline void a_node::brief_attribute(const char * aname, const ptr_char& avalue)
    {
        if ( !avalue || !*avalue )
            remove_attribute( aname );
        else
            ensure_attribute( aname ).set_value(avalue);
    }
    template<> 
    inline void a_node::brief_attribute(const char * aname, const std::string& avalue )
    {
        if ( avalue.empty() )
            remove_attribute( aname );
        else
            ensure_attribute( aname ).set_value(avalue);
    }
    typedef wchar_t * ptr_wchar_t;
    template<> 
    inline void a_node::brief_attribute(const char * aname, const ptr_wchar_t& avalue )
    {
        if ( !avalue || !*avalue )
            remove_attribute( aname );
        else
            ensure_attribute( aname ).set_value(avalue);
    }
    template<> 
    inline void a_node::brief_attribute(const char * aname, const std::wstring& avalue)
    {
        if ( avalue.empty() )
            remove_attribute( aname );
        else
            ensure_attribute( aname ).set_value(avalue);
    }
    template<class T> 
    void a_node::brief_attribute(const char * aname, const T& avalue)
    {
#if LINUX
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Waddress"
#pragma GCC diagnostic ignored "-Wnonnull-compare"
#endif
        if ( avalue )
            ensure_attribute( aname ).set_value( avalue );
        else
            remove_attribute( aname );
#if LINUX
#pragma GCC diagnostic pop
#endif
    }
    template<>
    void a_node::brief_attribute(const char * aname, const boost::posix_time::ptime& avalue);
    template<>
    void a_node::brief_attribute(const char * aname, const time_t& avalue);

    // быстрое определение XML-подобия (только первый символ)
    inline bool XmlStart( char front )
    {
        return (front == '<');
    }
    inline bool XmlFinish( char back )
    {
        return (back == '>');
    }

    // быстрое определение XML-подобия (только первый и последний символы)
    bool Xmllike( const std::vector<char>& buf );

    // проверка идентичности
    bool identical( const attic::a_attribute& lhs, const attic::a_attribute& rhs );
    bool identical( const attic::a_node&      lhs, const attic::a_node&      rhs );
    // упорядочение
    bool isLess( const attic::a_attribute& lhs, const attic::a_attribute& rhs );
    bool isLess( const attic::a_node&      lhs, const attic::a_node&      rhs );
    // подробное сравнение ( res<0, res=0, res>0 означает lhs<rhs, lhs==rhs, lhs>rhs соответственно )
    int accurateCompare( const attic::a_attribute& lhs, const attic::a_attribute& rhs );
    int accurateCompare( const attic::a_node&      lhs, const attic::a_node&      rhs );
};

// шаблонный ввод/вывод
// упрощает вывод класса в узел XML; ввод класса из узла XML
template<class Q>
attic::a_node operator <<(attic::a_node node, const Q& val);
template<class Q> 
const attic::a_node operator >>(const attic::a_node area, Q& val);

// свойства описывают имена узлов и атрибутов для классов
template<class Z>
struct xml_traits
{	
    static const char * node_name() { return Z::tag(); }
};

//
// частичная специализация для распространенных контейнеров
// ... Векторы ...
//
template<class QQ>
const attic::a_node operator >>(const attic::a_node& xnode, std::vector<QQ>& vec)
{
    vec.clear();
    const char * NN=xml_traits<QQ>::node_name();
    for( attic::a_node it=xnode.child(NN); it; it = it.next_sibling(NN) )
    {
        QQ temp;
        it >> temp;
        vec.push_back(temp);
    }
    return xnode;
}

template<typename Qq>
attic::a_node operator <<(attic::a_node xnode, const std::vector<Qq>& vec)
{
    // очистка от таких элементов
    const char * NN=xml_traits<Qq>::node_name();
    for( attic::a_node prev = xnode.child(NN); prev; prev = xnode.child(NN) )
        xnode.remove_child(prev);

    for( typename std::vector<Qq>::const_iterator it= vec.begin(); it!=vec.end(); ++it )
    {
        attic::a_node ch = xnode.append_child( NN );
        ch << *it;
    }
    return xnode;
}

// ... Отображения ...
template<class Qval>
const attic::a_node operator >>(const attic::a_node& xnode, std::map<int,Qval>& kv )
{
    kv.clear();
    const char * NN=xml_traits<Qval>::node_name();
    for( attic::a_node it=xnode.child( NN ); it; it = it.next_sibling( NN ) )
    {
        int k = it.attribute("key").as_int();
        it >> kv[k];
    }
    return xnode;
}

template<typename Qval>
attic::a_node operator <<(attic::a_node xnode, const std::map<int,Qval>& kv)
{
    // очистка от таких элементов
    const char * NN=xml_traits<Qval>::node_name();
    for( attic::a_node prev = xnode.child( NN ); prev; prev = xnode.child( NN ) )
        xnode.remove_child(prev);
    // добавление таких элементов
    for( typename std::map<int,Qval>::const_iterator it= kv.begin(); it!=kv.end(); ++it )
    {
        attic::a_node ch = xnode.append_child( NN );
        ch.brief_attribute("key",it->first);
        ch << it->second;
    }
    return xnode;
}

// forward declaration
#include "../helpful/Badge_fwd.h"

namespace attic
{
    template<>
    void a_node::brief_attribute<EsrKit>(const char * aname, const EsrKit& avalue);
    template<>
    void a_node::brief_attribute<BadgeU>(const char * aname, const BadgeU& avalue);
    template<>
    void a_node::brief_attribute<BadgeE>(const char * aname, const BadgeE& avalue);
};

// Код для контроля использования оперативной памяти attic
namespace attic_memory
{
    void Initialize();
    void SetInf( const char * _filename, const int _fileline );
    void ClearInf();
};

#endif // _ATTIC_H_
