#include "stdafx.h"
#include "TestAAdmin.h"
#include "../Augur/Possessions.h"
#include "../Augur/TrainRoutes.h"
#include "../Augur/Occupancy.h"
#include "TC_fabrClass.h"
#include "../helpful/TrainsPriorityOffsets.h"
#include "../Augur/AConflData.h"
#include "../helpful/LocoCharacteristics.h"
#include "../Augur/AInterval.h"

using namespace std;

bool TestAAdmin::checkOutrunTechStation=false;// проверять обгоны на тех.станциии


static void setDistrNumTr()
{
// 	{SttMove::Type::TIME_PASS,1,998}
// 	,{SttMove::Type::TIME_ELSECTION,5001,5998}
// 	,{SttMove::Type::TIME_ELSECTION,7631,7998}
// 	,{SttMove::Type::TIME_DIESEL,6001,7628}
// 
// 	,{SttMove::Type::TIME_GOODS,1001,3998}
// 	,{SttMove::Type::TIME_GOODS,9001,9798}
// 	,{SttMove::Type::TIME_GOODS,4001,4998}
// 	,{SttMove::Type::TIME_GOODS,8001,8048}
// 	,{SttMove::Type::TIME_GOODS,8051,8098}
// 	,{SttMove::Type::TIME_GOODS,8101,8998}
	std::wstring contents=L"<Augur>"
		L"<ForecastProperty>"
//		L"<Time  StopBeforeWrongPath=\"8\" />"
		L"<NumberTrain>"
		L"<Norma  value=\"1-998, 5001-5998,6001-7628\" />"
		L"<Passenger PVX=\"1-998\"  Limit=\" 1-1998, 5001-5998, 7801-7998\" />"
		L"<Electrosec PVX=\"6001-7798\"  Limit=\"6001-7798\" />"
		L"<Diesel  PVX=\"701-898,7801-7998\"  />"
		L"</NumberTrain>"
		L"</ForecastProperty>"
		L"</Augur>";
	attic::a_document configDoc;
	auto r=configDoc.load_wide(contents);
	attic::a_node root=configDoc.child("Augur");
	auto error=AAdmin::config(root.child("ForecastProperty"));
	ASSERT(error.empty());
}

NsiSetter::NsiSetter()
{
	shared_ptr<TestAugurNsiBasis> nsiPtr( new TestAugurNsiBasis() );
	TestAAdmin::setNsi( nsiPtr );
};

shared_ptr<TestAugurNsiBasis> TestAAdmin::testNsiBasisPtr;

void TestAAdmin::StopBefAftLockPath(time_t t,SpanLockType p, TypeS tp,bool wrong)
{/* стоянка перед выходом на непр.путь*/
	MSTOPTM* m=&delayLockSema;
	switch (tp)
	{
	case TestAAdmin::Before:
		 m=wrong?&stopBeforeLockPathWrong:&stopBeforeLockPath;
		break;
	case TestAAdmin::After:
		m=&stopAfterLockPath;
		break;
	}
	m->operator[](p)=t;
}

TestAAdmin::TestAAdmin(time_t lvM) : AAdmin(), possessions( testNsiBasisPtr )
{
	fnFltr.clear();// ф-ция  отбора заданий по обгонам
	fnFltr.push_back(nullptr);// ф-ция  отбора заданий по обгонам по исключениям
	useBackAdjust=false;// использовать обратную коррекцию после разрешения конфликта
	deltaWait=180;
	deltaIntrusion=deltaWait;
	maxWaitCounterPack=0 ;// время максимального ожидания при пропуске встречного пакета ПЕ
	AConflData::strongTnpRgm(true);// установка(true)/снятие(false) учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.

	init(testNsiBasisPtr, possessions,lvM ,TrPriorOffsetsPtr(new TrainsPriorityOffsets()),nullptr);
	setDistrNumTr();
}

TestAAdmin::TestAAdmin( const TestPossessions & poss, time_t lvM) : AAdmin() , possessions( poss )
{
	fnFltr.clear();// ф-ция  отбора заданий по обгонам
	fnFltr.push_back(nullptr);// ф-ция  отбора заданий по обгонам по исключениям
	useBackAdjust=false;// использовать обратную коррекцию после разрешения конфликта
	deltaWait=180;
	maxWaitCounterPack=0 ;// время максимального ожидания при пропуске встречного пакета ПЕ
	AConflData::strongTnpRgm(true);// установка(true)/снятие(false) учета интервала Тн в случае, если оба поезда прибывают с остановкой, то  можно не соблюдать.
	dispNormElSec=dispNorm=0;//  допустимое отклонение от нормативки для эл.секций+ допустимое отклонение от нормативки не для эл.секций
	init( testNsiBasisPtr, poss , lvM,TrPriorOffsetsPtr(new TrainsPriorityOffsets()), nullptr);
	setDistrNumTr();
}

TestAAdmin::~TestAAdmin()
{
	noSort=false;//----------- для тестовой отладки--------------
	ACounterInt::limit= 6; // по умолчанию
	LocoCharacteristics::Shutdowner();
}

void TestAAdmin::setNsi( std::shared_ptr<TestAugurNsiBasis> nsiPtr )
{
	testNsiBasisPtr = nsiPtr;
	AAdmin::setNsi( nsiPtr );
}

void TestAAdmin::clearOccupy()
{
	occup->trainLocks.clear();
	clash.clear();
}

