#include "stdafx.h"

#include "TC_rwInterval.h"
#include "../helpful/rwInterval.h"
#include "../helpful/PicketingInfo.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_rwInterval );

void TC_rwInterval::create()
{
    {
        rwInterval    n;
        CPPUNIT_ASSERT(n.empty()==true);
        CPPUNIT_ASSERT(n.str_with_dot()=="");
        CPPUNIT_ASSERT(n.wstr()==L"");
        CPPUNIT_ASSERT(n.size()==0);
        CPPUNIT_ASSERT(n.begin()<=n.end());
    }
    {
        rwInterval k = rwInterval::from_ordinal(75, 0);
        CPPUNIT_ASSERT(k.empty()==false);
        CPPUNIT_ASSERT(k.str_with_dot()=="75");
        CPPUNIT_ASSERT(k.wstr()==L"75 км");
        CPPUNIT_ASSERT(k.exact_km());
        CPPUNIT_ASSERT(k.begin()<k.end());
    }
    {
        rwInterval p = rwInterval::from_ordinal(7890, 11);
        CPPUNIT_ASSERT(p.empty()==false);
        CPPUNIT_ASSERT(p.str_with_dot()=="7890.11");
        CPPUNIT_ASSERT(p.wstr()==L"7890 км ПК11");
        CPPUNIT_ASSERT(p.exact_pk());
        CPPUNIT_ASSERT(p.begin()<p.end());
    }
    {
        rwInterval s( rwCoord(123,777), rwCoord(122,555) );
        CPPUNIT_ASSERT(s.empty()==false);
        CPPUNIT_ASSERT(s.str_with_dot()=="");
        CPPUNIT_ASSERT(s.wstr()==L"");
        CPPUNIT_ASSERT(s.size()==1222);
        CPPUNIT_ASSERT(s.begin()<s.end());
    }
    {
        rwInterval s( rwCoord(123,700), rwCoord(123,600) );
        CPPUNIT_ASSERT(s.empty()==false);
        CPPUNIT_ASSERT(s.str_with_dot()=="124.7");
        CPPUNIT_ASSERT(s.wstr()==L"124 км ПК7");
        CPPUNIT_ASSERT(s.size()==100);
        CPPUNIT_ASSERT(s.begin()<s.end());
    }
    {
        rwInterval a = rwInterval::from_ordinal("336");
        CPPUNIT_ASSERT(a.empty()==false);
        CPPUNIT_ASSERT(a.str_with_dot()=="336");
        CPPUNIT_ASSERT(a.wstr()==L"336 км");
        CPPUNIT_ASSERT(a.exact_km());
        CPPUNIT_ASSERT(a.begin()<a.end());
    }
    {
        rwInterval b = rwInterval::from_ordinal("1362.12");
        CPPUNIT_ASSERT(b.empty()==false);
        CPPUNIT_ASSERT(b.str_with_dot()=="1362.12");
        CPPUNIT_ASSERT(b.wstr()==L"1362 км ПК12");
        CPPUNIT_ASSERT(b.exact_pk());
        CPPUNIT_ASSERT(b.begin()<b.end());
    }
    {
        // слишком велик номер пикета
        CPPUNIT_ASSERT(rwInterval::from_ordinal("1362.70").empty());
    }
}

void TC_rwInterval::operate()
{
    {
        rwInterval    a = rwInterval::from_ordinal("345.2");
        rwInterval    b = rwInterval::from_ordinal(345,2);
        rwInterval    c = rwInterval::from_ordinal(345,0);
        rwInterval    d = rwInterval::from_ordinal(345,7);
        rwInterval    e = rwInterval::from_ordinal(111,2);
        CPPUNIT_ASSERT(a==b);
        CPPUNIT_ASSERT( !(a==c) );
        CPPUNIT_ASSERT( !(a==d) );
        CPPUNIT_ASSERT( !(a==e) );
    }
    {
        rwInterval    a = rwInterval::from_ordinal("500.1");
        rwInterval    b = rwInterval::from_ordinal("500.10");
        rwInterval    ab = rwInterval::from_ordinal("500");
        rwInterval    x=aggregate(a,b);
        rwInterval    y=aggregate(a,ab);
        rwInterval    z=aggregate(ab,b);
        CPPUNIT_ASSERT(x==ab);
        CPPUNIT_ASSERT(y==ab);
        CPPUNIT_ASSERT(z==ab);
    }
    {
        rwInterval    a0 = rwInterval::from_ordinal("500");
        rwInterval    a1 = rwInterval::from_ordinal("501");
        rwInterval    a2 = rwInterval::from_ordinal("502");
        rwInterval    x = rwInterval::from_ordinal("111");
        rwInterval    y = rwInterval::from_ordinal("999");
        rwInterval    all = aggregate(a0,a2);
        CPPUNIT_ASSERT(all.lower(a1)==a0);
        CPPUNIT_ASSERT(all.upper(a1)==a2);
        CPPUNIT_ASSERT(all.lower(x)==all);
        CPPUNIT_ASSERT(all.upper(x)==all);
        CPPUNIT_ASSERT(all.lower(y)==all);
        CPPUNIT_ASSERT(all.upper(y)==all);

        CPPUNIT_ASSERT( intersect(all,a0) == a0 );
        CPPUNIT_ASSERT( intersect(all,a1) == a1 );
        CPPUNIT_ASSERT( intersect(all,a2) == a2 );
        CPPUNIT_ASSERT( intersect(a0,a1) == rwInterval() );
        CPPUNIT_ASSERT( intersect(x,y) == rwInterval() );
        CPPUNIT_ASSERT( intersect(y,x) == rwInterval() );
    }
}

