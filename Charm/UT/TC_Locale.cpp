#include "stdafx.h"
// проверить преобразование кодировок

#include "../helpful/RT_Locale.h"
#include "TC_Locale.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Locale );

/*
проверить преобразование кодировок
*/

void TC_Locale::checkRu()
{
    std::wstring mix( L"оБыЧнЫй" );
    std::wstring lo = RT_tolowerLocale( mix );
    std::wstring up = RT_toupperLocale( mix );
    CPPUNIT_ASSERT( L"обычный" == lo );
    CPPUNIT_ASSERT( L"ОБЫЧНЫЙ" == up );
}

void TC_Locale::checkLv()
{
    std::wstring mix( L"ATŠĶirībā" );
    std::wstring lo = RT_tolowerLocale( mix );
    std::wstring up = RT_toupperLocale( mix );
    bool eqL = ( L"atšķirībā" == lo );
    CPPUNIT_ASSERT( eqL && "Lv to lower" );
    bool eqU = ( L"ATŠĶIRĪBĀ" == up );
    CPPUNIT_ASSERT( eqU && "Lv to upper" );
}
