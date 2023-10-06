/*!
	created:	2016/12/21
	created: 	16:57  21 ������� 2016
	filename: 	D:\potap\Charm\UT\TC_Thread.h
	file path:	D:\potap\Charm\UT
	file base:	TC_Thread
	file ext:	h
	author:		 Dremin
	
	purpose:	���� �������/�����
 */
#ifndef TC_Thread_h__
#define TC_Thread_h__
#include <cppunit/extensions/HelperMacros.h>
#include "../Augur/DefType.h"
class TestAAdmin;
struct NsiBasis;
struct SData;

class TC_AThread : 
	public CPPUNIT_NS::TestFixture
{
	void tstTRACE(const NsiBasis& bs,bool full=false);
	void tstTRACE(const TestAAdmin& adm, bool full=false);
	void tstTRACEtrn(const TestAAdmin& adm );
	void testSemiLock_(bool dbl);
	static void outTRACE(std::string t);
	static void outTRACE(std::wstring t);
private:
	CPPUNIT_TEST_SUITE( TC_AThread );
	CPPUNIT_TEST(testTopStart);// ���� ��� ��� �����������
	CPPUNIT_TEST(testPassOutrun);// ������� ��� ������ ��� �������� ������
	CPPUNIT_TEST(testAdjstFixSpanPassTrn);// ������������ ����.������� �� �������� ��� ���� �� ���� � �������� ��
	CPPUNIT_TEST(testOpposedStopRoute);// �� ���������� �������� �� ��������
	CPPUNIT_TEST(testOpposedRoute);// �� ���������� ���� � ��� �� ����
	CPPUNIT_TEST(testStopStationToOutrun);// ���� ������� ������� ���������� ��� ������� �� �����
	CPPUNIT_TEST(testAccordPath);// ���� ������� ���� �� �������
	CPPUNIT_TEST(testOpposedDisblPath);// ������ ���� ��� ����� �����.���������
	CPPUNIT_TEST(testManagePath);//�������� ������������� ����� �� ������� (�����.���������)
	CPPUNIT_TEST(testStationPathEndNorm);//����  c �������� ���� �� ������� � ����� �������� �� ����������
	CPPUNIT_TEST(testResortTrn);// ����� ������� �������� ��
	CPPUNIT_TEST(testGenerateIntrusion);// ��������� ������� �� ������� ��� ������������ ��������� � ������ �� �������
	CPPUNIT_TEST(testManyBackAdjust) ;// �������������  ������������ �� � ������ � ������� �� � ������
	CPPUNIT_TEST(testMorthStopMove);// ���� ����������� ������� � ������������� (������ � �����)
	CPPUNIT_TEST(testTnTsk);// ���� �����.����������
	CPPUNIT_TEST(testWndViaOpposed);// ���� ���� �� �������� ��� �������� ������������� ���������
	CPPUNIT_TEST(testModifyFrstStep);// ���� ��������� ����������� � �������� 
	CPPUNIT_TEST(testTopMove);// �������� �� Top ��� ������������� ������� �� �������
	CPPUNIT_TEST( testAdvEvent);//�������� c ������� ������ ��������
	CPPUNIT_TEST(testLnkRealErrorPvx);// �������� ��� ������� �������� ���� � ���������� ���
	CPPUNIT_TEST(testLinkFixFrc);// ���� ������� ����. � ���������� ���������
	CPPUNIT_TEST(testReLinkFixSpan);// ������������  ��������� ����������� � ��������
	CPPUNIT_TEST(testLnkFixArrival);// �������� ����.��������
	CPPUNIT_TEST(testFixFrcLine);// ���������� �������� � ������ �������������� ��������
	CPPUNIT_TEST(testAdjstStatFixWarning);// ������������ ��� �������� �� ������� ����� �������� � ���������������
	CPPUNIT_TEST(testAdjstFixSpanOnWnd);// ������������ ����.������� �� �������� ��� ���� �� ����
	CPPUNIT_TEST(testLnkFixDepart);// �������� �� �������� ����� ��. � ����.����������� � ���
	CPPUNIT_TEST(testAdjstFixWarning);// ������������ ��� �������� �� �������� � ���������������
	CPPUNIT_TEST(testFixLnk);// �������� � ���� ����� ���
	CPPUNIT_TEST(testBackAdjustFixMoveStop);// �������� ������������ ��� ��������� ���� ������������� �� �������
	CPPUNIT_TEST(testAdjstFixStopViaStart);// ������������ ���� �� �������� � ����. ��������/�����������
	CPPUNIT_TEST(testAdjstFixStopViaSpan);// ������������ ����. �������� � ���� �� �������� +��������
	CPPUNIT_TEST( testAdjustIntrusion);// ������������ ��� ���������� �������
	CPPUNIT_TEST(testPackViaPack); // ���� ���������� �� ������� ������ �������
	CPPUNIT_TEST(testIntrusionPackNoBan);
	CPPUNIT_TEST(testBanIntrusion);//������ ��� ������� �� ������ ������ c �������� �����������.��
	CPPUNIT_TEST(testBackAdjustIntrusion);// �������� ������������ �������� 
	CPPUNIT_TEST(testIntrusionNoRoot);// ���� �������� � ����������� �� �� �������� �������
	CPPUNIT_TEST( testConflCounterViaDiapBreakService);// �������� ���������� ��������� ��������� � ��������� ������� �������� ��������
	CPPUNIT_TEST(testRelinkNormTech);// ���� �������������� ���������� �� ���.������� � ����������� ���������
	CPPUNIT_TEST(testIntrusionPack);// ������� �� ������ ������
	CPPUNIT_TEST(testBlockPost);// ���� �� ���������� ���������� �������������� ������ ��������������� ������� (�������������).
	CPPUNIT_TEST(testBackAdjustSpan);// ��������  ������������ ��� �������� �� ��������
	CPPUNIT_TEST(testAbsenseStationPath);// ���������� ���� ��� �������
	CPPUNIT_TEST(testHardClashPassWnd);// ������� ������� ���������� c ��������� ��� ���� �� ��������
	CPPUNIT_TEST(testHardClashWnd);// ������� ������� ����������  ��� ���� �� ��������
	CPPUNIT_TEST(testManyHardClashWnd);// ������� �������� ���������� ��� ��������������� ���������� � ���� �� ��������
	CPPUNIT_TEST(testMisOutrun);// ��������� ������ ������� ��-�� �������� ����� �� �������
	CPPUNIT_TEST(testAdvFrc);// ���������� �������� � ��������� �������
	CPPUNIT_TEST(testFixTechSt);// �������� �������� �� ���.�������
	CPPUNIT_TEST(testOutrunInfo);// ���� ����� ���������� �� �������
	CPPUNIT_TEST(testStopOnSpan); // ���� ������� �� ��������
	CPPUNIT_TEST(testHardClash);// ������� ������� ����������  c ���������� �����
	CPPUNIT_TEST(testBackAdjust);// ���� �������� ��������� ����� ���������
	CPPUNIT_TEST(testConflCounterTechSt);// �������� � ��������� �� ���.�������
	CPPUNIT_TEST(testFixDepartMove);// �������� ����������� � ��������� �� ���������
	CPPUNIT_TEST(testFndPVX);// ������ ��� ��� ������ ����� ��
	CPPUNIT_TEST(testRejectArtifact);// ���� ������ ���������� ����������
	CPPUNIT_TEST(testAdvantageBrutto);// ���� �� ������������ �������� ��������
	CPPUNIT_TEST(testInflConfl);//��������� � �������������� (�����������  �������� ����� ��� ������� ���������� ���������)
	CPPUNIT_TEST(testOutrunFltrTask);// ���� ������� ��� ������� �� �������
	CPPUNIT_TEST(testNormNormStrong);// ���� ���������� ����� ��. ��� ������ �� ����������
	CPPUNIT_TEST(testNormNormInterval);// ���� ���������� ����� ��. ��� �� ����������
	CPPUNIT_TEST(testWaitlock);
	CPPUNIT_TEST(testCatchNorm);// �������� ����������
	CPPUNIT_TEST(testFixFrcConfl);// ���������� �������� � ������� �������������� �������� � ���������� ��� �������� � ����.���������
	CPPUNIT_TEST(testOutrunTechStation);// ����� �������� �� ���.������� ��� ������ ����������
	CPPUNIT_TEST(testELECTRIC_TOKENbndWndNorm); //��������������� ���������� � �� �� ������� ���� c ������.��������
	CPPUNIT_TEST(testCounterTELEPHONE_CONNECTION); //���������� �������� ����� ��� ���������
	CPPUNIT_TEST(testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS);//������������� �������������� � ��������� �� �������� ������������ ����������
	CPPUNIT_TEST(testPartWindow);// ����������� ������� ����
	CPPUNIT_TEST(testCrossBndWnd);// ����������� ������� ����( ���������� ������� �����)
	CPPUNIT_TEST(testELECTRIC_TOKEN); //��������������� ���������� ��� ��������
	CPPUNIT_TEST(testFixFrc);// ���������� �������� � ������� �������������� ��������
	CPPUNIT_TEST(testTELEPHONE_CONNECTION); //���������� �������� ����� ��� ��������
 	CPPUNIT_TEST(testExcepToOutrun);// ������� ������������� ������� ���� � ������� ��� ������
	CPPUNIT_TEST(testCounterELECTRIC_TOKEN); //��������������� ���������� ��� ���������
	CPPUNIT_TEST(testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS);//������������� �������������� � ��������� �� �������� ������������ ����������
	CPPUNIT_TEST(testIntrusionYieldMany);//����� ������� �� ����������� ����� ������� ��
	CPPUNIT_TEST(testWarnOnSpanFix);// ���� ������������� ���������� �� �������� ��� ��������
	CPPUNIT_TEST(testELECTRIC_TOKENbndWnd); //��������������� ���������� � �� �� ������� ����
	CPPUNIT_TEST(testConflInWindow);//�������� � ���� � �������� �� ����.����
	CPPUNIT_TEST(testAutoBlockDiffSpeed);// ����������� �� � ������ ��������� ��� ������������� 
	CPPUNIT_TEST(testAutoBlockPack);// ����������� ������ ��� ������������� 
	CPPUNIT_TEST(testConflStat);// �������� ��-�� ��������� ����� �� ������� (����������� ������� �� ����.�������)
	CPPUNIT_TEST(testFndNormOccupy);// ����� ��������� ���������� ��� ����������� �� ������� c �������� ��� ��
	CPPUNIT_TEST(testIntrusionShouldYieldForOtherCounter);
	CPPUNIT_TEST(testStationPath);//���� ������� ����� �� �������  ��� ������� � ��
	CPPUNIT_TEST(testFirstStepConfl);//���� ������ ��� �������� c ���������� �� ��������
	CPPUNIT_TEST(testMorphFrcReal);//����������� ������� � �������� � ����������� �� �������� �������� ���� 
	CPPUNIT_TEST(testMorphFrcFrc);//����������� ������� � �������� � ����������� �� �������� ���������� ���� 
	CPPUNIT_TEST(testMorphPack);//����������� ������� � �������� � ����������� �� �������� ���������� ���� 
	CPPUNIT_TEST(testMorphConflReal);//����������� ������� � �������� � ����������� �� �������� �������� ���� ��� ���������
	CPPUNIT_TEST(testFixSpeedupPass);// ����� �������������� �������
	CPPUNIT_TEST(testPackConflNextStation);//�������� � ������� �������� �� ��.������� � ��������� �������� �������
	
	CPPUNIT_TEST( testWrongPath );// ������ �� ������������� ����
	CPPUNIT_TEST( testLimStatPath);// ����� ���� ��� ������������� ��� �������������� (����������� ��������) �� �����.����

	CPPUNIT_TEST(testProxySpan);// ���� ������ ��������
	CPPUNIT_TEST(testFiltrPassEquPr);// ������ ������� �������� ����������������
	CPPUNIT_TEST(testTrOnSt);//  ������ ��� �������� ��� �� �� ������� � ���������� ��������� � �������� ����
	CPPUNIT_TEST(testTnpRegim);// ���� ������ ����� ��������� �� � ������, ���� ��� ������ ��������� � ����������, ��  ����� �� ���������.
	CPPUNIT_TEST(testWaitCounterPack);// ������� �� ����.�������� ���������� ������
	CPPUNIT_TEST( testReductionStInterval);// ��������� (���������� ) ��.���������� ��� �������� �� ���������� 
	CPPUNIT_TEST(testAllowPackWait); // ���� ���������� �� ������� �������
	CPPUNIT_TEST(testPassOutrunResort);// ���� ������ �������� ������� ����� ���������������� �������� ��
	CPPUNIT_TEST(testResort);//  ���� �������������� ������ ��
	CPPUNIT_TEST(testTopInterval);// ���� ����� Top (����������� �������� ���������������� ����������� � ��������� ��������, ��� �������, ��� ������� �� �������� ���� �������������� ��� ���������)
	CPPUNIT_TEST(testDeadlock);// ���� ������
	CPPUNIT_TEST(testIntrusionShouldYieldForOtherPass);// ������� �� ����������� ����� ������� ��
	CPPUNIT_TEST(testIntrusionCounterCorrection);// ������� �� ����������� ����� ������� �� c ��������������
	CPPUNIT_TEST(testAdjustFirstStep);// ���� ������������. ��������� ����������� �������� ���� �� ������� � �������� �� ���������� ����������� ��������. ���� ���������� ����������.
	CPPUNIT_TEST(testPrognosisFeedback);// ��������� ������ ��� ������
	CPPUNIT_TEST(testIntrusionTransitStations); // ������� �� ���������� ��������
	CPPUNIT_TEST(testSkrechArr);// ���� �� ����� ���������� ��������� ��� �� �� �������
	CPPUNIT_TEST(testUpOrderIntrusion);// ���� ������������� �� �������
	CPPUNIT_TEST(testFixFrcSpan);// ���� �������� �� �� ��������
	CPPUNIT_TEST(testAdvFix);// ���������� �������� � ��������� ������� � ����.��������� 
	CPPUNIT_TEST(testWarnOnSpan);// ���� ������������� ���������� �� ��������
	CPPUNIT_TEST(testFrstStep);// ������ ��� - ����������� �� ������� � ��������, �������� ������� �� ���������� �� �������
	CPPUNIT_TEST(testSortSecPath);// ���� ����������  ����� ����������� 
	CPPUNIT_TEST(testEmptyStationPathStop);//����  c �����������  ����� ��� ������� �� ������� 
	CPPUNIT_TEST(testFixOnSpan);// ���� ��������� ����.������� ��������� �� �������� ( ����)
	CPPUNIT_TEST( testFixSpan);// ���� ��������� ����.������� �� �������� ( ������������+ ����)
	CPPUNIT_TEST( testDrawThread);// #4809 	��������������� ������������ �����
	CPPUNIT_TEST(testAuditArch);// ���� ������� �������
	CPPUNIT_TEST(testMoveFix);// ���������� �������� � ������������� �������������� 
	CPPUNIT_TEST(testNormOnTechStation);// ���� ����� �� ������� � ���.������������� �� ����������
	CPPUNIT_TEST(testLimNormSpeedupSpan);// ���� �������������� �� �������� (�����.�������) ��� ������� ������� ����������
	CPPUNIT_TEST(testLimWarnSpan);// ���� �������������� �� �������� (�����.�������)
	CPPUNIT_TEST(testFewEvPrepare);// ��������� ������� �� ��������
	CPPUNIT_TEST(testArch);// ���� ���������
	CPPUNIT_TEST(testTrnEmptyInd);// ����� �������� ��� �� ��� �������
	CPPUNIT_TEST(testOutrunTask);// ���� ������� �� ������
	CPPUNIT_TEST(testOutrunUser);// ���� �������� ������������ �� ������
	CPPUNIT_TEST(testProxyStation);// ���� ������ �������
	CPPUNIT_TEST(testFixSimplFrc);// ���������� �������� �������� � ������� �������������� ��������
	CPPUNIT_TEST(testSortCargoTr);// �������� ���������� �������� ��
	CPPUNIT_TEST(testTypeEvIdealFrc);// ���� ������� � ��������� ��������
	CPPUNIT_TEST(testTechStation);// ���� ������� � ���.�������������
	CPPUNIT_TEST(testSkrech);// ���� ����� ���������� ��������� 
	CPPUNIT_TEST(testIdealForTrGoods);// ���������� ���������� �������� ��� ��������� ��
	CPPUNIT_TEST(testTrainStation);// ���� �� �� ������� ��� ��������
	CPPUNIT_TEST(testConfigNum);// ���� ������� ������������
	CPPUNIT_TEST(testCurrTime);// ���� ���������� � �������� ���� ������� � ������� �������� �������� 
	CPPUNIT_TEST(testSortTrn); // ���� ���������� �������
	CPPUNIT_TEST(testNormWnd);// ����� ���� ��� �������� �� ����������
	CPPUNIT_TEST( testFiltr );// ���������� � ������ ���.������
	CPPUNIT_TEST(testOnePath);// ������ �� ��������� (���� �� ��������)
	CPPUNIT_TEST(testSemiLock);// ���� ��� ��� (������������������)
	CPPUNIT_TEST(testTopology);
	CPPUNIT_TEST_SUITE_END();

	SData & sd;

public:
	TC_AThread();
	void testPassOutrun();// ������� ��� ������ ��� �������� ������
	void testSpanFrstStp();// ���� ����������� �� � ��������
	void testSkrechArr();// ���� �� ����� ���������� ��������� ��� �� �� �������
	void testSortCargoTr();// �������� ���������� �������� ��
	void testNormWnd();// ����� ���� ��� �������� �� ����������
	void testFiltr();
	void testTopology();
	void testBldIdeal();// ���������� ���������� �������� ����������+���
	void testPass();
	void testFrstStep();// ������ ��� - ����������� �� ������� � ��������, �������� ������� �� ���������� �� �������
	void testCatchNorm();// �������� ����������
	void testConflStat();// �������� ��-�� ��������� ����� �� ������� (����������� ������� �� ����.�������)
	void testOnePath();// ������ �� ������.���� (���� �� ��������)
	void testWrongPath();// ������ �� ������������� ����
	void testDeadlock();// ���� ������
	void testWaitlock();//  �������� ������� ����� �� �������
	void testSemiLock();// ���� ��� ��� (������������������)
	void testEmptyTrain();// ���� ������ ���� ��
	void testSkrech();// ���� ����� ���������� ��������� 
	void testOutrunInfo();// ���� ����� ���������� �� �������
	void testProxyStation();// ���� ������ �������
	void testProxySpan();// ���� ������ ��������
	void testAccordPath();// ���� ������� ���� �� �������
	void testTechStation();// ���� ������� � ���.�������������
	void testNormOnTechStation();// ���� ����� �� ������� � ���.������������� �� ����������
	void testVarForecast();// ���� ���������� ����������� �������  � ��������� ����
	void testVarForecastCreate();//���� �������� ����������� ������� ��� ����
	void testFirstStepWrongWay();//���� ������ ��� �������� �� ������.����
	void testCurrTime();// ���� ���������� � �������� ���� ������� � ������� �������� �������� 
	void testTopInterval();// ���� ����� Top (����������� �������� ���������������� ����������� � ��������� ��������, ��� �������, ��� ������� �� �������� ���� �������������� ��� ���������)
	void testBlockPost();// ���� �� ���������� ���������� �������������� ������ ��������������� ������� (�������������).
	void testSortTrn();// ���� ���������� �������
	void testStopOnSpan(); // ���� ������� �� ��������
	void testConfigNum();// ���� ������� ������������
	void testTrainStation();// ���� �� �� ������� ��� ��������
	void testIdealForTrGoods();// ���������� ���������� �������� ��� ��������� ��
	void testTrnEmptyInd();// ����� �������� ��� �� ��� �������
	void testNoInfoTrn();//����.����� ��� ���� ������ "��������������" ��������� �� ���� ������� �� ����������� ������� �������������� �� ������������ �������.
	void testTypeEvIdealFrc();// ���� ������� � ��������� ��������
	void testProperNorm();// ������ ��������� ���������� ��� ����������� �� �������
	void testFndPVX();// ������ ��� ��� ������ ����� ��
	void testFndProNorm();// ����� ��������� ���������� ��� ����������� �� ������� � ����������
	void testFixSimplFrc();// ���������� �������� �������� � ������� �������������� ��������
	void testFixFrc();// ���������� �������� � ������� �������������� ��������
	void testFixFrcLine();// ���������� �������� � ������ �������������� ��������
	void testFixFrcConfl();// ���������� �������� � ������� �������������� �������� � ���������� ��� �������� � ����.���������
	void testFixFrcPoss();// ������� � ������� �������������� �������� � ���������� ��� ������� �����
	void testOutrunUser();// ���� �������� ������������ �� ������
	void testOutrunTask();// ���� ������� �� ������
	void testArch();// ���� ���������
	void testFewEvPrepare();// ��������� ������� �� ��������
	void testFirstStepConfl();//���� ������ ��� �������� c ���������� �� ��������
	void testTrOnSt();//  ������ ��� �������� ��� �� �� ������� � ���������� ��������� � �������� ����
	void testLimWarnSpan();// ���� �������������� �� �������� (�����.�������)
	void testLimNormSpeedupSpan();// ���� �������������� �� �������� (�����.�������) ��� ������� ������� ����������
	void testStationPath();//���� ������� ����� �� �������  ��� ������� � ��
	void testOutrunFltrTask();// ���� ������� ��� ������� �� �������
	void testEmptyStationPathStop();//����  c �����������  ����� ��� ������� �� ������� 
	void testStationPathEndNorm();//����  c �������� ���� �� ������� � ����� �������� �� ����������
	void testFndNormOccupy();// ����� ��������� ���������� ��� ����������� �� ������� c �������� ��� ��
	void testExcepToOutrun();// ������� ������������� ������� ���� � ������� ��� ������
	void testAdvFrc();// ���������� �������� � ��������� �������
	void testMoveFix();// ���������� �������� � ������������� �������������� 
	void testAuditArch();// ���� ������� �������
	void testFixFrcSpan();// ���� �������� �� �� ��������
	void testFixSpan();// ���� ��������� ����.������� �� �������� ( ������������+ ����)
	void testDrawThread();// #4809 	��������������� ������������ �����
	void testFixOnSpan();// ���� ��������� ����.������� ��������� �� �������� ( ����)
	void testSortSecPath();// ���� ����������  ����� ����������� 
	void testLinkFixFrc();// ���� ������� ����. � ���������� ���������
	void testWarnOnSpan();// ���� ������������� ���������� �� ��������
	void testWarnOnSpanFix();// ���� ������������� ���������� �� �������� ��� ��������
	void testWaitCounterPack();// ������� �� ����.�������� ���������� ������
	void testAdvFix();// ���������� �������� � ��������� ������� � ����.��������� 
	void testStopStationToOutrun();// ���� ������� ������� ���������� ��� ������� �� �����
	void testUpOrderIntrusion();// ���� ������������� �� �������
	void testFiltrPassEquPr();// ������ ������� �������� ����������������
	void testIntrusionShouldYieldForOtherPass();// ������� �� ����������� ����� ������� ��
	void testIntrusionShouldYieldForOtherCounter();// ������� �� ����������� ����� ������� ��
	void testIntrusionTransitStations(); // ������� �� ���������� ��������
	void testIntrusionCounterCorrection();// ������� �� ����������� ����� ������� �� c ��������������
	void testPrognosisFeedback();// ��������� ������ ��� ������
	void testBackAdjust();// ���� �������� ��������� ����� ���������
	void testAdjustFirstStep();// ���� ������������. ��������� ����������� �������� ���� �� ������� � �������� �� ���������� ����������� ��������. ���� ���������� ����������.
	void testRejectArtifact();// ���� ������ ���������� ����������
	void testAllowPackWait(); // ���� ���������� �� ������� �������
	void testResort();//  ���� �������������� ������ ��
	void testPassOutrunResort();// ���� ������� �������� ������� ����� ���������������� �������� ��
	void testIntrusionPack();// ������� �� ������ ������
	void testBanIntrusion();//������ ��� ������� �� ������ ������ c �������� �����������.��
	void testIntrusionPackNoBan();
	void testOutrunTechStation();// ����� �������� �� ���.������� ��� ������ ����������
	void testConflCounterViaDiapBreakService();// �������� ���������� ��������� ��������� � ��������� ������� �������� ��������
	void testMorphFrcReal();//����������� ������� � �������� � ����������� �� �������� �������� ���� 
	void testReductionStInterval();// ��������� (���������� ) ��.���������� ��� �������� �� ����������  
	void testMorphFrcFrc();//����������� ������� � �������� � ����������� �� �������� ���������� ���� 
	void testPackViaPack(); // ���� ���������� �� ������� ������ �������
	void testAdvantageBrutto();// ���� �� ������������ �������� ��������
	void testModifyFrstStep();// ���� ��������� ����������� � �������� 
	void testRelinkNormTech();// ���� �������������� ���������� �� ���.������� � ����������� ���������
	void testMorthStopMove();// ���� ����������� ������� � ������������� (������ � �����)
	void testTnpRegim();// ���� ������ ����� ��������� �� � ������, ���� ��� ������ ��������� � ����������, ��  ����� �� ���������.
	void testIntrusionNoRoot();// ���� �������� � ����������� �� �� �������� �������
	void testMorphConflReal();//����������� ������� � �������� � ����������� �� �������� �������� ���� ��� ��������� 
	void testInflConfl();//��������� � �������������� (�����������  �������� ����� ��� ������� ���������� ���������)
	// ��� ��������
	void testBIDIR_AUTOBLOCK_WITH_LOCOSEMAS();//������������� �������������� � ��������� �� �������� ������������ ����������
	void testELECTRIC_TOKEN(); //��������������� ����������
	void testTELEPHONE_CONNECTION(); //���������� �������� �����
	void testELECTRIC_TOKENbndWnd(); //��������������� ���������� � �� �� ������� ����
	// ��� ���������
	void testCounterBIDIR_AUTOBLOCK_WITH_LOCOSEMAS();//������������� �������������� � ��������� �� �������� ������������ ����������
	void testCounterELECTRIC_TOKEN(); //��������������� ����������
	void testCounterTELEPHONE_CONNECTION(); //���������� �������� �����
	void testLimStatPath();// ����� ���� ��� ������������� ��� �������������� (����������� ��������) �� �����.����
	void testPackConflNextStation();//�������� � ������� �������� �� ��.������� � ��������� �������� �������
	void testBackAdjustIntrusion();// �������� ������������ �������� 
	void testFixSpeedupPass();// ����� �������������� �������
	void testMorphPack();//����������� ������� � �������� � ����������� �� �������� ���������� ���� 
	void testFixTechSt();// �������� �������� �� ���.�������
	void testBackAdjustSpan();// ��������  ������������ ��� �������� �� ��������
	void testAutoBlockPack();// ����������� ������ ��� ������������� 
	void testAutoBlockDiffSpeed();// ����������� �� � ������ ��������� ��� ������������� 
	void testIntrusionYieldMany();//����� ������� �� ����������� ����� ������� ��
	void testConflInWindow();//�������� � ���� � �������� �� ����.����
	void testMisOutrun();// ��������� ������ ������� ��-�� �������� ����� �� �������
	void testPartWindow();// ����������� ������� ����
	void testELECTRIC_TOKENbndWndNorm(); //��������������� ���������� � �� �� ������� ���� c ������.��������
	void testFixLnk();// �������� � ���� ����� ���
	void testLnkFixDepart();// �������� �� �������� ����� ��. � ����.����������� � ���
	void testFixDepartMove();// �������� ����������� � ��������� �� ���������
	void testNormNormInterval();// ���� ���������� ����� ��. ��� �� ����������
	void testOpposedRoute();// �� ���������� ���� � ��� �� ����
	void testOpposedStopRoute();// �� ���������� �������� �� ��������
	void testNormNormStrong();// ���� ���������� ����� ��. ��� ������ �� ����������
	void testCrossBndWnd();// ����������� ������� ����( ���������� ������� �����)
	void testAdjstFixWarning();// ������������ ��� �������� �� �������� � ���������������
	void testAdjstStatFixWarning();// ������������ ��� �������� �� ������� ����� �������� � ���������������
	void testBackAdjustFixMoveStop();// �������� ������������ ��� ��������� ���� ������������� �� �������
	void testAdjustIntrusion();// ������������ ��� ���������� �������
	void testTopStart();// ���� ��� ��� �����������
	void testWndViaOpposed();// ���� ���� �� �������� ��� �������� ������������� ���������
	void testManagePath();//�������� ������������� ����� �� ������� (�����.���������)
	void testConflCounterTechSt();// �������� � ��������� �� ���.�������
	void testHardClash();// ������� ������� ����������  c ���������� �����
	void testTnTsk();// ���� �����.����������
	void testHardClashWnd();// ������� ������� ����������  ��� ���� �� ��������
	void testManyHardClashWnd();// ������� �������� ���������� ��� ��������������� ���������� � ���� �� ��������
	void testHardClashPassWnd();// ������� ������� ���������� c ��������� ��� ���� �� ��������
	void testAbsenseStationPath();// ���������� ���� ��� �������
 	void testResortTrn();// ����� ������� �������� ��
	void testTopMove();// �������� �� Top ��� ������������� ������� �� �������
	void testAdjstFixStopViaStart();// ������������ ���� �� �������� � ����. ��������/�����������
	void testAdjstFixStopViaSpan();// ������������ ����. �������� � ���� �� �������� +��������
	void testAdjstFixSpanOnWnd();// ������������ ����.������� �� �������� ��� ���� �� ����
	void testLnkFixArrival();// �������� ����.��������
	void testReLinkFixSpan();// ������������  ��������� ����������� � ��������
	void testLnkRealErrorPvx();// �������� ��� ������� �������� ���� � ���������� ���
	void testAdvEvent();//�������� c ������� ������ ��������
	void testManyBackAdjust() ;// �������������  ������������ �� � ������ � ������� �� � ������
	void testGenerateIntrusion();// ��������� ������� �� ������� ��� ������������ ��������� � ������ �� �������
	void testOpposedDisblPath();// ������ ���� ��� ����� �����.���������
	void testAdjstFixSpanPassTrn();// ������������ ����.������� �� �������� ��� ���� �� ���� � �������� ��
};


#endif // TC_Thread_h__

