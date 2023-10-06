/*!
	created:	2017/01/27
	created: 	11:30  27 Январь 2017
	filename: 	D:\potap\Charm\UT\TC_fabrClass.h
	file path:	D:\potap\Charm\UT
	file base:	TC_fabrClass
	file ext:	h
	author:		 Dremin
	
	purpose:	фабрика классов для тестов
 */
#ifndef TC_fabrClass_h__
#define TC_fabrClass_h__

#include "../Augur/AThread.h"
#include "../Augur/DefType.h"
#include "TestAAdmin.h"
#include "../Augur/TrainRoutes.h"
#include "../helpful/SpanLockType.h"

#define  SPAN_LENGHT_KM 1
struct Stech
{// для ввода техн.ограничений в тесты
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1, int _e2, std::shared_ptr<unsigned int> _maxVelPtr );
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1 );
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1 , std::shared_ptr<unsigned int> _maxVelPtr);
	time_t or;
	time_t count;// длительность
	int path;
	int park;
	int e1;
	int e2;
	std::shared_ptr<unsigned int> maxVelPtr; //нулевой указатель означает окно
	double kmLen;// размер области  действия
	SpanLockTypePtr sR;// по верному пути
	SpanLockTypePtr sW;// по неверному пути
};

struct Spvx
{// время ПВХ e1->e2 и e2->e1 на 20% больше
	int e1;
	int e2;
	int pvx;
	Spvx(int e,int p,int x=0 );
	//:e1(e),e2(x?0:e+x),pvx(static_cast<int>(p*(x?1:PVX_COEFF_2_1))){}
};


struct Sevent 
{// для ввода нити событий в тесты
	Sevent( AEvent::Source evsrc, time_t mom, int _tSt, int _tSp, int _tendSp, int _e1, int _nSt ,int _endStop=0);
		
	AEvent::Source tp;// тип событий
	time_t or;// начало нити
	int tSt;// время на станции
	int tSp;// время на перегоне
	int tEndSp;// время на последнем перегоне
	int e1; // нач.станция
	int nSt;// число станций
	int endStop;// добавочный stop в конце событий
};

struct Strain
{
	Strain( const TrainDescr & _td, const Sevent & ev1, const Sevent & ev2 ) :
		tdescr( _td )
	{
		evn.emplace_back( ev1 );
		evn.emplace_back( ev2 );
	}
	TrainDescr tdescr;
	std::vector <Sevent> evn;
};

struct TC_StatPath
{
	TC_StatPath();
	TC_StatPath( ADProperties& p,int c=1 );
	void add(ADProperties& p,int c=1 );
	void add(ADProperties::Attrubute  t );
	V_SEC secs;
// 	unsigned int capacity; //вместимость в условных вагонах
// 	ADProperties adprops;
};

enum struct SpanBreadth
{
	ONE_WAY_SPAN,
	DOUBLE_WAY_SPAN
};
enum ForkTopology{NO_FORK, ONE_FORK};

typedef std::pair<int,TC_StatPath> NUM_PATH_TC;
typedef std::map<int,TC_StatPath> PATH_TC;
typedef std::map<int,PATH_TC> STATION_TC;

struct TC_AtopologyParams
{
private:
	void init(const std::vector<ADProperties::Attrubute>& v=std::vector<ADProperties::Attrubute>());
public:
	typedef const std::map<EsrKit, TechnicalTime> TechTimeCMap;
	TC_AtopologyParams( unsigned int stationsCount, unsigned int pathsNum, unsigned int parksNum, SpanBreadth spanBreadth, 
		SpanLockType spanLock = SpanLockType::BIDIR_AUTOBLOCK, TechTimeCMap & techtimemap = TechTimeCMap() );


