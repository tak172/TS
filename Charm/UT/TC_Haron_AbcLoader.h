#pragma once
#include <cppunit/extensions/HelperMacros.h>

//��������� �������� ������ Haron::AbcLoader
class TC_Haron_AbcLoader : public CPPUNIT_NS::TestFixture
{
    CPPUNIT_TEST_SUITE( TC_Haron_AbcLoader );
	//���� ������ ��� �������� �������������� ������ 
	CPPUNIT_TEST( LoadSteeringNevaUnit  );
	CPPUNIT_TEST( LoadSteeringTraktUnit  );
	CPPUNIT_TEST( ConvertHexData );
	CPPUNIT_TEST( GenerationSpellCmd );
	CPPUNIT_TEST( GenerationObsolete );
    CPPUNIT_TEST_SUITE_END();

public:
    void LoadSteeringNevaUnit();
	void LoadSteeringTraktUnit();
	void ConvertHexData();
	void GenerationSpellCmd();
	void GenerationObsolete();
};
