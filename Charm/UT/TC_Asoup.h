#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Asoup/Message.h"
#include "../Asoup/MessagePassengerLDZ.h"

class TC_Asoup : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_Asoup);

    CPPUNIT_TEST(tc_Creation);      // Простейшие тесты
    CPPUNIT_TEST(tc_Readers);       // Тесты классов чтения данных из потока
    CPPUNIT_TEST(tc_Parse1042);     // Тестирование разбора сообщения 1042
    CPPUNIT_TEST(tc_Parse1042_2);   // Тестирование разбора сообщения 1042
    CPPUNIT_TEST(tc_Parse1042_3);   // Тестирование разбора блока Ю1 сообщения 1042
    CPPUNIT_TEST(tc_Parse904);      // Тестирование разбора сообщения 904
    CPPUNIT_TEST(tc_Parse904_2);    // Тестирование разбора сообщения 904
    CPPUNIT_TEST(tc_Parse333);      // Тестирование разбора сообщения 333
    CPPUNIT_TEST(tc_TrimMessage);   // Тестирование обрезания пробельных символов с начала и с конца сообщения
    CPPUNIT_TEST(tc_Parse904_3);    // Тестирование разбора реального сообщения, которое не разбирала библиотека
    CPPUNIT_TEST(tc_Parse1042_4);   // Тестирование разбора реального сообщения, которое не разбирала библиотека
    CPPUNIT_TEST(tc_Parse1042_5);   // Тестирование разбора реального сообщения, которое не разбирала библиотека
    CPPUNIT_TEST(tc_SavingTheSource);   // Тестирование сохранения исходного текста в АСОУП сообщении
    CPPUNIT_TEST(tc_CleaningTheSource); // Тестирование очистки исходного текста от лишних символов при разборе
    CPPUNIT_TEST(tc_CreationTime);  // Тестирование правильной работы с датой получения АСОУП сообщения
    CPPUNIT_TEST(tc_NearYearChange);// Тестирование корректного приема АСОУП при наступлении Нового года
    CPPUNIT_TEST(tc_Parse1042_6);   // Тестирование разбора сообщения 1042 с длинным блоком Ю12
    CPPUNIT_TEST(tc_Parse1042_7);   // Тестирование разбора блока Ю3 сообщения 1042 с разными фамилиями машинистов локомотивов
    CPPUNIT_TEST(tc_February29);   // Тестирование разбора сообщений с датой 29 февраля
    CPPUNIT_TEST(tc_Parse17082016); // Тестирование разбора сообщения 1042, полученного 17 августа 2016 года
    CPPUNIT_TEST(tc_Parse1042_Yu3_WithoutMachinist); // Тестирование разбора блокаю Ю3 сообщения 1042 без фамилии машиниста
    CPPUNIT_TEST(tc_IsWithdrawal); // Тестирование определения типа сообщения об отмене другого АСОУП
    CPPUNIT_TEST(tc_Parse1042_Yu12_upd); // Тестирование разбора сообщения 1042 с Ю12 (обновление 10.2018)
    CPPUNIT_TEST(tc_ParseMessagePassagirInfo);    //Тестирование разбора сообщений по электричкам (PV LDZ)
    CPPUNIT_TEST(tc_Parse1042_8); // Разбор натурки более 100 вагонов с. 904 (ошибка разбора 27.07.2019 06:43..07:44 )
    CPPUNIT_TEST(tc_acceptAsoupEmptyCrew_6193);  //корректный прием и серилизация АСОУП-сообщения с локомотивом без бригады
    CPPUNIT_TEST(tc_StrangeAsoupCheck); //проверка некорректных сообщений АСОУП 

    CPPUNIT_TEST_SUITE_END();

public:
    virtual void setUp();
    virtual void tearDown();

protected:
    void tc_Creation();
    void tc_Readers();
    void tc_Parse904();
    void tc_Parse904_2();
    void tc_Parse1042();
    void tc_Parse1042_2();
    void tc_Parse1042_3();
    void tc_Parse333();
    void tc_TrimMessage();
    void tc_Parse904_3();
    void tc_Parse1042_4();
    void tc_Parse1042_5();
    void tc_SavingTheSource();
    void tc_CleaningTheSource();
    void tc_CreationTime();
    void tc_NearYearChange();
    void tc_Parse1042_6();
    void tc_Parse1042_7();
    void tc_February29();
    void tc_Parse17082016();
    void tc_Parse1042_Yu3_WithoutMachinist();
    void tc_IsWithdrawal();
    void tc_Parse1042_Yu12_upd();
    void tc_ParseMessagePassagirInfo();
    void tc_Parse1042_8(); // более 100 вагонов в составе поезда
    void tc_acceptAsoupEmptyCrew_6193();
    void tc_StrangeAsoupCheck();

private:
    template< typename messageType, typename enumType >
    void verify(const messageType& message, enumType attribute, const wchar_t *value) const;
    void verify(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, unsigned long value) const;
    void verify(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, Asoup::OperationCodeEnum value) const;
    void verifyDateTime(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute, time_t value) const;
    void verify(const Asoup::Message& message, const Trindex& value) const;
    void verifyNoAttr(const Asoup::Message& message, Asoup::AsoupMsgAttribute attribute) const;
    void verify(const Asoup::MessagePassengerLDZ& message, Asoup::AsoupMsgPassengerLdzAttribute attribute, const wchar_t *value) const;

    boost::gregorian::date m_receivedDate;
};
