#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"
#include "TC_Logic.h"

class TC_ObsoleteLoader : 
	private TC_Graphic,
	private TC_Logic,
	public CPPUNIT_NS::TestFixture
{
	CPPUNIT_TEST_SUITE( TC_ObsoleteLoader );
	CPPUNIT_TEST( ObsoleteSingleBoardBranch );
	CPPUNIT_TEST( ObsoleteGBoardBranch );
	CPPUNIT_TEST( ObsoleteSemaforsBranch );
	CPPUNIT_TEST( ObsoleteFontsRemoving );
	CPPUNIT_TEST( ObsoleteSwitchRadius );
	CPPUNIT_TEST( ObsoleteStationAttributes );
	CPPUNIT_TEST( LStripArrdepToWaynumStation );
	CPPUNIT_TEST( LStripArrdepToWaynumSpan );
	CPPUNIT_TEST( DeleteOecLogicLegType );
	CPPUNIT_TEST( DeleteLogLinkType );
	CPPUNIT_TEST( LStripCapacityToTwiceCapacity );
	CPPUNIT_TEST( DeleteJointsParityChanging );
	CPPUNIT_TEST_SUITE_END();

    void setUp() override;
    void tearDown() override;
protected:
	void ObsoleteSingleBoardBranch(); //загрузка ветки SINGLE_BOARDS и замена ее веткой TABLE_BOARDS
	void ObsoleteGBoardBranch(); //загрузка ветки BOARDS и замена ее ветками TABLE_BOARDS и MULTI_BOARDS (в зависимости от типа объекта)
	void ObsoleteSemaforsBranch(); //загрузка ветки SEMAFORS как ветки SEMALEGS
	void ObsoleteFontsRemoving(); //удаление атрибутов устаревших шрифтов
	void ObsoleteSwitchRadius(); //пересчет радиуса стрелки
	void ObsoleteStationAttributes(); //удаление станционных атрибутов
	void LStripArrdepToWaynumStation(); //замена признака п/о пути на номер п/о пути (в случае, если он таковой) [на станции]
	void LStripArrdepToWaynumSpan(); //удаление признака п/о пути. переименование атрибута[на перегоне]
	void DeleteOecLogicLegType(); //удаление типа логической мачты (удаление атрибута для объекта в файле oec)
	void DeleteLogLinkType(); //удаление атрибута типа из ноды для логического линка (CLink)
	void LStripCapacityToTwiceCapacity(); //замена ненаправленной вместимости на вместимость в обоих направлениях
	void DeleteJointsParityChanging(); //удаление атрибута смены четности из логического стыка
};