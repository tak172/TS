#include "stdafx.h"
#include "TC_Cartographer.h"

#include "../StatBuilder/Cartographer.h"

using namespace std;


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Cartographer );

void TC_Cartographer::AddNRemSingle()
{
	Cartographer C;
	wchar_t * obj_a = L"ObjectA";
	CGridRect grectA( 10, 10, 15, 15 );

	//добавляем
	C.Add( grectA, obj_a );
	set <GRID_COORD> x_coords, y_coords;
	CPPUNIT_ASSERT( C.Have( obj_a, x_coords, y_coords ) );
	CPPUNIT_ASSERT( *( x_coords.begin() )  == grectA.topleft.x ); 
    CPPUNIT_ASSERT( *( x_coords.rbegin() ) == grectA.rightbottom.x );
    CPPUNIT_ASSERT( *( y_coords.begin() )  == grectA.topleft.y );
    CPPUNIT_ASSERT( *( y_coords.rbegin() ) == grectA.rightbottom.y );

	//извлекаем
	CGridRect look_rect = grectA;
	look_rect.Shift( CGridSize( grectA.Width() / 2, grectA.Height() / 2 ) );
	vector <void *> inrect_elems = C.Get<void>( look_rect );
    CPPUNIT_ASSERT_EQUAL( size_t(1), inrect_elems.size() );
	CPPUNIT_ASSERT( inrect_elems.front() == obj_a );
	look_rect.Shift( CGridSize( grectA.Width(), grectA.Height() ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );

	//удаляем
	C.Remove( obj_a );
	CPPUNIT_ASSERT( C.Empty() );
}

void TC_Cartographer::AddNRemCommon( unsigned int x_num ,unsigned int y_num,
				double horiz_gap_factor, double vert_gap_factor )
{
	GRID_COORD width = 30, height = 40;
	GRID_COORD horiz_gap = ( 1 / ( double ) horiz_gap_factor ) * width - width;
	GRID_COORD vert_gap = ( 1 / ( double ) vert_gap_factor ) * height - height;

	//формирование
	Cartographer C;
	vector <pair <CGridRect, wchar_t *> > elements;
	for ( unsigned int x = 0; x < x_num; x++ )
	{
		for ( unsigned int y = 0; y < y_num; y++ )
		{
			GRID_COORD left = x * ( width + horiz_gap );
			GRID_COORD top = y * ( height + vert_gap );
			CGridRect grect( left, top, left + width, top + height );
			elements.push_back( make_pair( grect, new wchar_t ) );
		}
	}

	//добавление
	for( unsigned int k = 0; k < elements.size(); k++ )
		C.Add( elements[k].first, elements[k].second );

	//проверка добавления
	for( unsigned int k = 0; k < elements.size(); k++ )
	{
		CGridRect grect = elements[k].first;
		void * p_obj = elements[k].second;
		set <GRID_COORD> x_coords, y_coords;
		CPPUNIT_ASSERT( C.Have( p_obj, x_coords, y_coords ) );
		CPPUNIT_ASSERT( *( x_coords.begin() ) == grect.topleft.x && 
			*( x_coords.rbegin() ) == grect.rightbottom.x &&
			*( y_coords.begin() ) == grect.topleft.y &&
			*( y_coords.rbegin() ) == grect.rightbottom.y );
	}

	//проверка извлечения
	vector <void *> inrect_elems;
	vector <GRID_COORD> x_coords, y_coords;
	for( unsigned int k = 0; k < elements.size(); k++ )
	{
		CGridRect grect = elements[k].first;
		x_coords.push_back( grect.topleft.x );
		x_coords.push_back( grect.rightbottom.x );
		y_coords.push_back( grect.topleft.y );
		y_coords.push_back( grect.rightbottom.y );
		void * p_obj = elements[k].second;
		CGridRect look_rect = grect;
		look_rect.Shift( CGridSize( grect.Width() / 2, grect.Height() / 2 ) );
		inrect_elems = C.Get<void>( look_rect );
		auto it = std::find( inrect_elems.begin(), inrect_elems.end(), p_obj );
		auto it_to_null = std::find( inrect_elems.begin(), inrect_elems.end(), nullptr );
		CPPUNIT_ASSERT( it != inrect_elems.end() );
		CPPUNIT_ASSERT( it_to_null == inrect_elems.end() );
		look_rect.Shift( CGridSize( grect.Width(), grect.Height() ) );
		inrect_elems = C.Get<void>( look_rect );
        it = std::find( inrect_elems.begin(), inrect_elems.end(), p_obj );
		it_to_null = std::find( inrect_elems.begin(), inrect_elems.end(), nullptr );
		CPPUNIT_ASSERT( it == inrect_elems.end() );
		CPPUNIT_ASSERT( it_to_null == inrect_elems.end() );
	}
	sort( x_coords.begin(), x_coords.end() ); 
	sort( y_coords.begin(), y_coords.end() ); 
	CGridRect big_lookrect( CGridPoint( x_coords[0] + width / 2, y_coords[0] + height / 2 ),
		CGridPoint( x_coords[x_coords.size() - 1] - width / 2, y_coords[y_coords.size() - 1] - height / 2 )
		);
	inrect_elems = C.Get<void>( big_lookrect );
	CPPUNIT_ASSERT( inrect_elems.size() == elements.size() );

	//удаление (с конца)
	for( vector <pair <CGridRect, wchar_t *> >::reverse_iterator it = elements.rbegin(); it != elements.rend(); ++it )
	{
		CGridRect grect = it->first;
		void * p_obj = it->second;
		set <GRID_COORD> x_coords, y_coords;
		C.Remove( p_obj );
		CPPUNIT_ASSERT( !C.Have( p_obj, x_coords, y_coords ) );
		CPPUNIT_ASSERT( !C.Have( NULL, x_coords, y_coords ) );
	}
	CPPUNIT_ASSERT( C.Empty() );

	for( unsigned int k = 0; k < elements.size(); k++ )
		delete elements[k].second;
}

void TC_Cartographer::AddNRemMultipleNOvp()
{
	AddNRemCommon( 2, 2, 0.5, 0.5 );
}

void TC_Cartographer::AddNRemMultipleInJnt()
{
	AddNRemCommon( 3, 5, 1, 1 );
}

void TC_Cartographer::AddNRemMultipleOvp()
{
	AddNRemCommon( 3, 5, 2, 2 );
}

void TC_Cartographer::searchInner() // проверка поиска перекрытия
{
    // Расположение объектов-прямоугольников A,B,C
    // Поисковые запросы *.:?
    //     
    //    X ------ >
    //    1 2 3 4 5 6 7
    //      ??
    // Y1 AAAAAAAAAAAAAA
    // |2 AAAACCCCAA**AA   поиск '*' дает А
    // |3 AAbbCC..bbbbAA   поиск '.' дает A+b+C
    // V4 AAbbCCCC::bbAA   поиск ':' дает A+b
    //  5 AAAACCCCAAAAAA   поиск '?' дает A+b
    //  6 AAAACCCCAAAAAA
    //  7   ??

    Cartographer C;
    wchar_t * a = L"A";
    wchar_t * b = L"B";
    wchar_t * c = L"C";
    CGridRect rA( 1, 1, 7, 6 ); // БОЛЬШОЙ 
    CGridRect rB( 2, 3, 7, 5 ); // широкий
    CGridRect rC( 3, 2, 5, 7 ); // высокий

    CGridRect star( 6.1, 2.1, 6.5, 2.3 );
    CGridRect dot ( 4.1, 3.7, 4.7, 3.9 );
    CGridRect DDot( 5.1, 4.7, 5.7, 4.9 );
    CGridRect ques( 2.1, 0.9, 2.2, 7.1 );

    //добавляем
    C.Add( rA, a );
    C.Add( rB, b );
    C.Add( rC, c );
    // нахождение
    vector <wchar_t *> found;
    found = C.Get<wchar_t>( star );
    CPPUNIT_ASSERT( found.size() == 1 );
    CPPUNIT_ASSERT( found.front() == a );
    found = C.Get<wchar_t>( dot );
    CPPUNIT_ASSERT( found.size() == 3 );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), a ) != found.end() );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), b ) != found.end() );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), c ) != found.end() );
    found = C.Get<wchar_t>( DDot );
    CPPUNIT_ASSERT( found.size() == 2 );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), a ) != found.end() );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), b ) != found.end() );
    found = C.Get<wchar_t>( ques );
    CPPUNIT_ASSERT( found.size() == 2 );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), a ) != found.end() );
    CPPUNIT_ASSERT( std::find( found.begin(), found.end(), b ) != found.end() );
}

