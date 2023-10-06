#include "stdafx.h"

#include "../helpful/EsrKit.h"
#include "../helpful/Utf8.h"

using namespace std;

EsrKit::EsrKit( int _a, int _b )
    : alfa( _a ), omega( _b )
{
    if ( alfa == EMPTY_VALUE && omega != EMPTY_VALUE )
        wipe_both();
    else
    {
        ordering();
        if ( check_overflow() )
            wipe_both();
    }
}

EsrKit::EsrKit( const std::string& str, char delim ) // конструируем код из строки
    : alfa( EMPTY_VALUE ), omega( EMPTY_VALUE )
{
    char * p;
    // извлекаем первое число
    alfa = (unsigned int)strtoul(str.c_str(),&p,10);
    // анализ продолжения
    if (*p==delim)
    {
        ; // читать нечего
    }
    else if ( *p==SPLIT_CHAR )
    {
        omega = (unsigned int)strtoul(p+1,&p,10);
    }
    else
    {
        // ошибка - неизвестный символ
    }
    // проверка окончания и переполнения, а также упорядочение
    if ( *p!=delim || check_overflow() )
        wipe_both();
    else 
        ordering();
}

EsrKit::EsrKit( const std::wstring& wstr ) // конструируем код из строки
    : alfa( EMPTY_VALUE ), omega( EMPTY_VALUE )
{
    (*this) = EsrKit( ToUtf8(wstr) );
}

EsrKit::EsrKit( const std::string& str, char sepa, bool cut6 ) // конструируем код из строки ESR5/ESR6 с разделителем
    : alfa( EMPTY_VALUE ), omega( EMPTY_VALUE )
{
    if ( !str.empty() )
    {
        size_t pos_sepa = str.find_first_of( sepa );
        // нет разделителя
        if ( string::npos == pos_sepa )
        {
            size_t pos_bad = str.find_first_not_of( "1234567890" );
            if ( string::npos == pos_bad )
                alfa = atoi( str.c_str() );
        }
        // единственный разделитель
        else if ( str.find_last_of( sepa ) == pos_sepa &&
                  string::npos == str.find_first_not_of("1234567890",pos_sepa+1)
                )
        {
            alfa = atoi( str.c_str() );
            omega = atoi( str.c_str() + pos_sepa + 1 );
            ordering();
        }
        if ( cut6 )
        {
            alfa /= 10;
            omega /= 10;
        }
        if ( check_overflow() )
            wipe_both();
    }
}

template<class T>
void out5(T * buf, unsigned k)
{
    // код ниже написан с прямым использование 5 (пятерки)
    buf[4] = static_cast<char>( '0'+(k%10) ); k/= 10;
    buf[3] = static_cast<char>( '0'+(k%10) ); k/= 10;
    buf[2] = static_cast<char>( '0'+(k%10) ); k/= 10;
    buf[1] = static_cast<char>( '0'+(k%10) ); k/= 10;
    buf[0] = static_cast<char>( '0'+(k%10) ); k/= 10;
}

size_t EsrKit::to_str(char * buf, size_t sz) const
{
#ifndef LINUX
    UNREFERENCED_PARAMETER(sz);
#endif // !LINUX
    ASSERT(sz>=SZ_TOSTR); // наибольший требуемый буфер
    if ( terminal() )
    {
        out5<char>(buf,alfa);
        buf[DIGITS] = 0;
        return DIGITS;
    }
    else if ( span() )
    {
        out5<char>(buf,alfa);
        buf[DIGITS] = SPLIT_CHAR;
        out5<char>(buf+DIGITS+1,omega);
        buf[DIGITS*2+1] = 0;
        return DIGITS*2+1;
    }
    else
    {
        *buf = 0;
        return 0;
    }
}
size_t EsrKit::to_wstr(wchar_t * buf, size_t sz) const
{
#ifndef LINUX
    UNREFERENCED_PARAMETER(sz);
#endif // !LINUX
    ASSERT(sz>=DIGITS*2+1 +1); // наибольший требуемый буфер
    if ( terminal() )
    {
        out5<wchar_t>(buf,alfa);
        buf[DIGITS] = 0;
        return DIGITS;
    }
    else if ( span() )
    {
        out5<wchar_t>(buf,alfa);
        buf[DIGITS] = SPLIT_CHAR;
        out5<wchar_t>(buf+DIGITS+1,omega);
        buf[DIGITS*2+1] = 0;
        return DIGITS*2+1;
    }
    else
    {
        *buf = 0;
        return 0;
    }
}

