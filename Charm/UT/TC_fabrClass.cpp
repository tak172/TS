/*!
	created:	2017/01/27
	created: 	11:33  27 Январь 2017
	filename: 	D:\potap\Charm\UT\TC_fabrClass.cpp
	file path:	D:\potap\Charm\UT
	file base:	TC_fabrClass
	file ext:	cpp
	author:		 Dremin
	
	purpose:	
  
  */
 
#include "stdafx.h"
#include "TC_fabrClass.h"
#include "../helpful/StatIntervals.h"
#include "../helpful/NsiBasis.h"
#include "../Augur/ATrain.h"
#include "../Augur/RailLimitation.h"
#include "TC_data.h"
#include "../helpful/Amqp/RestClient.h"
#include "../helpful/RestrictSpeed.h"
#include "../helpful/ConjugateWaysLimit.h"
#include "TestAAdmin.h"

using namespace std;

static ADProperties::Attrubute tbl[]={		
	ADProperties::Attrubute::PASSENGER , //путь пассажирский
	ADProperties::Attrubute::MAJOR, //путь главный
	ADProperties::Attrubute::EMBARKATION //путь предназначен для посадки и высадки пассажиров (только для пассажирских)
	// 	NON_STOP_PASSING, //безостановочный пропуск
	// 	BID_CARGO //запрет для грузовых поездов
};

TC_Atopology::TC_Atopology( const TC_AtopologyParams & tplParams ):
prop( ARRAY_IT( tbl ),Oddness::UNDEF,WeightNorms())// для пасс., для посадки , главный
{

	const auto & statsCount = tplParams.stationsNum;
	const auto & nPath = tplParams.statPathsNum;
	const auto & nPark = tplParams.statParksNum;

	map <EsrKit, SpansRegistry::SpanInfo> spanInfos;
	map <EsrKit, StationsRegistry::StationInfo> statInfos;
	for ( const auto & ttPair : tplParams.techTime )
	{
		statInfos[ttPair.first].techTime = ttPair.second;
		statInfos[ttPair.first].departByHPath=true;
	}
	
	statCodes.emplace_back( startStatCode );
	for ( unsigned int j = 1; j < statsCount - 1; ++j )
		statCodes.emplace_back( startStatCode + j );
	statCodes.emplace_back( finishStatCode );
	switch (tplParams.ft)
	{
	case ForkTopology::ONE_FORK:
		{
			unsigned fLine2=statsCount/3;
			unsigned fL=statsCount-2*fLine2;
			vector<EsrKit> sL(statCodes.begin(),statCodes.begin()+fL), sL1(statCodes.begin()+fL-1,statCodes.begin()+fL+fLine2),
				sL2(statCodes.begin()+fL+fLine2,statCodes.end());
			sL2.insert(sL2.begin(),sL1.front());
			lineStations(sL,tplParams,spanInfos,statInfos);// построение линии станций
			lineStations(sL1,tplParams,spanInfos,statInfos);// построение линии станций
			lineStations(sL2,tplParams,spanInfos,statInfos);// построение линии станций
		}
		break;
	case ForkTopology::NO_FORK: 
		lineStations(statCodes,tplParams,spanInfos,statInfos);// построение линии станций
		break;
	}
	StatWayVec statVec;
	for ( unsigned int pth = 1; pth < nPath + 1; pth++ )
		for ( unsigned int prk = 1; prk < nPark + 1; prk++ )
		{
			StatWayInfo::Section section( L"", 0, 0, prop);
			statVec.push_back(StatWayInfoCPtr(new StatWayInfo(pth,prk,vector<StatWayInfo::Section>( 1, section ))));//пути станции
		}
	statInfos[EsrKit(statCodes.back())].statWays = statVec;

	TestAAdmin::setNsi( shared_ptr<TestAugurNsiBasis>( new TestAugurNsiBasis( spanInfos, statInfos ) ) );
	TestAAdmin::dThrough[1]=5;
	TestAAdmin::gapStop[1]=4;
}

