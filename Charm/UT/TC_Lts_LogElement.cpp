#include "stdafx.h"

#include "TC_Lts_LogElement.h"
#include "Parody.h"
#include "../helpful/Informator.h"
#include "../Fund/AcceptorsBank.h"

using namespace std;
using namespace Parody;


CPPUNIT_TEST_SUITE_REGISTRATION( TC_Lts_LogElement );

/// Тест загрузки/сохранения акцепторов ЛТС для \ref CLogicStrip
void TC_Lts_LogElement::load_save_strip()
{
    // создать объекты
    const wchar_t * data =
        L"<test>"
        L"    <element name='1п' type='0' non_stop_passing='Y' >"
        L"        <ts used='1сп.бит2' ab='5488' />"
        L"        <ts lock='1сп.бит1' ab='5488' />"
        L"        <ts art_used='миг.1сп.бит2' ab='5488' />"
        L"        <ts art_lock='миг.1сп.бит1' ab='5488' />"
        L"    </element>"
        L"    <element name='2п' type='0' non_stop_passing='Y' >"
        L"        <ts used='2сп.бит2' ab='5488' />"
        L"        <ts lock='2сп.бит1' ab='5488' />"
        L"    </element>"
        L"    <element name='3пBombardir' type='0' non_stop_passing='Y' >"
        L"        <ts used='3п.бит2' ab='5488' />"
        L"        <ts lock='3п.бит1' ab='5488' />"
        L"        <ts lockSh='3п.SSS' ab='58' />"
        L"        <ts lockTr='3п.TTT' ab='58' />"
        L"    </element>"

        L"    <element name='Н1У' type='0' non_stop_passing='Y' >"
        L"        <ts used='Н1У.бит1' ab='5488' />"
        L"    </element>"
        L"<test>";
    Elements temp( STRIP, "1п 2п Н1У 3пBombardir" );

    // загрузка
    string u8source = to_raw(data);
    loading_ts_byName(u8source);
    // сохранение
    string u8saved = storing_ts_byName(u8source);
    // проверка
    CPPUNIT_ASSERT(u8source==u8saved);
}

/// Тест загрузки/сохранения акцепторов ЛТС для \ref CLogicSwitch
void TC_Lts_LogElement::load_save_switch()
{
    // создать объекты
    const wchar_t * data =
        L"<test>"
        L"    <element name='с1' type='0' >"
        L"        <ts positiv='1.бит1' ab='5488' />"
        L"        <ts negativ='1.бит2' ab='5488' />"
        L"    </element>"
        L"    <element name='с2' type='0' >"
        L"        <ts negativ='2.бит2' ab='5488' />"
        L"    </element>"
        L"    <element name='с3' type='0' />"
        L"    <element name='с4' type='0' >"
        L"        <ts positiv='1.бит1' ab='5488' />"
        L"        <ts negativ='1.бит2' ab='5488' />"
        L"        <ts     ooc='1.бит3' ab='5488' />"
        L"    </element>"
        L"<test>";
    Elements temp( SWITCH, "с1 с2 с3 с4" );

    // загрузка
    string u8source = to_raw(data);
    loading_ts_byName(u8source);
    // сохранение
    string u8saved = storing_ts_byName(u8source);
    // проверка
    CPPUNIT_ASSERT(u8source==u8saved);
}

/// Тест загрузки/сохранения акцепторов ЛТС для \ref CLogicFigure
void TC_Lts_LogElement::load_save_figure()
{
    // создать объекты
    const wchar_t * data =
        L"<test>"
        L"    <element name='ф1' type='0'>"
        L"        <ts mark='ф1.бит1' ab='5488' />"
        L"        <ts mark2='ф1.бит2' ab='5488' />"
        L"        <ts wink='миг.ф1.бит1' ab='5488' />"
        L"        <ts wink2='миг.ф1.бит2' ab='5488' />"
        L"    </element>"
        L"    <element name='ф2' type='0'>"
        L"        <ts mark='ф2.бит1' ab='5488' />"
        L"        <ts wink='миг.ф2.бит1' ab='5488' />"
        L"    </element>"
        L"    <element name='ф3' type='0' />"
        L"<test>";
    Elements temp( FIGURE, "ф1 ф2 ф3" );

    // загрузка
    string u8source = to_raw(data);
    loading_ts_byName(u8source);
    // сохранение
    string u8saved = storing_ts_byName(u8source);
    // проверка
    CPPUNIT_ASSERT(u8source==u8saved);
}

/// Тест загрузки/сохранения акцепторов ЛТС для \ref CLogicHead
void TC_Lts_LogElement::load_save_head()
{
    // создать объекты
    const wchar_t * data =
        L"<test>"
        L"    <element name='г1' type='0' />"
        L"    <element name='г2' type='0' >"
        L"       <ts  allow='г2.open' ab='5488' />"
        L"    </element>"
        L"    <element name='г4' type='0' >"
        L"       <ts    allow='г4.open'  ab='5488' />"
        L"       <ts restrict='г4.close' ab='5488' />"
        L"       <ts     brok='г4.bad'   ab='5488' />"
        L"    </element>"
        L"<test>";
    Elements temp( HEAD, "г1 г2 г4" );

    // загрузка
    string u8source = to_raw(data);
    loading_ts_byName(u8source);
    // сохранение
    string u8saved = storing_ts_byName(u8source);
    // проверка
    CPPUNIT_ASSERT(u8source==u8saved);
}

