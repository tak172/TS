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
	void LegWOPicketage(); //размещение светофорной мачты с непроинициализированным пикетажом
	void VerticalOddFirstDistribution(); //вертикальное распределение светофоров при порядке следования путей логического профиля от нечетного
	void VerticalEvenFirstDistribution(); //вертикальное распределение светофоров при порядке следования путей логического профиля от четного
	void OutsideLeg(); //направление пограничной мачты, лежащей за пределами перегона, в нужном направлении
	void InvitationHeadSort(); //сортировка ближе к мачте для пригласительных голов при размещении графических светофоров на профиле

	//вспомогательные члены
	struct JointInfo 
	{
		JointInfo( const rwCoord & rc, bool _leg_exist = false, bool _swap_leg = false, 
			const std::vector <HEAD_TYPE> & _htypes = std::vector <HEAD_TYPE>( 1, TRAIN ) ) : 
		pos( rc ), leg_exist( _leg_exist ), swap_leg( _swap_leg ), headTypes( _htypes ){}
		rwCoord pos;
		bool leg_exist; //наличие мачты на стыке
		bool swap_leg; //признак разворота светофора в направлении обратному порядку следования элементов вектора
		std::vector <HEAD_TYPE> headTypes;
	};
	std::wstring axis_name;
	double gland_hei, gland_wid;
	CGraphicLandscape * gland;
	CLogicLandscape * lland;
	std::vector <BadgeE> CreateSpanWay( unsigned int way_num, const std::vector <JointInfo> & ); //результат - вектор бейджей мачт, установленных на стыках
	std::pair <CGraphicLandscape *, CLogicLandscape *> CreateLandscapes( std::wstring axis_name );
	void SendLandscapeLos( CGraphicLandscape *, CLogicLandscape * ); //необходимо для инициализации зон графического профиля
};