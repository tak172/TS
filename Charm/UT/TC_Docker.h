#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Docker/dk_docker.h"

/* 
 * Класс для проверки работы класса EsrKit
 */

class TC_Docker : public CPPUNIT_NS::TestFixture
{
  CPPUNIT_TEST_SUITE(TC_Docker);
  CPPUNIT_TEST(dk_responseTimeTest); /* Responce time test */
  CPPUNIT_TEST(dk_chainTest);        /* Asynchronous test */
  CPPUNIT_TEST(dk_manyDockers);
  CPPUNIT_TEST(dk_getNewChannels);
#ifndef LINUX
  CPPUNIT_TEST(dk_doubleChannelsTest);	/* don't work on linux*/
#endif // !LINUX
  CPPUNIT_TEST(dk_bigMessageSendingTime);
  /* CPPUNIT_TEST(dk_restartTest); TODO: Implement docker restart mechanism and enable this */
  CPPUNIT_TEST_SUITE_END();

public:
  void setUp() override; // только для контрольки памяти

  /* Test 4-cell chain docker linkage */
  void dk_chainTest();

  /* Test of creating many dockers in a row */
  void dk_manyDockers();

  /* Getting new channels */
  void dk_getNewChannels();

  /* Test of sending message to docker and back in some period of time */
  void dk_responseTimeTest();

  /* Test of detecting double channels with their closure */
  void dk_doubleChannelsTest();

  /* Tests restarting dockers */
  void dk_restartTest();

  /* Ensures big message fast sending time */
  void dk_bigMessageSendingTime();

  void tearDown() override;
};
