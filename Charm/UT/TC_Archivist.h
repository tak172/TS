#ifndef TC_ARCHIVIST_H
#define TC_ARCHIVIST_H

#include <cppunit/extensions/HelperMacros.h>

/// Тесты для проверки работы класса \ref Archivist
class TC_Archivist : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE( TC_Archivist );
  CPPUNIT_TEST( Positioning );
  CPPUNIT_TEST( EqualMoments );
  CPPUNIT_TEST( Content );
  CPPUNIT_TEST( LacunaDetection );
  CPPUNIT_TEST( PlayingSequence );
  CPPUNIT_TEST( PlayingLive );
  CPPUNIT_TEST( SkipPackages );
  CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();
protected:
    void Positioning();
    void EqualMoments();
    void Content();
    void LacunaDetection();
    void PlayingSequence();
    void PlayingLive();
    void SkipPackages();

    bool playInterval( int from, int to, int speed );
private:
    typedef boost::shared_ptr<attic::a_document> PXDOC;
    void SendRewindTo( time_t where );
    void SendSpeed( int _rapid_percent );
    void SendPause();
    void SendRealSpeed( time_t from );
    bool Synchronization();
    bool SynchronizationOneMoment( const time_t checked_moment );
    bool DoesNotContainData( const std::vector<PXDOC> &rcv ) const;
    bool ContainsData( const std::vector<PXDOC> &rcv ) const;
    std::vector<PXDOC> GetRewindData( time_t mom );
    std::vector<PXDOC> ReceiveRewindResponse();

    attic::a_document*  receive_xml();
    bool StampWasReached( const std::vector<PXDOC> &rcv, time_t need_mom ) const;
    void live_writer( boost::posix_time::time_duration dur, time_t first, unsigned count );

    time_t fix_moment;
    boost::posix_time::time_duration prev_bbx_delay;
    Bbx::Location bbxLocation;
    boost::scoped_ptr<ActorConnection> actorConnection;
    boost::scoped_ptr<Network::Docker> int_docker;
    unsigned                           int_channelId; // канал в докере
    boost::scoped_ptr<Network::Docker> ext_docker;

    std::vector<boost::posix_time::ptime> w_moms;
};


#endif // TC_ARCHIVIST_H
