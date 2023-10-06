#pragma once

#include "../StatBuilder/basic_defs.h"
#include "../helpful/Attic.h"

class CLogicDocumentTest;
class CLogicDocument;
class CLinkManager;
class CStationManager;
class CLObjectManager;
class EsrKit;
class TrainDescr;
class TC_Logic
{
protected:
    void setUp_helper();
    void tearDown_helper();
	CLogicElement * BuildObject( ELEMENT_CLASS, bool add_to_doc = true, CLogicDocument * ldoc = 0 );
	std::wstring GenerateName( std::wstring base_name );
	CLogicElement * BuildLeg( HEAD_TYPE headType, CLogicDocument * = nullptr );
	CLogicElement * BuildLeg( const std::vector <HEAD_TYPE> & htypes, CLogicDocument * = nullptr );
	//возврат - пара сгенерированных (по необходимости) около стыка участков:
	CLogicElement * BuildJoint( std::pair <CLogicElement *, CLogicElement *> & strips_near, //first - from, second - to
		CLogicElement * lleg = 0, CLogicDocument * ldoc = 0 );
	CLogicElement * BuildSwitchKit( CLogicElement *& base_strip, CLogicElement *& plus_strip, 
		CLogicElement *& minus_strip, CLogicDocument * ldoc = 0 );
	void LinkLeg( CLogicElement * lleg, const std::pair <CLogicElement *, CLogicElement *> & strips_near ) const;
	CLogicElement * CreateTestRoute(); //создание тестового маршрута и его возврат
	void MakeTrainNode( attic::a_node, CLogicElement * lstrip, bool presence );
	std::pair <attic::a_node, attic::a_node> CreateInfoDoc( attic::a_document &, const TrainDescr & tdescr, unsigned int tunityId ) const;
	static void SetStationEsr( CLogicDocument *, const EsrKit & statCode );
	static void SetSpanEsr( CLogicDocument *, const EsrKit & spanCode );

protected:
	std::unique_ptr<CLogicDocumentTest> logdoc;
	CLinkManager * plim;
	CStationManager * pstam;
	CLObjectManager * plom;
	EsrKit EsrCode( CLogicDocument * = 0 ) const;

private:
	std::set <std::wstring> used_names;
};