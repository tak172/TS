#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_AugurResponse : public CPPUNIT_NS::TestFixture
{
public:

private:
	CPPUNIT_TEST_SUITE( TC_AugurResponse );
	CPPUNIT_TEST( Serialize );
	CPPUNIT_TEST( Deserialize );
	CPPUNIT_TEST( SerializeAugurEventFeedback ); //только проверка (де)сериализации обратной связи по событию со стороны прогноза
	CPPUNIT_TEST( DeserializeAugurEventFeedback );
    //CPPUNIT_TEST( ProcessResponse );
    CPPUNIT_TEST_SUITE_END();

	void Serialize();
	void Deserialize();
	void SerializeAugurEventFeedback();
	void DeserializeAugurEventFeedback();
    void ProcessResponse();
};