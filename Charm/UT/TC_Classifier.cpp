#include "stdafx.h"
#include "TC_Classifier.h"
#include "UtHelpfulDateTime.h"

#include "../helpful/ClassifyGraph.h"
#include "../helpful/RailroadClassifier.h"
#include "../Actor/CarCharacteristics.h"
#include "../helpful/RoadCode.h"

/**
 *	��������� ��� ��������:
 *	  ���
 */

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Classifier );

enum class ToothProps
{
    Name,
    Position
};

void Properties<ToothProps>::load(const attic::a_node& loadFrom)
{
    tryReadAttr(ToothProps::Name, loadFrom, "name");
    tryReadAttr(ToothProps::Position, loadFrom, "pos");
}

typedef Classifier<ToothProps> TeethClassifier;

const std::wstring c_teethData = std::wstring() +
    L"<ClinicExportData>\n"
    L"  <Tooth name='" + trx_noop(L"���", L"���") + L"'>\n"
    L"    <Index from='11' to='18' />\n"
    L"    <Index from='21' to='28' />\n"
    L"    <Index from='31' to='38' />\n"
    L"    <Index from='41' to='48' />\n"
//     L"    <Tooth name='" + trx_noop(L"���", L"�����") + L"'>\n    // ������� �������� - ���������
//     L"      <Index from='16' to='18' />\n"
//     L"      <Index from='26' to='28' />\n"
//     L"      <Index from='36' to='38' />\n"
//     L"      <Index from='46' to='48' />\n"
//     L"    </Tooth>\n"
    L"    <Tooth name='" + trx_noop(L"���", L"��������") + L"'>\n"   // ����� ��������
    L"      <Tooth pos='" + trx_noop(L"���", L"�������") + L"'>\n"
    L"        <Index from='14' to='15' />\n" 
    L"        <Index from='24' to='25' />\n" 
    L"      </Tooth>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������") + L"'>\n"
    L"        <Index from='34' to='35' />\n"
    L"        <Index from='44' to='45' />\n"
    L"      </Tooth>\n"
    L"    </Tooth>\n"
    L"    <Tooth name='" + trx_noop(L"���", L"����") + L"'>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"�������") + L"'>\n"
    L"        <Index from='13' to='13' />\n"
    L"        <Index from='23' to='23' />\n"
    L"      </Tooth>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������") + L"'>\n"
    L"        <Index from='33' to='33' />\n"
    L"        <Index from='43' to='43' />\n"
    L"      </Tooth>\n"
    L"    </Tooth>\n"
    L"    <Tooth name='" + trx_noop(L"���", L"�����") + L"'>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������� �����������") + L"'>\n"
    L"        <Index from='11' to='11' />\n"
    L"        <Index from='21' to='21' />\n"
    L"      </Tooth>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������ �����������") + L"'>\n"
    L"        <Index from='31' to='31' />\n"
    L"        <Index from='41' to='41' />\n"
    L"      </Tooth>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������� �������") + L"'>\n"
    L"        <Index from='12' to='12' />\n"
    L"        <Index from='22' to='22' />\n"
    L"      </Tooth>\n"
    L"      <Tooth pos='" + trx_noop(L"���", L"������ �������") + L"'>\n"
    L"        <Index from='32' to='32' />\n"
    L"        <Index from='42' to='42' />\n"
    L"      </Tooth>\n"
    L"    </Tooth>\n"
    L"  </Tooth>\n"
    L"</ClinicExportData>\n";

void LoadTeethData(TeethClassifier& classifier, const std::wstring& data)
{
    attic::a_document doc;
    CPPUNIT_ASSERT(doc.load_wide(data));

    CPPUNIT_ASSERT_NO_THROW(classifier.load(doc.document_element(), "Tooth"));
}

void TC_Classifier::Loading()
{
    TeethClassifier classifier;
    LoadTeethData(classifier, c_teethData);
}

void TC_Classifier::ShouldKnowNames()
{
    TeethClassifier classifier;
    LoadTeethData(classifier, c_teethData);

    // ������ �� ����������� � ��������������, �� ����� 28 �������� � ��������� ������ �����
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"���"), classifier.getValue(28, ToothProps::Name));
    // ����� 11 �������� � ��������� ������ ����� � � ��������� ������ ������, �������
    // �������� ��� ������ ���� ����������� ��������� �����
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"�����"), classifier.getValue(11, ToothProps::Name));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"��������"), classifier.getValue(24, ToothProps::Name));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"����"), classifier.getValue(13, ToothProps::Name));

    CPPUNIT_ASSERT(classifier.getValue(19, ToothProps::Name).empty());
    CPPUNIT_ASSERT(classifier.getValue(0, ToothProps::Name).empty());
}

void TC_Classifier::ShouldKnowPositions()
{
    TeethClassifier classifier;
    LoadTeethData(classifier, c_teethData);

    // ������ �� �������, ��� �� ������ �������� �� ���������
    CPPUNIT_ASSERT(classifier.getValue(37, ToothProps::Position).empty());
    CPPUNIT_ASSERT(classifier.getValue(18, ToothProps::Position).empty());
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"������� �����������"), classifier.getValue(11, ToothProps::Position));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"������ �����������"), classifier.getValue(41, ToothProps::Position));
}

void TC_Classifier::RailroadClassifierData()
{
    RailroadClassifier rrClassifier;

    CPPUNIT_ASSERT_EQUAL(std::wstring(L"������"), rrClassifier.getValue(7840u, RailroadClassifier::Railroad::Country));
}

void TC_Classifier::CarClassifierData()
{
    CarCharacteristics carChars;

    CPPUNIT_ASSERT_EQUAL(std::wstring(L"��"), carChars.GetShortenedKind(52400001));
    CPPUNIT_ASSERT_EQUAL(std::wstring(L"4"), carChars.GetAxisCount(52400001));
}