void TC_Atopology::lineStations(std::vector<EsrKit> l,const TC_AtopologyParams & tplParams, map <EsrKit, SpansRegistry::SpanInfo>& spanInfos
									,map <EsrKit, StationsRegistry::StationInfo>& statInfos)
{// построение линии станций
	const auto & statsCount = tplParams.stationsNum;
	const auto & spanBreadth = tplParams.typicalSpanBreadth;
	const auto & spanLockType = tplParams.typicalSpanLock;
	const auto & spansWOWays = tplParams.spansWOWays;
	for (const auto& t: l)
		tplParams.buildStation(t.getTerm(),statInfos);
	for ( unsigned int k = 0; k < l.size() - 1; ++k )
	{
		SpanWayVec spanVec;
		auto curCode = l[k];
		auto nextCode = l[k+1];
		EsrKit spanCode( curCode.getTerm(),nextCode.getTerm() );
		spanCodes.emplace_back( spanCode );
		if ( spansWOWays.find( spanCode ) == spansWOWays.cend() )
		{
			for (int j = 1; j < ( spanBreadth == SpanBreadth::DOUBLE_WAY_SPAN ? 3 : 2 ); j++)
				spanVec.push_back(SpanWayInfoCPtr(new SpanWayInfo(j, spanLockType, SPAN_LENGHT_KM/*5*/, spanBreadth == SpanBreadth::ONE_WAY_SPAN ? ANY_PARITY : ( j & 1 ? ODD_PARITY : EVEN_PARITY ) )));// перегон
		}
		spanInfos[spanCode].spanWays=spanVec;
	}
}

V_EVNT TC_Atopology::bld(Sevent&t, int num) const
{
	V_EVNT v;
	int fn=num&1?1:-1;
	time_t tCurr=t.or;
	ASSERT( t.e1 > 0 );
	const auto startCode = EsrKit(t.e1);
	auto orderedCodes = statCodes;
	if ( fn == -1 )
		reverse( orderedCodes.begin(), orderedCodes.end() );
	auto startIt = find( orderedCodes.cbegin(), orderedCodes.cend(), startCode );
	ASSERT( startIt != orderedCodes.cend() );
	
	//зачистка вектора с обоих концов
	auto initOrdCodes = orderedCodes; //до сжатия
	orderedCodes.erase( orderedCodes.cbegin(), startIt );
	if (t.nSt<(int)orderedCodes.size())
		orderedCodes.erase( orderedCodes.cbegin() + t.nSt, orderedCodes.cend() );
	for ( auto scIt = orderedCodes.cbegin(); scIt != orderedCodes.cend(); ++scIt )
	{
		const EsrKit & statCode = *scIt;
		if(t.tp==AEvent::FIXED)
		{
			AEvent ev( statCode.getTerm() );
			ev.setSource(t.tp);
			ev.setType(t.tSt?AEvent::STOP_START:AEvent::MOVE);
			ev.diap.set(tCurr,tCurr+t.tSt);
			ev.setObj(fn>0?1:2,1);
			ev.adjustType();
			v.push_back(ev);
			tCurr+=t.tSt;
		}
		else
		{
			AEvent ev( statCode.getTerm() );
			ev.setSource(t.tp);
			ev.setType(t.tSt?AEvent::STOP_START:AEvent::MOVE);
			ev.diap.set(tCurr,tCurr+t.tSt/2);
			ev.setObj(fn>0?1:2,1);
			v.push_back(ev);
			bool lastCode = ( scIt == --orderedCodes.cend() );
			if( !lastCode || t.tEndSp)
			{
				ev.diap.set(tCurr+2+t.tSt/2,tCurr+t.tSt);
				v.push_back(ev);
				tCurr+=t.tSt;
				EsrKit nextCode( 99999 ); //выход за пределы топологии
				if ( !lastCode )
				{
					auto nextIt = scIt;
					++nextIt;
					nextCode = *nextIt;
				}
				else
				{
					auto iocIt = find( initOrdCodes.cbegin(), initOrdCodes.cend(), statCode );
					ASSERT( iocIt != initOrdCodes.cend() );
					++iocIt;
					if ( iocIt != initOrdCodes.cend() )
						nextCode = *iocIt;
				}
				AEvent ev1( statCode.getTerm(), nextCode.getTerm() );
				SpanWayVec rs=AAdmin::allPath<SpanWayInfo>(ev1.Esr());  
				if(rs.size()>1)
					ev1.setObj(fn>0?1:2,1);
				else
					ev1.setObj(1);
				ev1.setSource(t.tp);
				ev1.setType(AEvent::MOVE);
				ev1.diap.set(tCurr+2,tCurr+( !lastCode ? 1 : t.tEndSp ));
				if(ev1.diap.duration())
				{
					v.push_back(ev1);
					ev1.diap.set(ev1.diap.getEnd()+2,ev1.diap.getEnd()+3);
					v.push_back(ev1);
				}
			}
		}
		tCurr+=t.tSp;
	}
	return v;
}


