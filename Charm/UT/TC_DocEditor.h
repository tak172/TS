#pragma once

#include "TC_Project.h"

class TC_DocEditor : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_DocEditor );
	CPPUNIT_TEST( PseudoManipulation );
	CPPUNIT_TEST( LegGetting );
	CPPUNIT_TEST( TriangleJunctionUndo );
	CPPUNIT_TEST( SwitchedStripLogicSetUndo );
	CPPUNIT_TEST( UndoRemakingNamedSwitch );
	CPPUNIT_TEST( RedoUndoRemovingLObjectWithGLink );
	CPPUNIT_TEST( UndoRouteDoubleGenerating );
	CPPUNIT_TEST( ResetRemovingLObject );
	CPPUNIT_TEST( BlankLogicAction );
	CPPUNIT_TEST( ZChanging );
	CPPUNIT_TEST_SUITE_END();

	//работа с графикой
	void PseudoManipulation(); //имитаци€ манипул€ции, реально ничего не измен€ющей
	void LegGetting(); //манипул€ци€ с ножкой светофора (голова тоже должна манипулироватьс€)
	void TriangleJunctionUndo(); //откат создани€ треугольного соединени€ участков
	void SwitchedStripLogicSetUndo(); //откат назначени€ логической прив€зки дл€ стрелочного участка
	void UndoRemakingNamedSwitch(); //откат манипул€ции с участком, привод€щей к пересозданию именованной стрелки
	void ZChanging(); //манипул€ци€ с z-пор€дком

	//работа с логикой и графикой
	void RedoUndoRemovingLObjectWithGLink(); //откат удалени€ логического объекта с графической св€зью на него

	//работа с логикой
	void UndoRouteDoubleGenerating(); //откат после двойной генерации маршрутов
	void ResetRemovingLObject(); //сброс манипул€ции после попытки добавлени€ объекта
	void BlankLogicAction(); //действи€ с логическим объектом, не привод€щие к изменению
};