ATrainPtr TestAAdmin::fnd(const Strain& st) const
{
	return fnd(st.tdescr);
}

ATrainPtr TestAAdmin::fnd(const TrainDescr& dt) const
{
	ATrainPtr res;
	for (const auto& t: getTrn())
		if(t->descr()==dt)
		{
			res=t;
			break;
		}
		return res;
}

void TestAAdmin::add( ATrainPtr atrainPtr ,VRouteIntrusionPtr v)
{
	TrainRoutes<AEvent> liveTrainRoutes( TrainRoutes<AEvent>::Type::ForecastIn );
	const auto & tdescr = atrainPtr->descr();
	vector<AEvent> events = atrainPtr->get( AEvent::REAL );
	vector<AEvent> evFx = atrainPtr->get( AEvent::FIXED );
	events.insert(events.end(),evFx.begin(),evFx.end());
	liveTrainRoutes.addEventLine( make_pair( tdescr, events ) );
	addLiveTrains( liveTrainRoutes ,v?*v:vector<RouteIntrusion>());
}

void TestAAdmin::addNorma( const TrainDescr & tdescr, const V_EVNT & evec )
{
	TrainRoutes<AEvent> trainRoutes( TrainRoutes<AEvent>::Type::Schedule );

	trainRoutes.addEventLine( make_pair( tdescr, evec ) );

	addSchedule( trainRoutes );
}

void TestPossessions::setLimitation( Stech& t,TC_Atopology& tpl)
{
	RailLimitationCPtr railLimitCPtr = tpl.tech(t );
	std::vector<RailLimitationCPtr> vR(1,railLimitCPtr);
	std::vector<WayInfoCPtr> w;
	if(t.path)
		w.push_back(tpl.fnd(t));
	else
		if(t.e1 && t.e2)
		{
			SpanWayVec r=TestAAdmin::testNsiBasisPtr->spansRegPtr->getWays(EsrKit(t.e1,t.e2));
			for (const auto& pt:r )
				w.push_back(pt);
		}
	for (const auto& wt:w )
		setLimitation( wt, vR );
}

 
 ATrainPtr TestAAdmin::get(std::string num)
 {// получить поезд(всю инфу )
 	ATrainPtr res;
 	int n=atoi(num.c_str());
	if (n)
	{
		auto tmpIt = std::find_if(allTrn.begin(), allTrn.end(), [n](ATrainPtr& t) {
			return (n == t->getNum());
			});
		if ( tmpIt!=allTrn.end() )
			res = *tmpIt;
	}
 	return res;
 }

 void TestAAdmin::setCrossesNum(EsrKit& esr, unsigned int num)
 {// число  переездов на перегоне
	auto& spans= getNsi()->spansRegPtr;
	auto it=spans->spanInfos.find(esr);
	if(it!=spans->spanInfos.end())
		it->second.crossesNum=num;
 }

 vector<SpanWayInfoPtr> TestAAdmin::getWays( const EsrKit & ecode )
 {
	 vector<SpanWayInfoPtr> retWays;
	 const auto& spanInfos=getNsi()->spansRegPtr->spanInfos;
	 auto wvecIt = spanInfos.find( ecode );
	 if ( wvecIt != spanInfos.cend() )
		 for (const auto& t:wvecIt->second.spanWays )
			 retWays.push_back( const_pointer_cast<SpanWayInfo>(t));
	 return retWays;
 }
 
 void TestAAdmin::MinSmallStop(int t)
 {// время маленькой стоянки для  превращения в проследование
	 minStop.clear();
	 minStop.push_back(TM_INTERVALS(t,INTERVALS()));
 }

 void AAdmin::checkUseTrain(const char*p)
 {
	 typedef pair<int,int> PINT;
	 vector<PINT> v;
	 for (const auto& t: adm->allTrn)
		 v.push_back(PINT(t->getNum(),t.use_count()));
	 TRACE(p?p:"\n");
	 for (const auto& t: v)
		TRACE("ПЕ=%d  use=%d \n",t.first,t.second);
 }

 void TestAAdmin::setAsymmetricSema(EsrKit& esr,bool b)
 {// отсутствие дополнительного входного светофора
	for(auto& way:getWays( esr))
		way->asymmetricSemafor=b;
 }

 M_INT_EV fltrStatEv(long esr,const TrainRoutes<AEvent>& trFrc)
 {
	 M_INT_VEV m;
	 for (const auto& t:trFrc.getSpotLines())
		 m[t.first.GetNumber().getNumber()]=t.second;
	 M_INT_EV res;
	 for (const auto& t: m)
	 {
		 AEvent evr(esr);
		 for (const auto& ev:t.second )
		 {
			 if(ev.Esr()==EsrKit(esr))
			 {
				 if(evr.diap.empty())
					 evr=ev;
				 else
					 evr.merge(ev);
			 }
		 }
		 if(!evr.diap.empty())
			res[t.first]=evr;
	 }
		return res;
 }

 M_INT_EV fltrStatEv(long esr,const AAdmin::M_NORM &v)
 {
	 M_INT_VEV m;
	 for (const auto& t:v)
	 {
		 m[t.first]=t.second.front();
	 }
	 M_INT_EV res;
	 for (const auto& t: m)
		 for (const auto& ev:t.second )
		 {
			 if(ev.Esr()==EsrKit(esr))
				 res[t.first]=ev;
		 }
		 return res;
 }
 