#include "stdafx.h"
#include "TC_TrackerSpanStopping.h"
#include "../Guess/TrainContainer.h"

using namespace std;

static const std::string intervalSec_xAttr = "intervalSec";

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TrackerSpanStopping );

void TC_TrackerSpanStopping::setUp()
{
	TI.Reset();
	TimeConstraints & timConstr = tracker->GetTimeConstraints();
	timConstr.disappTrainsHolding = std::chrono::hours( 2 );
	timConstr.bufferLimitMs = 10 * 3600 * 1000; //������ �����
	timConstr.entranceStoppingLimitSec = 18 * 60; //18 �����
}

void TC_TrackerSpanStopping::CasualCaseWithLengthWithPicketage()
{
	//������� �� ������� W (����� 11000 ������, ���������� ����� ������� 5000 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 5000 ������ �� 50 ����� (3000 ������).

	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.IncreaseTime( 2900 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 200 * 1000 ); //��������� ������� �� ��������

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN ); 
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - WEntryMom );

	//��������� ������� �� ��������
	TI.DetermineStripBusy( L"H", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWithLengthWOPicketage()
{
	//������� �� ������� H (����� 7000 ������, ���������� ����� ������� ����������).
	//����� ������������ �� ���������������� ����� �������.
	//����� ������ �������� 7000 ������ �� 70 ����� (4200 ������).

	TI.DetermineStripBusy( L"A", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	time_t HEntryMom = TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"A", *tracker );
	TI.DetermineStripFree( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	TI.IncreaseTime( 4100 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 200 * 1000 ); //��������� ������� �� ��������

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - HEntryMom );

	//��������� ������� �� ��������
	time_t KEntryMom = TI.DetermineStripBusy( L"K", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == KEntryMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWOLengthWithPicketage()
{
	//������� �� ������� Q (����� �� ����������, ���������� ����� ������� 1550 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 1550 ������ �� 15.5 ����� (930 ������).

	TI.DetermineStripBusy( L"A", *tracker );
	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"A", *tracker );

	TI.IncreaseTime( 910 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 50 * 1000 ); //��������� ������� �� ��������

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - QEntryMom );

	//��������� ������� �� ��������
	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == WEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::CasualCaseWOLengthWOPicketage()
{
	//������� �� ������� X (����� �� ����������, ���������� ����� ������� ����������).
	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 1150 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 100 * 1000 ); //��������� ������� �� ��������

	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() );
	CPPUNIT_ASSERT( trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - XEntryMom );

	//��������� ������� �� ��������
	time_t IEntryMom = TI.DetermineStripBusy( L"I", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == IEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterReborn()
{
	//��������������� ���������� �� ������� X, ���������� � �������������� ������, ���������� � 
	//��������� ������ ������� �� ��������, ����������� �� ������� �� ���������� ������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //����� ����� �� X ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 800 * 1000 ); //����� ����������� ������������ �����, ����������� �������� ������� �� ��������
	time_t rebornMom = TI.DetermineStripBusy( L"X", *tracker ); //�������������� ��� ��

	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //������� ������� ������ ���� ����� ������������
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 8 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	auto spotDetPtr = ( *tevIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == rebornMom - XEntryMom );

	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterReborn2()
{
	//��������������� ���������� �� ������� X, ���������� � �������������� ������, ���������� � 
	//��������� ������ ������� �� ��������, ����������� �� ������� �� ���������� ������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //����� ����� �� X ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 500 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //������� ���
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	TI.IncreaseTime( 200 * 1000 );

	trackerEvents = tracker->GetTrainEvents(); //������� ����
	CPPUNIT_ASSERT( any_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterWideReborn()
{
	//��������������� ���������� �� ������� X, ���������� � �������������� �� ���������� ��������, ���������� � 
	//��������� ������ ������� �� ��������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //����� ����� ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"I", *tracker );
	TI.IncreaseTime( 800 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.FlushData( *tracker, false ); //������� ����

	tunities = tcont.GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->Head() == TI.Get( L"I" ).bdg );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( any_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
			return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
		} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterWideReborn()
{
	//��������������� ���������� �� ������� X, ���������� � �������������� �� ���������� ��������, �� ���������� � 
	//��������� ������ ������� �� ��������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //����� ����� ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 800 * 1000 );

	TI.DetermineStripBusy( L"X", *tracker, false );
	TI.DetermineStripBusy( L"I", *tracker, false );
	TI.FlushData( *tracker, false ); //������� ���

	tunities = tcont.GetUnities();

	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto curtrain = tunities.back();
	CPPUNIT_ASSERT( curtrain->Head() == TI.Get( L"I" ).bdg );

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRebornNear()
{
	//��������������� ���������� �� ������� X, ���������� � �������������� ������ �� �������� �������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 600 * 1000 ); //����� ����� �� X ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );

	TI.IncreaseTime( 800 * 1000 );

	auto rebornMom = TI.DetermineStripBusy( L"I", *tracker ); //�������������� �����

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 5 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStopAfterRebornAndWaiting()
{
	//������� ������� ������������ � �������� �������� ����� ���������������� ���������� � "�������" � ��������������.

	//���������� �� ������� X � ��������� ������ ������� �� ��������, ����������� � ������ ���������� ������
	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	time_t XEntryMom = TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 800 * 1000 ); //����� ����������� ��������������� ����� (������� �� �������� ��� ��� ���)
	TI.DetermineStripBusy( L"X", *tracker ); //�������������� ��� ��
	time_t currentMom = TI.IncreaseTime( 600 * 1000 ); //����� ����� �� X ��������� �����, ����������� ��� ��������� ������� �� ��������

	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //������� ������� ������ ���� ����� ������������
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 8 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	auto spotDetPtr = ( *tevIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - XEntryMom );

	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRelight()
{
	//��������������� ���������� �� ������� X, ���������� ���������� � ���������������� ������, �� ���������� � 
	//��������� ������ ������� �� ��������

	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).
	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();

	TI.DetermineStripFree( L"G", *tracker );

	TI.IncreaseTime( 600 * 1000 ); //����� ����� �� X ��������� �����

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 3 * 3600 * 1000 ); //����� ����������� ������������ �����, ����������� �������� ������� �� �������� � �������� ��� ��������������
	TI.DetermineStripBusy( L"X", *tracker ); //�������������� ��� ��
	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //������� ������� ������ ���� ����� ������������
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() != trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() != trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NonSpanStopAfterRelightAndWaiting()
{
	//������� ������� ������������ � �������� �������� ����� ���������������� ���������� � "�������" � ���������������� ��-�� ���������� ������ ��������������

	//���������� �� ������� X. ����� �������������� � �������� ������� �� �������� �����������.
	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).
	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();

	TI.DetermineStripFree( L"G", *tracker );

	//���������� �������� �������
	TI.DetermineStripFree( L"X", *tracker );
	TI.IncreaseTime( 3 * 3600 * 1000 ); //����� ����������� ��������������� �����
	TI.DetermineStripBusy( L"X", *tracker ); //�������������� ��� ��
	TI.DetermineStripBusy( L"I", *tracker );

	auto trackerEvents = tracker->GetTrainEvents(); //������� ������� ������ ���� ����� ������������
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::DEATH && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() != trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() != trainId );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::ParallelActivity()
{
	//������������ �� ������� X, ������������� �� �� ������ ��������, �������������� ������ �� ������� X

	const auto & tcont = tracker->GetTrainContainer();
	TI.DetermineStripBusy( L"G", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );

	auto tunities = tcont.GetUnities();
	CPPUNIT_ASSERT( tunities.size() == 1 );
	auto train = tunities.front();
	auto trainId = train->GetId();
	TI.DetermineStripFree( L"G", *tracker );
	TI.IncreaseTime( 1400 * 1000 ); //����� ��������� �� X ������������ �����, ����������� �������� ������� �� ��������
	TI.DetermineStripBusy( L"W", *tracker ); //��������� ������� ������ �� ������ ��������
	TI.DetermineStripBusy( L"I", *tracker ); //��������� ������� �� �������� ��������� ������

	auto trackerEvents = tracker->GetTrainEvents(); //������� ������� ������ ���� ����� ������������
	CPPUNIT_ASSERT( CheckTimeSequence( trackerEvents ) );
	CPPUNIT_ASSERT( trackerEvents.size() == 6 );
	auto tevIt = trackerEvents.cbegin();
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::FORM && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::STATION_EXIT && (*tevIt)->GetBadge() == TI.Get( L"G" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_BEGIN && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_STOPPING_END && (*tevIt)->GetBadge() == TI.Get( L"X" ).bdg && ( *tevIt )->GetId() == trainId );
	++tevIt;
	CPPUNIT_ASSERT( ( *tevIt )->GetCode() == HCode::SPAN_MOVE && (*tevIt)->GetBadge() == TI.Get( L"I" ).bdg && ( *tevIt )->GetId() == trainId );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingAndPocketEntry()
{
	//������� �� ������� PK3 (����� �� ����������, ���������� ����� ������� 440 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 440 ������ �� 4,4 ������ (264 �������).

	TI.DetermineStripBusy( L"PK2", *tracker );
	time_t PK2EntryMom = TI.DetermineStripBusy( L"PK3", *tracker );
	TI.DetermineStripFree( L"PK2", *tracker );

	TI.IncreaseTime( 260 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 10 * 1000 ); //��������� ������� �� ��������
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() && trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );
	auto spotDetPtr = trackerEvents.back()->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - PK2EntryMom );

	//��������� ������� �� �������� � ���� � ������
	time_t PocketEntryMom = TI.DetermineStripFree( L"PK3", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() && ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == PocketEntryMom );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() && ( *eventIt )->GetCode() == HCode::POCKET_ENTRY && ( *eventIt )->GetTime() == PocketEntryMom );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingIntoPocket()
{
	//������� � ������� 2 (����� ������� = 420 (PK9) + 1250 (PK10) + 2000 (PK11) = 3670 ������ ).
	//����� ������������ �� ����� ���� ���� ��������, �������� � ������.
	//����� ������ �������� 3670 ������ �� 36,7 ������ (2202 �������).

	TI.DetermineStripBusy( L"PK7", *tracker );
	TI.DetermineStripBusy( L"PK8", *tracker );
	TI.DetermineStripFree( L"PK7", *tracker );
	time_t PocketEntryMom = TI.DetermineStripFree( L"PK8", *tracker );

	TI.IncreaseTime( 2180 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	time_t currentMom = TI.IncreaseTime( 30 * 1000 ); //��������� ������� �� ��������
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );

	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() && ( *spanStopIt )->GetBadge() == TI.Get( L"PK10" ).bdg );
	auto spotDetPtr = ( *spanStopIt )->GetDetails();
	CPPUNIT_ASSERT( spotDetPtr && spotDetPtr->timeSecInterval == currentMom - PocketEntryMom );

	//��������� ������� �� �������� � ����� �� �������
	time_t PocketExitMom = TI.DetermineStripBusy( L"PK12", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK10" ).bdg );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::POCKET_EXIT && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK12" ).bdg );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_MOVE && ( *eventIt )->GetTime() == PocketExitMom && ( *eventIt )->GetBadge() == TI.Get( L"PK12" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SeveralStops()
{
	//������� �� ������� H (4200 ������) � ������� W (3000 ������)

	TI.DetermineStripBusy( L"K", *tracker );
	TI.DetermineStripBusy( L"H", *tracker );
	TI.DetermineStripFree( L"K", *tracker );

	//��������� ������ �������
	TI.IncreaseTime( 4220 * 1000 );

	time_t WEntryMom = TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripFree( L"H", *tracker );

	//��������� ������ �������
	TI.IncreaseTime( 3020 * 1000 );

	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );

	auto trackerEvents = tracker->GetTrainEvents();
	auto startStopStartCount = count_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( startStopStartCount == 2 );
	auto startStopEndCount = count_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( startStopEndCount == 2 );
	auto spanStopStartIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopStartIt != trackerEvents.cend() && ( *spanStopStartIt )->GetBadge() == TI.Get( L"H" ).bdg );
	auto spanStopEndIt = find_if( spanStopStartIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt != trackerEvents.cend() && ( *spanStopEndIt )->GetTime() == WEntryMom && ( *spanStopEndIt )->GetBadge() == TI.Get( L"H" ).bdg );
	spanStopStartIt = find_if( spanStopEndIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopStartIt != trackerEvents.cend() && ( *spanStopStartIt )->GetBadge() == TI.Get( L"W" ).bdg );
	spanStopEndIt = find_if( spanStopStartIt, trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt != trackerEvents.cend() && ( *spanStopEndIt )->GetTime() == QEntryMom && ( *spanStopEndIt )->GetBadge() == TI.Get( L"W" ).bdg );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::StopBeforeStation()
{
	//������� �� ������� Q (����� �� ����������, ���������� ����� ������� 1550 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 1550 ������ �� 15.5 ����� (930 ������).
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	TI.IncreaseTime( 910 * 1000 ); //������� �� �������� ��� ��� ���
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	TI.IncreaseTime( 50 * 1000 ); //��������� ������� �� ��������
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( !trackerEvents.empty() && trackerEvents.back()->GetCode() == HCode::SPAN_STOPPING_BEGIN );

	//��������� ������� �� ��������
	time_t AEntryMom = TI.DetermineStripBusy( L"A", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	auto eventIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );

	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	++eventIt;
	CPPUNIT_ASSERT( eventIt != trackerEvents.cend() );
	CPPUNIT_ASSERT( ( *eventIt )->GetCode() == HCode::SPAN_STOPPING_END && ( *eventIt )->GetTime() == AEntryMom );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::SpanStoppingEarlierThanEntraStopping()
{
	//������� �� ������� Q (����� �� ����������, ���������� ����� ������� 1550 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 1550 ������ �� 15.5 ����� (930 ������).

	//���������� �������� ������� ����� ������� ����� ������� �������� (18 ����� �� ���������)

	//����� �� Q
	TI.DetermineHeadClose( L"Head100", *tracker );
	TI.DetermineStripBusy( L"W", *tracker );
	time_t QEntryMom = TI.DetermineStripBusy( L"Q", *tracker );
	TI.DetermineStripFree( L"W", *tracker );

	//���������� ������� �� ��� ����, ��� �������� ��������� ���� ������� (����� ������� ������������ � ������������ ������������������)
	TI.IncreaseTime( 16 * 60 * 1000 ); //������� 16 �����
	TI.IncreaseTime( 4 * 60 * 1000 ); //������� 4 ������

	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() );
	trackerEvents = tracker->GetTrainEvents();
	auto entraStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( entraStopIt == trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::EntraStoppingEarlierThanSpanStopping()
{
	//������� �� ������� I (����� �� ����������, ���������� ����� ������� ����������).
	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	//���������� �������� ������� ����� ������� ����� ������� �������� (18 ����� �� ���������)

	//����� �� I
	TI.DetermineHeadClose( L"Head108", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	time_t IEntryMom = TI.DetermineStripBusy( L"I", *tracker );
	TI.DetermineStripFree( L"X", *tracker );

	//���������� ������� �� ��� ����, ��� �������� ��������� ���� ������� (����� ������� ������������ � ������������ ������������������)
	TI.IncreaseTime( 19 * 60 * 1000 ); //������� 19 �����
	TI.IncreaseTime( 2 * 60 * 1000 ); //������� 2 ������

	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );
	trackerEvents = tracker->GetTrainEvents();
	auto entraStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::ENTRANCE_STOPPING;
	} );
	CPPUNIT_ASSERT( entraStopIt != trackerEvents.cend() );

	//����� �� �������
	TI.DetermineStripBusy( L"J", *tracker );
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );
	auto spanStopEndIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_END;
	} );
	CPPUNIT_ASSERT( spanStopEndIt == trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::EntraStoppingEarlierThanSpanStopping2()
{
	//������� �� ������� I (����� �� ����������, ���������� ����� ������� ����������).
	//����� ������������ ������������� ��������� (2000 ������).
	//����� ������ �������� 2000 ������ �� 20 ����� (1200 ������).

	//���������� �������� ������� ����� ������� ����� ������� �������� (18 ����� �� ���������)

	//����� �� I
	TI.DetermineHeadClose( L"Head108", *tracker );
	TI.DetermineStripBusy( L"X", *tracker );
	TI.DetermineStripBusy( L"I", *tracker );

	TI.IncreaseTime( 19 * 60 * 1000 ); //������� 19 ����� (��������� ������� ����� �������)

	auto trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	auto backEvent = trackerEvents.back();
	CPPUNIT_ASSERT( backEvent->GetCode() == HCode::ENTRANCE_STOPPING );
	TI.DetermineHeadOpen( L"Head108", *tracker );

	TI.IncreaseTime( 5 * 60 * 1000 ); //����������� ������� �������������, ����������� ��� ��������� ������� �� ��������
	auto tunities = trainCont->GetUnities();
	TI.DetermineStripFree( L"X", *tracker ); //������������ ������, �������� ���������� � ��������� ������� �� ��������

	trackerEvents = tracker->GetTrainEvents();
	CPPUNIT_ASSERT( trackerEvents.size() == 3 );
	CPPUNIT_ASSERT( none_of( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} ) );

	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}

void TC_TrackerSpanStopping::NoTrainSpanStopping()
{
	//������� �� ������� W (����� 11000 ������, ���������� ����� ������� 5000 ������).
	//����� ������������ �� ���������� ����� �������.
	//����� ������ �������� 5000 ������ �� 50 ����� (3000 ������).
	TI.DetermineStripBusy( L"Q", *tracker, false );
	TI.DetermineStripBusy( L"W", *tracker, false );
	time_t bornMom = TI.FlushData( *tracker, false );
	TI.IncreaseTime( 3100 * 1000 ); //������� �� �������� ���� �� ������, �.�. ��� �� �����
	auto trackerEvents = tracker->GetTrainEvents();
	auto spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt == trackerEvents.cend() );

	//�� ��������� � ���������� �������
	//������� �� ������� H (����� 7000 ������, ���������� ����� ������� ����������).
	//����� ������������ �� ���������������� ����� �������.
	//����� ������ �������� 7000 ������ �� 70 ����� (4200 ������).
	TI.DetermineStripBusy( L"H", *tracker );
	TI.IncreaseTime( 4250 * 1000 ); //���� ������� �� ��������
	trackerEvents = tracker->GetTrainEvents();
	spanStopIt = find_if( trackerEvents.cbegin(), trackerEvents.cend(), []( TrackerEventPtr teventPtr ){
		return teventPtr->GetCode() == HCode::SPAN_STOPPING_BEGIN;
	} );
	CPPUNIT_ASSERT( spanStopIt != trackerEvents.cend() );
	
	CPPUNIT_ASSERT( !TI.TakeExceptions() );
	CPPUNIT_ASSERT( tracker->CheckIntegrity() );
}