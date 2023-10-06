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
	//----------- ��� �������� �������--------------
	using AAdmin::noSort;
	using AAdmin::getNorm;
	using AAdmin::getTrn;
	using AAdmin::getNsi;
	using AAdmin::prepareTrains;
	using AAdmin::sort;
	using AAdmin::fndAllNorm;// ��� ���� �� ������ ����������
	using AAdmin::rgSortCargo; // ����������� ���������� �������� �� ���� ��( #4451 ������������ �������� ������� ����� ���������)
	using AAdmin::allTrn;
	using AAdmin::tskOutrun;// ������� �� ������
	using AAdmin::fnFltr;// �-���  ������ ������� �� �������
	using AAdmin::numStepFrc;// ����.����� ����� �������� ��� ���������� ������� �� ���������
	using AAdmin::maxWaitCounterPack ;// ����� ������������� �������� ��� �������� ���������� ������ ��
	using AAdmin::deltaWait;// 3 ������ ����� ��� ������� ��� ���������� ������ 
	using AAdmin::deltaIntrusion;// ���.����� ��� ���������� ��������
	using AAdmin::useBackAdjust;// ������������ �������� ��������� ����� ���������� ���������
	using AAdmin::useAdjustFirstStepForecast;// ������������ ��������� ����������� �� ������ ���� ����� ������� ��������
	using AAdmin::regimLog;// ����� ������ ����
	using AAdmin::reductSt;// ��������� ��.���������� ��� �������� ����������
	using AAdmin::ADVANTAGE_BRUTTO;/*���������� ������ ��� ����.��, ������ ������������  ��� ���������*/
	using AAdmin::diffAdvBrutto;//���������� ������� ����� �������� ��������� � ��������� ������ �� ����� 10 �����.
	// --- ������ ������� �� ��������---
	using AAdmin::crossTmPass;//	��� ������������ ������� (1-998; 5001-7998) ����� t �� ��������� 3 ������.
	using AAdmin::crossTm;//	��� ��������� ������� (1001-4998; 8001-9798) ����� t �� ��������� 2,5 ������.
	using AAdmin::delayLockSema; /* �� ����������� �� ������������ ������� �� ���� � �������� ����������*/

	using AAdmin::tskWrng; /* �������� ��������� �� ������������ ������� �� ���� � �������� ����������*/
	using AAdmin::delayAsymmetricSema;	/*7.1.1.5.	� ������ �������� ������� �� ������������� ���� �� �������� ����, ��, �� 
									����������� � ����������� ������� ���� 2,5 ������ �� �������� ��� ���������� ��������������� �������� ���������*/
	using AAdmin::dispNormElSec;//  ���������� ���������� �� ���������� ��� ��.������
	using AAdmin::dispNorm;//  ���������� ���������� �� ���������� �� ��� ��.������
	using AAdmin::stopBeforeLockPath ;  /* ������� ����� ������� �� ���� � �������� ����������*/
	using AAdmin::stopBeforeLockPathWrong ;  /* ������� ����� ������� �� ���� � �������� ���������� �� ����.����*/
	using AAdmin::stopAfterLockPath; /* ������� ����� ������� � ���� � �������� ����������*/
	using AAdmin::dThrough;
	using AAdmin::gapStop;

	static bool checkOutrunTechStation;// ��������� ������ �� ���.��������
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
	static void StopBefAftLockPath(time_t t,SpanLockType p,TypeS tp,bool wrong); /* ������� ����� ������� �� ����.����*/
	ATrainPtr get(std::string num);// �������� �����(��� ���� )
	void setCrossesNum(EsrKit& esr,unsigned int num);// �����  ��������� �� ��������
	void setAsymmetricSema(EsrKit& esr,bool b);// ���������� ��������������� �������� ���������
	static std::vector<SpanWayInfoPtr> getWays( const EsrKit & ecode );
	void MinSmallStop(int t);// ����� ��������� ������� ���  ����������� � �������������
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
