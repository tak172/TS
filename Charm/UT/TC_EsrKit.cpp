#include "stdafx.h"

#include "TC_EsrKit.h"
#include "../helpful/EsrKit.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_EsrKit );

/*
проверить класс EsrKit
*/

void TC_EsrKit::Numbers()
{
    EsrKit zero;
    CPPUNIT_ASSERT( zero.empty() );
	CPPUNIT_ASSERT( !zero.terminal() );
    CPPUNIT_ASSERT( !zero.span() );
    EsrKit city(123);
    CPPUNIT_ASSERT( !city.empty() );
    CPPUNIT_ASSERT( city.terminal() );
    CPPUNIT_ASSERT( !city.span() );
    CPPUNIT_ASSERT( city.getTerm()==123 );

    EsrKit street(777,333);
    CPPUNIT_ASSERT( !street.empty() );
    CPPUNIT_ASSERT( !street.terminal() );
    CPPUNIT_ASSERT( street.span() );
    CPPUNIT_ASSERT( street.getSpan().first ==333 );
    CPPUNIT_ASSERT( street.getSpan().second==777 );

    EsrKit mistake(0,333);
    CPPUNIT_ASSERT( mistake.empty() );
    CPPUNIT_ASSERT( !mistake.terminal() );
    CPPUNIT_ASSERT( !mistake.span() );
}

void TC_EsrKit::Strings()
{
    EsrKit novalue;
    CPPUNIT_ASSERT( novalue.to_string()=="" );
    EsrKit city(111);
    CPPUNIT_ASSERT( city.to_string()=="00111" );
    EsrKit trassa(222,111);
    CPPUNIT_ASSERT( trassa.to_string()=="00111:00222" );

    EsrKit hole("");
    CPPUNIT_ASSERT( hole.empty() );
    EsrKit village("98765");
    CPPUNIT_ASSERT( village.getTerm()==98765 );
    EsrKit road("4321:1234");
    CPPUNIT_ASSERT( road.getSpan().first==1234 );
    CPPUNIT_ASSERT( road.getSpan().second==4321 );
    EsrKit grimace(":99:55:55:");
    CPPUNIT_ASSERT( grimace.empty() );

    bool CUT_6 = true;
    bool ONLY5 = false;
    CPPUNIT_ASSERT( EsrKit("12345",'+',CUT_6).getTerm()==1234 );
    CPPUNIT_ASSERT( EsrKit("12345",'+',ONLY5).getTerm()==12345 );
    CPPUNIT_ASSERT( EsrKit("54321~1234",'~',CUT_6).getSpan()==std::make_pair(StationEsr(123),StationEsr(5432)) );
    CPPUNIT_ASSERT( EsrKit("54321~1234",'~',ONLY5).getSpan()==std::make_pair(StationEsr(1234),StationEsr(54321)) );
    CPPUNIT_ASSERT( EsrKit("99:55",'+',CUT_6).empty() );
    CPPUNIT_ASSERT( EsrKit("99-55",'+',ONLY5).empty() );
}


void TC_EsrKit::Strings6()
{
	// генерация строк с контрольной цифрой
	const bool CUT6 = true;

	// пример из Вики (Единая сетевая разметка)
	EsrKit one( "635607", '-', CUT6 ); 
	CPPUNIT_ASSERT( "635607" == one.to_string6() );

	EsrKit two( "197511", '-', CUT6 );
	CPPUNIT_ASSERT( "197511" == two.to_string6() );

	// примеры из документа Диалог (Рига)
	EsrKit ValkaLugagi( "090204-110802", '-', CUT6 );
	std::string s6 = ValkaLugagi.to_string6();
	CPPUNIT_ASSERT( s6 == "090204:110802" );

	EsrKit mi("111222.111307", '.', CUT6 );
	std::wstring w6 = mi.to_wstring6();
	CPPUNIT_ASSERT( w6 == L"111222:111307" );
}

