#include "stdafx.h"

#include "Parody.h"
#include "TC_Span.h"
#include "../helpful/EsrGuide.h"
#include "../Fund/Span.h"

using namespace std;
using namespace Parody;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Span );

EsrKit TC_Span::TERM1 = EsrKit(111);
EsrKit TC_Span::TERM2 = EsrKit(222);
EsrKit TC_Span::SPAN12 = EsrKit(111,222);

void TC_Span::setUp()
{
    Etui::instance();

    // построение перегона
    chain = make_chain( "A 7~200 B 7~300 C", SPAN12, L"туда" );
    Strip nap ("NAP", TERM1);
    Strip chap("CHAP", TERM2);

    Joint( nap,  Strip( chain.front() ) ).coord(L"туда", "7~100");
    Joint( chap, Strip( chain.back()  ) ).coord(L"туда", "7~400");
    SemaLeg( "N", SPAN12 ).from_to( Strip(chain.front()), nap  ).oddness( Oddness::ODD  );
    SemaLeg( "P", SPAN12 ).from_to( Strip(chain.back() ), chap ).oddness( Oddness::EVEN );

    attic::a_document doc;
    doc.load_wide(
        L"<My>"
        L"<EsrGuide kit='00111' name='станция 1' picketing1_val='5~55' picketing1_comm='туда' />"
        L"<EsrGuide kit='00222' name='станция 2' picketing1_val='8~88' picketing1_comm='туда' />"
        L"</My>"
        );
    esrGuide.load_from(doc.document_element());
}

void TC_Span::tearDown()
{
    Etui::Shutdowner();
}

void TC_Span::way_normal()
{
    Etui::instance()->complete();
    auto& lex = Etui::instance()->lex();
    SpanNs::Span sp( chain, esrGuide, SPAN12, lex );
    CPPUNIT_ASSERT( !sp.empty() );
    vector<wstring> errs = sp.takeError();
    CPPUNIT_ASSERT( errs.empty() );
}

void TC_Span::unknown_sema_parity()
{
    SemaLeg( "N", SPAN12 ).oddness( Oddness::UNDEF  ); // четность не задана

    Etui::instance()->complete();
    auto& lex = Etui::instance()->lex();
    SpanNs::Span sp( chain, esrGuide, SPAN12, lex );
    CPPUNIT_ASSERT( !sp.empty() );
    vector<wstring> errs = sp.takeError();
    // согласно #4710 - ошибок не должно быть
    CPPUNIT_ASSERT( errs.empty() );
}

void TC_Span::reverse_sema_parity()
{
    SemaLeg( "rN", SPAN12 ).from_to( Strip("NAP", TERM1), Strip(chain.front()) ).oddness( Oddness::ODD  ); // обратный светофор неверной четности

    Etui::instance()->complete();
    auto& lex = Etui::instance()->lex();
    SpanNs::Span sp( chain, esrGuide, SPAN12, lex );
    CPPUNIT_ASSERT( !sp.empty() );
    vector<wstring> errs = sp.takeError();
    CPPUNIT_ASSERT( !errs.empty() );
}

vector<BadgeE> TC_Span::make_chain( string items, EsrKit esr, wstring ruler  )
{
    vector<BadgeE> result;
    vector<string> item = Elements::split_names( items );
    CPPUNIT_ASSERT( item.size() >= 3 && item.size()%2 == 1 ); // несколько элементов и координаты между ними
    for( auto it = item.begin(); item.end() != next(it) && item.end() != next(it,2); it+=2 )
    {
        auto coordinate = *next(it);
        auto nx = next(it,2);
        Joint curr_joint( Strip( *it, esr ), Strip( *nx, esr ) );
        if ( coordinate != "~" )
            curr_joint.coord( ruler, coordinate );
        if ( result.empty() )
            result.push_back( BadgeE( From1251( *it ), esr ) );
        result.push_back( BadgeE( From1251( *nx ), esr ) );
    }
    return result;
}
