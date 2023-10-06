#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Graphic.h"

class TC_Manipulator :
	public TC_Graphic,
	public CPPUNIT_NS::TestFixture
{
private:
	CPPUNIT_TEST_SUITE( TC_Manipulator );
	CPPUNIT_TEST( MoveStateWhileManipulating );
	CPPUNIT_TEST( EvenlenStripRotating );
	CPPUNIT_TEST( OddlenStripRotating );
	CPPUNIT_TEST( StripNFigureRotating );
	CPPUNIT_TEST( NullLengthStripCreating );
	CPPUNIT_TEST( NameMoving );
	CPPUNIT_TEST( LinksOrder );
	CPPUNIT_TEST( ScalingAndRemove );
	CPPUNIT_TEST( SmallSwitchRegenarating );
	CPPUNIT_TEST( RoughMoving );
	CPPUNIT_TEST( MoveFixedAndUnfixed );
	CPPUNIT_TEST( EmptyPoolAfterHit );
	CPPUNIT_TEST( ClickOnSwitchedStrip );
	CPPUNIT_TEST( ZTestAfterGhostSemaforAdding );
	CPPUNIT_TEST( TextMerging );
	CPPUNIT_TEST( SemaZChanging );
	CPPUNIT_TEST( SelectingGraphicTextsQuasiName );
	CPPUNIT_TEST_SUITE_END();

	void MoveStateWhileManipulating();
	void EvenlenStripRotating(); //вращение участка четной длины
	void OddlenStripRotating(); //вращение участка нечетной длины
	void StripNFigureRotating(); //вращение участка и фигуры
	void NullLengthStripCreating(); //создание участка нулевой длины
	void NameMoving(); //смещение имени
	void LinksOrder(); //проверка сохранени€ пор€дка следовани€ графических линков при отработке модификационного действи€
	void ScalingAndRemove(); //изменение масштаба перед удалением объекта
	void SmallSwitchRegenarating(); //пересоздание стрелки с малым радиусом
	void RoughMoving(); //грубое смещение (с нажатым Ctrl)
	void MoveFixedAndUnfixed(); //смещение зафиксированных и не зафиксированных объектов
	void EmptyPoolAfterHit(); //после тычка в графический объект пул манипул€ций должен быть пуст
	void ClickOnSwitchedStrip(); //тычок в стрелочный участок
	void ZTestAfterGhostSemaforAdding(); //проверка z-уровней после добавлени€ графического ghost-светофора
	void TextMerging(); //сли€ние текстовых полей с объектами
	void SemaZChanging(); //движение по истории после изменени€ z-уровн€
	void SelectingGraphicTextsQuasiName(); //попытка выделени€ текстового графического объекта за область псевдоимени
};