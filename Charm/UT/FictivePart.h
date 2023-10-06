#pragma once

#include "InfraPart.h"

class FictivePart : public InfraPart
{
public:
	FictivePart( const std::wstring & axis_name );

private:
	typedef ADProperties::Attrubute ADAttribute;
	const std::wstring & axis_name; //������� �������
	void CreateFirstPart(); //������ ����� ����������� ��������
	void CreateSecondPart(); //������ ����� ����������� ��������
	void CreateThirdPart(); //������ ����� ����������� ��������
	void CreateFourthPart(); //��������� ����� ����������� ��������
	void CreateFifthPart(); //����� ����� ����������� ��������
	void CreateWrongWayPart(); //����� ��� ������������ ������� ������ �� ������������ ���� ��������
	void CreatePocketPart(); //������� � ���������
	void CreateBlockPostPart(); //����-�����
	void CreateTechnicalStationPart(); //����������� �������
	void CreateLongSpan(); //����� ������� �������
	void SetDetails(); //��������� ���.��������� (������ �����, ������, ���������� ������)
	void SetStripDetails();
	void SetJointDetails();
	void SetSemaDetails();
	void SetSpanDetails();
	void SetMostSpanDetails();
	void SetIWSpanDetails();
	void SetJointCoords(); //���������� ������
};
