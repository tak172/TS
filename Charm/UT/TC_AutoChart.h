#ifndef TC_AUTOCHART_H
#define TC_AUTOCHART_H

#include <cppunit/extensions/HelperMacros.h>
#include "TrackerInfrastructure.h"
#include "TC_ChartImpl.h"

//������������ AutoChart ���������� � �������� XML-�����

class AutoChartTest;
class TC_AutoChart : 
	private TC_ChartImpl,
	public CPPUNIT_NS::TestFixture
{
public:
	TC_AutoChart();
	~TC_AutoChart();

private:
	boost::scoped_ptr<AutoChartTest> autoChart;

	CPPUNIT_TEST_SUITE( TC_AutoChart );
    CPPUNIT_TEST( SaveAndLoad );
    CPPUNIT_TEST( DirtyDataLoad );
    //CPPUNIT_TEST( ProfileLoad );
	CPPUNIT_TEST_SUITE_END();

protected:
    void setUp() override;
    void tearDown() override;

	void SaveAndLoad();    //�������� ������ � ����
    void DirtyDataLoad();  //�������� ����� � ��������
    void ProfileLoad(); // ������������ ��������/�������� �������
};

#endif