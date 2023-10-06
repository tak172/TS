#pragma once

#include "TC_Project.h"

class TC_DocEditor : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_DocEditor );
	CPPUNIT_TEST( PseudoManipulation );
	CPPUNIT_TEST( LegGetting );
	CPPUNIT_TEST( TriangleJunctionUndo );
	CPPUNIT_TEST( SwitchedStripLogicSetUndo );
	CPPUNIT_TEST( UndoRemakingNamedSwitch );
	CPPUNIT_TEST( RedoUndoRemovingLObjectWithGLink );
	CPPUNIT_TEST( UndoRouteDoubleGenerating );
	CPPUNIT_TEST( ResetRemovingLObject );
	CPPUNIT_TEST( BlankLogicAction );
	CPPUNIT_TEST( ZChanging );
	CPPUNIT_TEST_SUITE_END();

	//������ � ��������
	void PseudoManipulation(); //�������� �����������, ������� ������ �� ����������
	void LegGetting(); //����������� � ������ ��������� (������ ���� ������ ����������������)
	void TriangleJunctionUndo(); //����� �������� ������������ ���������� ��������
	void SwitchedStripLogicSetUndo(); //����� ���������� ���������� �������� ��� ����������� �������
	void UndoRemakingNamedSwitch(); //����� ����������� � ��������, ���������� � ������������ ����������� �������
	void ZChanging(); //����������� � z-��������

	//������ � ������� � ��������
	void RedoUndoRemovingLObjectWithGLink(); //����� �������� ����������� ������� � ����������� ������ �� ����

	//������ � �������
	void UndoRouteDoubleGenerating(); //����� ����� ������� ��������� ���������
	void ResetRemovingLObject(); //����� ����������� ����� ������� ���������� �������
	void BlankLogicAction(); //�������� � ���������� ��������, �� ���������� � ���������
};