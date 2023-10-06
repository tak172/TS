#pragma once

//проверка работы со статическими данными отслеживания (набор связанных логических объектов)

#include <cppunit/extensions/HelperMacros.h>
#include "TC_TrackerTrainImpl.h"

class TC_TrainField : 
	private TC_TrackerTrainImpl,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_TrainField );
	CPPUNIT_TEST( PathThroughTwoSwitches );
	CPPUNIT_TEST( BidStationPockets ); //генерация карманов на станциях запрещена
	CPPUNIT_TEST( PathFindNearWOTsStrip ); //поиск пути через стрелку рядом с участком без ТС
	CPPUNIT_TEST( PathSearchingDepthDeficit ); //недостаточная глубина обхода при анализе движения ПЕ
	CPPUNIT_TEST( PathSearchingDepthDeficit2 );
	CPPUNIT_TEST( PathSearchingDepthDeficit3 );
	CPPUNIT_TEST( PathSearching );
	CPPUNIT_TEST( PathSearching2 );
	CPPUNIT_TEST( PathSearching3 );
	CPPUNIT_TEST( PathSearching4 );
	CPPUNIT_TEST( BranchedPathSearching );
	CPPUNIT_TEST( BranchedPathSearching2 );
	CPPUNIT_TEST( PaveOverlappedPaths );
	CPPUNIT_TEST( PaveOverlappedPaths2 );
	CPPUNIT_TEST( TopologicallyUnreachable );
	CPPUNIT_TEST( PathSearchingSymmetry );
	CPPUNIT_TEST_SUITE_END();

	void PathThroughTwoSwitches();
	void BidStationPockets();
	void PathFindNearWOTsStrip();
	void PathSearchingDepthDeficit();
	void PathSearchingDepthDeficit2();
	void PathSearchingDepthDeficit3();
	void PathSearching();
	void PathSearching2();
	void PathSearching3();
	void PathSearching4();
	void BranchedPathSearching();
	void BranchedPathSearching2();
	void PaveOverlappedPaths();
	void PaveOverlappedPaths2();
	void TopologicallyUnreachable();
	void PathSearchingSymmetry();
};