#include "stdafx.h"
#include <boost/date_time/posix_time/posix_time.hpp>
#include <boost/static_assert.hpp>
#include <boost/utility/string_ref.hpp>
#include "Attic.h"
#include "Time_Iso.h"


using namespace std;

bool attic::a_attribute::set_value( const std::wstring& avalue )
{
    return set_value( ToUtf8(avalue).c_str() );
}

bool attic::a_attribute::set_value( const UUID_t& value )
{
    return set_value( value.to_string() );
}

bool attic::a_attribute::set_value( const wchar_t* avalue )
{
    return set_value( ToUtf8(avalue).c_str() );
}
std::wstring attic::a_attribute::as_wstring() const
{
    return FromUtf8(value());
}
wchar_t attic::a_attribute::as_wchar() const
{
	return *FromUtf8(value()).c_str();
}
#ifdef _WIN64
bool attic::a_attribute::set_value(size_t avalue)
{
    char buf[32];
    _ui64toa_s(avalue, buf, sizeof(buf), 10);
    return set_value( buf );
}
#endif

time_t attic::a_attribute::as_time() const
{
    return time_from_iso( value() );
}

UUID_t attic::a_attribute::as_uuid() const
{
    return UUID_t( as_string() );
}

bool attic::a_attribute::set_value( time_t avalue)
{
    if ( avalue==0 )
        return set_value("");
    else
        return set_value( time_to_iso(avalue) );
}

bool attic::a_attribute::set_value( const boost::posix_time::ptime& avalue)
{
    if ( avalue.is_not_a_date_time() )
        return set_value("");
    else
        return set_value( to_iso_string( avalue ) + "Z" );
}

bool attic::a_attribute::set_value(const boost::posix_time::time_duration& avalue)
{
    return set_value( (unsigned)avalue.total_seconds() );
}

attic::a_parse_result attic::a_document::load_wide( boost::wstring_ref contents )
{
    std::string s=ToUtf8(contents);
    return pugi::xml_document::load_buffer( s.c_str(), s.size(), pugi::parse_full | pugi::parse_trim_pcdata );
}

bool attic::a_document::load_strange_file( const wstring& fname )
{
    string s_all = read_whole_file(fname);
    return load_strange( s_all );
}

bool attic::a_document::load_strange( boost::string_ref contents )
{
    attic::a_parse_result parse_res;
    std::string encval = read_encoding( contents );
    unsigned cp = detect_cp( encval );
    if ( CODEPAGE_UTF8 != cp )
    {
        // перекодировка
        wstring ws = FromOtherCP( contents, cp );
        string u8s = ToUtf8(ws);
        parse_res = load_utf8( u8s );
    }
    else
    {
        parse_res = load_utf8( contents );
    }
    if ( parse_res && CODEPAGE_UTF8 != cp )
    {
        // удалить явную кодировку
        for( a_node decl : root().children() )
        {
            if ( pugi::node_declaration == decl.type() )
            {
                decl.remove_attribute( "encoding" );
                break;
            }
        }
    }
    if ( parse_res )
        return true;
    else
        return false;
}

unsigned attic::a_document::detect_cp( const std::string& encval )
{
    if ( !encval.empty() )
    {
        struct CP_2_NAME
        {
            unsigned codepage;
            const char * iana_name;
        };

        // Выдержка из https://www.iana.org/assignments/character-sets/character-sets.xhtml
        const CP_2_NAME known[] = 
        {
            { CODEPAGE_UTF8, "UTF-8" },
            { 1251, "Windows-1251" },
            { 1251, "csWindows1251" },
            { 866,  "IBM866" },
            { 866,  "cp866" },
            { 866,  "866" },
            { 866,  "csIBM866" },
        };

        for( auto& line : known )
        {
#ifndef LINUX
            if (0 == _stricmp(line.iana_name, encval.c_str()))
#else
            if (0 == strcasecmp(line.iana_name, encval.c_str()))
#endif // !LINUX
                return line.codepage;
        }
    }
    return CODEPAGE_UTF8;
}

