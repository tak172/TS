#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "../Guess/TrainChanges.h"

//общий функционал для классов тестирования

class TestTracker;
class TrainDescr;
class TrainContainer;
class TC_TrackerTrainImpl
{
public:
	TC_TrackerTrainImpl();
	~TC_TrackerTrainImpl();

protected:
	const TrainContainer * trainCont;
	ChangesUnion changes; //фиксация изменений поездной информации
	TrackerInfrastructure & TI;
	TestTracker * tracker;
	TrainInfoPackage CreateInfoPackage( bool disform, unsigned int trainIden, const TrainDescr &, const BadgeE &, 
		const TrainCharacteristics::Source & = TrainCharacteristics::Source::User, time_t * = nullptr ) const;
	void SetRandomOddInfo( TrainUnityCPtr, TestTracker * = nullptr ); //установить случайную поездную информацию на ПЕ (нечетный номер)
	void SetRandomEvenInfo( TrainUnityCPtr, TestTracker * = nullptr ); //установить случайную поездную информацию на ПЕ (нечетный номер)
	void SetInfo( TrainUnityCPtr, const TrainDescr &, TestTracker * = nullptr ); //установить информацию по поезду (предопределенно)

	TrainUnityCPtr SetRandomOddInfo( const BadgeE & onPlace ); //установить случайную поездную информацию на ПЕ по месту (нечетный номер)
	TrainUnityCPtr SetRandomEvenInfo( const BadgeE & onPlace ); //установить случайную поездную информацию на ПЕ по месту (четный номер)
	//установить информацию по месту нахождения:
	TrainUnityCPtr SetInfo( const BadgeE & placeBdg, const TrainDescr &, const TrainCharacteristics::Source & defsource = TrainCharacteristics::Source::User );
	
	std::string DisformTrain( unsigned int trainId, const TrainCharacteristics::Source & = TrainCharacteristics::Source::Guess, TestTracker * = nullptr ); //расформировать поезд
	std::string DisformTrain( unsigned int trainId, const BadgeE & place, const TrainDescr & tdescr, 
		const TrainCharacteristics::Source & = TrainCharacteristics::Source::Guess, TestTracker * = nullptr ); //расформировать поезд (развернутая сигнатура)
	static TrainDescr RandomTrainDescr( bool oddNumber = true /*номер поезда нечетный*/ ); //создание псевдослучайного описания поезда
	static PlaceViewChangePtr FindIn( const std::list <PlaceViewChange> &, const BadgeE & changedBdg );
	static PlaceViewChangePtr FindIn( const std::list <PlaceViewChange> &, const BadgeE & changedBdg, bool appeared );
	//втягивание "хвоста" при въезде на п/о путь C (результат - было ли сгенерировано событие прибытия):
	bool PullingTailToCWay( TrainCharacteristics::TrainFeature = TrainCharacteristics::TrainFeature::NoInfo );
	void PullingTailToEWay(); //втягивание "хвоста" при въезде на п/о путь E
	bool CheckTimeSequence( const std::list <TrackerEventPtr> & tevents ) const; //проверка временных штампов событий на последовательное неубывание
	//принудительное помещение ПЕ на указанное место (результат - успешность):
	bool LocateUnityByForce( TrainDescrPtr, const std::list<std::wstring> & place, TestTracker * customTracker = nullptr );
    // проверка кода и бейджа
    bool chk_eq( const TrackerEventPtr ev, HCode hcode, const wchar_t* bdg_name );

protected:
	//асинхронная генерация события прибытия при проезде п/о пути с открытым светофором:
	void OpenWayAsyncArriving( TrainCharacteristics::TrainFeature, DWORD waitingTimeMs );

private:
	static PlaceViewChangePtr FindIn( const std::list <PlaceViewChange> &, std::function<bool( const PlaceViewChange &)> );
};

inline PlaceViewChangePtr TC_TrackerTrainImpl::FindIn( const std::list <PlaceViewChange> & changes, const BadgeE & changedBdg )
{
	auto pred = [&changedBdg]( const PlaceViewChange & tchange ){
		return tchange.place == changedBdg;
	};
	return FindIn( changes, pred );
}

inline PlaceViewChangePtr TC_TrackerTrainImpl::FindIn( const std::list <PlaceViewChange> & changes, const BadgeE & changedBdg, bool appeared )
{
	auto pred = [&changedBdg, appeared]( const PlaceViewChange & tchange ){
		return tchange.place == changedBdg && tchange.appeared == appeared;
	};
	return FindIn( changes, pred );
}

inline PlaceViewChangePtr TC_TrackerTrainImpl::FindIn( const std::list <PlaceViewChange> & changes, std::function<bool( const PlaceViewChange &)> pred )
{
	PlaceViewChangePtr retchange;
	auto tcIt = find_if( changes.cbegin(), changes.cend(), pred );
	if ( tcIt != changes.cend() )
		retchange.reset( new PlaceViewChange( *tcIt ) );
	return retchange;
}