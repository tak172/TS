/*!
	created:	2017/01/27
	created: 	11:30  27 ������ 2017
	filename: 	D:\potap\Charm\UT\TC_fabrClass.h
	file path:	D:\potap\Charm\UT
	file base:	TC_fabrClass
	file ext:	h
	author:		 Dremin
	
	purpose:	������� ������� ��� ������
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
{// ��� ����� ����.����������� � �����
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1, int _e2, std::shared_ptr<unsigned int> _maxVelPtr );
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1 );
	Stech( time_t mom, time_t dur, int _path, int _park, int _e1 , std::shared_ptr<unsigned int> _maxVelPtr);
	time_t or;
	time_t count;// ������������
	int path;
	int park;
	int e1;
	int e2;
	std::shared_ptr<unsigned int> maxVelPtr; //������� ��������� �������� ����
	double kmLen;// ������ �������  ��������
	SpanLockTypePtr sR;// �� ������� ����
	SpanLockTypePtr sW;// �� ��������� ����
};

struct Spvx
{// ����� ��� e1->e2 � e2->e1 �� 20% ������
	int e1;
	int e2;
	int pvx;
	Spvx(int e,int p,int x=0 );
	//:e1(e),e2(x?0:e+x),pvx(static_cast<int>(p*(x?1:PVX_COEFF_2_1))){}
};


struct Sevent 
{// ��� ����� ���� ������� � �����
	Sevent( AEvent::Source evsrc, time_t mom, int _tSt, int _tSp, int _tendSp, int _e1, int _nSt ,int _endStop=0);
		
	AEvent::Source tp;// ��� �������
	time_t or;// ������ ����
	int tSt;// ����� �� �������
	int tSp;// ����� �� ��������
	int tEndSp;// ����� �� ��������� ��������
	int e1; // ���.�������
	int nSt;// ����� �������
	int endStop;// ���������� stop � ����� �������
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
// 	unsigned int capacity; //����������� � �������� �������
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
	//-- ������� --
	void add(int stat,int path, TC_StatPath& t);
	void buildStation(StationEsr esr,std::map<EsrKit, StationsRegistry::StationInfo>& statInfos)const;
	void setCapacity(unsigned esr, int capacity);// ��������� ������� ����� �� �������
	void setCapacity( int capacity);// ��������� ������� ����� �� ��������
	void addPropWay(std::vector<ADProperties::Attrubute>& v);
	//-- ������ --
	unsigned int stationsNum;
	unsigned int statPathsNum;
	unsigned int statParksNum;
	SpanBreadth typicalSpanBreadth;
	SpanLockType typicalSpanLock;
	struct StationDetalization
	{
		StationDetalization( const NsiBasis::StationKind &, const std::list<StationsRegistry::TransrouteDescr> & = std::list<StationsRegistry::TransrouteDescr>() );
		NsiBasis::StationKind statKind; //��� �������
		std::list<StationsRegistry::TransrouteDescr> transroutes; //�������� �������� (�����������)
	};
	void setDetalization( StationEsr esr, const StationDetalization & details ){ stationDetails.insert( std::make_pair( esr, details ) ); }
	std::map<StationEsr, StationDetalization> stationDetails;
	std::set<EsrKit> spansWOWays;
	TechTimeCMap techTime;
	std::set<EsrKit>departureByHemPath;// ��� ������ �������� � ����������
	ForkTopology ft;// ������� ��������
	unsigned kmBtwn;// ����������  ����� ���������
	STATION_TC mStat;// �������� ����� �� ��������
};

class RailLimitation;
template <typename T> class TrainRoutes;
class TC_Atopology
{
public:
	enum Touch{NONE, FIRST,END,BOTH};//���������� ������� �� �������� ���, � ������, � �����, ��� � � ������ � � �����
static	ATrainPtr add(const TrainDescr &,V_EVNT&v);// �������� ���� � ��������� 
	typedef std::map<std::pair<int,int>, STT_Val> MAP_SPANTIME;	

private:
	ATrainPtr add(const TrainDescr &,Sevent&t,Sevent*tFix,int path,Touch tp);// �������� ���� � ��������� ����
	void lineStations(std::vector<EsrKit> l,const TC_AtopologyParams & tplParams,
		std::map <EsrKit, SpansRegistry::SpanInfo>&,std::map <EsrKit, StationsRegistry::StationInfo>&);// ���������� ����� �������
	 ADProperties prop;// ��-�� �����.����� �� ���������
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

	ATrainPtr add(Strain&t, int path=0, Touch tp=NONE);// �������� ���� � ��������� ���� FIX �  ���������� �� ��������
	ATrainPtr add(const TrainDescr &, AThread*thrR);// �������� ���� � ��������� ���� real � FIX

	V_EVNT norma(Sevent&t,int num);// ��������� ����������
	std::vector <EsrKit> getStatCodes() const { return statCodes; }
	std::vector <EsrKit> getSpanCodes() const { return spanCodes; }
	static int tstComplete(V_EVNT& v);// ���������� ����� �������
	static void tstNormalize(V_EVNT& v);// �������� ������������ ������� �������
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

