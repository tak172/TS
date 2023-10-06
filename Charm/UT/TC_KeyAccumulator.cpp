#include "stdafx.h"

#include "TC_KeyAccumulator.h"
#include "Parody.h"
#include "../helpful/Dictum.h"
#include "../helpful/Dictum_Const.h"
#include "../helpful/Log.h"
#include "../TeleControl/KeyAccumulator.h"
#include "../TeleControl/StationControl.h"
#include "../helpful/Informator.h"
#include "../StatBuilder/LogicCommand.h"

using namespace Parody;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_KeyAccumulator );

const EsrKit TC_KeyAccumulator::e(30172);
const BadgeE TC_KeyAccumulator::x(L"x",e);
const BadgeE TC_KeyAccumulator::Y(L"Y",e);
const BadgeE TC_KeyAccumulator::z(L"z",e);
const BadgeE TC_KeyAccumulator::w(L"w",e);

void TC_KeyAccumulator::setUp()
{
    // создание команд
    Etui::instance();
    Elements::commonEsr( e );
    Elements dummy( COMMAND, "S1  S21  S32  S33  S44");
    CmdLtu("S32").addBut("Y");
    CmdLtu("S33").addBut("x").addBut("Y");
    CmdLtu("S44").addBut("x").addBut("z");
    Head( "Y" ).setType( TRAIN ).onLeg("legY");
    Elements temp( FIGURE, "x", SIMPLE_TEXT );
    Etui::instance()->complete();
    // группировка команд
    Lexicon& lex = Etui::instance()->lex();

    Mission::StationControl sc;
    sc.loadOneCommand( lex.LogElBy( BadgeE(L"S1", e)  ) );
    sc.loadOneCommand( lex.LogElBy( BadgeE(L"S21",e)  ) );
    sc.loadOneCommand( lex.LogElBy( BadgeE(L"S32",e)  ) );
    sc.loadOneCommand( lex.LogElBy( BadgeE(L"S33",e)  ) );
    sc.loadOneCommand( lex.LogElBy( BadgeE(L"S44",e)  ) );
    cmds = sc.cref_cmds();
}

void TC_KeyAccumulator::tearDown()
{
    cmds.clear();
    Etui::Shutdowner();
    CInformator::Shutdowner();
}

/// 
void TC_KeyAccumulator::accum1b()
{
    Mission::KeyAccumulator ka;

    CPPUNIT_ASSERT( ka.readyCmds(cmds).empty() );
    CPPUNIT_ASSERT( ka.suitable(cmds,x) );
    CPPUNIT_ASSERT( ka.suitable(cmds,Y) );
    CPPUNIT_ASSERT( !ka.suitable(cmds,z) );
    CPPUNIT_ASSERT( !ka.suitable(cmds,w) );
    // команда Y
    CPPUNIT_ASSERT( ka.readyCmds(cmds).empty() );
    ka.append(Y);
    CPPUNIT_ASSERT( ka.readyCmds(cmds).size()==1 );
    CPPUNIT_ASSERT( ka.getCmd(cmds)->GetName() ==L"S32" );
}

void TC_KeyAccumulator::accum2b()
{
    Mission::KeyAccumulator ka;
    // команда xY
    CPPUNIT_ASSERT( ka.readyCmds(cmds).empty() );

    // кнопка x
    CPPUNIT_ASSERT( ka.readyCmds(cmds).empty() );
    ka.append(x);
    CPPUNIT_ASSERT( ka.readyCmds(cmds).empty() );
    // кнопка Y
    CPPUNIT_ASSERT( !ka.suitable(cmds,w) );
    CPPUNIT_ASSERT( ka.suitable(cmds,Y) );
    ka.append(Y);
    CPPUNIT_ASSERT( ka.readyCmds(cmds).size()==1 );
    CPPUNIT_ASSERT( ka.getCmd(cmds)->GetName() == L"S33" );
}

void TC_KeyAccumulator::curs()
{
    Mission::KeyAccumulator ka;
    Lexicon& lex = Etui::instance()->lex();

    // до начала - обычный курсор
    CPPUNIT_ASSERT( !ka.needCursor() );
    CPPUNIT_ASSERT( ka.getCursorInfo(lex).first == F2A_GUI::CRS_NORMAL );
    // команда x... (от фигуры)
    ka.append(x);
    CPPUNIT_ASSERT( ka.needCursor() );
    CPPUNIT_ASSERT( ka.getCursorInfo(lex).first == F2A_GUI::CRS_WAGEN );
    // команда ...Y
    ka.append(Y);
    CPPUNIT_ASSERT( ka.needCursor() );
    CPPUNIT_ASSERT( ka.getCursorInfo(lex).first == F2A_GUI::CRS_WAGEN );
    ka.clear();

    // теперь поездная команда
    ka.append(Y);
    CPPUNIT_ASSERT( ka.needCursor() );
    CPPUNIT_ASSERT( ka.getCursorInfo(lex).first == F2A_GUI::CRS_LOKOM );
    ka.clear();
}