void TC_Cartographer::AddNRemSingleDistrNOvp()
{
	Cartographer C;
	wchar_t * obj_a = L"ObjectA";
	CGridRect grect1( 10, 100, 20, 200 );
	CGridRect grect2 = grect1;
	grect2.Shift( CGridSize( grect1.Width() * 2, grect1.Height() * 2 ) );

	//добавляем
	C.Add( grect1, obj_a );
	C.Add( grect2, obj_a );
	set <GRID_COORD> x_coords, y_coords;
	CPPUNIT_ASSERT( C.Have( obj_a, x_coords, y_coords ) );
	CPPUNIT_ASSERT( *( x_coords.begin() ) == grect1.topleft.x && 
		*( x_coords.rbegin() ) == grect2.rightbottom.x &&
		*( y_coords.begin() ) == grect1.topleft.y &&
		*( y_coords.rbegin() ) == grect2.rightbottom.y );

	//извлекаем
	CGridRect look_rect = grect1;
	look_rect.Inflate( 5, 5 );
	vector <void *> inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.size() == 1 && (*inrect_elems.begin()) == obj_a );
	look_rect = grect2;
	look_rect.Inflate( 5, 5 );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.size() == 1 && (*inrect_elems.begin()) == obj_a );
	look_rect = grect1;
	look_rect.Shift( CGridSize( 0, grect1.Height() + 5 ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );
	look_rect = grect1;
	look_rect.Shift( CGridSize( grect1.Width() + 5, 0 ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );
	look_rect = CGridRect( CGridPoint( grect1.rightbottom.x + ( GRID_COORD ) 1, grect1.rightbottom.y + ( GRID_COORD ) 1 ), 
		CGridPoint( grect2.topleft.x - ( GRID_COORD ) 1, grect2.topleft.y - ( GRID_COORD ) 1 ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );

	//удаляем
	C.Remove( obj_a );
	CPPUNIT_ASSERT( C.Empty() );
}

void TC_Cartographer::AddNRemSingleDistrOvp()
{
	Cartographer C;
	wchar_t * obj_a = L"ObjectA";
	CGridRect grect1( 10, 100, 30, 300 );
	CGridRect grect2 = grect1;
	grect2.Shift( CGridSize( grect1.Width() / 2, grect1.Height() / 2 ) );

	//добавляем
	C.Add( grect1, obj_a );
	C.Add( grect2, obj_a );
	set <GRID_COORD> x_coords, y_coords;
	CPPUNIT_ASSERT( C.Have( obj_a, x_coords, y_coords ) );
	CPPUNIT_ASSERT( *( x_coords.begin() ) == grect1.topleft.x );
	CPPUNIT_ASSERT( *( x_coords.rbegin() ) == grect2.rightbottom.x );
	CPPUNIT_ASSERT( *( y_coords.begin() ) == grect1.topleft.y );
	CPPUNIT_ASSERT( *( y_coords.rbegin() ) == grect2.rightbottom.y );

	//извлекаем
	CGridRect init_look_rect = CGridRect( CGridPoint( grect1.topleft.x + 1, grect1.topleft.y + 1 ), 
		CGridPoint( grect1.topleft.x + grect1.Width() / 2 - 1, grect1.topleft.y + grect1.Height() / 2 - 1 ) );
	CGridRect look_rect = init_look_rect;
	look_rect.Shift( CGridSize( 0, grect1.Height() ) );
	vector <void *> inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );
	look_rect = init_look_rect;
	look_rect.Shift( CGridSize( grect1.Width(), 0 ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.empty() );
	look_rect = CGridRect( CGridPoint ( grect1.rightbottom.x - grect1.Width() / 2, grect1.rightbottom.y - grect1.Height() / 2 ),
		CGridPoint ( grect2.rightbottom.x - grect2.Width() / 2, grect2.rightbottom.y - grect2.Height() / 2 ) );
	inrect_elems = C.Get<void>( look_rect );
	CPPUNIT_ASSERT( inrect_elems.size() == 1 );
    CPPUNIT_ASSERT( (*inrect_elems.begin()) == obj_a );

	//удаляем
	C.Remove( obj_a );
	CPPUNIT_ASSERT( C.Empty() );
}

void TC_Cartographer::Dimension()
{
    // Расположение объектов-прямоугольников B,C,W
    //     
    //  X --------- >
    // Y  1 2 3 4 5 6
    // |1               
    // |2     CCCC    
    // |3   bbCwCCbbbb
    // |4   bbCCCCbbbb
    // |5     CCCC    
    // V6     CCCC      

    wchar_t * b = L"B";
    wchar_t * c = L"C";
    wchar_t * w = L"W";
    CGridRect rB( 2, 3, 7.1, 5   ); // широкий
    CGridRect rC( 3, 2, 5,   7.1 ); // высокий
    CGridRect rW( 3.1, 3.1, 3.2, 3.2 ); // внутренний

    std::pair<int,int> dim_( 0, 0 );
    std::pair<int,int> dimB( int(ceil(rB.rightbottom.x)), int(ceil(rB.rightbottom.y)) );
    std::pair<int,int> dimC( int(ceil(rC.rightbottom.x)), int(ceil(rC.rightbottom.y)) );
    std::pair<int,int> dimBC( max(dimB.first,dimC.first), max(dimB.second,dimC.second) );

    // проверка
    Cartographer K;
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dim_ );
    K.Add( rB, b );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dimB );
    K.Add( rC, c );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dimBC );
    K.Add( rW, w );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dimBC );
    K.Remove( w );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dimBC );
    K.Remove( b );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dimC );
    K.Remove( c );
    CPPUNIT_ASSERT( K.GetFactGridDimensions()==dim_ );
}
