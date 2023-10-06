#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../helpful/EsrGuide.h"
#include "../Hem/HemHelpful.h"
#include "../Hem/HappenLayer.h"

class TC_Hem_aeCut : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_aeCut );
    CPPUNIT_TEST( Middle );
    CPPUNIT_TEST( WithStart );
    CPPUNIT_TEST( WithEnd );
    CPPUNIT_TEST( Whole );
    CPPUNIT_TEST( TooShort );
    CPPUNIT_TEST( SaveNumber );
    CPPUNIT_TEST( Case3674 );
    CPPUNIT_TEST( StoppingBegin_Death_3889 );
    CPPUNIT_TEST( Ticket3912_split );
    CPPUNIT_TEST( Ticket3912_toEnd );
    CPPUNIT_TEST( CutPathBeginningUntilDeparture );
    CPPUNIT_TEST( CutPathBeginningUntilTransition );
    CPPUNIT_TEST( WorkaroundCutPathStartingDeparture );
    CPPUNIT_TEST( CutPathTailStationSpan );
    CPPUNIT_TEST( StoppingBegin_End_WrongWay_3889 );
    CPPUNIT_TEST( RemoveEndOfPath );
    CPPUNIT_TEST( RemoveBeginOfPath );
	CPPUNIT_TEST_SUITE_END();

public:
    void tearDown() override;

private:
	void Middle(); // ������ � �������� ����
    void WithStart(); // �������� ������ �������� ����
    void WithEnd(); // �������� ������ �������� ����
    void Whole(); // �������� ������ �������� ����
    void TooShort(); // �������� ����� �������� ���� Form-Death
    void SaveNumber(); // ���������� ������ ������ ����� �������
    void Case3674(); // �������� ������ #3674
    void StoppingBegin_Death_3889(); // ������ ��������� ������� �� ��������
    void StoppingBegin_End_WrongWay_3889(); //������ ������ ������� �� ��������, �� ������������ ����
    void Ticket3912_split(); // ������ ������� �� ��������
    void Ticket3912_toEnd(); // �������� �� ������ ������� �� �������� �� ����� ����
    void CutPathBeginningUntilDeparture();
    void CutPathBeginningUntilTransition();
    void WorkaroundCutPathStartingDeparture(); // "���������" ������, ���� ���� ���������� � �����������
    void CutPathTailStationSpan();          // ������ ������ ���� �� ������������ ������� �� SpanMove
    void RemoveEndOfPath();                 // �������� ������ ����
    void RemoveBeginOfPath();               // �������� ������ ����

    void collectUndo( HappenLayer& hl );
    void implementUndo( HappenLayer& hl );

    static const time_t T=3600*(240+23);
    attic::a_document            undoer;
    EsrGuide esrGuide;

    std::shared_ptr<const SpotEvent> getSpotPtr(HappenLayer& happenLayer, const SpotEvent& spot);
    std::shared_ptr<Hem::HappenPath> getPathPtr(HappenLayer& happenLayer, const SpotEvent& spot);
};