string attic::a_document::read_whole_file( const wstring &fname )
{
    string res;

    FILE* file;
#ifndef LINUX
    errno_t err = _wfopen_s(&file, fname.c_str(), L"rb");
#else
    file = fopen( ToUtf8(fname).c_str(), "rb" );
#endif // !LINUX

#ifndef LINUX
    if (0!=err || NULL==file)
        return res;
#else
    if (NULL == file)
        return res;
#endif // !LINUX

    if ( 0!=fseek(file,0,SEEK_END) )
    {
        fclose(file);
        return res;
    }
    // get file size (can result in I/O errors)
    long sz = ftell(file);
    if( sz<=4)
    {
        fclose(file);
        return res;
    }

    res.resize(sz);

    // read file in memory
    if ( 0!=fseek(file,0,SEEK_SET) )
    {
        fclose(file);
        res.clear();
        return res;
    }
    size_t read_size = fread(&*res.begin(), 1, sz, file);
    fclose(file);
    if (read_size != size_t(sz) )
    {
        res.clear();
        return res;
    }
    return res;
}

std::string attic::a_document::read_encoding( boost::string_ref s_all )
{
    using namespace boost;
    //<?xml version='1.0' encoding='Windows-1251' ?>
    string_ref header( s_all );
    header = header.substr( 0, 4*1024 );
    size_t pos_decl = header.find("?>");
    if ( string::npos != pos_decl )
    {
        // оставить только заголовок
        header = header.substr( 0, pos_decl ); 
        string_ref ENC( "encoding=" );
        size_t pos_enc = header.find( ENC );
        if ( string::npos != pos_enc )
        {
            // оставить только кодировку в разделителях + остаток заголовка
            header = header.substr( pos_enc + ENC.size() );
            if ( header.size() > 2 )
            {
                char sep_left = header.front(); // первый разделитель
                header.remove_prefix(1);
                size_t pos_right = header.find( sep_left );
                if ( string::npos != pos_right )
                {
                    header = header.substr(0, pos_right);
                    return header.to_string();
                }
            }
        }
    }
    return string();
}

void attic::a_node::append_comment( const std::wstring& comment )
{
    PARENT::append_child( pugi::node_comment ).set_value( ToUtf8( comment ).c_str() );
}

attic::a_node attic::a_node::append_text( const std::string& text )
{
    a_node res;
    auto last_ch = PARENT::last_child();
    if ( last_ch.type() == pugi::node_pcdata )
    {
        // добавление в конец существующего элемента
        res = last_ch;
        res.set_value( (res.value() + text).c_str() );
    }
    else
    {
        res = PARENT::append_child( pugi::node_pcdata );
        res.set_value( text.c_str() );
    }
    return res;
}

attic::a_node attic::a_node::append_text( const std::wstring& text )
{
    return append_text( ToUtf8( text ) );
}

attic::a_node attic::a_node::append_text( int val )
{
    return append_text( std::to_string( val ) );
}

attic::a_node attic::a_node::append_exact_text( boost::string_ref content )
{
    const char*  SEQ = "]]>"; // недопустимая последовательность (необходимо разделение на отдельные ноды)
    a_node res;
    for( auto pos = content.find(SEQ); content.npos != pos; pos = content.find(SEQ) )
    {
        res = PARENT::append_child( pugi::node_cdata );
        std::string frag( content.data(), pos+1 ); // захват первого символа запрещ. последовательности
        res.set_value( frag.c_str() );
        content.remove_prefix( pos+1 );
    }
    res = PARENT::append_child( pugi::node_cdata );
    res.set_value( content.to_string().c_str() );
    return res;
}

attic::a_parse_result attic::a_node::append_buffer( boost::string_ref contents )
{
    return PARENT::append_buffer( contents.data(), contents.size() );
}

template<>
void attic::a_node::brief_attribute(const char * aname, const boost::posix_time::ptime& avalue)
{
    if (avalue.is_not_a_date_time())
        remove_attribute( aname );
    else
        ensure_attribute( aname ).set_value(avalue);
}

