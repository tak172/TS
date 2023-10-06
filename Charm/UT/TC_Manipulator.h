#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_Manipulator :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_Manipulator );
	CPPUNIT_TEST( MoveStateWhileManipulating );
	CPPUNIT_TEST( EvenlenStripRotating );
	CPPUNIT_TEST( OddlenStripRotating );
	CPPUNIT_TEST( StripNFigureRotating );
	CPPUNIT_TEST( NullLengthStripCreating );
	CPPUNIT_TEST( NameMoving );
	CPPUNIT_TEST( LinksOrder );
	CPPUNIT_TEST( ScalingAndRemove );
	CPPUNIT_TEST( SmallSwitchRegenarating );
	CPPUNIT_TEST( RoughMoving );
	CPPUNIT_TEST( MoveFixedAndUnfixed );
	CPPUNIT_TEST( EmptyPoolAfterHit );
	CPPUNIT_TEST( ClickOnSwitchedStrip );
	CPPUNIT_TEST( ZTestAfterGhostSemaforAdding );
	CPPUNIT_TEST( TextMerging );
	CPPUNIT_TEST( SemaZChanging );
	CPPUNIT_TEST( SelectingGraphicTextsQuasiName );
	CPPUNIT_TEST_SUITE_END();

	void MoveStateWhileManipulating();
	void EvenlenStripRotating(); //�������� ������� ������ �����
	void OddlenStripRotating(); //�������� ������� �������� �����
	void StripNFigureRotating(); //�������� ������� � ������
	void NullLengthStripCreating(); //�������� ������� ������� �����
	void NameMoving(); //�������� �����
	void LinksOrder(); //�������� ���������� ������� ���������� ����������� ������ ��� ��������� ���������������� ��������
	void ScalingAndRemove(); //��������� �������� ����� ��������� �������
	void SmallSwitchRegenarating(); //������������ ������� � ����� ��������
	void RoughMoving(); //������ �������� (� ������� Ctrl)
	void MoveFixedAndUnfixed(); //�������� ��������������� � �� ��������������� ��������
	void EmptyPoolAfterHit(); //����� ����� � ����������� ������ ��� ����������� ������ ���� ����
	void ClickOnSwitchedStrip(); //����� � ���������� �������
	void ZTestAfterGhostSemaforAdding(); //�������� z-������� ����� ���������� ������������ ghost-���������
	void TextMerging(); //������� ��������� ����� � ���������
	void SemaZChanging(); //�������� �� ������� ����� ��������� z-������
	void SelectingGraphicTextsQuasiName(); //������� ��������� ���������� ������������ ������� �� ������� �����������
};