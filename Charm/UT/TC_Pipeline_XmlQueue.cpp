#include "stdafx.h"

#include <boost/dynamic_bitset.hpp>
#include "TC_Pipeline_XmlQueue.h"
#include "../helpful/Attic.h"
#include "../Fund/PipeQueue.h"
#include "../Fund/PipeParseXml.h"
#include "../helpful/RT_ThreadName.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PipelineXmlQueue );

using namespace std;

class Test_ParseXml : public Pipeline::ParseXml
{
public:
    Test_ParseXml(  const std::vector<char>& v, unsigned _pos,
                    boost::dynamic_bitset<>& _alloc,
                    boost::dynamic_bitset<>& _freed
        )
        : Pipeline::ParseXml( v.begin(), v.end() ),
        pos(_pos),
        alloc(_alloc), freed(_freed)
    {
        CPPUNIT_ASSERT( !alloc.test_set(pos) );
    }

    ~Test_ParseXml()
    {
        CPPUNIT_ASSERT( !freed.test_set(pos) );
    }
private:
    const unsigned pos;
    boost::dynamic_bitset<>& alloc;
    boost::dynamic_bitset<>& freed;
};

class Test_XmlPipe : public Pipeline::ActionQueue
{
public:
    Test_XmlPipe( unsigned _count )
        : count(_count), alloc(), freed()
    {
        alloc.resize(count);
        freed.resize(count);
        subthread = boost::thread( [this](){
            generator();
        });

    }
    ~Test_XmlPipe()
    {
        if ( subthread.joinable() )
        {
            subthread.interrupt();
            subthread.join();
        }
    }
    virtual void notify_ready() // уведомить о готовности
    {}
    bool compare( const boost::dynamic_bitset<>& proceed )
    {
        return alloc == proceed && freed == proceed;
    }
private:
    const unsigned count;
    boost::dynamic_bitset<> alloc;
    boost::dynamic_bitset<> freed;
    boost::thread subthread;

    void generator();
};

void Test_XmlPipe::generator()
{
    try
    {
        RT_SetThreadName( "XmlQueue" );
        vector<char> sm,bg;
        const string DIGS = "1234567890";
        {
            string temp;
            attic::a_document smdoc("sml");
            smdoc.document_element().append_child("Position").ensure_attribute("value").set_value(DIGS);
            temp = smdoc.to_str();
            sm.assign( temp.begin(), temp.end() );

            attic::a_document big("big");
            auto node = big.document_element().append_child("Position");
            node.ensure_attribute("value").set_value(DIGS);
            node.ensure_attribute("trash").set_value( string(1024*1024,'*') );
            temp = big.to_str();
            bg.assign( temp.begin(), temp.end() );
        }
        size_t curr_place;
        {
            vector<char>::iterator it_sm = find( sm.begin(), sm.end(), DIGS.front() );
            vector<char>::iterator it_bg = find( bg.begin(), bg.end(), DIGS.front() );
            CPPUNIT_ASSERT( it_sm != sm.begin() );
            curr_place = it_sm - sm.begin();
            CPPUNIT_ASSERT( curr_place == it_bg - bg.begin() );
        }
        for( unsigned curr = count-1; !boost::this_thread::interruption_requested() && curr<count; --curr )
        {
            string val = to_string(curr) + '"';
            val.resize( DIGS.size()+1, ' ' );

            vector<char>& v = ( curr % 100 == 0 )? bg : sm;
            std::copy( val.begin(), val.end(), v.begin()+curr_place ); 
            auto act = std::make_shared<Test_ParseXml>( v, curr, alloc, freed );
            push(act);
        }
    }
    catch( boost::thread_interrupted& /*e*/ )
    {
        RT_SetThreadName( "Test_XmlPipe::generator [interrupt]" );
    }
    catch( ... )
    {
        RT_SetThreadName( "Test_XmlPipe::generator [exception]" );
    }
}


void TC_PipelineXmlQueue::check()
{
    unsigned count = 1024;
    boost::dynamic_bitset<> proceed;
    boost::scoped_ptr< Test_XmlPipe > pipeque;
    pipeque.reset( new Test_XmlPipe(count) );

    proceed.resize(count);
    unsigned accepted = 0;
    while( accepted<count )
    {
        int pass = 17;
        std::shared_ptr<Pipeline::ParseXml> act;
        for( act = pipeque->pop_ready<Test_ParseXml>(); NULL!=act; act = pipeque->pop_ready<Test_ParseXml>() )
        {
            int pos = act->getXDoc().document_element().child("Position").attribute("value").as_int();
            CPPUNIT_ASSERT( pos>=0 );
            CPPUNIT_ASSERT( !proceed.test_set(pos) );
            ++accepted;
            if ( --pass <= 0 )
                break;
        }
        static size_t sz = 0;
        size_t temp = pipeque->size();
        if ( temp > sz )
        {
            sz = temp;
            boost::wformat wf(L"\naccept %d pipesize %d");
            wstring ws = (wf % accepted % temp).str();
            //OutputDebugString( ws.c_str() );
        }
    }

    CPPUNIT_ASSERT( pipeque->compare(proceed) );
    pipeque.reset();
}

const auto GRANULAR = std::chrono::milliseconds(100); // отчетливая задержка

class SlowAction : public Pipeline::Action
{
private:
    void DoProcess() override
    {
        std::this_thread::sleep_for( GRANULAR );
    }
};

void TC_PipelineXmlQueue::quick_check()
{
    Pipeline::ActionQueue pipe;

    unsigned threadCount = 64;

    // заполняем очередь медленными заданиями
    for( auto pass=threadCount; pass>0; --pass )
        pipe.push( std::make_shared<SlowAction>() );
    // проверяем, что никогда не ждем
    unsigned count = 0;
    while( pipe.size()>0 )
    {
        std::shared_ptr<SlowAction> act;
        auto start = std::chrono::steady_clock::now();
        act = pipe.pop_ready<SlowAction>();
        if ( act )
            ++count;
        auto total = std::chrono::steady_clock::now() - start;
        CPPUNIT_ASSERT( total < GRANULAR / 2 );
    }
    CPPUNIT_ASSERT_EQUAL(threadCount, count);
}