std::string attic::a_node::text_as_string() const
{
    std::string res;
    if ( first_child().type() == pugi::node_cdata )
    {
        for( a_node n = first_child(); pugi::node_cdata == n.type(); n = n.next_sibling() )
        {
            res += reinterpret_cast<PARENT&>(n).text().as_string();
        }
    }
    else
        res = PARENT::text().as_string();
    return res;
}

unsigned attic::a_node::text_as_uint( unsigned def ) const
{
    unsigned res = def;
    try {
        std::string s = text_as_string();
        if ( !s.empty() )
            res = std::stoi( s );
        else
            res = def;
    } catch ( ... ) {
        res = def;
    }
    return res;
}

int attic::a_node::text_as_int( int def ) const
{
    int res = def;
    try {
        std::string s = text_as_string();
        if ( !s.empty() )
            res = std::stoi( s );
        else
            res = def;
    } catch ( ... ) {
        res = def;
    }
    return res;
}

template<>
void attic::a_node::brief_attribute(const char * aname, const time_t& avalue)
{
    if ( avalue==0 )
        remove_attribute( aname );
    else
        ensure_attribute( aname ).set_value( time_to_iso(avalue) );
}

std::string attic::a_node::to_str() const
{
    std::stringstream buffer;
    pugi::xml_node::print( buffer, "", pugi::format_raw | pugi::format_raw_1space );
    std::string s = buffer.str();
    return s;
}

std::wstring attic::a_node::pretty_wstr() const
{
    return FromUtf8( pretty_str() );
}

std::string attic::a_node::pretty_str( unsigned depth ) const
{
    std::stringstream buffer;
    const unsigned int FMT = pugi::format_indent | pugi::format_write_bom | pugi::format_save_file_text;
    pugi::xml_node::print(buffer,"  ", FMT, pugi::encoding_utf8, depth);
    return buffer.str();
}


void attic::a_document::append_declaration( unsigned encoding_cp /*= 0*/ )
{
    using namespace pugi;

    a_node d;
    for( d=root().first_child(); d && node_declaration != d.type(); d = d.next_sibling() )
        ; // nothing
    if ( node_declaration != d.type() )
        d = root().prepend_child(node_declaration);

    d.ensure_attribute( "version" ).set_value("1.0");
    const char * enc = "UTF-8";
    switch( encoding_cp )
    {
    case 1251:
        enc = "windows-1251";
        break;
    case CODEPAGE_UTF8:
        enc = "utf-8";
        break; 
    default:
        enc = "UTF-8"; // верхний регистр оставлен для совместимости со старым кодом
        break; 
    }
    d.ensure_attribute( "encoding" ).set_value( enc );
    d.ensure_attribute( "standalone" ).set_value("yes");
}

void attic::a_document::append_comment( const std::wstring& comm )
{
    root().append_child( pugi::node_comment ).set_value( ToUtf8( comm ).c_str() );
}

bool attic::Xmllike(const std::vector<char>& buf)
{
    if ( !buf.empty() )
    {
        // непробельный символ
        auto nosp = [](char ch){
            return !std::isspace(ch);
        };
        auto itFirst = std::find_if( buf.begin(), buf.end(), nosp );
        if ( buf.end() != itFirst && XmlStart( *itFirst ) )
        {
            auto itLast = std::find_if( buf.rbegin(), buf.rend(), nosp );
            return XmlFinish(*itLast);
        }
    }
    return false;
}

// идентичность атрибутов
bool attic::identical( const a_attribute& lhs, const a_attribute& rhs )
{
    int diff = accurateCompare( lhs, rhs );
    return ( diff == 0 );
}

// идентичность узлов (упорядоченное сравнение и атрибутов и потомков)
bool attic::identical( const a_node& lhs, const a_node& rhs )
{
    int diff = accurateCompare( lhs, rhs );
    return ( diff == 0 );
}

// упорядочение
bool attic::isLess( const a_attribute& lhs, const a_attribute& rhs )
{
    int diff = accurateCompare( lhs, rhs );
    return ( diff < 0 );
}

