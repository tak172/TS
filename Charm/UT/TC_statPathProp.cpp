/*!
	created:	2019/05/21
	created: 	12:09  21 ��� 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_statPathProp.cpp
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_statPathProp
	file ext:	cpp
	author:		 Dremin
	
	purpose:	��������� ������� �����.�����
  
  */
 
 #include "stdafx.h"
 #include "TC_statPathProp.h"
#include "../Augur/TypeHeader.h" 
 
static ADProperties::Attrubute tblNoStop[]={		//ADProperties::Attrubute::PASSENGER , //���� ������������
	ADProperties::Attrubute::MAJOR, //���� �������
	//ADProperties::Attrubute::EMBARKATION //���� ������������ ��� ������� � ������� ���������� (������ ��� ������������)
	ADProperties::Attrubute::NON_STOP_PASSING //��������������� �������
	,ADProperties::Attrubute::PASSENGER
	// 	BID_CARGO //������ ��� �������� �������
};

static ADProperties::Attrubute tblMaj[]={	
	ADProperties::Attrubute::MAJOR //���� �������
	,ADProperties::Attrubute::PASSENGER
};

static ADProperties::Attrubute tblpassMaj[]={	
	ADProperties::Attrubute::PASSENGER,  //���� ������������
	ADProperties::Attrubute::MAJOR, //���� �������
	//ADProperties::Attrubute::EMBARKATION //���� ������������ ��� ������� � ������� ���������� (������ ��� ������������)
	//NON_STOP_PASSING //��������������� �������
	// 	BID_CARGO //������ ��� �������� �������
};

static ADProperties::Attrubute tblpassStop[]={	
	ADProperties::Attrubute::PASSENGER  //���� ������������
	//ADProperties::Attrubute::MAJOR, //���� �������
	//ADProperties::Attrubute::EMBARKATION //���� ������������ ��� ������� � ������� ���������� (������ ��� ������������)
	//NON_STOP_PASSING //��������������� �������
	// 	BID_CARGO //������ ��� �������� �������
};

static ADProperties::Attrubute tblMovePr[]={	
	ADProperties::Attrubute::PASSENGER,  //���� ������������
	//ADProperties::Attrubute::MAJOR, //���� �������
	//ADProperties::Attrubute::EMBARKATION //���� ������������ ��� ������� � ������� ���������� (������ ��� ������������)
	ADProperties::Attrubute::NON_STOP_PASSING //��������������� �������
	// 	BID_CARGO //������ ��� �������� �������
};

static ADProperties::Attrubute tblStopInOutPr[]={	
	ADProperties::Attrubute::PASSENGER,  //���� ������������
	//ADProperties::Attrubute::MAJOR, //���� �������
	ADProperties::Attrubute::EMBARKATION //���� ������������ ��� ������� � ������� ���������� (������ ��� ������������)
	//NON_STOP_PASSING //��������������� �������
	// 	BID_CARGO //������ ��� �������� �������
};

static ADProperties::Attrubute tblEmpty[]={	
	ADProperties::Attrubute::PASSENGER
};

//------------------------------------------------------------------------------------------------
ADProperties passStopInOutPr( ARRAY_IT( tblStopInOutPr ),Oddness::UNDEF,WeightNorms());//����+��� ������� +�� �� + �� ������
ADProperties noStopPr( ARRAY_IT( tblNoStop ),Oddness::UNDEF,WeightNorms());// ������� ��� ������������� (��������)// �� + ������
ADProperties passMovePr( ARRAY_IT( tblMovePr ),Oddness::UNDEF,WeightNorms());// ��� �����. �� �� , �� ���� �  ������
ADProperties pMaj( ARRAY_IT( tblMaj ),Oddness::UNDEF,WeightNorms());//�� 
ADProperties passMaj( ARRAY_IT( tblpassMaj ),Oddness::UNDEF,WeightNorms());//����+ �� 
ADProperties passStopPr( ARRAY_IT( tblpassStop ),Oddness::UNDEF,WeightNorms());//����+�� ��� ������� +�� �� + �� ������
ADProperties evenMajor( ARRAY_IT( tblMaj ),Oddness::EVEN,WeightNorms());//������+ �������
ADProperties oddMajor( ARRAY_IT( tblMaj ),Oddness::ODD,WeightNorms());//��������+ �������
ADProperties evenPr( ARRAY_IT( tblEmpty ),Oddness::EVEN,WeightNorms());//������
ADProperties oddPr( ARRAY_IT( tblEmpty ),Oddness::ODD,WeightNorms());//��������
ADProperties allPr( ARRAY_IT( tblEmpty ),Oddness::UNDEF,WeightNorms());//� ����� �����������

