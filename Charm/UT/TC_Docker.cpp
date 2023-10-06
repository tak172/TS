#include "stdafx.h"

#include <iostream>
#include <stdlib.h>
#ifndef LINUX
#include <conio.h>
#endif // !LINUX
#include "../helpful/Log.h"

#include "TC_Docker.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Docker );

#define DOCKER_REACTION_TIME 1100

void TC_Docker::setUp()
{    
}

/* Try receiving buffer from docker until there is result or second expires */
#define ASYNC_RECEIVE_TEST(res, docker)                              \
{                                                                    \
  DWORD dk_time_stamp = GetTickCount();                              \
  while (docker->empty() && (GetTickCount() < dk_time_stamp + 1000)) \
  {}                                                                 \
  res = docker->receive();                                           \
  CPPUNIT_ASSERT(res->size());                                       \
}

LONG64 checkSum(Network::buffer::const_iterator begin, Network::buffer::const_iterator end)
{
  LONG64 sum = 0;
  for (Network::buffer::const_iterator i = begin; i != end; ++i)
    sum += *i;

  return sum;
}

/* Gets random char to fill the vector */
unsigned char randomChar()
{
  static bool firstCall = true;
  if (firstCall)
  {
    srand((unsigned int)GetTickCount());
    firstCall = false;
  }

  return ((unsigned char)rand());
}

/* Test 4-cell chain docker linkage */
void TC_Docker::dk_chainTest()
{
  static const unsigned int portA = 30111;
  static const unsigned int portB = portA + 1;
  static const unsigned int portC = portB + 1;
  static const unsigned int portD = portC + 1;

  /* Information about dockers */
  Network::addressee hostA, hostB, hostC, hostD;
  Network::addresseeList addrA, addrB, addrC, addrD;

  hostA = Network::addressee("localhost", portA);
  hostB = Network::addressee("localhost", portB);
  hostC = Network::addressee("localhost", portC);
  hostD = Network::addressee("localhost", portD);  

  /* Set up connection graph */
  addrA.push_back(hostB);
  addrC.push_back(hostB);
  addrC.push_back(hostD);  

  /* Start connecting */
  typedef boost::shared_ptr<Network::Docker> DockerPtr;
  DockerPtr m_dA(new Network::Docker(Network::Docker::createId(), portA, addrA));
  CPPUNIT_ASSERT(m_dA->start());
  DockerPtr m_dB(new Network::Docker(Network::Docker::createId(), portB, addrB));
  CPPUNIT_ASSERT(m_dB->start());
  DockerPtr m_dC(new Network::Docker(Network::Docker::createId(), portC, addrC));
  CPPUNIT_ASSERT(m_dC->start());
  DockerPtr m_dD(new Network::Docker(Network::Docker::createId(), portD, addrD));
  CPPUNIT_ASSERT(m_dD->start());
  CPPUNIT_ASSERT(!m_dA->is_dead() && !m_dB->is_dead() && !m_dC->is_dead() && !m_dD->is_dead());

  /* Here we need to let the dockers connect in the right order */
  std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );
  
  /* For now, there should be 1,2,2 and 1 connections per docker 
     so we should see 
                      A+ ~ +B+ ~ +C+ ~ +D 
     */
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dA->get_channels().size(), 1u);
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dB->get_channels().size(), 2u);
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dC->get_channels().size(), 2u);
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dD->get_channels().size(), 1u);

  /* Test message */
  char text[] = "quick brown fox jumps over the lazy dog";
  Network::buffer message(text, text + strlen(text));
  
  /* Test the docker receiving the right-formatted buffer */
  CPPUNIT_ASSERT(m_dA->send( boost::string_ref( &*message.begin(), message.size() )));

  { /* Receive the B message and resend it to the C and A */
    Network::bufferPtr receivedB;    
    ASYNC_RECEIVE_TEST(receivedB, m_dB);
    CPPUNIT_ASSERT(*receivedB == message);

    /* Send the buffer, that will be destroyed right after the send function returns */
    m_dB->send( boost::string_ref(&*receivedB->begin(), receivedB->size()));
  }

  { /* Receive the C message and resend it to the C and D */
    Network::bufferPtr receivedC;
    ASYNC_RECEIVE_TEST(receivedC, m_dC);  
    CPPUNIT_ASSERT(*receivedC == message);

    /* Send the buffer, that will be destroyed right after the send function returns */
    m_dC->send( boost::string_ref(&*receivedC->begin(), receivedC->size()));
  }
  
  { /* Receive the D message */
    Network::bufferPtr receivedD;
    ASYNC_RECEIVE_TEST(receivedD, m_dD);  
    CPPUNIT_ASSERT(*receivedD == message);
  }

  m_dA->async_shutdown();
  m_dB->async_shutdown();
  m_dC->async_shutdown();
  m_dD->async_shutdown();
  /* pause between tests */
  std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

/**
 * @brief Tests many dockers. 
 * @author Dmitry 'Mordet' Raspopov
 * @date 22.12.2010
 */
void TC_Docker::dk_manyDockers()
{
  static const unsigned int number = 100;
  static const Network::PORT_TYPE port = 30000;
  typedef boost::shared_ptr<Network::Docker> DockerPtr;
  std::vector<DockerPtr> dockers;

  Network::addresseeList addressee;
  addressee.push_back(Network::addressee("localhost", port));
  
  unsigned int created = 0;

  /* Allocate memory */
  dockers.reserve(number);

  /* Create main docker with 3000 port */
  DockerPtr newDock(new Network::Docker(Network::Docker::createId(), port));
  CPPUNIT_ASSERT(newDock->start());
  dockers.push_back(newDock);

  /* Create a bunch of dockers with another porst to connect to the main one */
  for (Network::PORT_TYPE i = 1; i < number; i++)
  {
    DockerPtr dockerPtr(new Network::Docker(Network::Docker::createId(), (unsigned short)(i + port), addressee));
	CPPUNIT_ASSERT(dockerPtr->start());
    
    /* It's possible that system resources will end if you enter a bit too big number */
    if (!dockerPtr->is_dead())
    {
      dockers.push_back(dockerPtr);
      created++;
    }
  }
  
  /* There is quite a number of connection to set up */
  std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );
    
  /* If there is no resource error, there should be an equal number of connections */
  std::vector<unsigned int> channels = dockers.at(0)->get_channels();
  CPPUNIT_ASSERT(channels.size() == created);

  /* Perform asynchronous stop, not the blocking shutdown */
  for (std::vector<DockerPtr>::iterator i = dockers.begin(); i != dockers.end(); ++i)
    (*i)->async_shutdown();
  /* pause between tests */
  std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

/* Getting new channels */
void TC_Docker::dk_getNewChannels()
{
  using namespace Network;

  /* Create two dockers, one pointing to another */
  addresseeList addr;
  const unsigned int port = 5050;
  addr.push_back(addressee("localhost", port));
  Docker d1(Docker::createId(), port), d2(Docker::createId(), port + 1, addr);
  CPPUNIT_ASSERT(d1.start());
  CPPUNIT_ASSERT(d2.start());
  CPPUNIT_ASSERT(!d1.is_dead() && !d2.is_dead());

  /* Give them some time and then get the new channels number */
  std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

  std::vector<unsigned int> d1_connections = d1.get_new_channels();
  std::vector<unsigned int> d2_connections = d2.get_new_channels();
  CPPUNIT_ASSERT(d1_connections.size() == 1);
  CPPUNIT_ASSERT(d2_connections.size() == 1);

  addressee d1Addressee = d1.channel_addressee(d1_connections.front());
  CPPUNIT_ASSERT(d1Addressee.first.empty());
  CPPUNIT_ASSERT_EQUAL(0u, d1Addressee.second);

  addressee d2Addressee = d2.channel_addressee(d2_connections.front());
  CPPUNIT_ASSERT_EQUAL(addr.front().first, d2Addressee.first);
  CPPUNIT_ASSERT_EQUAL(addr.front().second, d2Addressee.second);

  /* OK, let's check if there is another guest to maintain */
  d1_connections = d1.get_new_channels();
  d2_connections = d2.get_new_channels();
  CPPUNIT_ASSERT(d1_connections.empty());
  CPPUNIT_ASSERT(d2_connections.empty());

  /* Work complete */
  d1.shutdown();
  d2.shutdown();  
  /* pause between tests */
  std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

/* Test of sending message to docker and back in some period of time */
void TC_Docker::dk_responseTimeTest()
{
  using namespace Network;
  typedef boost::shared_ptr<Docker> DockerPtr;
  const unsigned int portA = 30311;
  const unsigned int portB = portA + 1; 
  addresseeList addrA;  
  addrA.push_back(addressee("localhost", portB));

  /* Start connecting */
  DockerPtr m_dB(new Docker(Docker::createId(), portB));
  CPPUNIT_ASSERT(m_dB->start());
  DockerPtr m_dA(new Docker(Docker::createId(), portA, addrA));  
  CPPUNIT_ASSERT(m_dA->start());
  CPPUNIT_ASSERT(!m_dA->is_dead() && !m_dB->is_dead());

  /* Here we need to let the dockers connect in the right order */
  std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

  /* Check current connections number */
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dA->get_channels().size(), 1u);
  CPPUNIT_ASSERT_EQUAL((unsigned int)m_dB->get_channels().size(), 1u);

  /* Form a message */
  const size_t size = 1024;
  Network::buffer message;
  message.reserve(size);
  for (size_t i = 0; i < size; i++)
    message.push_back(randomChar());
  
  /* Test the docker receiving the right-formatted buffer */
  CPPUNIT_ASSERT(m_dA->send(boost::string_ref(&*message.begin(), message.size())));

  { 
    /* Receive the B message and resend it to A */
    Network::bufferPtr receivedB;  
    ASYNC_RECEIVE_TEST(receivedB, m_dB);
    CPPUNIT_ASSERT(*receivedB == message);

    /* Send the buffer, that will be destroyed right after the send function returns */
    m_dB->send(boost::string_ref(&*receivedB->begin(), receivedB->size()));
  }

  {     
    Network::bufferPtr receivedA;
    ASYNC_RECEIVE_TEST(receivedA, m_dA);  
    CPPUNIT_ASSERT(*receivedA == message);
  }

  m_dA->async_shutdown();
  m_dB->async_shutdown();
  /* pause between tests */
  std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

void TC_Docker::tearDown()
{
}

/* Test of detecting double channels with their closure */
void TC_Docker::dk_doubleChannelsTest()
{
    using namespace Network;
    Identifier id_less("a_Docker"), id_more("b_Docker");
    
    /* These two dockers will connect to each other and dock1 should have priority over dock2 to
       command to disconnect redundant connections, so only one remain */
    addresseeList addr_list1, addr_list2;  
    addr_list1.push_back(addressee("localhost", 30112));
    addr_list1.push_back(addressee("localhost", 30112));
    addr_list2.push_back(addressee("localhost", 30111));
    addr_list2.push_back(addressee("localhost", 30111));
    Docker dock1(id_less, 30111, addr_list1);
	CPPUNIT_ASSERT(dock1.start());
//    std::this_thread::sleep_for(std::chrono::milliseconds(1000));
    Docker dock2(id_more, 30112, addr_list2);
	CPPUNIT_ASSERT(dock2.start());

    /* There two dockers will connect to each other and dock3 will have equal rights with dock4,
       so they will use all 4 channels */
    addresseeList addr_list3, addr_list4;
    addr_list3.push_back(addressee("localhost", 30114));
    addr_list3.push_back(addressee("localhost", 30114));
    addr_list4.push_back(addressee("localhost", 30113));
    addr_list4.push_back(addressee("localhost", 30113));
    Docker dock3(Docker::createId(), 30113, addr_list3);
	CPPUNIT_ASSERT(dock3.start());
    Docker dock4(Docker::createId(), 30114, addr_list4);
	CPPUNIT_ASSERT(dock4.start());

    /* It takes time for dockers to find their collisions and disconnect redundant connections */
	std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

    CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)dock1.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)dock2.get_channels().size());

    CPPUNIT_ASSERT_EQUAL(4u, (unsigned int)dock3.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(4u, (unsigned int)dock4.get_channels().size());
    dock1.async_shutdown();
    dock2.async_shutdown();
    dock3.async_shutdown();
    dock4.async_shutdown();
	/* pause between tests */
	std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

/* Tests restarting dockers */
void TC_Docker::dk_restartTest()
{
    using namespace Network;
    Docker d1(Docker::createId(), 30100);
    d1.add_addressee(std::make_pair("localhost", 30200));
    Docker d2(Docker::createId(), 30200);

    /* Start dockers and ensure they connected to each other */
    CPPUNIT_ASSERT(d1.start() && !d1.start());
    CPPUNIT_ASSERT(d2.start() && !d2.start());
	std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

	CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d1.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d2.get_channels().size());

    /* Stop dockers and check their state again */
    d1.async_shutdown();
    d2.async_shutdown();
    d1.shutdown();
    d2.shutdown();
    CPPUNIT_ASSERT_EQUAL(0u, (unsigned int)d1.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(0u, (unsigned int)d2.get_channels().size());
	/* pause between tests */
	std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));

    /* Restart both dockers, they must work fine */
    CPPUNIT_ASSERT(d1.start());
    CPPUNIT_ASSERT(d2.start());
	std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

	CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d1.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d2.get_channels().size());

    d1.async_shutdown();
    d2.async_shutdown();
	/* pause between tests */
	std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}

/* Ensures big message fast sending time */
void TC_Docker::dk_bigMessageSendingTime()
{
    using namespace Network;
    Identifier id_less("a_Docker"), id_more("b_Docker");

    Docker d1(id_less, 30100);
    Docker d2(id_more, 30200);
    d1.add_addressee(std::make_pair("localhost", 30200));
    CPPUNIT_ASSERT(d1.start() && d2.start());

	std::this_thread::sleep_for( std::chrono::milliseconds(DOCKER_REACTION_TIME) );

	CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d1.get_channels().size());
    CPPUNIT_ASSERT_EQUAL(1u, (unsigned int)d2.get_channels().size());

    /* Form a message */
    const size_t size = 1024 * 1024 * 32;
    Network::buffer message;
    message.reserve(size);
    for (size_t i = 0; i < size; i++)
        message.push_back(randomChar());

    d1.send(boost::string_ref(&*message.begin(), message.size()));

    { 
        /* Receive the B message and resend it to A */
        Network::bufferPtr receivedB;  
        ASYNC_RECEIVE_TEST(receivedB, (&d2));
        CPPUNIT_ASSERT(*receivedB == message);
    }
	d1.async_shutdown();
	d2.async_shutdown();
	/* pause between tests */
	std::this_thread::sleep_for(std::chrono::milliseconds(DOCKER_REACTION_TIME));
}