// упорядочение
bool attic::isLess( const a_node& lhs, const a_node& rhs )
{
    int diff = accurateCompare( lhs, rhs );
    return ( diff < 0 );
}

// базовое подробное сравнение двух строк
static int accCmp( boost::string_ref lhs, boost::string_ref rhs )
{
    return lhs.compare( rhs );
}
// подробное сравнение ( res<0, res=0, res>0 означает lhs<rhs, lhs==rhs, lhs>rhs соответственно )
int attic::accurateCompare( const attic::a_attribute& lhs, const attic::a_attribute& rhs )
{
    int diff = accCmp( lhs.name(), rhs.name() );
    if ( 0 == diff )
        diff = accCmp( lhs.value(), rhs.value() );
    return diff;
}

int attic::accurateCompare( const attic::a_node& lhs, const attic::a_node& rhs )
{
    if ( int diff = accCmp( lhs.name(), rhs.name() ) )
        return diff;
    if ( int diff = accCmp( lhs.value(), rhs.value() ) )
        return diff;

    for( a_attribute la = lhs.first_attribute(), ra = rhs.first_attribute();
        la || ra;
        la = la.next_attribute(), ra = ra.next_attribute()
        )
    {
        if ( int diff = accurateCompare( la, ra ) )
            return diff;
    }
    for( a_node ln = lhs.first_child(), rn = rhs.first_child();
        ln || rn;
        ln = ln.next_sibling(), rn = rn.next_sibling()
        )
    {
        if ( int diff = accurateCompare( ln, rn ) )
            return diff;
    }
    return 0;
}


#define _ATTIC_MEMORY_CHECK_    0 /* возможности контроля размещения памяти */

#if _ATTIC_MEMORY_CHECK_

/*
 * Блок проверки памяти, используемой attic
 */
#include <boost/thread/tss.hpp>
#include "../helpful/Log.h"

typedef std::pair<const char *, int> MCustomer;

struct MBlock
{
    size_t block_size;
    MBlock * next;
    MCustomer customer;
    char filler[ 32 - ( sizeof(size_t)+ sizeof(MBlock *) + sizeof(MCustomer) ) ];
};

class MList
{
public:
    MList() : top(nullptr), count(0) {}
    void push( MBlock* block );
    MBlock* pop();
    void take_away( MBlock* block );
    size_t size() const { return count; }
    size_t popular_block_size() const;
    void output_customer() const;
private:
    size_t  count;
    MBlock* top;
    // следующий размер после указанного
    size_t next_request( size_t request ) const;
    // количество блоков такого размера
    size_t number_match( size_t request ) const;
    // следующий заказчик после указанного
    MCustomer next_customer( MCustomer custom ) const;
    // общий размер блоков этого заказчика
    size_t customer_area( MCustomer custom ) const;

    static const size_t POPULAR = 10; // количество повторений для популярности
};

static MList free_list; // список свободных страниц
static MList used_list; // список занятых блоков
static size_t big_count = 1000;    // максимальное количество заказов всего
static size_t PAGESIZE = 0;
static boost::thread_specific_ptr< MCustomer > inf_customer; // текущий потребитель памяти
const MCustomer NOCUSTOM = make_pair( (const char*)nullptr, int(0) );

void attic_memory::SetInf( const char * _filename, const int _fileline )
{
    if ( !inf_customer.get() )
        inf_customer.reset( new MCustomer );
    inf_customer->first  = _filename;
    inf_customer->second = _fileline;
}

void attic_memory::ClearInf()
{
    if ( !inf_customer.get() )
        inf_customer.reset( new MCustomer );
    *inf_customer = NOCUSTOM;
}

