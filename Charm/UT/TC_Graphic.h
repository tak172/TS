#pragma once

#include "../StatBuilder/basic_defs.h"
#include "../StatBuilder/Geo.h"

class CGraphicElement;
class CGraphicDocumentTest;
class CGObjectManager;
class CGridPoint;
class CGridSize;
class Blinker;
class GViewSkin;
class TC_Graphic
{
public:
	TC_Graphic();
	virtual ~TC_Graphic();
    void setUp_helper();
    void tearDown_helper();
	GraphElemPtr FastCreate( ELEMENT_CLASS ) const;
	//при возвращении указателя удалять его не надо. уничтожением занимается сам документ
	//быстрое создание стрелочного комплекта (все участки берут начало из точки стрелки)
	CGraphicElement * BuildSwitchKit( const CGridPoint & switch_pt, 
		const CGridSize & base_size = CGridSize( -2, 0 ),
		const CGridSize & plus_size = CGridSize( 2, 0 ), 
		const CGridSize & minus_size = CGridSize( 2, 2 ) ) const;
	CGraphicElement * BuildSemalegKit( const CGridPoint &, unsigned int heads_num ) const;
	void Select( ELEMENT_CLASS ) const;
	void SelectAll() const;

protected:
	CGObjectManager * pgom;
	boost::scoped_ptr <CGraphicDocumentTest> gdoc;

private:
	ULONG_PTR gdiplusToken;
	
};