void TC_EsrKit::Baddies()
{
    EsrKit big(12345689);
    CPPUNIT_ASSERT( big.empty() );
    CPPUNIT_ASSERT( !big.terminal() );
    CPPUNIT_ASSERT( !big.span() );
    EsrKit order(0,1234);
    CPPUNIT_ASSERT( order.empty() );
    CPPUNIT_ASSERT( !order.terminal() );
    CPPUNIT_ASSERT( !order.span() );
    EsrKit bigbig(1234567,9876543);
    CPPUNIT_ASSERT( bigbig.empty() );
    CPPUNIT_ASSERT( !bigbig.terminal() );
    CPPUNIT_ASSERT( !bigbig.span() );
    EsrKit negative(-3);
    CPPUNIT_ASSERT( negative.empty() );
    CPPUNIT_ASSERT( !negative.terminal() );
    CPPUNIT_ASSERT( !negative.span() );
    EsrKit strange(1234,0);
    CPPUNIT_ASSERT( !strange.empty() );
    CPPUNIT_ASSERT( strange.terminal() );
    CPPUNIT_ASSERT( !strange.span() );
}

void TC_EsrKit::compare()
{
    CPPUNIT_ASSERT( EsrKit()==EsrKit() );
    CPPUNIT_ASSERT( EsrKit()==EsrKit(0) );
    CPPUNIT_ASSERT( EsrKit()==EsrKit(0,0) );
    CPPUNIT_ASSERT( EsrKit()==EsrKit(0,3) );
    CPPUNIT_ASSERT( EsrKit(1)==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit(1)!=EsrKit(7) );
    CPPUNIT_ASSERT( EsrKit(2)< EsrKit(7) );
    CPPUNIT_ASSERT( EsrKit(3)==EsrKit(3,0) );
    CPPUNIT_ASSERT( EsrKit(2,4)!=EsrKit(2,9) );
    CPPUNIT_ASSERT( EsrKit(2,4)==EsrKit(4,2) );
    CPPUNIT_ASSERT( EsrKit(2,4)< EsrKit(8,7) );
    CPPUNIT_ASSERT( EsrKit(2,4)< EsrKit(2,7) );
    CPPUNIT_ASSERT( EsrKit(4,2)< EsrKit(4,7) );
}
void TC_EsrKit::replace()
{
    CPPUNIT_ASSERT( EsrKit(2,4).replace(StationEsr(2),StationEsr(7))==EsrKit(4,7) );
    CPPUNIT_ASSERT( EsrKit(2,4).replace(StationEsr(4),StationEsr(7))==EsrKit(2,7) );
    CPPUNIT_ASSERT( EsrKit(2,4).replace(StationEsr(5),StationEsr(7))==EsrKit(2,4) );
    CPPUNIT_ASSERT( EsrKit(3).replace(StationEsr(3),StationEsr(4))==EsrKit(4) );
    CPPUNIT_ASSERT( EsrKit(3).replace(StationEsr(4),StationEsr(7))==EsrKit(3) );
}

void TC_EsrKit::intersect()
{
    // выделение общей части из двух ЕСР
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(3),EsrKit(3))==EsrKit(3) );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(1),EsrKit(2))==EsrKit() );

    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(1),EsrKit(1,7))==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(2,7),EsrKit(7))==EsrKit(7) );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(1),EsrKit(2,3))==EsrKit() );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(4,5),EsrKit(6))==EsrKit() );

    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(2,4),EsrKit(2,4))==EsrKit(2,4) );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(1,2),EsrKit(3,4))==EsrKit() );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(2,4),EsrKit(4,7))==EsrKit(4) );
    CPPUNIT_ASSERT( EsrKit::intersect(EsrKit(2,4),EsrKit(5,2))==EsrKit(2) );
}

void TC_EsrKit::complement()
{
    // дополнение
    CPPUNIT_ASSERT( EsrKit(1).complement(EsrKit(2))==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit(1).complement(EsrKit(2,3))==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit(3).complement(EsrKit(3))==EsrKit() );
    CPPUNIT_ASSERT( EsrKit(3).complement(EsrKit(3,4))==EsrKit() );
    CPPUNIT_ASSERT( EsrKit(1,2).complement(EsrKit(1))==EsrKit(2) );
    CPPUNIT_ASSERT( EsrKit(1,2).complement(EsrKit(2))==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit(1,2).complement(EsrKit(1,3))==EsrKit(2) );
    CPPUNIT_ASSERT( EsrKit(1,2).complement(EsrKit(2,3))==EsrKit(1) );
    CPPUNIT_ASSERT( EsrKit(1,2).complement(EsrKit(3,4))==EsrKit(1,2) );
}