static void* Checked_Alloc(size_t size)
{
    ASSERT( size != 0 );
    MBlock* result = nullptr;
    if ( size == PAGESIZE )
        result = free_list.pop();
    if ( !result )
    {
        // заказать новое
        void * p = new char[ size + sizeof(MBlock) ];
        result = reinterpret_cast<MBlock*>(p);
        result->block_size = size;
        result->next = nullptr;
    }
    if ( !inf_customer.get() )
        inf_customer.reset( new MCustomer( NOCUSTOM ) );
    result->customer = *inf_customer;

    used_list.push(result);
    if ( 0 == PAGESIZE )
        PAGESIZE = used_list.popular_block_size();

    if ( !result->customer.first || !result->customer.second )
        WriteLog(L"attic::MBlock: unknown allocation");
    if ( big_count < free_list.size() + used_list.size() )
    {
        big_count = free_list.size() + used_list.size();
        WriteLog( L"attic::MBlock: big = %d (alloc)", big_count );
        used_list.output_customer();
    }
    return (result+1);
}

static void Checked_Dealloc(void* _ptr)
{
    MBlock* am = reinterpret_cast<MBlock*>(_ptr);
    am -= 1;
    used_list.take_away( am );
    // добавить в стек страницу или просто освободить блок
    if ( PAGESIZE == am->block_size )
    {
        free_list.push( am );
    }
    else
    {
        delete [] reinterpret_cast< char * >( am );
    }
}

void attic_memory::Initialize()
{
    pugi::set_memory_management_functions( Checked_Alloc, Checked_Dealloc );
}

void MList::push(MBlock* block)
{
    block->next = top;
    top = block;
    ++count;
}

MBlock* MList::pop()
{
    MBlock* result = top;
    take_away( result );
    return result;
}

void MList::take_away(MBlock* block)
{
    if ( block )
    {
        if ( block == top )
        {
            top = top->next;
            --count;
            block->next = nullptr;
            return;
        }
        else
        {
            for( MBlock* ptr = top; ptr; ptr = ptr->next )
            {
                if ( block == ptr->next )
                {
                    ptr->next = ptr->next->next;
                    --count;
                    return;
                }
            }
            ASSERT( false && "not found removed block!" );
        }
    }
}

size_t MList::popular_block_size() const
{
    if ( size() > POPULAR )
    {
        for( size_t request = next_request(0); request != 0; request = next_request( request ) )
        {
            if ( number_match( request ) > POPULAR )
                return request;
        }
    }
    return 0;
}

void MList::output_customer() const
{
    for( MCustomer cust = next_customer(NOCUSTOM); NOCUSTOM != cust; cust = next_customer( cust ) )
    {
        WriteLog( L"attic::MBlock: area = %d (%s : %d)", customer_area(cust), cust.first, cust.second );
    }
}

// количество блоков такого размера
size_t MList::number_match( size_t request_size ) const
{
    size_t number = 0;
    for( const MBlock* ptr = top; ptr; ptr = ptr->next )
    {
        if ( ptr->block_size == request_size )
            ++number;
    }
    return number;
}

MCustomer MList::next_customer(MCustomer custom) const
{
    MCustomer up_custom = NOCUSTOM;
    for( const MBlock* ptr = top; ptr; ptr = ptr->next )
    {
        if ( ptr->customer > custom )
        {
            up_custom = (NOCUSTOM == up_custom)? ptr->customer : (std::min)( up_custom, ptr->customer );
        }
    }
    return up_custom;
}

size_t MList::customer_area(MCustomer custom) const
{
    size_t area = 0;
    for( const MBlock* ptr = top; ptr; ptr = ptr->next )
    {
        if ( ptr->customer == custom )
            area += ptr->block_size;
    }
    return area;
}

// следующий размер после указанного
size_t MList::next_request( size_t last_request ) const
{
    size_t up_request = 0;
    for( const MBlock* ptr = top; ptr; ptr = ptr->next )
    {
        if ( ptr->block_size > last_request)
        {
            up_request = (0 == up_request)? ptr->block_size : (std::min)( up_request, ptr->block_size );
        }
    }
    return up_request;
}

#else //_ATTIC_MEMORY_CHECK_ == 0

void attic_memory::Initialize() {}
void attic_memory::SetInf( const char * /*_filename*/, const int /*_fileline*/ ) {}
void attic_memory::ClearInf() {}

#endif //_ATTIC_MEMORY_CHECK_
