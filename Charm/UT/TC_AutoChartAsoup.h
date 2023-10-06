#ifndef TC_AUTOCHARTASOUP_H
#define TC_AUTOCHARTASOUP_H

#include <set>
#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"
#include "../Hem/HemHelpful.h"
#include "../helpful/Attic.h"
#include "UtTearDown.h"

//������������ AutoChart.AsoupLayer

class AutoChartTest;
class TC_AutoChartAsoup : 
	protected TC_ChartImpl,
	public UtTearDowner
{
public:
	TC_AutoChartAsoup();
	~TC_AutoChartAsoup();

private:
	TrackerInfrastructure & TI;
	boost::scoped_ptr<AutoChartTest> autoChart;
	CPPUNIT_TEST_SUITE( TC_AutoChartAsoup );
	CPPUNIT_TEST( AsoupMessage );
	CPPUNIT_TEST( ArrivingAsoupBeforeLOS );
	CPPUNIT_TEST( ArrivingAsoupAfterLOS );
	CPPUNIT_TEST( ParseRawAsoupText );
	CPPUNIT_TEST( WithdrawalAsoupMessage );
	CPPUNIT_TEST( WithdrawalDuplicateMessages );
	CPPUNIT_TEST( ShrinkAsoup );
	//CPPUNIT_TEST( UralMessage );
	CPPUNIT_TEST( ArrivingAsoupAfterLOSRealBadges );
	CPPUNIT_TEST( ServedChanges );
	CPPUNIT_TEST( WithdrawalAsoupMessageUndoGlue );
	CPPUNIT_TEST( WithdrawalAsoupMessageUndoGlueWithSpanStopping );
	CPPUNIT_TEST_SUITE_END();
//
protected:
	void AsoupMessage(); //������� �������� ��� ����� �������
	void ArrivingAsoupBeforeLOS(); //�����-��������� � �������� ��������� ������� �� ������� ���������
	void ArrivingAsoupAfterLOS(); //�����-��������� � �������� ��������� ������� ����� ������� ���������
	void ParseRawAsoupText(); //������ ��������� ����� � ����� ����� ���������, �������� ������� ��������� ���������
	void WithdrawalAsoupMessage(); //������ ��������� �� ������ ������� ����� ��������� (������� ��������)
	void WithdrawalDuplicateMessages(); //�������� ��������� ��� ������������ ��������� �����
	void ShrinkAsoup(); // �������� �������� ������ ���������
	//void UralMessage(); //�������� ��� ���-���� �������
	void ArrivingAsoupAfterLOSRealBadges();
	void ServedChanges();
	void WithdrawalAsoupMessageUndoGlue();
	void WithdrawalAsoupMessageUndoGlueWithSpanStopping();

private:
	bool PassAsoupMessageThrough(const std::wstring& messageText);
};
#endif // TC_AUTOCHARTASOUP_H