void TC_EsrKit::bogus()
{
    // наличие фиктивных кодов ЕСР
    CPPUNIT_ASSERT( !EsrKit().bogus() );
    CPPUNIT_ASSERT( EsrKit(1).bogus() );
    CPPUNIT_ASSERT( EsrKit(999).bogus() );
    CPPUNIT_ASSERT( !EsrKit(1000).bogus() );
    CPPUNIT_ASSERT( EsrKit(256,1256).bogus() );
    CPPUNIT_ASSERT( !EsrKit(1000,1256).bogus() );
}

void TC_EsrKit::term6()
{
    // Все коды с контрольными цифрами на https://sovetgt.org/tr4/
    CPPUNIT_ASSERT(  90204 == EsrKit( 9020).getTerm6() ); //Валка (рзд)
    CPPUNIT_ASSERT(  95104 == EsrKit( 9510).getTerm6() ); //Лигатне
    CPPUNIT_ASSERT( 110802 == EsrKit(11080).getTerm6() ); //Лугажи (эксп.)
    CPPUNIT_ASSERT(  95119 == EsrKit( 9511).getTerm6() ); //Сигулда
    CPPUNIT_ASSERT( 110817 == EsrKit(11081).getTerm6() ); //Лугажи
    CPPUNIT_ASSERT(  95123 == EsrKit( 9512).getTerm6() ); //ОП Силциемс
    CPPUNIT_ASSERT( 110821 == EsrKit(11082).getTerm6() ); //Сауле
    CPPUNIT_ASSERT(  95138 == EsrKit( 9513).getTerm6() ); //ОП Эгльупе
    CPPUNIT_ASSERT( 110836 == EsrKit(11083).getTerm6() ); //ОП Седа
    CPPUNIT_ASSERT(  95208 == EsrKit( 9520).getTerm6() ); //Инчукалнс
    CPPUNIT_ASSERT( 110906 == EsrKit(11090).getTerm6() ); //Стренчи
    CPPUNIT_ASSERT(  95301 == EsrKit( 9530).getTerm6() ); //Вангажи
    CPPUNIT_ASSERT( 110910 == EsrKit(11091).getTerm6() ); //Бренгули
    CPPUNIT_ASSERT(  95405 == EsrKit( 9540).getTerm6() ); //Криевупе
    CPPUNIT_ASSERT( 111006 == EsrKit(11100).getTerm6() ); //Валмиера
    CPPUNIT_ASSERT(  95509 == EsrKit( 9550).getTerm6() ); //Гаркалне
    CPPUNIT_ASSERT( 111010 == EsrKit(11101).getTerm6() ); //Бале
    CPPUNIT_ASSERT(  95513 == EsrKit( 9551).getTerm6() ); //ОП Балтэзерс
    CPPUNIT_ASSERT( 111107 == EsrKit(11110).getTerm6() ); //Лоде
    CPPUNIT_ASSERT(  95706 == EsrKit( 9570).getTerm6() ); //Югла
    CPPUNIT_ASSERT( 111114 == EsrKit(11111).getTerm6() ); //ОП Янямуйжа
    CPPUNIT_ASSERT(  95800 == EsrKit( 9580).getTerm6() ); //Чиекуркалнс
    CPPUNIT_ASSERT(  95000 == EsrKit( 9500).getTerm6() ); //Земитаны
    CPPUNIT_ASSERT( 111203 == EsrKit(11120).getTerm6() ); //Цесис
    CPPUNIT_ASSERT( 111218 == EsrKit(11121).getTerm6() ); //Арайши
    CPPUNIT_ASSERT( 111222 == EsrKit(11122).getTerm6() ); //ОП Мелтури
    CPPUNIT_ASSERT( 111307 == EsrKit(11130).getTerm6() ); //Иерики
}

void TC_EsrKit::statEsr()
{
    StationEsr no;
    StationEsr some(9011);

    CPPUNIT_ASSERT( no.empty() );
    CPPUNIT_ASSERT( 0 == no );
    CPPUNIT_ASSERT( "" == no.to_string() );

    CPPUNIT_ASSERT( !some.empty() );
    CPPUNIT_ASSERT( 9011 == some );
    CPPUNIT_ASSERT( "09011" == some.to_string() );
}
