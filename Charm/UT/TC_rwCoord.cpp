#include "stdafx.h"
#include "TC_rwCoord.h"
#include "../helpful/rwInterval.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_rwCoord );

void TC_rwCoord::setUp()
{}

void TC_rwCoord::tearDown()
{
    rwCoord::forget_irregular();
}

void TC_rwCoord::simple_empty()
{
    rwCoord x;
    CPPUNIT_ASSERT(x.empty()==true);
    CPPUNIT_ASSERT(x.str()=="");
    rwCoord y("");
    CPPUNIT_ASSERT( y.empty() );
}

void TC_rwCoord::from_text()
{
    CPPUNIT_ASSERT( rwCoord("").empty() );
    CPPUNIT_ASSERT( rwCoord("123~1234") == rwCoord(123,1234) );
    CPPUNIT_ASSERT( rwCoord("0~45") == rwCoord(0,45) );
    CPPUNIT_ASSERT( rwCoord("765~0") == rwCoord(765,0) );
    CPPUNIT_ASSERT( rwCoord("765~") == rwCoord(765,0) );
    CPPUNIT_ASSERT( rwCoord("765") == rwCoord(765,0) );
    CPPUNIT_ASSERT( rwCoord("0~0") == rwCoord(0,0) );
    CPPUNIT_ASSERT( rwCoord("0") == rwCoord(0,0) );
    CPPUNIT_ASSERT( rwCoord("123~1234").str() == "123~1234" );
    CPPUNIT_ASSERT( rwCoord("0~45").str() == "0~45" );
    CPPUNIT_ASSERT( rwCoord("765~0").str() == "765~" );
    CPPUNIT_ASSERT( rwCoord("765~").str() == "765~" );
    CPPUNIT_ASSERT( rwCoord("765").str() == "765~" );
    CPPUNIT_ASSERT( rwCoord("0~0").str() == "0~" );
    CPPUNIT_ASSERT( rwCoord("0").str() == "0~" );
}

void TC_rwCoord::exactKm()
{
    rwCoord x(74, 0);
    CPPUNIT_ASSERT(x.empty()==false);
    CPPUNIT_ASSERT(x.str()=="74~");
}

void TC_rwCoord::Km_and_tiny()
{
    rwCoord x(72, 13);
    CPPUNIT_ASSERT(x.empty()==false);
    CPPUNIT_ASSERT(x.str()=="72~13");
}

void TC_rwCoord::exactPiquet()
{
    rwCoord x(52, 300);
    CPPUNIT_ASSERT(x.empty()==false);
    CPPUNIT_ASSERT(x.str()=="52~300");
}

void TC_rwCoord::piquet_and_tiny()
{
    rwCoord x(52, 2307);
    CPPUNIT_ASSERT(x.empty()==false);
    CPPUNIT_ASSERT(x.str()=="52~2307");
}

void TC_rwCoord::using_flat_meter()
{
    rwCoord x(123,777);
    CPPUNIT_ASSERT(x.empty()==false);
    CPPUNIT_ASSERT(x.str()=="123~777");
}

void TC_rwCoord::equals()
{
    rwCoord t("123~1234");
    rwCoord x(123,1234);
    rwCoord y(124,234);
    rwCoord z(124,234);
    CPPUNIT_ASSERT(t==x);
    CPPUNIT_ASSERT(x!=y);
    CPPUNIT_ASSERT(y==z);
}

void TC_rwCoord::compare_normal()
{
    rwCoord x(122,705);
    rwCoord y(123,305);
    rwCoord z(125,405);
    CPPUNIT_ASSERT(x==x);
    CPPUNIT_ASSERT(x!=y);
    CPPUNIT_ASSERT(calc_extent(x,x)==0);
    CPPUNIT_ASSERT(x<y);
    CPPUNIT_ASSERT(calc_extent(x,y)==600);
    CPPUNIT_ASSERT(calc_extent(y,x)==600);
    CPPUNIT_ASSERT(z>y);
    CPPUNIT_ASSERT(calc_extent(y,z)==2100);
    CPPUNIT_ASSERT(calc_extent(z,y)==2100);
}

void TC_rwCoord::compare_strange()
{
    rwCoord x(123,1703);
    rwCoord y(124, 0);
    rwCoord z(124, 703);
    CPPUNIT_ASSERT(x!=z);
    CPPUNIT_ASSERT(x<y);
    CPPUNIT_ASSERT(x<z);
    CPPUNIT_ASSERT(calc_extent(x,y)>=0);
    CPPUNIT_ASSERT(calc_extent(y,z)==703);
    CPPUNIT_ASSERT(calc_extent(x,z)>=703);
    CPPUNIT_ASSERT(calc_extent(x,z)<=calc_extent(x,y)+calc_extent(y,z));
}

