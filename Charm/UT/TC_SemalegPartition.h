#pragma once

#include "TC_Project.h"

class TC_SemalegPartition : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_SemalegPartition );
	CPPUNIT_TEST( StationCase );
	// CPPUNIT_TEST( SpanCase );
	CPPUNIT_TEST( StationUnresolvedConflict );
	CPPUNIT_TEST( StationResolvedConflict );
	CPPUNIT_TEST( AlienLegOnRoute );
	CPPUNIT_TEST( DistributeAfterSemaUnjoin );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StationCase(); //���������� ���������� �� �������
	void SpanCase(); //���������� ���������� �� ��������
	void StationUnresolvedConflict(); //������� ��������� ����������� ���� (�������������)
	void StationResolvedConflict(); //������� ��������� ����������� ���� (�����������)
	void AlienLegOnRoute(); //����� �������� � ����� ������
	void DistributeAfterSemaUnjoin(); //����������������� ���������� ����� ������������ ������ �� ���
};