ATrainPtr TC_Atopology::add(const TrainDescr & tdescr,V_EVNT&v)
{// добавить нить с событиями 
	AThread thr(v);
	return ATrainPtr( new ATrain(tdescr,&thr,nullptr) );
}

ATrainPtr TC_Atopology::add(const TrainDescr & tdescr,Sevent&tReal,Sevent* tFix,int path, Touch tp)
{
    unsigned const trainNumber = tdescr.GetNumber().getNumber();
	V_EVNT r=bld(tReal, trainNumber);
	if(tReal.endStop && !r.empty())
	{
		auto ev=r.back();
		ev.setType(AEvent::STOP);
		ev.diap.set(ev.diap.getEnd(),ev.diap.getEnd()+tReal.endStop);
		r.push_back(ev);
	}
	if(tFix)
	{
		V_EVNT fix=bld(*tFix, trainNumber);
 		if(tFix->tEndSp)
 		{// непрерывная нить событий
 			V_EVNT::iterator it=fix.begin();
 			V_EVNT v(1,*it);
 			it++;
 			for (;it!=fix.end();it++)
 			{
 				AEvent ev(v.back().Esr().getTerm(),it->Esr().getTerm());
 				ev.diap.set(v.back().diap.getEnd(),v.back().diap.getEnd());
 				ev.setSource(AEvent::FIXED);
 				v.push_back(ev);
 				v.push_back(*it);
 			}
			bool odd=tdescr.GetNumber().getNumber()%2?true:false;
			switch (tp)
			{
			case TC_Atopology::BOTH:
			case TC_Atopology::FIRST:
				{
					AEvent ev=v.front();
					auto esr=ev.Esr().getTerm();
					ev.resetEsr(EsrKit(esr,esr+ (odd?-1:1)));
					ev.setType(AEvent::MOVE);
					ev.setObj(0);
					ev.diap.set(ev.diap.getOrig(),ev.diap.getOrig());
					v.insert(v.begin(),ev);
				}
				if(tp==TC_Atopology::FIRST)
					break;
			case TC_Atopology::END:
				{
					AEvent ev=v.back();
					auto esr=ev.Esr().getTerm();
					ev.resetEsr(EsrKit(esr,esr+ (odd?1:-1)));
					ev.setType(AEvent::MOVE);
					ev.setObj(0);
					ev.diap.set(ev.diap.getEnd(),ev.diap.getEnd());
					v.push_back(ev);
				}
				break;
			}
 			r.insert(r.end(),v.begin(),v.end());
 		}
 		else
 			r.insert(r.end(),fix.begin(),fix.end());
	}
	AThread thr(r);
	for (auto& ev:thr)
		if(!ev.onSpan() && path)
			ev.setObj(path,ev.Park());
	return ATrainPtr( new ATrain(tdescr,&thr,nullptr) );
}

