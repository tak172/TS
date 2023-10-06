#pragma once

#include "TC_Project.h"
#include "../helpful/rwCoord.h"

class CGraphicLandscape;
class CLogicLandscape;
class TC_SemaLandPlacer : public TC_Project
{
public:
	void setUp() override;
    void tearDown() override;

private:

	CPPUNIT_TEST_SUITE( TC_SemaLandPlacer );
	CPPUNIT_TEST( TwoWaysThreeLegs );
	//CPPUNIT_TEST( LegWOPicketage );
	CPPUNIT_TEST( VerticalOddFirstDistribution );
	CPPUNIT_TEST( VerticalEvenFirstDistribution );
	CPPUNIT_TEST( OutsideLeg );
	CPPUNIT_TEST( InvitationHeadSort );
	CPPUNIT_TEST_SUITE_END();

	void TwoWaysThreeLegs();
	void LegWOPicketage(); //���������� ����������� ����� � ����������������������� ���������
	void VerticalOddFirstDistribution(); //������������ ������������� ���������� ��� ������� ���������� ����� ����������� ������� �� ���������
	void VerticalEvenFirstDistribution(); //������������ ������������� ���������� ��� ������� ���������� ����� ����������� ������� �� �������
	void OutsideLeg(); //����������� ����������� �����, ������� �� ��������� ��������, � ������ �����������
	void InvitationHeadSort(); //���������� ����� � ����� ��� ��������������� ����� ��� ���������� ����������� ���������� �� �������

	//��������������� �����
	struct JointInfo 
	{
		JointInfo( const rwCoord & rc, bool _leg_exist = false, bool _swap_leg = false, 
			const std::vector <HEAD_TYPE> & _htypes = std::vector <HEAD_TYPE>( 1, TRAIN ) ) : 
		pos( rc ), leg_exist( _leg_exist ), swap_leg( _swap_leg ), headTypes( _htypes ){}
		rwCoord pos;
		bool leg_exist; //������� ����� �� �����
		bool swap_leg; //������� ��������� ��������� � ����������� ��������� ������� ���������� ��������� �������
		std::vector <HEAD_TYPE> headTypes;
	};
	std::wstring axis_name;
	double gland_hei, gland_wid;
	CGraphicLandscape * gland;
	CLogicLandscape * lland;
	std::vector <BadgeE> CreateSpanWay( unsigned int way_num, const std::vector <JointInfo> & ); //��������� - ������ ������� ����, ������������� �� ������
	std::pair <CGraphicLandscape *, CLogicLandscape *> CreateLandscapes( std::wstring axis_name );
	void SendLandscapeLos( CGraphicLandscape *, CLogicLandscape * ); //���������� ��� ������������� ��� ������������ �������
};