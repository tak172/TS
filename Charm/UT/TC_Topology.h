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

	void IncorrectRequest(); //запрос на несуществующие значения
	void StationsDigraphCrotch(); //наличие развилки (тестирование StationsDigraph)
	void StationsDigraphUnidir(); //наличие одностороннего перехода (тестирование StationsDigraph)
	void StationsNetCrotch(); //наличие развилки (тестирование StationsNet)
	void StationsNetDoubleCrotch(); //наличие двух развилок (тестирование StationsNet)
	void StationsNetUnidir(); //наличие одностороннего перехода (тестирование StationsNet)
	void StationsNetSynonyms(); //проверка синонимов
	void Commentaries(); //пропуск комментариев
	void StationsNetSpanToSpan(); //поиск по перегонам
	void ReversiveTrios(); //разбор противонаправленных троек
    void SingleSpanSearch(); //поиск внутри одного вложенного пути
    void FileTopologyTest(); //поиск пути по реальному файлу топологии
	void MultiPaths(); //ситуация множественных путей между станциями
	void StationPassingBid(); //запрет на движение через станцию
    void BidirectionSearch(); // поиск внутри одной цепочки - в двух направлениях
	void Deserialize();
    void DeserializeHelper();
	void PathBetweenSynonyms(); //запрос на путь между кодами-синонимами должен оставлять их в результате
    void ShortestPath(); // поиск кратчайшего пути #4422
    void WishPath();     // поиск пути через указанные станции
};