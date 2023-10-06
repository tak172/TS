#pragma once

#include <string>
#include <utility>

// Промежуточное представление ЕСР для станций
class StationEsr
{
public:
    StationEsr()
        : m_terminal(EMPTY_VALUE)
    {}
    explicit StationEsr( unsigned _terminal )
        : m_terminal( _terminal )
    {}
    operator unsigned () const
    {
        return m_terminal;
    }
    std::string to_string() const;
    bool empty() const
    {
        return EMPTY_VALUE == m_terminal;
    }
private:
    unsigned int m_terminal;
    static const size_t  DIGITS = 5; ///< количество знаков в коде ЕСР (контрольный не хранится)
    static const unsigned EMPTY_VALUE = 0;     // отсутсвие кода
};

/// Класс представления ЕСР-кода для станции или перегона в системе Charm.
/// Обеспечивает создание, сравнение и необходимый сервис.
class EsrKit
{
public:
    EsrKit();                 ///< конструктор по умолчанию
    explicit EsrKit( int a ); ///< конструктор по коду станции
    EsrKit( int a, int b );   ///< конструктор перегона по кодам ограничивающих станций
    explicit EsrKit( const std::string& str, char delim = 0 );   ///< конструктор по строке с разными разделителями
    explicit EsrKit( const std::wstring& wstr );                 ///< конструктор по строке
    EsrKit( const std::string& str, char sepa, bool cut6 );      ///< конструктор по строке ESR5/ESR6 с разделителем
    // конструирование через подмену одного кода
    EsrKit replace( StationEsr before, StationEsr after ) const;
    // выделение общей части из двух ЕСР
    static EsrKit intersect( const EsrKit& one, const EsrKit& two );
    // дополнение
    EsrKit complement(const EsrKit& two) const;
    // предикаты
    bool terminal() const; ///< это код станции ?
    bool span() const;     ///< это код перегона ?
    bool empty() const;    ///< это вообще не код ?
    bool filled() const;   ///< непустышка ?
    bool bogus() const;    ///< присутствуют фальшивые коды станций
    // сравнения
    bool operator ==( const EsrKit& other ) const;
    bool operator !=( const EsrKit& other ) const;
    bool operator <( const EsrKit& other ) const;
    // доступ
    StationEsr getTerm() const; ///< код самой станции
    long getTerm6() const;    ///< код станции с контрольной цифрой
    std::pair<StationEsr, StationEsr> getSpan() const; ///< коды концов перегона
    std::pair<EsrKit, EsrKit> split_span() const;  ///< коды концов перегона

    // превращение в текст
    // быстрые методы
    size_t to_str(char * buff, size_t sz) const;
    size_t to_wstr(wchar_t * buff, size_t sz) const;
    // удобные методы
    std::string to_string() const;
    std::wstring to_wstring() const;
	std::string to_string6() const;   ///< вывод с контрольной цифрой
	std::wstring to_wstring6() const; ///< вывод с контрольной цифрой

    static bool well_formed(const std::string& str); ///< приемлем ли данный текст?
    static bool well_formed(const std::wstring& str); ///< приемлем ли данный текст?
	static bool vacuum( StationEsr statEsr );
	bool occured( StationEsr stationEsr ) const;

private:
    bool check_overflow() const;
    void wipe_both();
    void ordering();
	static char check_digit( unsigned value );  ///< вычисление контрольной цифры

    unsigned alfa;   ///< меньший код ЕСР из 5 знаков
    unsigned omega;  ///< больший код ЕСР (5 зн.) для перегона или EMPTY_VALUE

    static const unsigned UPPER_VALUE = 99999; // наибольший допустимый код станции
    static const unsigned LOWER_VALUE = 1000;  // наименьший истинный код станции (если меньше, то это фиктивный)
    static const unsigned EMPTY_VALUE = 0;     // отсутсвие кода
    static const size_t  DIGITS = 5; ///< количество знаков в коде ЕСР (контрольный не хранится)
    static const char SPLIT_CHAR = ':';
	static const unsigned VACUUM_VALUE = 99999; //магический код, олицетворяющий выход в никуда

public: // раньше описать не удалось
    static const unsigned SZ_TOSTR = DIGITS + 1 + DIGITS + 1;
};