bool EsrKit::well_formed( const std::wstring& wstr ) // приемлем ли данный текст?
{
    return well_formed(ToUtf8(wstr));
}

bool EsrKit::vacuum( StationEsr statEsr )
{ 
    return ( statEsr == VACUUM_VALUE );
}

EsrKit EsrKit::replace(StationEsr before, StationEsr after) const
{
    if ( alfa != EMPTY_VALUE && alfa==before )
        return EsrKit( after, omega );
    else if ( omega != EMPTY_VALUE && omega==before )
        return EsrKit( alfa, after );
    else
        return *this;
}

EsrKit EsrKit::intersect(const EsrKit& one, const EsrKit& two)
{
    StationEsr x( two.occured( StationEsr( one.alfa  ) )? one.alfa  : EMPTY_VALUE );
    StationEsr y( two.occured( StationEsr( one.omega ) )? one.omega : EMPTY_VALUE );
    if ( EMPTY_VALUE==x )
        std::swap(x,y);
    return EsrKit(x,y);
}

EsrKit EsrKit::complement(const EsrKit& two) const
{
    StationEsr x( two.occured( StationEsr(alfa)  )? EMPTY_VALUE : alfa  );
    StationEsr y( two.occured( StationEsr(omega) )? EMPTY_VALUE : omega );
    if ( EMPTY_VALUE==x )
        std::swap(x,y);
    return EsrKit(x,y);
}

bool EsrKit::bogus() const
{
    bool a = (EMPTY_VALUE == alfa  ) || ( LOWER_VALUE <= alfa  &&  alfa <= UPPER_VALUE );
    bool o = (EMPTY_VALUE == omega ) || ( LOWER_VALUE <= omega && omega <= UPPER_VALUE );
    return !a || !o;
}

StationEsr EsrKit::getTerm() const /// код самой станции
{
    ASSERT( !span() );
    return StationEsr( terminal()? alfa : 0 );
}

long EsrKit::getTerm6() const ///< 6-значный код станции
{
    ASSERT( !span() );
    if ( terminal() )
        return alfa * 10 + ( check_digit(alfa) - '0' );
    else
        return 0;
}

char EsrKit::check_digit( unsigned value )
{
	ASSERT( LOWER_VALUE <= value && value <= UPPER_VALUE );
	// расчет при нумерации позиций 1..5
	unsigned sum = 0;
	for( int t = value, pos = 5; pos >= 1; --pos ) {
		sum += (t % 10) * pos;
		t /= 10;
	}
	char result;
	auto rest = sum % 11;
	if ( 10 != rest ) {
		result = static_cast<char>( rest + '0' );
	} else {
		// расчет при нумерации тех же позиций 3..7
		unsigned cosum = 0;
		for( int t = value, pos = 7; pos >= 3; --pos ) {
			cosum += (t % 10) * pos;
			t /= 10;
		}
		auto corest = cosum % 11;
		if ( 10 != corest )
			result = static_cast<char>( corest + '0' );
		else
			result = '0';
	}
	return result;
}

std::string EsrKit::to_string6() const
{
	if ( terminal() ) {
		std::string res;
		res.resize( DIGITS + 1 );
		out5<char>( &*res.begin(), alfa );
		res[ DIGITS ] = check_digit( alfa );
		return res;
	} else if ( span() ) {
		std::string res;
		res.resize( ( DIGITS + 1 ) * 2 + 1 );
		auto it = res.begin();
		out5<char>( &*it, alfa );
		it += DIGITS;
		*( it++ ) = check_digit( alfa );
		*( it++ ) = SPLIT_CHAR;
		out5<char>( &*it, omega );
		it += DIGITS;
		*( it++ ) = check_digit( omega );
		return res;
	} else {
		return std::string();
	}
}

std::wstring EsrKit::to_wstring6() const
{
	return FromUtf8( to_string6() );
}

std::string StationEsr::to_string() const
{
    if ( !empty() )
    {
        std::string res( DIGITS, '0' );
        out5<char>( &*res.begin(), m_terminal );
        return res;
    }
    else
        return std::string();
}
