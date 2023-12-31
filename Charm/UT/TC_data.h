/*!
	created:	2017/02/02
	created: 	13:20  02 ������� 2017
	filename: 	D:\potap\Charm\UT\TC_data.h
	file path:	D:\potap\Charm\UT
	file base:	TC_data
	file ext:	h
	author:		 Dremin
	
	purpose:	������ ��� ������
 */
#ifndef TC_data_h__
#define TC_data_h__

static unsigned int startStatCode = 10; //�������� � ����������� ������, ������� 10 ��� ����� ����� ������� �������
static unsigned int finishStatCode = 180;
static unsigned int stationsCount = 22; //���������� ������� � ���������

#define  PVX_STANDART 30 /* ������. ����� ��� �� ������� �� �������*/
#define  PVX_COEFF_2_1 1.0 /* ����� ��� e1->e2 � e2->e1 �� 20% ������*/
#define  INCR_PVX_CAT 10 /* ��� ���������� ��� 
 ����+INCR= �������
 ����+2*INCR= ������
 ����+3*INCR= ����
 */
#define  COEFF_START 1/4 /* �� ������� ��� ��� ������� ���� ��*/
#define  COEFF_RESERV 1/6/* �� ������� ��� ��� ������� ���� ��*/
#define  COEFF_STOP 1/5/* �� ������� ��� ��� ������� ���� ��*/


#include "TC_fabrClass.h"


struct SData
{
public:
	static SData & instance()
	{
		if ( sdataPtr == nullptr )
			sdataPtr.reset( new SData() );
		return *sdataPtr;
	}

	Strain tblTr1, tblTr3, tblTr5, tblTrCtchNrm, tblTr13, tblTrCnfl, tblTrCnfl2, tblTrDdl1001, tblTrDdl1003,
				   tblTrCnfl1, tblTrCnfl3, tblTr2, tblTr4, tblTr6,
				 // ���� ��� ��� (������������������)
				  tblSTr1, tblSTr3,tblSTr1003, tblSTr2,tblSTr4,tblSTr1002		
				 ;
	Stech tWnd, tblW;
	Sevent tblCtchNrm, tblCnflNrm,  tblNrm1003;
	std::vector <Sevent> tblNorma;
	
private:
	static std::unique_ptr<SData> sdataPtr;
	SData();
};

#define  NO_EVENT 0,0,0,0,0,0
#define  NO_FIXED 	AEvent::FIXED,NO_EVENT
#define  NO_REAL 	AEvent::REAL,NO_EVENT

