#pragma once

#include "../helpful/StateLTS.h"
#include "../StatBuilder/basic_defs.h"
#include "../StatBuilder/Link.h"
#include "../helpful/Attic.h"
#include "TestTracker.h"
#include "../helpful/ADProperties.h"
#include "TestSpanKit.h"

//вспомогательный класс тестирования логического элемента
//(файл картинки тестовых объектов и связей - \potap\Charm\Рабочая документация\Отслеживание\Тестовый полигон.vsd)
class LOS_base;
class CLogicElement;
class TestElem
{
public:
	TestElem( ELEMENT_CLASS, const BadgeE & elbadge, bool have_ts, unsigned int objtype, bool intoOneWaySpan = false );
	boost::shared_ptr <CLogicElement> lePtr;
	BadgeE bdg;
	bool intoOneWaySpan;
	void Link( const CLink & );
	std::shared_ptr <LOS_base> Los() const;

private:
	static std::shared_ptr <LOS_base> CreateLos( StateLTS firstLts, StateLTS secondLts, ELEMENT_CLASS );
	std::shared_ptr <LOS_base> losPtr;
};
typedef std::shared_ptr<TestElem> TestElemPtr;

typedef std::map<std::wstring, TestElem> TstElmap;

class InfraPart
{
public:
	virtual TestElemPtr Get( std::wstring ) const;
	const TstElmap & GetAllObjects() const { return tstobjects; }
	const std::map<EsrKit, TestSpanKitPtr> & GetAllSpans() const { return spanKits; }
	void Reset();

protected:
	void Insert( ELEMENT_CLASS, BadgeE[], unsigned int arr_size, bool have_ts = true, unsigned int objtype = 0, bool one_way_span = false );
	void InsertRoute( const BadgeE & route, const std::vector <CLink> & ); //вставка маршрута

	CLink FormLink( std::wstring, unsigned int link_type = CLink::UNDEF ) const;
	void Link( std::wstring, const CLink & );
	void Link( std::wstring, const CLink &, const CLink & );
	void Link( std::wstring, const std::vector <CLink> & );
	void LinkSema( std::wstring legBdg, std::wstring fromStrip, std::wstring toStrip, const std::vector<std::wstring> & heads_of_leg );

	TstElmap tstobjects; //todo: критической необходимости в данном контейнере нет (надо использовать лосы из Tracker'а)
	std::map<EsrKit, TestSpanKitPtr> spanKits; //перегоны
};