/*!
	created:	2019/05/21
	created: 	12:09  21 Май 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_statPathProp.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_statPathProp
	file ext:	cpp
	author:		 Dremin
	
	purpose:	настройка свойств станц.путей
  
  */
 
 #include "stdafx.h"
 #include "TC_statPathProp.h"
#include "../Augur/TypeHeader.h" 
 
static ADProperties::Attrubute tblNoStop[]={		//ADProperties::Attrubute::PASSENGER , //путь пассажирский
	ADProperties::Attrubute::MAJOR, //путь главный
	//ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	ADProperties::Attrubute::NON_STOP_PASSING //безостановочный пропуск
	,ADProperties::Attrubute::PASSENGER
	// 	BID_CARGO //запрет для грузовых поездов
};

static ADProperties::Attrubute tblMaj[]={	
	ADProperties::Attrubute::MAJOR //путь главный
	,ADProperties::Attrubute::PASSENGER
};

static ADProperties::Attrubute tblpassMaj[]={	
	ADProperties::Attrubute::PASSENGER,  //путь пассажирский
	ADProperties::Attrubute::MAJOR, //путь главный
	//ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	//NON_STOP_PASSING //безостановочный пропуск
	// 	BID_CARGO //запрет для грузовых поездов
};

static ADProperties::Attrubute tblpassStop[]={	
	ADProperties::Attrubute::PASSENGER  //путь пассажирский
	//ADProperties::Attrubute::MAJOR, //путь главный
	//ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	//NON_STOP_PASSING //безостановочный пропуск
	// 	BID_CARGO //запрет для грузовых поездов
};

static ADProperties::Attrubute tblMovePr[]={	
	ADProperties::Attrubute::PASSENGER,  //путь пассажирский
	//ADProperties::Attrubute::MAJOR, //путь главный
	//ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	ADProperties::Attrubute::NON_STOP_PASSING //безостановочный пропуск
	// 	BID_CARGO //запрет для грузовых поездов
};

static ADProperties::Attrubute tblStopInOutPr[]={	
	ADProperties::Attrubute::PASSENGER,  //путь пассажирский
	//ADProperties::Attrubute::MAJOR, //путь главный
	ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	//NON_STOP_PASSING //безостановочный пропуск
	// 	BID_CARGO //запрет для грузовых поездов
};

static ADProperties::Attrubute tblEmpty[]={	
	ADProperties::Attrubute::PASSENGER
};

//------------------------------------------------------------------------------------------------
ADProperties passStopInOutPr( ARRAY_IT( tblStopInOutPr ),Oddness::UNDEF,WeightNorms());//пасс+для стоянки +не гл + не безост
ADProperties noStopPr( ARRAY_IT( tblNoStop ),Oddness::UNDEF,WeightNorms());// главный для проследования (безостан)// гл + безост
ADProperties passMovePr( ARRAY_IT( tblMovePr ),Oddness::UNDEF,WeightNorms());// для просл. не гл , не пасс и  безост
ADProperties pMaj( ARRAY_IT( tblMaj ),Oddness::UNDEF,WeightNorms());//гл 
ADProperties passMaj( ARRAY_IT( tblpassMaj ),Oddness::UNDEF,WeightNorms());//пасс+ гл 
ADProperties passStopPr( ARRAY_IT( tblpassStop ),Oddness::UNDEF,WeightNorms());//пасс+не для стоянки +не гл + не безост
ADProperties evenMajor( ARRAY_IT( tblMaj ),Oddness::EVEN,WeightNorms());//четный+ главный
ADProperties oddMajor( ARRAY_IT( tblMaj ),Oddness::ODD,WeightNorms());//нечетный+ главный
ADProperties evenPr( ARRAY_IT( tblEmpty ),Oddness::EVEN,WeightNorms());//четный
ADProperties oddPr( ARRAY_IT( tblEmpty ),Oddness::ODD,WeightNorms());//нечетный
ADProperties allPr( ARRAY_IT( tblEmpty ),Oddness::UNDEF,WeightNorms());//в любом направлении

