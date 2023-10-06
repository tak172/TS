#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class FutureLayer;
class RouteIntrusionKeeper;

// ������������ ������������� �������� ����� ������� ��������
class TC_AdjustIntrusions : public CPPUNIT_NS::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TC_AdjustIntrusions );
    CPPUNIT_TEST( SelfVisit );
    CPPUNIT_TEST( OtherVisit );
    CPPUNIT_TEST( SameTrainNumber );
	CPPUNIT_TEST_SUITE_END();

    void SelfVisit();    // �������� �� ����������� ��������� (��� ��� �������� ��� � � ���������)
    void OtherVisit();    // �������� �� ��������� ������ �������
    void SameTrainNumber(); // ��������� ������� � ����� ������� � ������� ���������

    void setUp() override;
    void tearDown() override;

private:
    std::shared_ptr< UtLayer<FutureLayer> > future_layer;
    std::shared_ptr<RouteIntrusionKeeper>   rIntrusionKeeper;
    time_t liveMoment;

    // ��������� �������� �������
    void setLiveMoment( std::string _currentMoment );
    // ��������� ��������� �������
    std::wstring HEAD( unsigned HHMMss ) const;
};
