#include "stdafx.h"

#include "Parody.h"
#include "TC_StationControl.h"
#include "../TeleControl/StationControl.h"
#include "../helpful/Informator.h"

using namespace std;
using namespace Mission;
using namespace Parody;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_StationControl );

static const EsrKit KIT(30172);

void TC_StationControl::setUp()
{
	const wstring p= 
		L"<LOGIC_COLLECTION name='Дикая' ESR_code='30172' >"
		L"    <COMMANDS>"
		L"        <CMD LTU='S1' Keyboard='1ПВСН' type='3' comment='ПАСН по 1П Молочная-Дикая' marshTuFlag='0' extra_flags='64' crucial='1' />"
		L"        <CMD LTU='S21' Keyboard='1НПВ' type='3' comment='ОАСН по 1П Молочная-Дикая' marshTuFlag='0' extra_flags='64' crucial='1' />"
		L"        <CMD LTU='S32' Keyboard='-10' type='3' comment='Перевод 10  в минус' marshTuFlag='0'>"
		L"        	<but name='10' ESR_code='30172' />"
		L"        	<more LTU='S21.after' lookTS='1Ня' ab='7' />"
		L"        </CMD>"
		L"        <CMD LTU='S33' Keyboard='+10' type='3' comment='Перевод 10  в плюс' marshTuFlag='0'>"
		L"        	<but name='10' ESR_code='30172' />"
		L"        </CMD>"
		L"        <CMD LTU='S44' Keyboard='ОСУ' type='3' comment='Отмена СУ' marshTuFlag='0' extra_flags='16'>"
		L"        	<but name='ОП' ESR_code='30172' />"
		L"        	<but name='СУ_' ESR_code='30172' />"
		L"        </CMD>"
		L"    </COMMANDS>"
		L"    <WinCommand>"
		L"    	<Panel Knob='ОВПМК' Comment='Поездные команды' Source='poezdn.wcm' Xw='439' Yw='98'>"
		L"    		<Group name='Установка маршрутов' X='3' Y='1' W='141' H='93' />"
		L"    		<Text name='по Ч' X='5' Y='10' W='65' H='8' />"
		L"    		<Button name='3п' LTU='S21' X='6' Y='29' W='30' H='13' Comment='ОАСН по 1П Молочная-Дикая' />"
		L"    	</Panel>"
		L"    </WinCommand>"
		L"</LOGIC_COLLECTION>";

	attic::a_parse_result res = x_doc.load_wide( p );
	CPPUNIT_ASSERT( res );

    // создание команд
    Etui::instance();
    Elements::commonEsr( KIT );
    Elements dummy( COMMAND, "S1  S21  S32  S33  S44");
    CmdLtu("S32").addBut("10").addFollower("S21.after");
    CmdLtu("S33").addBut("10");
    CmdLtu("S44").addBut("ОП").addBut("СУ_");
    Etui::instance()->complete();
    // группировка команд
    Lexicon& lex = Etui::instance()->lex();

    msc.loadOneCommand( lex.LogElBy( BadgeE(L"S1", KIT)  ) );
    msc.loadOneCommand( lex.LogElBy( BadgeE(L"S21",KIT)  ) );
    msc.loadOneCommand( lex.LogElBy( BadgeE(L"S32",KIT)  ) );
    msc.loadOneCommand( lex.LogElBy( BadgeE(L"S33",KIT)  ) );
    msc.loadOneCommand( lex.LogElBy( BadgeE(L"S44",KIT)  ) );

    msc.loadPanels(x_doc.child("LOGIC_COLLECTION").child("WinCommand"));
}

void TC_StationControl::tearDown()
{
	CInformator::Shutdowner();
    Etui::Shutdowner();
}

// проверяем загрузку составной команды
void TC_StationControl::Load_Compound()
{
    CPPUNIT_ASSERT( msc.findFeatures( L"S1"  ).getFollower().empty() );
	CPPUNIT_ASSERT( msc.findFeatures( L"S32" ).getFollower() == L"S21.after" );
}

// проверяем загрузку панели
void TC_StationControl::Load_Panel()
{
	CPPUNIT_ASSERT( msc.havePanelFrom( BadgeE(L"ОВПМК", KIT ) ) );
}

//проверяем количество команд с нажимными объектами
void TC_StationControl::Load_Simple()
{
    EsrKit nekit(11111);
	CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"10", KIT) ).size() == 2 );
	CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"ОП",KIT) ).size() == 1 );
    CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"зю", nekit) ).size() == 0 );
}
