#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

//�������� ������������ ��������� ������� ������������ ��� �������� �� Hem

class TC_TrackerEntranceStopping : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	virtual void setUp();

private:
	CPPUNIT_TEST_SUITE( TC_TrackerEntranceStopping );
    CPPUNIT_TEST( DelayAndGenerate );
    CPPUNIT_TEST( ShortDelay );
    CPPUNIT_TEST( FormWOGeneration );
    CPPUNIT_TEST( ShortDelayAndMove );
    CPPUNIT_TEST( JointPicketage );
    CPPUNIT_TEST( SpanExitAndStop );
	CPPUNIT_TEST( BlockpostStopping );
	CPPUNIT_TEST( OpenSemaStopping );
	//CPPUNIT_TEST( StopEventOnSemaClose ); //��������������� ��-�� �������� �.5.3 �� ��������� ������� ����� ������� �� ����� ��� ����������
	CPPUNIT_TEST( NoStopEventOnSemaClose );
	CPPUNIT_TEST( ShortBlockpostFastPassing );
	CPPUNIT_TEST( DeathAfterStop );
	CPPUNIT_TEST( RecloseSemaAfterStop );
	CPPUNIT_TEST( LittleStopOnShortSpan );
	CPPUNIT_TEST( NoEntStopOnSingleStripSABWay );
	CPPUNIT_TEST( EntStopOnMultiStripSABWay );
	CPPUNIT_TEST( EntStopOnSingleStripFABWay );
	CPPUNIT_TEST_SUITE_END();

    void DelayAndGenerate(); //������� ����� ������� � ��������� �������
    void ShortDelay(); //������ ��������� ��� ��������
    void FormWOGeneration(); //������������ ������ ����� ���������� (������� �� ������������)
    void ShortDelayAndMove(); //�������� ������� ����� ������� � ����������� ������� �� �������
    void JointPicketage(); //�������� �������� ������ �������� ����� ����� �������� ���������
    void SpanExitAndStop(); //����� �� ������� � ��������� �� ����������
	void BlockpostStopping(); //������� ����� ����-������ (������� ������� ����� ������� �� ������������)
	void OpenSemaStopping(); //������� ����� �������� ����������
	void StopEventOnSemaClose(); //��������� ������� ������� ����� ������� �� �������� ���������
	void NoStopEventOnSemaClose(); //���������� ������� ������� ����� ������� �� �������� ������������ ���������
	void ShortBlockpostFastPassing(); //������� ������������� ��������� ����-����� �� �������� ����� ������� �� ��������� �������
	void DeathAfterStop(); //������ ������ ����� ��������� ����� �������
	void RecloseSemaAfterStop(); //������� ����� ������� � ����������� ��������� � ��������� ���������
	void LittleStopOnShortSpan(); //��������� ��������� �� �������� ��������
	void NoEntStopOnSingleStripSABWay(); //���������� ������� ����� ������� � ������ ���� �������� � ������������ �������� � ���
	void EntStopOnMultiStripSABWay(); //������� ����� ������� � ������ ���� �������� � ����������� ��������� � ���
	void EntStopOnSingleStripFABWay(); //������� ����� ������� � ������ ���� �������� � ������������ �������� � ��
};