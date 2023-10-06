#pragma once

#include "../Augur/AAdmin.h"
#include "../Augur/ATrain.h"
#include "../Augur/DefType.h"
#include "TestAugurNsiBasis.h"
#include "TestPossessions.h"
#include "../Hem/RouteIntrusion.h"
#include "../Augur/ProNorm.h"
#include "../Augur/AReduction.h"
#include "../helpful/SpanWayInfo.h"

typedef 	std::map<int,V_EVNT> M_INT_VEV;
typedef 	std::pair<int,V_EVNT> INT_VEV;
typedef 	std::map<int,AEvent>  M_INT_EV;
typedef 	std::pair<int,AEvent>  INT_EV;
typedef std::shared_ptr<SpanWayInfo> SpanWayInfoPtr;

struct Strain;

class NsiSetter
{
	NsiSetter();
	static NsiSetter nsiSetter;
};

class TestAAdmin : public AAdmin
{
	friend class NsiSetter;

public:
	//----------- для тестовой отладки--------------
	using AAdmin::noSort;
	using AAdmin::getNorm;
	using AAdmin::getTrn;
	using AAdmin::getNsi;
	using AAdmin::prepareTrains;
	using AAdmin::sort;
	using AAdmin::fndAllNorm;// для всех ПЕ ищется нормативка
	using AAdmin::rgSortCargo; // расширенная сортировка грузовых по весу ПЕ( #4451 Преимущество груженых поездов перед порожними)
	using AAdmin::allTrn;
	using AAdmin::tskOutrun;// задание на обгоны
	using AAdmin::fnFltr;// ф-ция  отбора заданий по обгонам
	using AAdmin::numStepFrc;// макс.число шагов прогноза при выполнении задания по прогнозам
	using AAdmin::maxWaitCounterPack ;// время максимального ожидания при пропуске встречного пакета ПЕ
	using AAdmin::deltaWait;// 3 минуты зазор для расчета при ликвидации обгона 
	using AAdmin::deltaIntrusion;// доп.зазор для реализации интрузии
	using AAdmin::useBackAdjust;// использовать обратную коррекцию после разрешения конфликта
	using AAdmin::useAdjustFirstStepForecast;// использовать коррекцию дотягивания на первом шаге после расчета прогноза
	using AAdmin::regimLog;// режим работы лога
	using AAdmin::reductSt;// понижение ст.интервалов при проверке конфликтов
	using AAdmin::ADVANTAGE_BRUTTO;/*превышение брутто для груз.ПЕ, дающее преимущество  для встречных*/
	using AAdmin::diffAdvBrutto;//допустимая разница между стоянкой груженого и порожнего поезда не более 10 минут.
	// --- расчет времени на перегоне---
	using AAdmin::crossTmPass;//	Для пассажирских поездов (1-998; 5001-7998) время t по умолчанию 3 минуты.
	using AAdmin::crossTm;//	Для остальных поездов (1001-4998; 8001-9798) время t по умолчанию 2,5 минуты.
	using AAdmin::delayLockSema; /* на отправление по запрещающему сигналу на пути с системой блокировки*/

	using AAdmin::tskWrng; /* интервал скрещения по запрещающему сигналу на пути с системой блокировки*/
	using AAdmin::delayAsymmetricSema;	/*7.1.1.5.	В случае движения поездов по неправильному пути по сигналам АЛСН, ТС, ЭЖ 
									добавляется к перегонному времени хода 2,5 минуты на прибытие при отсутствии дополнительного входного светофора*/
	using AAdmin::dispNormElSec;//  допустимое отклонение от нормативки для эл.секций
	using AAdmin::dispNorm;//  допустимое отклонение от нормативки не для эл.секций
	using AAdmin::stopBeforeLockPath ;  /* стоянка перед выходом на путь с системой блокировки*/
	using AAdmin::stopBeforeLockPathWrong ;  /* стоянка перед выходом на путь с системой блокировки по непр.пути*/
	using AAdmin::stopAfterLockPath; /* стоянка после прихода с пути с системой блокировки*/
	using AAdmin::dThrough;
	using AAdmin::gapStop;

	static bool checkOutrunTechStation;// проверять обгоны на тех.станциии
	typedef std::shared_ptr<std::vector<RouteIntrusion>> VRouteIntrusionPtr;

	enum TypeS{	Before,After,LockSem};

	TestAAdmin(time_t lvM = 1);
	TestAAdmin( const TestPossessions & , time_t lvM = 0);
	~TestAAdmin();
	void add( ATrainPtr atrainPtr ,VRouteIntrusionPtr v=VRouteIntrusionPtr());
	void addNorma( const TrainDescr &, const V_EVNT & );
	static void setNsi( std::shared_ptr<TestAugurNsiBasis> );
	void clearOccupy();
	ATrainPtr fnd(const Strain& t)const;
	ATrainPtr fnd(const TrainDescr& t)const;
	V_TRN sort(){return AAdmin::sort(allTrn);}
	static std::shared_ptr<TestAugurNsiBasis> testNsiBasisPtr;
	static void StopBefAftLockPath(time_t t,SpanLockType p,TypeS tp,bool wrong); /* стоянка перед выходом на непр.путь*/
	ATrainPtr get(std::string num);// получить поезд(всю инфу )
	void setCrossesNum(EsrKit& esr,unsigned int num);// число  переездов на перегоне
	void setAsymmetricSema(EsrKit& esr,bool b);// отсутствие дополнительного входного светофора
	static std::vector<SpanWayInfoPtr> getWays( const EsrKit & ecode );
	void MinSmallStop(int t);// время маленькой стоянки для  превращения в проследование
private:
	TestPossessions possessions;
};


class TestProNorm:public ProNorm
{
public:
	using ProNorm::fndPreset;
};

class TestReduction: public AReduction
{
public:
	using AReduction::defaultReduction;
	TestReduction():AReduction(){}
};


M_INT_EV fltrStatEv(long esr,const TrainRoutes<AEvent>& v);
 M_INT_EV fltrStatEv(long esr,const AAdmin::M_NORM &v);
  V_EVNT fnd(const V_EVNT& v,const EsrKit& esr, AEvent::Source s);
