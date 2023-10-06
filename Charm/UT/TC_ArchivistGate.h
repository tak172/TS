#pragma once

#include "../Fund/Archivist.h"
#include "../BlackBox/bbx_BlackBox.h"
#include "UtGateway.h"

#include <cppunit/extensions/HelperMacros.h>

/*
 * Класс для проверка работы класса Archivist
 */

class TC_ArchivistGate : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_ArchivistGate );
    CPPUNIT_TEST( Positioning );
    CPPUNIT_TEST( EqualMoments );
    CPPUNIT_TEST( LacunaDetection );
    CPPUNIT_TEST( Content );
    CPPUNIT_TEST( PlayingSequence );
    CPPUNIT_TEST( ReadAll );
    CPPUNIT_TEST( ReadLiveExpansion );
    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void Positioning();
    void EqualMoments();
    void LacunaDetection();
    void Content();
    void PlayingSequence();
    void ReadAll();
    void ReadLiveExpansion();

private:
    Bbx::Location blackBoxLocation;

    std::shared_ptr<UtGateway> gateway;
    boost::scoped_ptr<Archivist> archivist;

    time_t baseMoment;
	unsigned ref_records;
	unsigned inc_records;
    unsigned incoming_packages;
    unsigned outbox_packages;
    

    void DefaultArchivistInitialization();
	void AbsoluteArchivistInitialization();
    void Wipe(UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek);
    void Wipe(UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek, UtGateway::vecDocAndOpt& data);
    bool RewindAndReceiveResponse(time_t stamp, UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek);
    void Receive( UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek );
    bool ReceiveRewindResponse(time_t stamp, UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek);
	bool ContainsData(UtGateway::vecDocAndOpt& read) const;
    bool DoesNotContainData(UtGateway::vecDocAndOpt& read, UtGateway::vecDocAndOpt& seek) const;
	bool MomReached( const time_t stamp, const UtGateway::vecDocAndOpt& seek ) const;
    bool StampWasReached(const UtGateway::vecDocAndOpt& seek, time_t stamp);
    void CheckLacunaDetectionInVector(UtGateway::vecDocAndOpt& vec, time_t stamp);
    bool playInterval( int from, int to, int speed );
};