V_EVNT TC_Atopology::norma(Sevent&t,int num)
{// построить нормативку
	V_EVNT r=bld(t, num);
	r=AThread(r).compress();
	V_EVNT nr;
	for (auto& t: r)
		if(!t.onSpan() && (nr.empty() || nr.back().Esr()!=t.Esr()))
		{
			t.setObj(0);
			nr.push_back(t);
		}
	return nr;
}

ATrainPtr TC_Atopology::add(Strain&t,int path, Touch tp)
{//примыкание событий на перегоне нет, в начале, в конце, оба и в начале и в конце
	Sevent* tFix=&t.evn[1];
	if(!tFix->e1 ||!tFix->nSt )
	{
		tFix=nullptr;
		tp=NONE;
	}
	return add(t.tdescr,t.evn[0],tFix,path,tp);
}

ATrainPtr TC_Atopology::add(const TrainDescr &descr, AThread*thrR)
{// добавить нить с событиями типа real и FIX
	return ATrainPtr(new ATrain(descr,thrR,nullptr));
}

RailLimitationCPtr TC_Atopology::tech(Stech&t)
{
	shared_ptr<RestrictSpeed> restrictSpeedPtr;
	if ( t.maxVelPtr )
	{
		restrictSpeedPtr.reset( new RestrictSpeed() );
		(*restrictSpeedPtr)[RestrictSpeed::ALL] = *t.maxVelPtr;
	}
	
	return RailLimitationCPtr( make_shared<const RailLimitation>( RailLimitation( restrictSpeedPtr, ADiap(t.or,t.or+t.count)
	,t.kmLen>.001?std::make_shared<double>(t.kmLen) : nullptr, t.sR||t.sW?shared_ptr<ConjugateWaysLimit>(new ConjugateWaysLimit(t.sR,t.sW)) :nullptr ) ) );
}

WayInfoCPtr TC_Atopology::fnd(Stech& t)
{
	if(t.e1 && t.e2)
		return TestAAdmin::getNsi()->spansRegPtr->getWay(EsrKit(t.e1,t.e2),t.path);
	return TestAAdmin::getNsi()->stationsRegPtr->getWay(EsrKit(max(t.e1,t.e2)),t.path,t.park);
}

void TC_Atopology::setPvx(int pvh,st_intervals_t* s)
{
	auto statTime = pvh;
	st_intervals_t   tmp(5.f);
	if(!s)
		s=&tmp;
	for ( const auto& sc :statCodes)
	{
		set<EsrKit> v;
		for (const auto& t: spanCodes)
			if(t.occured(sc.getTerm()))
				v.insert(EsrKit(t.getSpan().first==sc.getTerm()?t.getSpan().second:t.getSpan().first ));
		for(set<EsrKit> ::iterator it=v.begin();it!=v.end();it++)
			for (set<EsrKit>::iterator itn=it;itn!=v.end();itn++)
				if(itn!=it)
				{
					addStationInterval( sc,*it,*itn, s);
					addStationInterval( sc,*itn,*it, s);
				}
				else
					addStationInterval(sc,EsrKit(),*it,s);
	}
	for ( const auto & spanCode : spanCodes )
		addSpanInterval( spanCode, pvh );
}

