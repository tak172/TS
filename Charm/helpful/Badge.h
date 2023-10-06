#pragma once

#ifndef _BADGE_H_
#define _BADGE_H_

#include "../helpful/EsrKit.h"
#include "../helpful/Utf8.h"
#include "../helpful/RT_Macros.h"

namespace attic
{
    class a_attribute;
}

//
// класс бейджик - хранит строку и число
// 
// Используется, например, в качестве 
// индекса для хранилища ссылок на обьекты LOS_Base
template <class T>
class Badge_base
{
public:
    // типы данных
    typedef T NumType;
public:
    // конструкторы
    Badge_base() : m_str(), m_num() 
    {}
    Badge_base(const std::wstring& str, const NumType& num) : m_str(ToUtf8(str)), m_num(num)
    {}
    Badge_base(const wchar_t* pstr, const NumType& num) : m_str(ToUtf8(pstr)), m_num(num)
    {}
    static Badge_base fromU8(const char* u8str, const NumType& num)
    {
         return Badge_base(u8str,num);
    }
    explicit Badge_base(const attic::a_attribute& atr);

    // работа с потоком
    std::vector<char>::const_iterator from_area( std::vector<char>::const_iterator area_begin, 
                                                 std::vector<char>::const_iterator area_end );
    void to_area(std::vector<char>& area) const; // сохранение в поток
    // ввод-вывод строковый
    std::string to_string() const;
    std::wstring to_wstring() const;
	std::wstring to_brief_wstring( const EsrKit & ) const;
    // сравнения
    bool empty() const
    { 
        return ( m_str.empty() || m_num==NumType(0) ); 
    }
    bool operator<(const Badge_base& rhs) const
    { 
        return m_num<rhs.m_num 
            || (m_num==rhs.m_num && m_str<rhs.m_str);
    }
    bool operator==(const Badge_base& rhs) const
    { 
        return (  m_num==rhs.m_num 
               && m_str==rhs.m_str);
    }
    bool operator!=(const Badge_base& rhs) const
    { 
        return !( *this == rhs );
    }
    bool operator>(const Badge_base& rhs) const
    { 
        return (rhs < *this);
    }

    // доступ на чтение
    const std::wstring str() const { return FromUtf8(m_str); }
    const std::string& str_u8() const { return m_str; }
	const NumType& num() const { return m_num; }
    static const Badge_base lower_value(const NumType& num);
    static const Badge_base upper_value(const NumType& num);

	//получение модифицированной копии (вместо доступа на запись)
	Badge_base set_num( const NumType & _num ) const;
	Badge_base set_str ( const std::wstring& _str ) const;
    Badge_base set_u8str( const char*        _str ) const;
    Badge_base set_u8str( const std::string& _str ) const;

private:
    // хранящиеся данные
    std::string m_str;
    NumType m_num;
    // конструкторы из utf-8 (только для внутреннего пользования)
    Badge_base(const char* u8str,        const NumType& num) : m_str(u8str), m_num(num) {}
    Badge_base(const std::string& u8str, const NumType& num) : m_str(u8str), m_num(num) {}

#ifdef _MFC_VER
private:
    // закрыт и запрещен к использованию. Объявление сделано для обнаружения неявных преобразований.
    Badge_base(const CString& str, NumType num);
#endif
};


typedef Badge_base<unsigned> BadgeU;
typedef Badge_base<EsrKit>   BadgeE;


// чтение из потока
template<class T>
std::vector<char>::const_iterator
    Badge_base<T>::from_area( std::vector<char>::const_iterator area_begin, 
                              std::vector<char>::const_iterator area_end )
{
    if ( area_begin!=area_end )
    {
        std::vector<char>::const_iterator temp_it = find( area_begin, area_end, 0 );
        m_str = std::string(area_begin, temp_it);
        area_begin = temp_it;
        if ( area_begin != area_end )
        {
            ++area_begin;
            const ptrdiff_t SZNUM = sizeof(NumType);
            if ( std::distance(area_begin,area_end) >= SZNUM )
            {
                // число
                m_num = *reinterpret_cast<const NumType*>(&*area_begin);
                area_begin += SZNUM;
                // результат построен
                return area_begin;
            }
        }
    }
    // иначе очистка
    m_str.clear();
    m_num = NumType(0);
    return area_begin;
}

template<class T>
void Badge_base<T>::to_area(std::vector<char>& area) const // сохранение в поток
{
    std::string temp_str = str_u8();
    area.insert(area.end(),temp_str.begin(),temp_str.end());
    area.push_back(0);
    char const * pN = reinterpret_cast<char const*>(&m_num);
    area.insert(area.end(),pN,pN+sizeof(NumType));
}

template<>
inline Badge_base<unsigned>::Badge_base() 
: m_str(), m_num(0) // конкретизирован именно для инициализации нулем  
{}

template<class T>
inline std::wstring Badge_base<T>::to_wstring() const
{
    return FromUtf8( to_string() );
}

template<class T>
inline std::wstring Badge_base<T>::to_brief_wstring( const EsrKit & ecode ) const
{
	return num() == ecode ? str() : to_wstring();
}

template<class T>
const Badge_base<T> Badge_base<T>::lower_value(const T& num)
{
    return Badge_base<T>(std::wstring(),num);
}
template<class T>
const Badge_base<T> Badge_base<T>::upper_value(const T& num)
{
    return Badge_base<T>(std::wstring(5,wchar_t(-1)),num);
}

//получение модифицированной копии (вместо доступа на запись)
template<class T>
Badge_base<T> Badge_base<T>::set_num( const NumType & _num ) const
{
    return Badge_base<T>( this->m_str, _num );
}

template<class T>
Badge_base<T> Badge_base<T>::set_str ( const std::wstring& _str ) const
{
    return Badge_base<T>( _str, this->m_num );
}

template<class T>
Badge_base<T> Badge_base<T>::set_u8str( const char*        _str ) const
{
    return Badge_base<T>( _str, this->m_num );
}

template<class T>
Badge_base<T> Badge_base<T>::set_u8str( const std::string& _str ) const
{
    return Badge_base<T>( _str, this->m_num );
}

template<class T>
std::wostream& operator<<(std::wostream& os, const Badge_base<T>& r)
{
    os << r.to_wstring();
    return os;
}

namespace std
{
	template<class T>
	struct hash< Badge_base<T> >
	{
		size_t operator()(Badge_base<T> const& bb) const
		{
			size_t const h1(std::hash<std::string>()(bb.str_u8()));
			size_t const h2(std::hash<T>()(bb.num()));
			return h1 ^ (h2 << 1);
		}
	};
}

#endif //_BADGE_H_
