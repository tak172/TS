#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "../Asoup/Message.h"
#include "../Asoup/MessagePassengerLDZ.h"

class TC_Asoup : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE(TC_Asoup);

    CPPUNIT_TEST(tc_Creation);      // ���������� �����
    CPPUNIT_TEST(tc_Readers);       // ����� ������� ������ ������ �� ������
    CPPUNIT_TEST(tc_Parse1042);     // ������������ ������� ��������� 1042
    CPPUNIT_TEST(tc_Parse1042_2);   // ������������ ������� ��������� 1042
    CPPUNIT_TEST(tc_Parse1042_3);   // ������������ ������� ����� �1 ��������� 1042
    CPPUNIT_TEST(tc_Parse904);      // ������������ ������� ��������� 904
    CPPUNIT_TEST(tc_Parse904_2);    // ������������ ������� ��������� 904
    CPPUNIT_TEST(tc_Parse333);      // ������������ ������� ��������� 333
    CPPUNIT_TEST(tc_TrimMessage);   // ������������ ��������� ���������� �������� � ������ � � ����� ���������
    CPPUNIT_TEST(tc_Parse904_3);    // ������������ ������� ��������� ���������, ������� �� ��������� ����������
    CPPUNIT_TEST(tc_Parse1042_4);   // ������������ ������� ��������� ���������, ������� �� ��������� ����������
    CPPUNIT_TEST(tc_Parse1042_5);   // ������������ ������� ��������� ���������, ������� �� ��������� ����������
    CPPUNIT_TEST(tc_SavingTheSource);   // ������������ ���������� ��������� ������ � ����� ���������
    CPPUNIT_TEST(tc_CleaningTheSource); // ������������ ������� ��������� ������ �� ������ �������� ��� �������
    CPPUNIT_TEST(tc_CreationTime);  // ������������ ���������� ������ � ����� ��������� ����� ���������
    CPPUNIT_TEST(tc_NearYearChange);// ������������ ����������� ������ ����� ��� ����������� ������ ����
    CPPUNIT_TEST(tc_Parse1042_6);   // ������������ ������� ��������� 1042 � ������� ������ �12
    CPPUNIT_TEST(tc_Parse1042_7);   // ������������ ������� ����� �3 ��������� 1042 � ������� ��������� ���������� �����������
    CPPUNIT_TEST(tc_February29);   // ������������ ������� ��������� � ����� 29 �������
    CPPUNIT_TEST(tc_Parse17082016); // ������������ ������� ��������� 1042, ����������� 17 ������� 2016 ����
    CPPUNIT_TEST(tc_Parse1042_Yu3_WithoutMachinist); // ������������ ������� ������ �3 ��������� 1042 ��� ������� ���������
    CPPUNIT_TEST(tc_IsWithdrawal); // ������������ ����������� ���� ��������� �� ������ ������� �����
    CPPUNIT_TEST(tc_Parse1042_Yu12_upd); // ������������ ������� ��������� 1042 � �12 (���������� 10.2018)
    CPPUNIT_TEST(tc_ParseMessagePassagirInfo);    //������������ ������� ��������� �� ����������� (PV LDZ)
    CPPUNIT_TEST(tc_Parse1042_8); // ������ ������� ����� 100 ������� �. 904 (������ ������� 27.07.2019 06:43..07:44 )
    CPPUNIT_TEST(tc_acceptAsoupEmptyCrew_6193);  //���������� ����� � ����������� �����-��������� � ����������� ��� �������
    CPPUNIT_TEST(tc_StrangeAsoupCheck); //�������� ������������ ��������� ����� 

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
    void tc_Parse1042_8(); // ����� 100 ������� � ������� ������
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