void TC_Atopology::setPvx( int pvh, SttMove::Type tp )
{
	auto stt = TestAAdmin::getSpanTravelTime();
	for ( const auto & spanCode : spanCodes )
	{
		SpanTravelTimeInfo sp(EsrKit( spanCode.getSpan().first ), EsrKit( spanCode.getSpan().second ));

		STT_Val sV = stt->GetSpanTime( sp );
		switch (tp)
		{
		case SttMove::Type::TIME_PASS:
			sV.set_pass( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
			break;
		case SttMove::Type::TIME_GOODS:
			sV.set_gruz( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
			break;
		case SttMove::Type::TIME_ELSECTION:
			sV.set_elek( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
			break;
		case SttMove::Type::TIME_DIESEL:
			sV.set_dies( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
			break;
		}
        stt->SetSpanTime( sp, sV );
        stt->SetSpanTime( sp.invert(), sV );
	}
}

void TC_Atopology::setPvx(const STT_Val& v)
{
	for ( const auto & spanCode : spanCodes )
	{
		auto esr=spanCode.getSpan();
		addSpanInterval(esr,v);
		esr=make_pair(esr.second,esr.first);
		addSpanInterval(esr,v);
	}
}

void TC_Atopology::setPvx(const STT_Val& v,const MAP_SPANTIME& m )
{
	for ( const auto & spanCode : spanCodes )
	{
		auto esr=spanCode.getSpan();
		if(m.find(esr)==m.end())
			addSpanInterval(esr,v);
		else
			addSpanInterval(esr,m.find(esr)->second);
		esr=make_pair(esr.second,esr.first);
		if(m.find(esr)==m.end())
			addSpanInterval(esr,v);
		else
			addSpanInterval(esr,m.find(esr)->second);
	}
}

st_intervals_t TC_Atopology::getInterval(StationEsr esr,StationEsr eNext)
{
	return AAdmin::getNsi()->stIntPtr->getInterval(esr,eNext);
}

STT_Cat_Val TC_Atopology::getPVX(std::pair<int,int>* p,SttMove::Type tp)
{
	auto stt = TestAAdmin::getNsi()->spanTravTimePtr; // ПВХ
	if( p )
	{
		SpanTravelTimeInfo sp(EsrKit( p->first ), EsrKit( p->second ));
		auto s = stt->GetSpanTime(sp);
		if( auto p = s.get_data( (int)tp ) )
			return *p;
	}
	else
    {
        for ( const auto & spanCode : spanCodes )
        {
            auto ps=spanCode.getSpan();
            SpanTravelTimeInfo sp(EsrKit( ps.first ), EsrKit( ps.second ));
            auto s= stt->GetSpanTime(sp);
            if( auto p = s.get_data( (int)tp ) )
                return *p;
        }
    }
	return STT_Cat_Val(0,0,0,0);
}


void TC_Atopology::setStInt(const st_intervals_t& v)
{
	for ( auto scIt = statCodes.cbegin(); scIt != statCodes.cend(); ++scIt )
	{
		EsrKit nextCode;
		if ( scIt != --statCodes.cend() )
		{
			auto nextIt = scIt;
			++nextIt;
			nextCode = *nextIt;
		}
		EsrKit prevCode;
		if ( scIt != statCodes.cbegin() )
		{
			auto prevIt = scIt;
			--prevIt;
			prevCode = *prevIt;
		}
		addStationInterval( *scIt, prevCode, nextCode, &v);
	}
}

void TC_Atopology::addStationInterval( const EsrKit & statCode, const EsrKit & prevStatCode, const EsrKit & nextStatCode, int pvh )
{
	auto csint=TestAAdmin::getNsi()->stIntPtr;// коллекция станц.интервалов
	st_intervals_t st;

	st.tsk=chrono::seconds( pvh/10+1 );
	st.tnp=st.tsk+chrono::seconds( 11 );
	st.tn=st.tnp+chrono::seconds( 17 );
	st.tpr=st.tot=st.tp=st.tbsk=st.tsk+chrono::seconds( 1 );
	st.tpbo=st.tp+chrono::seconds( 7 );
	st.Ipr=st.tsk+chrono::seconds( 10 );
	st.Iot=st.tsk+chrono::seconds( 15 );
	st.Ipk=st.tsk+chrono::seconds( 20 );
	auto nextEsr = nextStatCode.getTerm();
	csint->add(statCode.getTerm(), &nextEsr, st);

	st.tsk+=chrono::seconds( 2 );
	st.tnp=st.tsk+chrono::seconds( 11 );
	st.tn=st.tnp+chrono::seconds( 17 );
	st.tpr=st.tot=st.tp=st.tbsk=st.tsk+chrono::seconds( 1 );
	st.tpbo=st.tp+chrono::seconds( 7 );
	st.Ipr=st.tsk+chrono::seconds( 10 );
	st.Iot=st.tsk+chrono::seconds( 15 );
	st.Ipk=st.tsk+chrono::seconds( 20 );
	nextEsr = prevStatCode.getTerm();
	csint->add(statCode.getTerm(), &nextEsr, st);
}

void TC_Atopology::addStationInterval(const EsrKit & statCode, const EsrKit & prevStatCode, const EsrKit & nextStatCode,const st_intervals_t* s)
{
	auto csint=TestAAdmin::getNsi()->stIntPtr;// коллекция станц.интервалов
	auto nextEsr = nextStatCode.getTerm();
	csint->add(statCode.getTerm(), &nextEsr, *s);
	nextEsr = prevStatCode.getTerm();
	csint->add(statCode.getTerm(), &nextEsr, *s);
}

void TC_Atopology::addSpanInterval( const EsrKit & spanCode, int pvh )
{
	STT_Val v;
	v.set_pass( pvh/4,pvh,pvh/6,pvh/5);
	pvh+=INCR_PVX_CAT;
	v.set_elek( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
	pvh+=INCR_PVX_CAT;
	v.set_dies( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
	pvh+=INCR_PVX_CAT;
	v.set_gruz( pvh*COEFF_START,pvh,pvh*COEFF_RESERV,pvh*COEFF_STOP );
	auto sp=spanCode.getSpan();
	addSpanInterval(sp,v);
	sp=make_pair(sp.second,sp.first);
	addSpanInterval(sp,v);
}

void TC_Atopology::addSpanInterval( const std::pair<unsigned, unsigned>& spanCode, const STT_Val& v )
{
	auto stt = TestAAdmin::getSpanTravelTime(); // ПВХ
	SpanTravelTimeInfo sp(EsrKit( spanCode.first ), EsrKit( spanCode.second ));
	stt->SetSpanTime(sp,v);

}

Spvx::Spvx(int e,int p,int x/*=0*/ ):e1(e+startStatCode),e2(x?0:e+x+startStatCode)
	,pvx(static_cast<int>(p*(x?1:PVX_COEFF_2_1))){}


Stech::Stech( time_t mom, time_t dur, int _path, int _park, int _e1, int _e2, shared_ptr<unsigned int> _maxVelPtr ) :
	or( mom ), count( dur ), path( _path ), park( _park )
	, e1( _e1 + startStatCode), e2( _e2+startStatCode ), maxVelPtr( _maxVelPtr )
	, kmLen(0.)// размер области  действия
	, sR(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по верному пути
	, sW(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по неверному пути
{}

Stech::Stech(time_t mom, time_t dur, int _path, int _park, int _e1):
	or( mom ), count( dur ), path( _path ), park( _park )
	, e1( _e1), e2( 0), maxVelPtr()
	, kmLen(0.)// размер области  действия
	, sR(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по верному пути
	, sW(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по неверному пути
{}


Stech::Stech( time_t mom, time_t dur, int _path, int _park, int _e1 , std::shared_ptr<unsigned int> _maxVelPtr):
	or( mom ), count( dur ), path( _path ), park( _park )
	, e1( _e1), e2( 0), maxVelPtr(_maxVelPtr)
	, kmLen(0.)// размер области  действия
	, sR(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по верному пути
	, sW(make_shared<SpanLockType>(SpanLockType::BIDIR_AUTOBLOCK))// по неверному пути
{}


Sevent::Sevent( AEvent::Source evsrc, time_t mom, int _tSt, int _tSp, int _tendSp, int _e1, int _nSt ,int _endStop):// добавочный stop в конце событий
	tp( evsrc ), or( mom ), tSt( _tSt ), tSp( _tSp ), tEndSp( _tendSp ), e1( _e1+startStatCode ), nSt( _nSt ),
endStop(_endStop)// добавочный stop в конце событий
{}

//********************************************************
TC_AtopologyParams::StationDetalization::StationDetalization( const NsiBasis::StationKind & stkind, const std::list<StationsRegistry::TransrouteDescr> & _transroutes ) :
	statKind( stkind ), transroutes( _transroutes )
{
	if ( ( statKind == NsiBasis::StationKind::Blockpost || statKind == NsiBasis::StationKind::Hybrid ) && transroutes.empty() )
	{
		//ASSERT( !transroutes.empty() ); //по хорошему здесь должен быть ассерт
		//принудительное создание какого-нибудь сквозного маршрута (для непротиворечивости указанным типам)
		transroutes.emplace_back( StationsRegistry::TransrouteDescr( EsrKit( 77000, 88000 ), EsrKit( 88000, 99000 ) ) );
	}
}

void TC_AtopologyParams::init(const std::vector<ADProperties::Attrubute>& v)
{
	TC_StatPath stP;
	for (const auto& t: v)
		stP.add(t);
// 	for ( auto& sc:stP.secs )
// 	{
// 		auto pr=sc.GetADProperties();
// 		pr.SetAttribute(ADProperties::Attrubute::PASSENGER);
// 		sc=StatWayInfo::Section(0,0,pr);
// 	}
 	for (unsigned int n=0;n<stationsNum;n++ )
 		for (unsigned int p=0;p<statPathsNum;p++ )
 			add(n+startStatCode,p+1,stP);
 /*	PATH_TC paths;
 	for (unsigned int p=0;p<statPathsNum;p++ )
 		paths[p+1]=stP;
 	
 	for (unsigned int n=0;n<stationsNum;n++ )
 		mStat[n+startStatCode]=paths;*/
}

void TC_AtopologyParams::addPropWay(std::vector<ADProperties::Attrubute>& v)
{
	init(v);
}

TC_AtopologyParams::TC_AtopologyParams(unsigned int stationsCount, unsigned int pathsNum, unsigned int parksNum
									   , SpanBreadth spanBreadth, SpanLockType spanLock 
									   , TechTimeCMap & techtimemap): 
									   ft(NO_FORK),stationsNum( stationsCount ), statPathsNum( pathsNum ), statParksNum( parksNum ), 
									   typicalSpanBreadth( spanBreadth ), typicalSpanLock( spanLock ), techTime( techtimemap )
									   ,kmBtwn(1)// километров  между станциями
									   {init(std::vector<ADProperties::Attrubute>(1,ADProperties::Attrubute::PASSENGER));}

TC_AtopologyParams::TC_AtopologyParams(ForkTopology f, unsigned int stationsCount, unsigned int pathsNum
									   , unsigned int parksNum, SpanBreadth spanBreadth
									   , SpanLockType spanLock , TechTimeCMap & techtimemap) : 
ft(f),stationsNum( stationsCount ), statPathsNum( pathsNum ), statParksNum( parksNum ), 
	typicalSpanBreadth( spanBreadth ), typicalSpanLock( spanLock ), techTime( techtimemap )
	,kmBtwn(1)// километров  между станциями
{init(std::vector<ADProperties::Attrubute>(1,ADProperties::Attrubute::PASSENGER));}

void TC_AtopologyParams::add(int stat,int path,TC_StatPath& t)
{
	STATION_TC::iterator it=mStat.find(stat);
	if(it!=mStat.end())
		mStat.insert(std::pair<int,PATH_TC>(stat,PATH_TC()));
	mStat[stat][path]=t;//	(it->second)[path]=t;
}

void TC_AtopologyParams::buildStation(StationEsr esr, map <EsrKit, StationsRegistry::StationInfo>& statInfos)const
{
	const auto & nPath = statPathsNum;
	const auto & nPark = statParksNum;
	StatWayVec res;

	auto sdIt = stationDetails.find( esr );
	NsiBasis::StationKind statKind = NsiBasis::StationKind::Casual; //по умолчанию считается обычной станцией
	if ( sdIt != stationDetails.cend() )
		statKind = sdIt->second.statKind;
	StationsRegistry::RoutesDefinitionPtr routesDefPtr;
	if ( statKind == NsiBasis::StationKind::Blockpost || statKind == NsiBasis::StationKind::Hybrid || statKind == NsiBasis::StationKind::Casual )
		routesDefPtr.reset( new StationsRegistry::RoutesDefinition() );
	if ( statKind == NsiBasis::StationKind::Casual || statKind == NsiBasis::StationKind::Hybrid )
	{
		STATION_TC::const_iterator it=mStat.find(esr);// описание путей на станциях
		if(it!= mStat.end())
			for ( unsigned int prk = 1; prk < nPark + 1; prk++ )
			{
				std::for_each(it->second.begin(),it->second.end(),[prk,&res](const NUM_PATH_TC& t){
					V_SEC v;
					for (const auto& sec:t.second.secs )
						v.emplace_back( L"", sec.Capacity(Oddness::EVEN), sec.Capacity(Oddness::ODD), sec.GetADProperties());
					StatWayInfoCPtr p(new StatWayInfo(t.first,prk,v));
					res.push_back(p);//пути станции
				});
			}
	}
	if ( statKind == NsiBasis::StationKind::Blockpost || statKind == NsiBasis::StationKind::Hybrid )
	{
		ASSERT( routesDefPtr && sdIt != stationDetails.cend() );
		if ( routesDefPtr && sdIt != stationDetails.cend() )
			routesDefPtr->transroutes = sdIt->second.transroutes;
	}

	auto esrK=EsrKit(esr);
	StationsRegistry::StationInfo& info=statInfos[esrK];
	info.statWays = res;
	info.routesDefPtr = routesDefPtr;
	rwRuledCoord  v(L"alfa",rwCoord(esr*kmBtwn,0));	// километров  между станциями
	info.pickInfo.SetCoord(  v );
	if(departureByHemPath.find(esrK)!=departureByHemPath.end())
		info.departByHPath=true;// где искать привязку к нормативкеc
}

void TC_AtopologyParams::setCapacity(unsigned esr, int capacity)
{// установка емкости путей на станции
	auto it=mStat.find(esr);// описание путей на станциях
	ASSERT(it!=mStat.end());
	for ( auto& t:it->second)
		for ( auto& s:t.second.secs )
		{
			StatWayInfo::Section tmp(L"", capacity, capacity, s.GetADProperties());
			s=tmp;
		}
}

void TC_AtopologyParams::setCapacity( int capacity)
{// установка емкости путей на станции
	for ( auto& st:mStat)// описание путей на станциях
		for ( auto& pt: st.second)
			for ( auto& s:pt.second.secs )
			{
				StatWayInfo::Section tmp(L"", capacity, capacity, s.GetADProperties());
				s=tmp;
			}
}

TC_StatPath::TC_StatPath()
{
	add(ADProperties(),0);
}

TC_StatPath::TC_StatPath(ADProperties& p,int c/*=1 */)
{
	add(p,c);
}

void TC_StatPath::add(ADProperties& p,int c/*=1 */)
{
	secs.push_back(StatWayInfo::Section(L"",c, c, p));
}

void TC_StatPath::add(ADProperties::Attrubute t)
{
	V_SEC tmp;
	for (const auto& s:secs )
	{
		auto pr=s.GetADProperties();
		pr.SetAttribute(t);
		tmp.push_back(StatWayInfo::Section(L"", s.Capacity(Oddness::EVEN), s.Capacity(Oddness::ODD), pr));
	}
	secs=tmp;
}

STT_Val generateSTT(int start,int full, int reserv,int stop)
{
	STT_Val pvx;
	pvx.set_gruz( start, full, reserv, stop );
	pvx.set_pass( start, full, reserv, stop );
	pvx.set_dies( start, full, reserv, stop );
	pvx.set_elek( start, full, reserv, stop );
	return pvx;
}
