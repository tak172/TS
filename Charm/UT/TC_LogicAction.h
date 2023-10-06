#pragma once

#include "TC_Project.h"

class TC_LogicAction : public TC_Project
{
private:
	CPPUNIT_TEST_SUITE( TC_LogicAction );
	CPPUNIT_TEST( JointedStripChanging );
	CPPUNIT_TEST( GLinkedStripChanging );
	CPPUNIT_TEST( JointedStripRemoving );
	CPPUNIT_TEST( ViewChangesOnHeadModify );
	CPPUNIT_TEST_SUITE_END();

	//изменение лог. участка, связанного со стыком:
	void JointedStripChanging();
	//изменение лог. участка, на который существует ссылка из графического документа:
	void GLinkedStripChanging();
	//удаление участка связанного со стыком
	void JointedStripRemoving();
	//проверка изменений для отображения дерева логических объектов при модификации головы
	void ViewChangesOnHeadModify();
};