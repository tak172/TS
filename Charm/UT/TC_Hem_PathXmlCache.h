#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "UtHemHelper.h"

class TopologyTest;

class TC_Hem_PathXmlCache : public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_Hem_PathXmlCache );
    CPPUNIT_TEST( ChangesHappenLayer );
    CPPUNIT_TEST( ChangesAllEvents );
    CPPUNIT_TEST( ChangesFutureLayer );
    CPPUNIT_TEST( ReplaceSingleEvent );
    CPPUNIT_TEST( ChangesBySplit );
    CPPUNIT_TEST( ChangesByConcatenate );
    CPPUNIT_TEST( Auto2User );
    CPPUNIT_TEST( Auto2User_Misplaced );
    CPPUNIT_TEST( MultiReplaceSingleEvent );
    CPPUNIT_TEST( Auto2User_IdentifyNonexistent );
    CPPUNIT_TEST( AddManyEventsInHead );
	CPPUNIT_TEST_SUITE_END();

    void setTopologyPath( unsigned from, unsigned to, TopologyTest* topology, unsigned third = 1379 );

private:
    void tearDown() override;
    void ChangesHappenLayer(); // �������� ��������� ��������� �� HappenLayer
    void ChangesAllEvents();   // �������� ��������� ����� ���� ������� ����
    void ChangesFutureLayer(); // �������� ��������� ��������� ��� ���� FutureLayer
    void ReplaceSingleEvent(); // �������� ��������� ��� ������� ������������� �������
    void ChangesBySplit(); // ��������� ��������� ����������� �����
    void ChangesByConcatenate(); // ��������� ��������� �������� �����
    void Auto2User(); // �������� ��������� �� AutoChart � UserChart
    void Auto2User_Misplaced(); // �������� ��������� �� AutoChart � UserChart (�������� ����������)
    void MultiReplaceSingleEvent(); // ��������� �������� ��������� ���������� ��������� ��������� Trio
    void Auto2User_IdentifyNonexistent(); // ������������� ��������������� �������
    void AddManyEventsInHead(); // ������ ���������� ���������� ������� � ������ ����

    UtIdentifyCategoryList identifyCategory;
};