	TC_AtopologyParams( ForkTopology f,
		unsigned int stationsCount, unsigned int pathsNum, unsigned int parksNum, SpanBreadth spanBreadth, 
		SpanLockType spanLock = SpanLockType::BIDIR_AUTOBLOCK, TechTimeCMap & techtimemap = TechTimeCMap() );
	//-- функции --
	void add(int stat,int path, TC_StatPath& t);
	void buildStation(StationEsr esr,std::map<EsrKit, StationsRegistry::StationInfo>& statInfos)const;
	void setCapacity(unsigned esr, int capacity);// установка емкости путей на станции
	void setCapacity( int capacity);// установка емкости путей на станциях
	void addPropWay(std::vector<ADProperties::Attrubute>& v);
	//-- данные --
	unsigned int stationsNum;
	unsigned int statPathsNum;
	unsigned int statParksNum;
	SpanBreadth typicalSpanBreadth;
	SpanLockType typicalSpanLock;
	struct StationDetalization
	{
		StationDetalization( const NsiBasis::StationKind &, const std::list<StationsRegistry::TransrouteDescr> & = std::list<StationsRegistry::TransrouteDescr>() );
		NsiBasis::StationKind statKind; //тип станции
		std::list<StationsRegistry::TransrouteDescr> transroutes; //сквозные маршруты (опционально)
	};
	void setDetalization( StationEsr esr, const StationDetalization & details ){ stationDetails.insert( std::make_pair( esr, details ) ); }
	std::map<StationEsr, StationDetalization> stationDetails;
	std::set<EsrKit> spansWOWays;
	TechTimeCMap techTime;
	std::set<EsrKit>departureByHemPath;// где искать привязку к нормативке
	ForkTopology ft;// признак развилки
	unsigned kmBtwn;// километров  между станциями
	STATION_TC mStat;// описание путей на станциях
};

class RailLimitation;
template <typename T> class TrainRoutes;
class TC_Atopology
{
public:
	enum Touch{NONE, FIRST,END,BOTH};//примыкание событий на перегоне нет, в начале, в конце, оба и в начале и в конце
static	ATrainPtr add(const TrainDescr &,V_EVNT&v);// добавить нить с событиями 
	typedef std::map<std::pair<int,int>, STT_Val> MAP_SPANTIME;	

private:
	ATrainPtr add(const TrainDescr &,Sevent&t,Sevent*tFix,int path,Touch tp);// добавить нить с событиями типа
	void lineStations(std::vector<EsrKit> l,const TC_AtopologyParams & tplParams,
		std::map <EsrKit, SpansRegistry::SpanInfo>&,std::map <EsrKit, StationsRegistry::StationInfo>&);// построение линии станций
	 ADProperties prop;// св-ва станц.путей по умолчанию
public:
	TC_Atopology( const TC_AtopologyParams & );
	~TC_Atopology(){}
	RailLimitationCPtr tech(Stech&t); 
	WayInfoCPtr fnd(Stech& t);
	void setPvx(int pvx,st_intervals_t* s=nullptr);
	void setPvx(const STT_Val& v);
	void setPvx(const STT_Val& v,const MAP_SPANTIME& m );
	void setStInt(const st_intervals_t& v);
	void setPvx(int pvx,SttMove::Type tp);
	STT_Cat_Val getPVX(std::pair<int,int>*p,SttMove::Type tp=SttMove::Type::TIME_GOODS);
	st_intervals_t getInterval(StationEsr esr=StationEsr(11),StationEsr eNext=StationEsr(10));

	ATrainPtr add(Strain&t, int path=0, Touch tp=NONE);// добавить нить с событиями типа FIX и  примыкание на перегоне
	ATrainPtr add(const TrainDescr &, AThread*thrR);// добавить нить с событиями типа real и FIX

	V_EVNT norma(Sevent&t,int num);// построить нормативку
	std::vector <EsrKit> getStatCodes() const { return statCodes; }
	std::vector <EsrKit> getSpanCodes() const { return spanCodes; }
	static int tstComplete(V_EVNT& v);// возвращаем число станций
	static void tstNormalize(V_EVNT& v);// проверка нормализации вектора событий
	V_EVNT bld(Sevent&t, int num) const;
	void addSpanInterval( const EsrKit & spanCode, int spanTravTime );
private:
	std::vector <EsrKit> statCodes, spanCodes;
	void addSpanInterval( const std::pair<unsigned, unsigned>& spanCode, const STT_Val& s );
	void addStationInterval( const EsrKit & statCode, const EsrKit & prevStatCode, const EsrKit & nextStatCode, int timeInterval );
	void addStationInterval( const EsrKit & statCode, const EsrKit & prevStatCode, const EsrKit & nextStatCode, const st_intervals_t* s );
};


class Srch{
	int num;
	EsrKit esr;
public:
	Srch(int n):num(n){}
	Srch(int n,int k):num(0),esr(n,k){}
	bool operator()(const TrainRoutes<AEvent>::EventLine & l){ return l.first.GetNumber().getNumber()==num;}
	bool operator()(const AEvent& ev){return ev.Esr()==esr;}
};

AEvent fndFrcEv(const TrainRoutes<AEvent>& f,int nTr,int esr, int e=0);
STT_Val generateSTT(int start,int full, int reserv,int stop);

#endif // TC_fabrClass_h__

