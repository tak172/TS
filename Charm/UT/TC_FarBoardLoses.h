#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"
#include "LogicDocumentTest.h"
#include "../helpful/Oddness.h"

class IndirectLosesNotifier;
class Lexicon;
class TrainDescr;
class TrainPlacesReview;
class RoutesChangesDetector;
class TC_FarBoardLoses : 
	public TC_Project
{

	CPPUNIT_TEST_SUITE( TC_FarBoardLoses );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteOddNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByRouteEvenNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripOddNumUndefSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumOddSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumEvenSema );
	CPPUNIT_TEST( SHO_SpanToStatByStripEvenNumUndefSema );
	CPPUNIT_TEST( SHO_StatToSpanByRoute );
	CPPUNIT_TEST( SHO_StatToSpanByStrip );
	CPPUNIT_TEST( SHO_StatToSpanErrorCase );
	CPPUNIT_TEST( LEN_SpanToStatByRoute );
	CPPUNIT_TEST( LEN_SpanToStatByStrip );
	CPPUNIT_TEST( LEN_IntoStatByRoute );
	CPPUNIT_TEST( LEN_IntoStatByStrip );
	CPPUNIT_TEST( LEN_StatToSpanByRoute );
	CPPUNIT_TEST( LEN_StatToSpanByStrip );
	CPPUNIT_TEST( BP_AmbivalentByRoute );
	CPPUNIT_TEST( BP_AmbivalentByStrip );
	CPPUNIT_TEST_SUITE_END();

public:
	virtual void setUp();

private:
	void MakeShortSetObjects();
	void MakeShortSetStationObjects();
	void MakeLengthySetObjects();
	void MakeLengthySetStatObjects();
	void MakeBPostSetObjects();
	CLogicElement * MakeSpanObjects( CLogicDocumentTest *, std::vector <CLogicElement *> & lstrips ); //результат - объект поездного табло
	void MakeStripLine( CLogicDocumentTest *, std::vector <CLogicElement *> & lstrips ); //генерация цепочки участков и их связывание
	void InitLexicon();

	void SHO_SpanToStatByRouteOddNumOddSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. нечетный поезд. нечетный светофор
	void SHO_SpanToStatByRouteOddNumEvenSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. нечетный поезд. четный светофор
	void SHO_SpanToStatByRouteOddNumUndefSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. нечетный поезд. неопределенный светофор
	void SHO_SpanToStatByRouteEvenNumOddSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. четный поезд. нечетный светофор
	void SHO_SpanToStatByRouteEvenNumEvenSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. четный поезд. четный светофор
	void SHO_SpanToStatByRouteEvenNumUndefSema(); //простой маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне. четный поезд. неопределенный светофор
	void SHO_SpanToStatByRoute( bool numIsOdd, Oddness semaOddness, bool permitted /*разрешен ли проброс номера*/ );

	void SHO_SpanToStatByStripOddNumOddSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. нечетный поезд. нечетный светофор
	void SHO_SpanToStatByStripOddNumEvenSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. нечетный поезд. четный светофор
	void SHO_SpanToStatByStripOddNumUndefSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. нечетный поезд. неопределенный светофор
	void SHO_SpanToStatByStripEvenNumOddSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. четный поезд. нечетный светофор
	void SHO_SpanToStatByStripEvenNumEvenSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. четный поезд. четный светофор
	void SHO_SpanToStatByStripEvenNumUndefSema(); //простой маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию. четный поезд. неопределенный светофор
	void SHO_SpanToStatByStrip( bool numIsOdd, Oddness semaOddness, bool permitted /*разрешен ли проброс номера*/ );

	void SHO_StatToSpanByRoute(); //простой маршрут. реакция на задание маршрута со станции на перегон при нахождении поезда перед светофором
	void SHO_StatToSpanByStrip(); //простой маршрут. реакция на появление поезда перед светофором при заданном маршруте со станции на перегон
	void SHO_StatToSpanErrorCase(); //простой маршрут. проблемный случай при задании маршрута со станции на перегон

	void LEN_SpanToStatByRoute(); //составной маршрут. реакция на задание маршрута с перегона на станцию при нахождении поезда на перегоне
	void LEN_SpanToStatByStrip(); //составной маршрут. реакция на появление поезда на перегоне при заданном маршруте с перегона на станцию
	void LEN_IntoStatByRoute(); //составной маршрут. реакция на задание маршрута в рамках одной станции при нахождении поезда перед светофором
	void LEN_IntoStatByStrip(); //составной маршрут. реакция на появление поезда перед светофором при заданном маршруте с перегона на станцию
	void LEN_StatToSpanByRoute(); //составной маршрут. реакция на задание маршрута со станции на перегон при нахождении поезда перед светофором
	void LEN_StatToSpanByStrip(); //составной маршрут. реакция на появление поезда перед светофором при заданном маршруте со станции на перегон

	void BP_AmbivalentByRoute(); //маршрут сквозь блок-пост, который должен интерпертироваться как маршрут приема и отправления одновременно. реакция на задание маршрута
	void BP_AmbivalentByStrip(); //маршрут сквозь блок-пост, который должен интерпертироваться как маршрут приема и отправления одновременно. реакция на появление поезда

	void SetRouteByLeg( CLogicElement * lleg );
	TrainDescr SetTrainOnStrip( CLogicElement * lstrip, bool oddNumber = true /*четность номера поезда*/ ); //установка поезда на участке
	void CleanTrainOnStrip( bool only_number, CLogicElement * lstrip ); //сброс номера поезда на удаленном участке перегона (only_number = занятость остается)
	void TrainsPlacingNotify( attic::a_node trainsNode );

	std::unique_ptr <IndirectLosesNotifier> indirLosesNotifier;
	std::unique_ptr <Lexicon> lexiconPtr;
	std::unique_ptr <TrainPlacesReview> tplcReviewer;

	RoutesChangesDetector MakeRouteChangesDetector( const std::set<const CLogicElement *> & lroutes ) const;

	//данные для отработки коротких (не составных) маршрутов
	struct ShortSet
	{
		//набор объектов, описывающих станцию и примыкающий к ней перегон (для отработки задания односоставных маршрутов)
		CLogicDocumentTest spanDoc, anotherStatDoc;
		CLogicElement * statBoard, * spanBoard;
		std::vector <CLogicElement *> statStrips;
		std::vector <CLogicElement *> spanStrips;
		CLogicElement * statLegFromSpanToStat; //станционный светофор направлением от перегона head
		CLogicElement * bndLegFromSpanToStat; //граничный светофор направлением от перегона head3
		CLogicElement * statLegFromStatToSpan; //станционный светофор направлением на перегон head2
	} sset;

	//данные для отработки длинных (составных) маршрутов
	struct LengthySet
	{
		CLogicDocumentTest lspanDoc, rspanDoc;
		CLogicElement * lspanBoard, * rspanBoard;
		std::vector <CLogicElement *> statBoards;
		std::vector <CLogicElement *> semalegs;
		std::vector <CLogicElement *> lspanStrips, statStrips, rspanStrips;
	} lset;

	//данные для отработки маршрута через блок-пост
	struct BPostSet
	{
		CLogicDocumentTest lspanDoc, bpostDoc, rspanDoc;
		CLogicElement * rspanBoard;
		CLogicElement * semaleg;
		std::vector <CLogicElement *> lspanStrips, statStrips, rspanStrips;
	} bpset;
};