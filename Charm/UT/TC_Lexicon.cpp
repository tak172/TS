#include "stdafx.h"

#include "TC_Lexicon.h"
#include "../helpful/Attic.h"
#include "../Fund/BbxConst.h"
#include "Parody.h"
#include "../StatBuilder/LogicElementFactory.h"

using namespace std;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Lexicon );

void TC_Lexicon::LogEl_vs_Badge()
{
    Lexicon lex;
    put_test_objects_to(lex);

    for( size_t k=0; k<OBJS.size(); k++ )
    {
        // получение ЛОСа
        CPPUNIT_ASSERT( NULL!=lex.getLos(OBJS[k]) );
        // поиск объекта по имени
        CPPUNIT_ASSERT( OBJS[k] == lex.LogElBy( BADGES[k] ) );
        // имя по объекту
        CPPUNIT_ASSERT( BADGES[k] == lex.BadgeBy( OBJS[k] ) );
    }
}

void TC_Lexicon::change_all()
{
    Lexicon lex;
    put_test_objects_to(lex);

    CPPUNIT_ASSERT( lex.noneChanges() );
    lex.set_all_changed();
    CPPUNIT_ASSERT( lex.countChanges() == OBJS.size() );
    lex.reset_all_changed();
    CPPUNIT_ASSERT( lex.noneChanges() );
}

void TC_Lexicon::change_single()
{
    Lexicon lex;
    put_test_objects_to(lex);

    // проверим индивидуальное изменение
    CPPUNIT_ASSERT( lex.noneChanges() );
    for( size_t k=0; k<OBJS.size(); k++ )
    {
        lex.reset_all_changed();
        lex.markAsChanged( OBJS[k] );
        auto ch = lex.getChanges();
        CPPUNIT_ASSERT( !ch.none() );
        CPPUNIT_ASSERT( OBJS[k]==get<0>( ch.front(lex) ) );
    }
}

void TC_Lexicon::append_repeatly()
{
    Lexicon lex;
    put_test_objects_to(lex);
    CPPUNIT_ASSERT( !lex.append( OBJS[0], ESR ) );
}

void TC_Lexicon::putAllXml()
{
    Lexicon lex;
    put_test_objects_to(lex);

    // проверим вывод всего в xml
    attic::a_document doc;
    doc.load_utf8("<myXML />");
    wstring pre  = FromUtf8( attic::a_node(doc).to_str() );
    lex.putAllToXml(doc.document_element(), "ref" );
    wstring post = FromUtf8( attic::a_node(doc).to_str() );
    CPPUNIT_ASSERT( pre!=post );

    attic::a_node node = doc.document_element().child("ref");
    CPPUNIT_ASSERT( 0!=node );
    set<BadgeE> already_met;
    for( attic::a_node it=node.first_child(); 0!=it; it=it.next_sibling() )
    {
        BadgeE bb;
        attic::a_attribute atr=it.attribute( BBX_L_BD() );
        CPPUNIT_ASSERT( 0!=atr );
        bb = BadgeE( atr );
        CPPUNIT_ASSERT( already_met.find(bb)==already_met.end() );
        already_met.insert(bb);
    }
    CPPUNIT_ASSERT( already_met.size()==OBJS.size() );

    lex.putAllToXml(doc.document_element(), "ref" );
    wstring post2 = FromUtf8( attic::a_node(doc).to_str() );
    CPPUNIT_ASSERT( post==post2 );
}

void TC_Lexicon::putChangesXml()
{
    Lexicon lex;
    put_test_objects_to(lex);

    // проверим вывод всего в xml
    CPPUNIT_ASSERT( lex.noneChanges() );
    for( size_t k=0; k<OBJS.size(); k++ )
    {
        attic::a_document doc_bef;
        attic::a_document doc_aft;
        doc_bef.set_document_element("pack");
        doc_aft.set_document_element("pack");

        lex.markAsChanged( OBJS[k] );
        lex.getLos( OBJS[k] )->next_typical_state(); // сделаем реальные изменения этого ЛОСа

        auto ch = lex.getChanges();
        lex.putChangedToXml_and_reset(doc_bef.document_element(),doc_aft.document_element());
        wstring bef = FromUtf8( attic::a_node(doc_bef).to_str());
        wstring aft = FromUtf8( attic::a_node(doc_aft).to_str());
        CPPUNIT_ASSERT( bef!=aft );

        attic::a_node node = doc_bef.child("pack");
        attic::a_node it=node.first_child();
        attic::a_attribute atr=it.attribute( BBX_L_BD() );
        BadgeE bb = BadgeE( atr );
        CPPUNIT_ASSERT( get<2>( ch.front(lex) ) == BADGES[k] );
        CPPUNIT_ASSERT( bb == BADGES[k] );
    }
}

void TC_Lexicon::setUp()
{
	CLogicElementFactory LEF;
    ESR = EsrKit(789);
    // подготовка данных
    MIX z(NULL,NULL,NULL);
    MIXS.push_back( z ); MIXS.back().get<2>() = LEF.CreateObject( SWITCH );
    MIXS.push_back( z ); MIXS.back().get<1>() = LEF.CreateObject( STRIP );
    MIXS.push_back( z ); MIXS.back().get<2>() = LEF.CreateObject( SWITCH );
    MIXS.push_back( z ); MIXS.back().get<1>() = LEF.CreateObject( STRIP );
    MIXS.push_back( z ); MIXS.back().get<0>() = LEF.CreateObject( FIGURE );
    BADGES.push_back( BadgeE( L"стрелка1",ESR) );
    BADGES.push_back( BadgeE( L"участо2к",ESR) );
    BADGES.push_back( BadgeE( L"стрел3ка",ESR) );
    BADGES.push_back( BadgeE( L"учас4ток",ESR) );
    BADGES.push_back( BadgeE( L"фиг5ураа",ESR) );
    for( size_t i=0; i<MIXS.size(); i++ )
    {
        CLogicElement* le=NULL;
        if ( !le ) le = MIXS[i].get<0>();
        if ( !le ) le = MIXS[i].get<1>();
        if ( !le ) le = MIXS[i].get<2>();
        OBJS.push_back(le);
        OBJS[i]->SetName( BADGES[i].str().c_str() );
    }

    CPPUNIT_ASSERT( MIXS.size()==OBJS.size() );
    CPPUNIT_ASSERT( MIXS.size()==BADGES.size() );
}
void TC_Lexicon::tearDown()
{
    for( size_t i=0; i<OBJS.size(); i++ )
        delete OBJS[i];
    OBJS.clear();
    BADGES.clear();
    MIXS.clear();
    ESR = EsrKit();
}

void TC_Lexicon::put_test_objects_to( Lexicon &lex )
{
    lex.reserve( MIXS.size() );
    for( size_t i=0; i<MIXS.size(); i++ )
    {
        if( !OBJS[ i ] )
            CPPUNIT_FAIL( "не все элементы заполнены!" );
        else if( MIXS[ i ].get<0>() ) CPPUNIT_ASSERT( lex.append( MIXS[ i ].get<0>(), ESR ) );
        else if( MIXS[ i ].get<1>() ) CPPUNIT_ASSERT( lex.append( MIXS[ i ].get<1>(), ESR ) );
        else if( MIXS[ i ].get<2>() ) CPPUNIT_ASSERT( lex.append( MIXS[ i ].get<2>(), ESR ) );
    }
}
