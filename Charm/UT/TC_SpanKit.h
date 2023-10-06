#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class rwCoord;
class TC_SpanKit : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_SpanKit );
	CPPUNIT_TEST( OnlyStripsSpan );
	CPPUNIT_TEST( PicketageIncreasingStripStarting );
	CPPUNIT_TEST( PicketageDecreasingStripStarting );
	CPPUNIT_TEST( PicketageIncreasingJointStarting );
	CPPUNIT_TEST( LegDirection );
	CPPUNIT_TEST( OneLinkedManyUnlinked );
	CPPUNIT_TEST( OneUnlinked );
	CPPUNIT_TEST( OneLinked );
	CPPUNIT_TEST( ManyUnlinked );
	CPPUNIT_TEST( ManyLinked );
	CPPUNIT_TEST( NoneLinked );
	CPPUNIT_TEST( CheckLockType );
	CPPUNIT_TEST( AutoOddness );
	CPPUNIT_TEST( FillForGuess );
	CPPUNIT_TEST_SUITE_END();

public:
	TC_SpanKit();

protected:
	void OnlyStripsSpan(); //������� ������ � ��������� (��� ����������)
	void PicketageIncreasingStripStarting(); //����������� ����������� �������� (���� ���������� � �������)
	void PicketageDecreasingStripStarting(); //����������� �������� ��������
	void PicketageIncreasingJointStarting(); //����������� ����������� �������� (���� ���������� �� �����)
	void LegDirection(); //����������� ����������� �����
	void OneLinkedManyUnlinked(); //���� ���� ������ �� ������� �������� ������, ������ ���
	void OneUnlinked(); //���� ����, �� ��������� �� ������� ��������
	void OneLinked(); //���� ����, ��������� �� ������� ��������
	void ManyUnlinked(); //��������� �����, �� ��������� �� ������� ��������
	void ManyLinked(); //��������� �����, ��������� �� ������� ��������
	void NoneLinked(); //���� �����������
	void CheckLockType(); //���������� ����� �������� ���������� ����� ����������
	void AutoOddness(); //��������������� �������� ��������
	void FillForGuess(); //���������� xml-���� ��� Guess

	//�������� ������������� ����������� ����, ������������� � ������� (������������ �������� - ��������� ����)
	CLogicElement * MakeSpanStripStarting( const rwCoord & first_joint_coord, const rwCoord & third_joint_coord );

private:
	std::wstring pckname;
};