void TC_rwCoord::set_irregular()
{
    rwCoord a( 41, 850 );
    rwCoord b( 42, 0 );
    // установка и удлинение
    CPPUNIT_ASSERT( rwCoord::set_irregular( a, b, 0  ) ); // т.е. 41 км всего 850 м
    CPPUNIT_ASSERT( rwCoord::set_irregular( a, b, 20 ) ); // удлинить можно - теперь 870
    CPPUNIT_ASSERT( rwCoord::set_irregular( a, b, 50 ) ); // удлинить можно - теперь 900
    // установка и укорачивание
    CPPUNIT_ASSERT( !rwCoord::set_irregular( a, b, 49 ) ); // укорачивать нельзя
    // ошибочные случаи
    rwCoord c(1771, 1);
    rwCoord d(1773, 1);
    CPPUNIT_ASSERT( !rwCoord::set_irregular( c, d, 2000 ) ); // несмежные километры
    rwCoord e(1774, 900);
    CPPUNIT_ASSERT( !rwCoord::set_irregular( d, e, 800 ) ); // длина 800 < чем координата 900
}

void TC_rwCoord::calc_irregular()
{
    rwCoord m41( 41, 900 );
    rwCoord m42( 42, 0 );
    rwCoord::set_irregular( m41, m42, 0    ); // т.е. 41-й км всего 900 м
    rwCoord m44( 44, 0 );
    rwCoord m45( 45, 0 );
    rwCoord::set_irregular( m44, m45, 1500 ); // зато 44-й км целых 1500 м
    // вычисления
    rwCoord x( 41, 100 );
    rwCoord y( 43, 200 );
    rwCoord z( 45, 100 );
    CPPUNIT_ASSERT( calc_extent( x, x ) == 0 );
    CPPUNIT_ASSERT( calc_extent( x, y ) == 2000 );
    CPPUNIT_ASSERT( calc_extent( y, z ) == 2400 );
    CPPUNIT_ASSERT( calc_extent( z, z ) == 0 );
    // корректируем и снова вычисляем
    rwCoord::set_irregular( m41, m42, 50 );
    CPPUNIT_ASSERT( calc_extent( x, y ) == 2050);
}

void TC_rwCoord::offset_w_irregular()
{
    rwCoord m44( 44, 0 );
    rwCoord m45( 45, 0 );
    rwCoord::set_irregular( m44, m45, 1500 ); // 44-й км составляет 1500 м
    // вычисления с неправильным километром
    rwCoord x( 43, 900 );
    x.offset( 100+1500+745 );
    CPPUNIT_ASSERT( x == rwCoord( 45, 745 ) );
    x.offset_backward( 100+1500+745 );
    CPPUNIT_ASSERT( x == rwCoord( 43, 900 ) );
    // вычисления на правильных километрах
    rwCoord y( 75, 900 );
    y.offset( 100+1500+745 );
    CPPUNIT_ASSERT( y == rwCoord( 78, 245 ) );
    y.offset_backward( 100+1500+745 );
    CPPUNIT_ASSERT( y == rwCoord( 75, 900 ) );
}

void TC_rwCoord::ceil_floor()
{
    // обычный случай
    rwCoord a( 234, 56 );
    CPPUNIT_ASSERT( a.floor() == rwCoord( 234, 0 ) );
    CPPUNIT_ASSERT( a.ceil()  == rwCoord( 235, 0 ) );
    // граничные случаи
    rwCoord left( 75, 0 );
    CPPUNIT_ASSERT( left.floor() == rwCoord( 75, 0 ) );
    CPPUNIT_ASSERT( left.ceil()  == rwCoord( 75, 0 ) );
    rwCoord right( 75, 1000 );
    CPPUNIT_ASSERT( right.floor() == rwCoord( 76, 0 ) );
    CPPUNIT_ASSERT( right.ceil()  == rwCoord( 76, 0 ) );

    // проверка при неправильном километре
    rwCoord::set_irregular( rwCoord( 44, 0 ), rwCoord( 45, 0 ), 840 ); // 44-й км составляет 840 м

    rwCoord x( 44, 0 );
    CPPUNIT_ASSERT( x.floor() == rwCoord( 44, 0 ) );
    CPPUNIT_ASSERT( x.ceil()  == rwCoord( 44, 0 ) );
    rwCoord y( 44, 333 );
    CPPUNIT_ASSERT( y.floor() == rwCoord( 44, 0 ) );
    CPPUNIT_ASSERT( y.ceil()  == rwCoord( 45, 0 ) );
    rwCoord z( 44, 840 );
    CPPUNIT_ASSERT( z.floor() == rwCoord( 45, 0 ) );
    CPPUNIT_ASSERT( z.ceil()  == rwCoord( 45, 0 ) );
}
