#pragma once

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Project.h"

class TC_LObjectManager : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_LObjectManager );
	CPPUNIT_TEST( GenerateRouteAlienLastJoint );
	CPPUNIT_TEST( JointNameChanging );
	CPPUNIT_TEST( JointConflictNameChanging );
	CPPUNIT_TEST( ChangeStripNameToSame );
	CPPUNIT_TEST( RouteUndependencyToCommutatorRemove );
	CPPUNIT_TEST( RouteDependencyToAlienStrip );
	CPPUNIT_TEST( RenameHeadIntoRoute );
	CPPUNIT_TEST( RenameRoutesCommonFinalStrip );
	CPPUNIT_TEST( InterstationRouteAndCommutator );
	CPPUNIT_TEST_SUITE_END();

	void GenerateRouteAlienLastJoint(); //генерация маршрута с последним стыком на соседней станции
	void JointNameChanging(); //проверка смены имени стыка
	void JointConflictNameChanging(); //проверка смены имени стыка при конфликте имен
	void ChangeStripNameToSame(); //изменение имени участка со стыком на то же
	void RouteUndependencyToCommutatorRemove(); //удаление коммутатора не должно затрагивать существование маршрута
	void RouteDependencyToAlienStrip(); //удаление участка, входящего в чужую станцию, должно разрушать маршрут
	void RenameHeadIntoRoute(); //переименование головый светофора, входящего в маршрут
	void RenameRoutesCommonFinalStrip(); //переименование финального участка, входящего в несколько маршрутов
	void InterstationRouteAndCommutator(); //межстанционный маршрут со связанным коммутатором на чужой станции
};