// проверка преобразований из путейской системы в интервал и обратно
void TC_rwInterval::convert_ordinal()
{
    // рабочие переменные
    rwInterval area; 
    int km,pk;
    // ровно километр
    area = rwInterval::from_ordinal( 11, 0 );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 11 && pk == 0 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 11 && pk == 0 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 11 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 11 && pk == 10 );
    // ровно пикет
    area = aggregate( rwInterval::from_ordinal( 22, 7 ), rwInterval::from_ordinal( 22, 7 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 22 && pk == 7 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 22 && pk == 7 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 22 && pk == 7 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 22 && pk == 7 );
    // несколько пикетов внутри одного километра
    area = aggregate( rwInterval::from_ordinal( 33, 5 ), rwInterval::from_ordinal( 33, 4 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 33 && pk == 4 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 33 && pk == 5 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 33 && pk == 4 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 33 && pk == 5 );
    // пикет - километр с переходом через километровый столб
    area = aggregate( rwInterval::from_ordinal( 44, 2 ), rwInterval::from_ordinal( 47, 0 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 44 && pk == 2 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 47 && pk == 0 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 44 && pk == 2 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 47 && pk == 10 );
    // километр - пикет с переходом через километровый столб
    area = aggregate( rwInterval::from_ordinal( 55, 0 ), rwInterval::from_ordinal( 59, 3 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 55 && pk == 0 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 59 && pk == 3 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 55 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 59 && pk == 3 );
    // километр - километр с переходом через километровый столб
    area = aggregate( rwInterval::from_ordinal( 66, 0 ), rwInterval::from_ordinal( 68, 0 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 66 && pk == 0 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 68 && pk == 0 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 66 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 68 && pk == 10 );
    // упрощение - указаны пикеты, а результат в километрах
    area = aggregate( rwInterval::from_ordinal( 77, 1 ), rwInterval::from_ordinal( 78, 10 ) );
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 77 && pk == 0 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 78 && pk == 0 );
    CPPUNIT_ASSERT( area.begin_ordinal_mark(km,pk) && km == 77 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal_mark(  km,pk) && km == 78 && pk == 10 );
}

void TC_rwInterval::interval_from_coord()
{
    PicketingInfo pi1 = PicketingInfoFromStr( L"<Up  picketing1_val='0~1' picketing1_comm='Москва'/>");
    PicketingInfo pi2 = PicketingInfoFromStr( L"<Up  picketing1_val='3~' picketing1_comm='Москва'/>");
    std::wstring ruler = L"Москва";
    rwInterval area (pi1.GetCoord(ruler),pi2.GetCoord(ruler));
    int km,pk;
    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 1 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 3 && pk == 0 );      

    pi1 = PicketingInfoFromStr( L"<Up  picketing1_val='0~1' picketing1_comm='Москва'/>");
    pi2 = PicketingInfoFromStr( L"<Up  picketing1_val='3~1100' picketing1_comm='Москва'/>");
    area = rwInterval(pi1.GetCoord(ruler),pi2.GetCoord(ruler));


    CPPUNIT_ASSERT( area.begin_ordinal(km,pk) && km == 1 && pk == 1 );
    CPPUNIT_ASSERT( area.end_ordinal(  km,pk) && km == 4 && pk == 0 );      
}

PicketingInfo TC_rwInterval::PicketingInfoFromStr(const std::wstring& str)
{
    attic::a_document doc;
    doc.load_wide( str );
    PicketingInfo pi;
    pi.readFromXml( doc.document_element() );
    return pi;
}
