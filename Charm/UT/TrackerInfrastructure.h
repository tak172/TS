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
	const std::wstring axis_name; //������� �������

	TestElem Get( std::wstring ) const;
	void CreateStation( TrainField &, bool directly /*� ������ ��� ����������� �������*/ ) const;
	time_t DetermineStripBusy( std::wstring, TestTracker &, bool instantly = true ); //����������� ��������� �� �������
	time_t DetermineStripFree( std::wstring, TestTracker &, bool instantly = true, unsigned int shiftSec = 1 ); //����������� ����������� �� �������
	void DetermineStripLocked( std::wstring, TestTracker &, bool instantly = true ); //����������� "�������������" �� �������
	void DetermineStripUnlocked( std::wstring, TestTracker &, bool instantly = true ); //������ "�������������" � �������
	void DetermineSwitchPlus( std::wstring, TestTracker &, bool instantly = true ); //����������� ������� � ����
	void DetermineSwitchMinus( std::wstring, TestTracker &, bool instantly = true ); //����������� ������� � �����
	void DetermineSwitchOutOfControl( std::wstring, TestTracker &, bool instantly = true ); //����������� ������� ��� ��������
	void DetermineHeadOpen( std::wstring, TestTracker &, bool instantly = true ); //�������� ���������
	void DetermineHeadClose( std::wstring, TestTracker &, bool instantly = true ); //�������� ���������
	time_t DetermineRouteSet( std::wstring, TestTracker &, bool instantly = true ); //������� ��������
	void DetermineRouteCutting( std::wstring, TestTracker &, bool instantly = true ); //������ �������� �������� (������� ������)
	void DetermineRouteUnset( std::wstring, TestTracker &, bool instantly = true ); //������ (������ ��������) ��������
	void ImitateMotion( std::vector <std::wstring>, TestTracker & ); //�������� �������� �� ������������������ ��������
	typedef std::function<void( TrackerInfrastructure &, TestTracker &)> TIActivity;
	void DoActivities( const std::vector <TIActivity> &, TestTracker & );
	time_t FlushData( TestTracker &, bool parcel_type /*��� ������� (true = reference)*/, unsigned int shiftSec = 1 ); //�������� ������������� ��������� ����� ��������
	TestTracker * Tracker() { return &tracker; }
	TrainFieldPtr TField() { return tfieldPtr; }
	void Reset();
	time_t IncreaseTime( unsigned int incdeltaMs ); //� ������������ � Tracker
	time_t GetTimeMoment() const { return curmoment; }
	bool TakeExceptions() 
	{
		bool retexceptions = false;
		std::swap( retexceptions, exceptionCatched );
		return retexceptions;
	}

private:

	const EsrKit techStatCode; //���-��� ����������� �������

    std::shared_ptr<const TestGuessNsiBasis> nsiBasisPtr;
	TrainFieldPtr tfieldPtr;
	TestTracker tracker;
	mutable bool exceptionCatched;
	time_t curmoment;

	typedef std::pair<std::wstring, TestElem> StrElPair;
	
	FictivePart fictivePart;
	RealPart realPart;

	mutable std::list<std::wstring> cachedObjects; //��������� ��� ����������� ������ �������
	void GenerateXmlDoc( attic::a_document &, bool isRef, const std::vector<TestElem> & ) const;
	//��������� � Tracker ��������� ������ �������:
	time_t DetermineSingle( std::wstring, bool instantly, TestTracker &, unsigned int shiftSec = 1 );
	//��������� � Tracker ��������� ������ ��������:
	void DetermineBatch( const std::list <std::wstring> &, bool reference, TestTracker &, unsigned int shiftSec = 1 );
	std::map <BadgeE, unsigned int> MakeCoInfo( const TestElem & ) const;
	void UpdateRouteLoses( const std::vector<TestElem> &  ) const;
};