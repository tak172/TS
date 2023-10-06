#pragma once

#include "FictivePart.h"
#include "RealPart.h"
#include "../helpful/RT_Singleton.h"

class TrainField;
class TestTracker;
class LOS_base;
class SpanKit;
struct TestGuessNsiBasis;
class TrackerInfrastructure
{
    DECLARE_SINGLETON(TrackerInfrastructure);
public:
	const std::wstring axis_name; //система отсчета

	TestElem Get( std::wstring ) const;
	void CreateStation( TrainField &, bool directly /*в прямом или реверсивном порядке*/ ) const;
	time_t DetermineStripBusy( std::wstring, TestTracker &, bool instantly = true ); //выставление занятости на участок
	time_t DetermineStripFree( std::wstring, TestTracker &, bool instantly = true, unsigned int shiftSec = 1 ); //выставление свободности на участок
	void DetermineStripLocked( std::wstring, TestTracker &, bool instantly = true ); //выставление "назначенности" на участок
	void DetermineStripUnlocked( std::wstring, TestTracker &, bool instantly = true ); //снятие "назначенности" с участка
	void DetermineSwitchPlus( std::wstring, TestTracker &, bool instantly = true ); //выставление стрелки в плюс
	void DetermineSwitchMinus( std::wstring, TestTracker &, bool instantly = true ); //выставление стрелки в минус
	void DetermineSwitchOutOfControl( std::wstring, TestTracker &, bool instantly = true ); //выставление стрелки вне контроля
	void DetermineHeadOpen( std::wstring, TestTracker &, bool instantly = true ); //открытие светофора
	void DetermineHeadClose( std::wstring, TestTracker &, bool instantly = true ); //закрытие светофора
	time_t DetermineRouteSet( std::wstring, TestTracker &, bool instantly = true ); //задание маршрута
	void DetermineRouteCutting( std::wstring, TestTracker &, bool instantly = true ); //начало разделки маршрута (гашение головы)
	void DetermineRouteUnset( std::wstring, TestTracker &, bool instantly = true ); //снятие (полная разделка) маршрута
	void ImitateMotion( std::vector <std::wstring>, TestTracker & ); //эмуляция движения по последовательности участков
	typedef std::function<void( TrackerInfrastructure &, TestTracker &)> TIActivity;
	void DoActivities( const std::vector <TIActivity> &, TestTracker & );
	time_t FlushData( TestTracker &, bool parcel_type /*тип посылки (true = reference)*/, unsigned int shiftSec = 1 ); //передать произведенные изменения одной посылкой
	TestTracker * Tracker() { return &tracker; }
	TrainFieldPtr TField() { return tfieldPtr; }
	void Reset();
	time_t IncreaseTime( unsigned int incdeltaMs ); //с автопосылкой в Tracker
	time_t GetTimeMoment() const { return curmoment; }
	bool TakeExceptions() 
	{
		bool retexceptions = false;
		std::swap( retexceptions, exceptionCatched );
		return retexceptions;
	}

private:

	const EsrKit techStatCode; //ЕСР-код технической станции

    std::shared_ptr<const TestGuessNsiBasis> nsiBasisPtr;
	TrainFieldPtr tfieldPtr;
	TestTracker tracker;
	mutable bool exceptionCatched;
	time_t curmoment;

	typedef std::pair<std::wstring, TestElem> StrElPair;
	
	FictivePart fictivePart;
	RealPart realPart;

	mutable std::list<std::wstring> cachedObjects; //контейнер для кэширования данных посылки
	void GenerateXmlDoc( attic::a_document &, bool isRef, const std::vector<TestElem> & ) const;
	//отправить в Tracker состояние одного объекта:
	time_t DetermineSingle( std::wstring, bool instantly, TestTracker &, unsigned int shiftSec = 1 );
	//отправить в Tracker состояние набора объектов:
	void DetermineBatch( const std::list <std::wstring> &, bool reference, TestTracker &, unsigned int shiftSec = 1 );
	std::map <BadgeE, unsigned int> MakeCoInfo( const TestElem & ) const;
	void UpdateRouteLoses( const std::vector<TestElem> &  ) const;
};