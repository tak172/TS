#pragma once

#include "TC_Project.h"

class TC_SemalegPartition : public TC_Project
{
	CPPUNIT_TEST_SUITE( TC_SemalegPartition );
	CPPUNIT_TEST( StationCase );
	// CPPUNIT_TEST( SpanCase );
	CPPUNIT_TEST( StationUnresolvedConflict );
	CPPUNIT_TEST( StationResolvedConflict );
	CPPUNIT_TEST( AlienLegOnRoute );
	CPPUNIT_TEST( DistributeAfterSemaUnjoin );
	CPPUNIT_TEST_SUITE_END();

protected:
	void StationCase(); //разделение светофоров на станции
	void SpanCase(); //разделение светофоров на перегоне
	void StationUnresolvedConflict(); //наличие конфликта направлений мачт (неразрешенный)
	void StationResolvedConflict(); //наличие конфликта направлений мачт (разрешенный)
	void AlienLegOnRoute(); //связь маршрута с чужой мачтой
	void DistributeAfterSemaUnjoin(); //перераспределение светофоров после разъединения одного из них
};
