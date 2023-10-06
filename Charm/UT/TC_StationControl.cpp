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
		L"<LOGIC_COLLECTION name='�����' ESR_code='30172' >"
		L"    <COMMANDS>"
		L"        <CMD LTU='S1' Keyboard='1����' type='3' comment='���� �� 1� ��������-�����' marshTuFlag='0' extra_flags='64' crucial='1' />"
		L"        <CMD LTU='S21' Keyboard='1���' type='3' comment='���� �� 1� ��������-�����' marshTuFlag='0' extra_flags='64' crucial='1' />"
		L"        <CMD LTU='S32' Keyboard='-10' type='3' comment='������� 10  � �����' marshTuFlag='0'>"
		L"        	<but name='10' ESR_code='30172' />"
		L"        	<more LTU='S21.after' lookTS='1��' ab='7' />"
		L"        </CMD>"
		L"        <CMD LTU='S33' Keyboard='+10' type='3' comment='������� 10  � ����' marshTuFlag='0'>"
		L"        	<but name='10' ESR_code='30172' />"
		L"        </CMD>"
		L"        <CMD LTU='S44' Keyboard='���' type='3' comment='������ ��' marshTuFlag='0' extra_flags='16'>"
		L"        	<but name='��' ESR_code='30172' />"
		L"        	<but name='��_' ESR_code='30172' />"
		L"        </CMD>"
		L"    </COMMANDS>"
		L"    <WinCommand>"
		L"    	<Panel Knob='�����' Comment='�������� �������' Source='poezdn.wcm' Xw='439' Yw='98'>"
		L"    		<Group name='��������� ���������' X='3' Y='1' W='141' H='93' />"
		L"    		<Text name='�� �' X='5' Y='10' W='65' H='8' />"
		L"    		<Button name='3�' LTU='S21' X='6' Y='29' W='30' H='13' Comment='���� �� 1� ��������-�����' />"
		L"    	</Panel>"
		L"    </WinCommand>"
		L"</LOGIC_COLLECTION>";

	attic::a_parse_result res = x_doc.load_wide( p );
	CPPUNIT_ASSERT( res );

    // �������� ������
    Etui::instance();
    Elements::commonEsr( KIT );
    Elements dummy( COMMAND, "S1  S21  S32  S33  S44");
    CmdLtu("S32").addBut("10").addFollower("S21.after");
    CmdLtu("S33").addBut("10");
    CmdLtu("S44").addBut("��").addBut("��_");
    Etui::instance()->complete();
    // ����������� ������
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

// ��������� �������� ��������� �������
void TC_StationControl::Load_Compound()
{
    CPPUNIT_ASSERT( msc.findFeatures( L"S1"  ).getFollower().empty() );
	CPPUNIT_ASSERT( msc.findFeatures( L"S32" ).getFollower() == L"S21.after" );
}

// ��������� �������� ������
void TC_StationControl::Load_Panel()
{
	CPPUNIT_ASSERT( msc.havePanelFrom( BadgeE(L"�����", KIT ) ) );
}

//��������� ���������� ������ � ��������� ���������
void TC_StationControl::Load_Simple()
{
    EsrKit nekit(11111);
	CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"10", KIT) ).size() == 2 );
	CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"��",KIT) ).size() == 1 );
    CPPUNIT_ASSERT( msc.allCommandsWith( BadgeE(L"��", nekit) ).size() == 0 );
}