inline EsrKit::EsrKit()
    : alfa( EMPTY_VALUE ), omega( EMPTY_VALUE )
{
}

inline EsrKit::EsrKit( int _a )
    : alfa( _a ), omega( EMPTY_VALUE )
{
    if ( check_overflow() )
        wipe_both();
}

inline bool EsrKit::terminal() const /// это код станции ?
{
    return ( omega == EMPTY_VALUE && alfa != EMPTY_VALUE );
}

inline bool EsrKit::span() const     /// это код перегона ?
{
    return ( omega != EMPTY_VALUE && alfa != EMPTY_VALUE );
}

inline bool EsrKit::empty() const    /// это вообще не код ?
{
    return ( alfa == EMPTY_VALUE && omega == EMPTY_VALUE );
}

inline bool EsrKit::filled() const
{
    return !empty();
}

inline bool EsrKit::operator ==( const EsrKit& other ) const
{
    return ( alfa == other.alfa && omega == other.omega );
}

inline bool EsrKit::operator !=( const EsrKit& other ) const
{
    return !( *this == other );
}

inline bool EsrKit::operator <( const EsrKit& other ) const
{
    return ( alfa < other.alfa
             ||  ( alfa == other.alfa && omega < other.omega )
           );
}

inline std::string EsrKit::to_string() const
{
    char buff[SZ_TOSTR];
    to_str(buff,SZ_TOSTR);
    return std::string(buff);
}

inline std::wstring EsrKit::to_wstring() const
{
    wchar_t buff[SZ_TOSTR];
    to_wstr(buff,SZ_TOSTR);
    return std::wstring(buff);
}

inline std::pair<StationEsr, StationEsr> EsrKit::getSpan() const /// коды концов перегона
{
    return std::make_pair( StationEsr(alfa), StationEsr(omega) );
}

inline std::pair<EsrKit, EsrKit> EsrKit::split_span() const  /// коды концов перегона
{
    if ( span() )
        return std::make_pair( EsrKit(alfa), EsrKit(omega) );
    else
        return std::make_pair( EsrKit(), EsrKit() );
}

inline bool EsrKit::check_overflow() const
{
    return ( alfa > UPPER_VALUE || omega > UPPER_VALUE );
}

inline void EsrKit::wipe_both()
{
    alfa = EMPTY_VALUE;
    omega = EMPTY_VALUE;
}

inline void EsrKit::ordering()
{
    if ( alfa!=EMPTY_VALUE &&
        omega!=EMPTY_VALUE &&
        alfa>omega
        )
        std::swap(alfa,omega);
}

inline bool EsrKit::well_formed( const std::string& str ) // приемлем ли данный текст?
{
    const char EOS = 0;
    return !EsrKit(str,EOS).empty();
}

inline bool EsrKit::occured( StationEsr code ) const
{
    return ( EMPTY_VALUE != code && ( code == alfa || code == omega ) );
}

namespace std
{
    template<>
    struct hash< StationEsr >
    {
        size_t operator()( const StationEsr& sesr ) const
        {
            size_t const h1 ( hash<unsigned>()( sesr ) );
            return h1;
        }
    };

    template<>
    struct hash< EsrKit >
    {
        size_t operator()( const EsrKit& kit ) const
        {
            if ( kit.terminal() )
            {
                auto a = kit.getTerm();
                size_t const h1 ( hash<unsigned>()( a ) );
                return h1;
            }
            else
            {
                auto ab = kit.getSpan();
                size_t const h1 ( hash<unsigned>()( ab.first  ) );
                size_t const h2 ( hash<unsigned>()( ab.second ) );
                return h1 ^ (h2 << 1);
            }
        }
    };
}
