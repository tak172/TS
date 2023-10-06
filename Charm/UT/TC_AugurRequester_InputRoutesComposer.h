#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"
#include "../Hem/UncontrolledStations.h"

class HappenLayer;
class FutureLayer;

// ������������ ����� �������������� ����� ��� Augur
class TC_InputRoutesComposer : public CPPUNIT_NS::TestFixture
{
public:
	CPPUNIT_TEST_SUITE( TC_InputRoutesComposer );
    CPPUNIT_TEST( HappenLive );
    CPPUNIT_TEST( HappenDeath );
    CPPUNIT_TEST( FutureSimple );
    CPPUNIT_TEST( FutureWithHappen );
	CPPUNIT_TEST_SUITE_END();

    void HappenLive();   // ���� ������������� ����� ������������ ����
    void HappenDeath();  // ���� ������� ����� ������������ ����
    void FutureSimple(); // ���� ������ �� ����������� ����
    void FutureWithHappen(); // ���� �� ����������� � ������������ �����

    void setUp() override;
    void tearDown() override;

private:
    std::shared_ptr< UtLayer<HappenLayer> > happen_layer;
    std::shared_ptr< UtLayer<FutureLayer> > future_layer;
    HappenLayer::PathSignMap pathForFutureMap;
    UncontrolledStations uncontrolledStations;
    time_t liveMoment;
  
    // ��������� �������� �������
    void setLiveMoment( std::string _currentMoment );
    // ��������� �������� ������� �������
    std::wstring HMS( unsigned val ) const;
    // �������� ����� �� ������ ������
    HappenLayer::PathPtr              getHappenPath( std::wstring _numTrain ) const;
    std::shared_ptr<const Hem::FixablePath> getFuturePath( std::wstring _numTrain ) const;
    // ��������� ����� ���������� ���� � ��������
    void attachFutu2Happen( std::wstring trainNum );

    // ��������� ����������
    typedef std::pair< TrainDescr, std::vector<FixableEvent> > ReqItem;
    typedef std::vector< ReqItem > ReqLines;
    // ���������� ���������� ��� ��������
    ReqLines processing();
    // ����� ���������� ����������� ����� ���������� ��� nullptr
    static const ReqItem* findItem( const ReqLines& reqlines, const TrainDescr& td );
};
