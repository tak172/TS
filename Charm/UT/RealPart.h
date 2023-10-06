#pragma once

#include "InfraPart.h"

class RealPart : public InfraPart
{
public:
	RealPart();

private:
	void CreateLigatne(); //�������
	void CreateRiga(); //����
	void CreateLiepaja(); //������
	void CreateVangazi(); //�������
	void CreateSloka(); //�����
	void CreateKarsava(); //�������
	void CreateKraslava(); //��������
	void CreateKrauja(); //�����
	void CreateMezvidi(); //�������
	void CreateSkirotava(); //���������
	void CreateKemeri(); //������
	void CreateRezekne2(); //������� 2
	void CreateJelgava(); //������
	void CreateBale(); //����
	void SetDetails(); //��������� ���.��������� (������ �����, ������, ���������� ������)
	void SetStripDetails();
	void SetSemaDetails();
};