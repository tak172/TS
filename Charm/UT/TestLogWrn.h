/*!
	created:	2018/07/12
	created: 	18:54  12 ���� 2018
	filename: 	F:\potapTrunk\Charm\UT\TestLogWrn.h
	file path:	F:\potapTrunk\Charm\UT
	file base:	TestLogWrn
	file ext:	h
	author:		 Dremin
	
	purpose:	��������� ��� ������
 */
#ifndef TestLogWrn_h__
#define TestLogWrn_h__

#include "../Augur/ALogWrn.h"

class TestLogWrn:public ALogWrn
{
public:
	TestLogWrn(){}
	~TestLogWrn(){}
	using AArchive::pathArch; // ���������� ��� �����������
	using AArchive::pathOut;// ���������� ��� �������, ������ � ����
	using AModuleArch::init;
	using AModuleArch::archName;// ���������
	using ALogWrn::sz;// ����. ����� �������  � ���� ����������
	using AModuleArch::lib;// dll ����������
private:

};

#endif // TestLogWrn_h__