inline SData::SData() :
	tblTr1( Strain( TrainDescr( L"1001" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			110,//����� ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			5,// ����� �� ��������� ��������
			2, // ���.�������
			3// ����� �������
			),
		Sevent(
			// ����.�������
			AEvent::FIXED,// ��� �������
			140,// ����� ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			5, // ���.�������
			4// ����� �������
		) ) ),

	tblTr3( Strain( TrainDescr( L"3", L"1-100-18" ),		
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			108,// ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			0,// ����� �� ��������� ��������
			2, // ���.�������
			4// ����� �������
			),
		Sevent(
			// ����.�������
			AEvent::FIXED,// ��� �������
			140,// ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			2, // ���.�������
			5// ����� �������
		) ) ),

	tblTr5( Strain( TrainDescr( L"5" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			112,// ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			5+2,// ����� �� ��������� ��������
			2, // ���.�������
			3// ����� �������
			),
		Sevent(
			// ����.�������
			AEvent::FIXED,// ��� �������
			240,// ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			1, // ���.�������
			6// ����� �������
		) ) ),

	tblTrCtchNrm( Strain( TrainDescr( L"5" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			15,// ����� �� �������
			25,// ����� �� ��������
			5,// ����� �� ��������� ��������
			1, // ���.�������
			3// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTr13( Strain( TrainDescr( L"13" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			108,// ������ ����
			50,// ����� �� �������
			21,// ����� �� ��������
			0,// ����� �� ��������� ��������
			2, // ���.�������
			4// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrCnfl( Strain( TrainDescr( L"1005", L"1-100-18" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			15,// ����� �� �������
			25,// ����� �� ��������
			5,// ����� �� ��������� ��������
			1, // ���.�������
			1// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrCnfl2( Strain( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			15,// ����� �� �������
			25,// ����� �� ��������
			5,// ����� �� ��������� ��������
			9, // ���.�������
			1// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrDdl1001( Strain( TrainDescr( L"1001", L"1-100-18" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			115,// ����� �� �������
			25,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			1// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrDdl1003( Strain( TrainDescr( L"1003", L"1-100-18" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			115,// ����� �� �������
			25,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			1// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrCnfl1( Strain( TrainDescr( L"1" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			10,// ������ ����
			15,// ����� �� �������
			25,// ����� �� ��������
			5,// ����� �� ��������� ��������
			1, // ���.�������
			2// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTrCnfl3( Strain( TrainDescr( L"3" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			5,// ������ ����
			15,// ����� �� �������
			25,// ����� �� ��������
			5,// ����� �� ��������� ��������
			1, // ���.�������
			2// ����� �������
			),
		Sevent( NO_FIXED )
		) ),

	tblTr2( Strain( TrainDescr( L"2", L"18-100-1" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			20,// ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			5,// ����� �� ��������� ��������
			8, // ���.�������
			3// ����� �������
			),
		Sevent(
			// ����.�������
			AEvent::FIXED,// ��� �������
			40,// ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			6// ����� �������
		) ) ),

	tblTr4( Strain( TrainDescr( L"1004" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			22,// ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			5,// ����� �� ��������� ��������
			8, // ���.�������
			3// ����� �������
			),
		Sevent(
			AEvent::FIXED,// ��� �������
			40,// ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			8// ����� �������
		) ) ),

	tblTr6( Strain( TrainDescr( L"6" ),
		Sevent(
			//�������� ����
			AEvent::REAL,// ��� �������
			12,// ������ ����
			11,// ����� �� �������
			21,// ����� �� ��������
			7,// ����� �� ��������� ��������
			12, // ���.�������
			3// ����� �������
			),
		Sevent(
			AEvent::FIXED,// ��� �������
			40,// ������ ����
			20,// ����� �� �������
			15,// ����� �� ��������
			0,// ����� �� ��������� ��������
			13, // ���.�������
			4// ����� �������
		) ) ),

		 tblSTr1(Strain(TrainDescr( L"1"),
		Sevent(
		//�������� ����
		AEvent::REAL,// ��� �������
		10,// ������ ����
		15,// ����� �� �������
		25,// ����� �� ��������
		0,// ����� �� ��������� ��������
		1, // ���.�������
		2// ����� �������
		),
		Sevent( NO_FIXED )
		)),

		tblSTr3(Strain(TrainDescr( L"3"),
				Sevent(
				//�������� ����
				AEvent::REAL,// ��� �������
				12,// ������ ����
				15,// ����� �� �������
				25,// ����� �� ��������
				0,// ����� �� ��������� ��������
				1, // ���.�������
				2// ����� �������
				),
				Sevent( NO_FIXED )
				)),

		tblSTr1003(Strain(TrainDescr( L"23"),		
				Sevent(
				//�������� ����
				AEvent::REAL,// ��� �������
				7,// ������ ����
				15,// ����� �� �������
				25,// ����� �� ��������
				0,// ����� �� ��������� ��������
				1, // ���.�������
				2// ����� �������
				),
				Sevent( NO_FIXED )
				)),

			tblSTr2(Strain(TrainDescr( L"2"),
				Sevent(		
				//�������� ����
				AEvent::REAL,// ��� �������
				20,// ������ ����
				15,// ����� �� �������
				25,// ����� �� ��������
				0,// ����� �� ��������� ��������
				11, // ���.�������
				2// ����� �������
				),
				Sevent( NO_FIXED )
				)),
			tblSTr4(Strain(TrainDescr( L"4"),
				Sevent(
				//�������� ����
				AEvent::REAL,// ��� �������
				22,// ������ ����
				15,// ����� �� �������
				25,// ����� �� ��������
				0,// ����� �� ��������� ��������
				11, // ���.�������
				2// ����� �������
				),
				Sevent( NO_FIXED )
				)),
			tblSTr1002(Strain(TrainDescr( L"22"),
				Sevent(
				//�������� ����
				AEvent::REAL,// ��� �������
				24,// ������ ����
				15,// ����� �� �������
				25,// ����� �� ��������
				0,// ����� �� ��������� ��������
				11, // ���.�������
				2// ����� �������
				),
				Sevent( NO_FIXED )
				)),

		tWnd( Stech( 10, 9989 /*������������*/, 1, 1/*park*/, 6/*e1*/, 5/*e2*/, nullptr/*limit*/ ) ),
		tblW( Stech( 0/*or*/,100/*end*/,1/*path*/,1/*park*/,4/*e1*/,5/*e2*/, std::make_shared<unsigned>( 10 )/*limit*/ ) ),
		tblCtchNrm(  Sevent( 
			AEvent::NONE,// ��� �������
			30,// ������ ����
			10,// ����� �� �������
			20,// ����� �� ��������
			0,// ����� �� ��������� ��������
			1, // ���.�������
			16// ����� �������
		) ),

		tblCnflNrm( Sevent(
			AEvent::NONE,// ��� �������
			30,// ������ ����
			50,// ����� �� �������
			100,// ����� �� ��������
			0,// ����� �� ��������� ��������
			1, // ���.�������
			10// ����� �������
		) ),


		tblNrm1003( Sevent(
			AEvent::NONE,// ��� �������
			60,// ������ ����
			410,// ����� �� �������
			20,// ����� �� ��������
			0,// ����� �� ��������� ��������
			10, // ���.�������
			10// ����� �������
		) )
{
	tblNorma.emplace_back( Sevent(
				// ������
				AEvent::NONE,// ��� �������
				28,// ������ ����
				100,// ����� �� �������
				21,// ����� �� ��������
				0,// ����� �� ��������� ��������
				18, // ���.�������
				16// ����� �������
			) );
	tblNorma.emplace_back( Sevent(
				// ��������
				AEvent::NONE,// ��� �������
				30,// ������ ����
				100,// ����� �� �������
				20,// ����� �� ��������
				0,// ����� �� ��������� ��������
				1, // ���.�������
				16 // ����� �������
			) );
}



 #endif // TC_data_h__

