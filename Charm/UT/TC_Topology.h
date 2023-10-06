#pragma once

#include <cppunit/extensions/HelperMacros.h>

class TC_Topology : public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_Topology );
	CPPUNIT_TEST( IncorrectRequest );
	CPPUNIT_TEST( StationsDigraphCrotch );
	CPPUNIT_TEST( StationsDigraphUnidir );
	CPPUNIT_TEST( StationsNetCrotch );
	CPPUNIT_TEST( StationsNetDoubleCrotch );
	CPPUNIT_TEST( StationsNetUnidir );
	CPPUNIT_TEST( StationsNetSynonyms );
	CPPUNIT_TEST( Commentaries );
	CPPUNIT_TEST( StationsNetSpanToSpan );
	CPPUNIT_TEST( ReversiveTrios );
    CPPUNIT_TEST( SingleSpanSearch );
	CPPUNIT_TEST( MultiPaths );
	CPPUNIT_TEST( StationPassingBid );
    CPPUNIT_TEST( BidirectionSearch );
	CPPUNIT_TEST( Deserialize );
    CPPUNIT_TEST( DeserializeHelper );
	CPPUNIT_TEST( PathBetweenSynonyms );
    CPPUNIT_TEST( ShortestPath );
    CPPUNIT_TEST( WishPath );

	CPPUNIT_TEST_SUITE_END();

	void IncorrectRequest(); //������ �� �������������� ��������
	void StationsDigraphCrotch(); //������� �������� (������������ StationsDigraph)
	void StationsDigraphUnidir(); //������� �������������� �������� (������������ StationsDigraph)
	void StationsNetCrotch(); //������� �������� (������������ StationsNet)
	void StationsNetDoubleCrotch(); //������� ���� �������� (������������ StationsNet)
	void StationsNetUnidir(); //������� �������������� �������� (������������ StationsNet)
	void StationsNetSynonyms(); //�������� ���������
	void Commentaries(); //������� ������������
	void StationsNetSpanToSpan(); //����� �� ���������
	void ReversiveTrios(); //������ ������������������� �����
    void SingleSpanSearch(); //����� ������ ������ ���������� ����
    void FileTopologyTest(); //����� ���� �� ��������� ����� ���������
	void MultiPaths(); //�������� ������������� ����� ����� ���������
	void StationPassingBid(); //������ �� �������� ����� �������
    void BidirectionSearch(); // ����� ������ ����� ������� - � ���� ������������
	void Deserialize();
    void DeserializeHelper();
	void PathBetweenSynonyms(); //������ �� ���� ����� ������-���������� ������ ��������� �� � ����������
    void ShortestPath(); // ����� ����������� ���� #4422
    void WishPath();     // ����� ���� ����� ��������� �������
};