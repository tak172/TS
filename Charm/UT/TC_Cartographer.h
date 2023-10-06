#ifndef TC_CARTOGRAPHER_H
#define TC_CARTOGRAPHER_H

#include <cppunit/extensions/HelperMacros.h>
#include "../StatBuilder/Geo.h"
/* 
* ����� ��� �������� ������ ������ Cartographer
*/

class TC_Cartographer : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Cartographer );
	CPPUNIT_TEST( AddNRemSingle );
	CPPUNIT_TEST( AddNRemMultipleNOvp );
	CPPUNIT_TEST( AddNRemMultipleInJnt );
	CPPUNIT_TEST( AddNRemMultipleOvp );
	CPPUNIT_TEST( AddNRemSingleDistrNOvp );
	CPPUNIT_TEST( AddNRemSingleDistrOvp );
	CPPUNIT_TEST( searchInner );
    CPPUNIT_TEST( Dimension );

	CPPUNIT_TEST_SUITE_END();

public:

protected:
	void AddNRemSingle(); // ������� ��������� ����������
	void AddNRemMultipleNOvp(); // ������������� ���������� (��� ����������)
	void AddNRemMultipleInJnt(); // ������������� ���������� (� ����)
	void AddNRemMultipleOvp(); // ������������� ���������� (� �����������)
	void AddNRemSingleDistrNOvp(); // ��������� ���������� ������� � �������������� �������� (��� ����������)
	void AddNRemSingleDistrOvp(); // ��������� ���������� ������� � �������������� �������� (� �����������)
    void searchInner(); // �������� ������ ����������
    void Dimension(); // �������� ��������� ����� ��������

private:
	//����������� ����������/�������� (� ������������ ������ ��������� � ������������� ����������( >1 - ���������� ���� ) )
	void AddNRemCommon( unsigned int x_num ,unsigned int y_num,
		double horiz_gap, double vert_gap);
};


#endif // TC_CARTOGRAPHER_H