#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Asoup/MessageMarkGui.h"

class TC_GidUralMark : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_GidUralMark);

    CPPUNIT_TEST(Parse_1);     // разбор
	CPPUNIT_TEST(Parse_2);     
	CPPUNIT_TEST(Parse_3);
	CPPUNIT_TEST(Parse_4);
	CPPUNIT_TEST(Parse_5);
	CPPUNIT_TEST(CreateEvent);	//создание пометки
	CPPUNIT_TEST(LoadAndSaveMessage);	//сериализация/десериализация сообщения ГИД-Урала
	CPPUNIT_TEST(LoadToEventAndSaveToMessage_1);	//десериализация, создание события, серилизация события в сообщение ГИД-Урала
	CPPUNIT_TEST(LoadToEventAndSaveToMessage_2);
	CPPUNIT_TEST(LoadToEventAndSaveToMessage_3);
	CPPUNIT_TEST(LoadToEventAndSaveToMessage_4);

    CPPUNIT_TEST_SUITE_END();

public:

protected:
    void Parse_1();
	void Parse_2();
	void Parse_3();
	void Parse_4();
	void Parse_5();
	void CreateEvent();
	void LoadAndSaveMessage();
	void LoadToEventAndSaveToMessage_1();
	void LoadToEventAndSaveToMessage_2();
	void LoadToEventAndSaveToMessage_3();
	void LoadToEventAndSaveToMessage_4();

private:
};