/// Тест обработки акцепторов ЛТС для \ref CLogicElement
void TC_Lts_LogElement::accept()
{
    // создать объекты
    const wchar_t * data =
        L"<test>"
        L"    <element name='1п' type='0' >"
        L"        <ts used='xxx' ab='5488' />"
        L"        <ts lock='yyy' ab='5488' />"
        L"        <ts art_used='zzz' ab='5488' />"
        L"        <ts art_lock='xxx' ab='5488' />"
        L"    </element>"
        L"    <element name='2п' type='0' >"
        L"        <ts used='xxx' ab='5488' />"
        L"        <ts lock='www' ab='5488' />"
        L"    </element>"
        L"    <element name='3п' type='0' >"
        L"        <ts used='zzz' ab='5488' />"
        L"        <ts lock='zzz' ab='5488' />"
        L"    </element>"
        L"<test>";
    Elements temp( STRIP, "1п 2п 3п" );
    loading_ts_byName(to_raw(data));

    const int ab=5488;
    BadgeU x(L"xxx", ab ), y(L"yyy", ab ), z(L"zzz", ab ), w(L"www", ab );
    Lexicon& lex = Etui::instance()->lex();
    AcceptorsBank  accLts;
    vector<const CLogicElement*> vAll = lex.getAll();
    for( const CLogicElement* it : vAll )
    {
        accLts.append( it );
    }
    vector<const CLogicElement*> vx = proc_one_change_lts(lex, accLts, x, StateLTS::ACTIVE);
    CPPUNIT_ASSERT(vx.size()==2);
    vector<const CLogicElement*> vy = proc_one_change_lts(lex, accLts, y, StateLTS::ACTIVE);
    CPPUNIT_ASSERT(vy.size()==1);
    vector<const CLogicElement*> vz = proc_one_change_lts(lex, accLts, z, StateLTS::ACTIVE);
    CPPUNIT_ASSERT(vz.size()==2);
    CPPUNIT_ASSERT(vx!=vz);
    vector<const CLogicElement*> vw = proc_one_change_lts(lex, accLts, w, StateLTS::ACTIVE);
    CPPUNIT_ASSERT(vw.size()==1);
    CPPUNIT_ASSERT(vy!=vw);
}

/// Тест умолчаний ЛТС для \ref CLogicStrip
void TC_Lts_LogElement::omittedTs_strip()
{
    Elements temp( STRIP, "1п" );
    wstring PASS4 = L"<test sig=\"ijklut\" trst=\"unk\" />";
    CPPUNIT_ASSERT( PASS4==FromUtf8( Elements("1п").get_los_state() ) );
}

/// Тест умолчаний ЛТС для \ref CLogicSwitch
void TC_Lts_LogElement::omittedTs_switch()
{
    Elements temp( SWITCH, "7" );
    wstring UNKN2 = L"<test />";
    CPPUNIT_ASSERT( UNKN2==FromUtf8( Elements("7").get_los_state() ) );
}

void TC_Lts_LogElement::setUp()
{
}
void TC_Lts_LogElement::tearDown()
{
    Etui::Shutdowner();
    CInformator::Shutdowner();
}

void TC_Lts_LogElement::loading_ts_byName( const string& u8 )
{
    attic::a_document src;
    src.load_string(u8.c_str());

    Lexicon& lex = Etui::instance()->lex();
    vector<const CLogicElement*> all = lex.getAll();
    for( attic::a_node n : src.document_element().children("element") )
    {
        wstring nm = n.attribute("name").as_wstring();
        BadgeE b(nm,Elements::commonEsr());
        CLogicElement* le = const_cast<CLogicElement*>(lex.LogElBy(b));
        CPPUNIT_ASSERT( NULL!=le );
        le->loadFrom(n,Elements::commonEsr());

        vector<const CLogicElement*>::iterator it = find(all.begin(),all.end(),le);
        CPPUNIT_ASSERT( it!=all.end() );
        all.erase(it);
    }
    CPPUNIT_ASSERT( all.empty() );
}

string TC_Lts_LogElement::storing_ts_byName( const string& u8 )
{
    attic::a_document trg;
    trg.load_string(u8.c_str());
    Lexicon& lex = Etui::instance()->lex();
    vector<const CLogicElement*> all = lex.getAll();
    for( attic::a_node n=trg.document_element().child("element"); n; n=n.next_sibling("element") )
    {
        wstring nm = n.attribute("name").as_wstring();
        BadgeE b(nm,Elements::commonEsr());
        CLogicElement* le = const_cast<CLogicElement*>(lex.LogElBy(b));
        CPPUNIT_ASSERT( NULL!=le );
        le->saveTo(Elements::commonEsr(),n);

        vector<const CLogicElement*>::iterator it = find(all.begin(),all.end(),le);
        CPPUNIT_ASSERT( it!=all.end() );
        all.erase(it);
    }
    CPPUNIT_ASSERT( all.empty() );
    return trg.to_str();
}

std::string TC_Lts_LogElement::to_raw( const wchar_t * data )
{
    attic::a_document src;
    src.load_wide(data);
    return src.to_str();
}