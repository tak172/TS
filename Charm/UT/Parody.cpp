#include "stdafx.h"

#include <cppunit/extensions/HelperMacros.h>
#include <boost/bind/bind.hpp>
#include <boost/tokenizer.hpp>
#ifndef LINUX
#include <tchar.h>
#endif // !LINUX

#include "Parody.h"
#include "../Fund/Aileron.h"
#include "../StatBuilder/LogicElementFactory.h"
#include "../StatBuilder/LogicJoint.h"

using namespace std;

const int INVALID_ALL_TYPE = max((int)INVALID_GSTRIP_TYPE,
								 max((int)INVALID_JOINT_TYPE,
								 max((int)INVALID_FIGURE_TYPE,
								 max((int)INVALID_BOARD_TYPE,
								 max((int)INVALID_HEAD_TYPE,
								 max((int)INVALID_LSTRIP_TYPE,
								 (int)INVALID_SEMAFOR_TYPE
								 ))))));

// вспомогательные классы
namespace Parody
{
    ALLOCATE_SINGLETON(Etui);

    int Element::last_ts = 7;
    EsrKit Elements::commonESR = EsrKit(54321);

    Etui::Etui() 
        : pLex( new Lexicon ),
          pAilerons( new Ailerons(*pLex) ),
          wasComplete( false ) 
    {
        const size_t TEST_OBJECT_COUNT = 100;
        pLex->reserve(TEST_OBJECT_COUNT);
        pLex->setAutoDelete(true);
    }
    Etui::~Etui()             
    {}
    void Etui::complete()
    {
        // завершить построение: достроить элероны
        if ( !wasComplete )
        {
            wasComplete = true;
            // заполнить все элероны
            typedef vector<const CLogicElement*> TYPE_COLL;
            TYPE_COLL coll = lex().getAll();
            for( TYPE_COLL::iterator it = coll.begin(); it!=coll.end(); ++it )
            {
                if ( (*it)->GetClass() == SWITCH )
                    ailerons().append( *it );
            }
            // инициализировать элероны
            ailerons().init();
        }
    }
    const char * Element::ts_name( char letter )
    {
        switch(toupper(letter))
        {
        case 'U': return "used";
        case 'A':
        case 'L': return "lock";
        case '+':
        case 'P': return "positiv";
        case '-':
        case 'M': return "negativ";
        default:
            CPPUNIT_FAIL("unknown ts letter");
            return NULL;
        }
    }
    StateLTS Element::ts_value( char letter )
    {
        switch(toupper(letter))
        {
        case '0': return StateLTS(StateLTS::PASSIVE);
        case '1': return StateLTS(StateLTS::ACTIVE);
        case '?': return StateLTS(StateLTS::UNDEF);
        default:
            CPPUNIT_FAIL("unknown ts value");
            return StateLTS(StateLTS::UNDEF);
        }
    }
    Element::STATE_GETTER Element::select_getter( char letter )
    {
        switch(toupper(letter))
        {
        case '+':
        case 'P': return &LOS_base::isPlus;
        case '-':
        case 'M': return &LOS_base::isMinus;
        case 'U': return &LOS_base::isStableUsed;
        case 'A':
        case 'L': return &LOS_base::isLock;
        case 'G': return &LOS_base::isPartNeck;
        case 'K': return &LOS_base::isKnown;

        default:
            CPPUNIT_FAIL("unknown getter");
            return &LOS_base::isPlus;
        }
    }

    Element& Element::make_ts( const char * ts_list )
    {
        if ( !logel )
            CPPUNIT_FAIL("объект ещё не создан");
        else if ( Etui::instance()->completed() )
            CPPUNIT_FAIL("создание уже завершено");
        else
        {
            for( const char * letter=ts_list; *letter; letter++ )
            {
                logel->setLts( ts_name(*letter), BadgeU(L"контакт",++last_ts) );
            }
        }
        return *this;
    }

    Element& Element::fill_ts( const char * attr_name, const BadgeU& attr_value )
    {
        if ( !logel )
            CPPUNIT_FAIL("объект ещё не создан");
        else if ( Etui::instance()->completed() )
            CPPUNIT_FAIL("создание уже завершено");
        else
        {
            if ( !logel->getLts(attr_name).empty() )
                CPPUNIT_FAIL("атрибут уже использован");
            logel->setLts(attr_name,attr_value);
        }
        return *this;
    }


    void Element::set_ts( const char * ts_list )
    {
        Etui::instance()->complete();
        bool changes = false;
        for( const char * tspair=ts_list; tspair[0] && tspair[1]; tspair+=2 )
        {
            char let = tspair[0];
            char val = tspair[1];

            LOS_base::ACCEPTOR_LTS acc = logel->getAcceptorLts(ts_name(let));
            if ( acc != NULL )
            {
                LOS_base* los = Etui::instance()->lex().getLos(logel);
                bool change = (los->*acc)( ts_value(val) );
                changes |= change;
            }
        }
        if ( changes )
            mark_and_expanse();
    }

    bool Element::check( const char * state_letters )
    {
        Etui::instance()->complete();
        for( const char * oneletter=state_letters; *oneletter; oneletter++ )
        {
            bool bval = (isupper(*oneletter)!=0);

            LOS_base* los = Etui::instance()->lex().getLos(logel);
            STATE_GETTER getter=select_getter( *oneletter );
            if ( bval != (los->*getter)() )
                return false;
        }
        return true;
    }
    std::string Element::getName() const
    {
        return Etui::instance()->lex().BadgeBy(logel).str_u8();
    }
    EsrKit Element::getEsr() const
    {
        return Etui::instance()->lex().BadgeBy(logel).num();
    }

    CLogicElement* Element::ensure( ELEMENT_CLASS cl, const std::string& snam, const EsrKit& esr, unsigned el_type )
    {
		CLogicElementFactory LEF;
        BadgeE obj( From1251( snam ), esr );
        const CLogicElement* el = Etui::instance()->lex().LogElBy( obj );
        if ( !el )
        {
            if ( Etui::instance()->completed() )
            {
                //TRACE("Объект %s нельзя создать т.к. этот этап уже завершен", snam);
                CPPUNIT_FAIL("создание уже завершено");
            }
			CLogicElement * lelem = CLogicElementFactory().CreateObject( cl );
			if ( !lelem )
				CPPUNIT_FAIL( "unknown class" );
			lelem->SetName( obj.str() );
			lelem->SetType( el_type );
            CPPUNIT_ASSERT( Etui::instance()->lex().append( lelem, esr ) );
            el = Etui::instance()->lex().LogElBy( obj );
        }
        return const_cast<CLogicElement*>(el);
    }
    void Element::update( const char* snam, const EsrKit& esr )
    {
        logel = ensure( NONE, snam, esr, INVALID_ALL_TYPE );
    }
    void Element::call_los( BOOL_SET_MEMBER memb, bool val )
    {
        LOS_base* los = Etui::instance()->lex().getLos(logel);
        (los->*memb)( val );
        mark_and_expanse();
    }
    bool Element::call_los( BOOL_GET_MEMBER memb )
    {
        const LOS_base* los = Etui::instance()->lex().getLos( logel );
        return (los->*memb)();
    }

    Elements::Elements( const char* self_nam )
    {
        names = split_names(self_nam);
    }

    Elements::Elements( ELEMENT_CLASS cl, const char* self_nam, unsigned el_type )
    {
        names = split_names(self_nam);
        //
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            Element curr;
            curr.ensure( cl, (*it).c_str(), commonEsr(), el_type );
        }
    }
    Elements::Elements( const EsrKit& esr, ELEMENT_CLASS cl, const char* self_nam, unsigned el_type )
    {
        names = split_names(self_nam);
        //
        commonEsr(esr);
        Elements xx(cl,self_nam,el_type);
    }


    void Elements::make_ts( const char * ts_list )
    {
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            Element curr;
            curr.update( (*it).c_str(), commonEsr() );
            curr.make_ts(ts_list);
        }
    }
    void Elements::set_ts( const char * ts_list )
    {
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            Element curr;
            curr.update( (*it).c_str(), commonEsr() );
            curr.set_ts(ts_list);
        }
    }
    bool Elements::check( const char * ts_list )
    {
        bool res=true;
        bool found=false;
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            found = true;

            Element curr;
            curr.update( (*it).c_str(), commonEsr() );
            if ( !curr.check(ts_list) )
            {
                //TRACE("check %s (%s) is false !!!\n", it->c_str(), ts_list );
                res = false;
            }

        }
        return found && res;
    }
    void Elements::call_los( BOOL_SET_MEMBER memb, bool val )
    {
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            Element curr;
            curr.update( (*it).c_str(), commonEsr() );
            curr.call_los(memb, val);
        }
    }
    bool Elements::chk_los( BOOL_GET_MEMBER memb, bool val )
    {
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            Element curr;
            curr.update( (*it).c_str(), commonEsr() );
            if ( val != curr.call_los(memb) )
                return false;
        }
        return true;
    }

    void Element::mark_and_expanse()
    {
        Etui::instance()->lex().markAsChanged(logel);
        Etui::instance()->ailerons().expanser(logel);
        //notify_boards(obj);
    }


    std::vector<std::string> Elements::split_names( const std::string& self_nam )
    {
        vector<string> res;
        typedef boost::tokenizer<boost::char_separator<char>,const char * > TOKENIZER;
        boost::char_separator<char> sep(" ,");
        const TOKENIZER tok(self_nam.c_str(),self_nam.c_str()+self_nam.size(),sep);
        for(TOKENIZER::iterator it=tok.begin(); it!=tok.end(); ++it )
            res.push_back(*it);
        return res;
    }

    string Elements::get_los_state()
    {
        //
        string res;
        for( std::vector<std::string>::iterator it=names.begin(); it!=names.end(); ++it )
        {
            EsrKit esr = commonEsr();
            BadgeE obj( From1251( *it ), esr );
            attic::a_document doc;
            attic::a_node node = doc.set_document_element("test");
            LOS_base* one = Etui::instance()->lex().getLos( obj );
            one->write_to_xml(node);
            res += node.to_str();
        }
        return res;
    }

    Switch::Switch( const char* snam )
        : Element()
    {
        // обеспечить наличие стрелки
        logel = ensure( SWITCH, snam, commonEsr() );
    }

    void Switch::make_and_link( CLink::SWITCH_STRIP_LINK lnktype, const char* snam )
    {
        /*CLogicElement* le_strip =*/ ensure( STRIP, snam, commonEsr() );
        Strip strip = Strip( snam );
        linkTo( *this, lnktype, strip );
    }

    Switch& Switch::base( const char* snam )
    {
        make_and_link( CLink::BASE, snam );
        return *this;
    }

    Switch& Switch::plus( const char* snam )
    {
        make_and_link( CLink::PLUS, snam );
        return *this;
    }

    Switch& Switch::minus( const char* snam )
    {
        make_and_link( CLink::MINUS, snam );
        return *this;
    }

    Strip::Strip( const string& snam, const EsrKit esrKit )
        : Element()
    {
        // обеспечить наличие участка
        logel = ensure( STRIP, snam, (esrKit.empty()? commonEsr() : esrKit) );
    }

    Strip::Strip( BadgeE badge )
        : Element()
    {
        // обеспечить наличие участка
        logel = ensure( STRIP, badge.str_u8(), badge.num() );
    }

    Head::Head( const char* snam )
        : Element()
    {
        // обеспечить наличие головы
        logel = ensure( HEAD, snam, commonEsr() );
    }
    Head& Head::onLeg( const char* snam )
    {
        /*CLogicElement* le_leg =*/ ensure( SEMALEG, snam, commonEsr() );
        SemaLeg semaLeg = SemaLeg( snam );
        linkTo( *this, logel->GetType(), semaLeg );
        return *this;
    }
    Head& Head::setType(unsigned _type)
    {
        logel->SetType(_type);
        return *this;
    }

    Joint::Joint( Strip& one, Strip& two )
        : Element()
    {
        // обеспечить наличие
        logel = ensure( JOINT, one.getName() + "~" + two.getName(), (std::min)( one.getEsr(), two.getEsr() ) );
        linkTo( *this, 0, one );
        linkTo( *this, 0, two );
    }
    Joint& Joint::linkStrip( const char* snam )
    {
        /*CLogicElement* le_strip =*/ ensure( STRIP, snam, commonEsr() );
        Strip strip = Strip( snam );
        linkTo( *this, 0, strip );
        return *this;
    }
    Joint& Joint::linkStrip( Strip& strip )
    {
        linkTo( strip, 0, *this );
        return *this;
    }
    Joint& Joint::coord( const wstring& ruler, const string& coord )
    {
        CLogicJoint* le_joint = reinterpret_cast<CLogicJoint*>( logel );
        PicketingInfo pi = le_joint->GetPicketingInfo();
        pi.SetCoord( rwRuledCoord( ruler, rwCoord(coord) ) );
        le_joint->SetPicketingInfo( pi );
        return *this;
    }
    SemaLeg::SemaLeg( const char* snam, EsrKit esr )
        : Element()
    {
        // обеспечить наличие
        logel = ensure( SEMALEG, snam, (esr.empty()? commonEsr() : esr) );
    }
    SemaLeg& SemaLeg::from_to( Strip& from, Strip& to )
    {
        // связи
        linkTo( *this, CLink::FROM, from );
        linkTo( *this, CLink::TO, to );
        return *this;
    }
    SemaLeg& SemaLeg::oddness( Oddness odn )
    {
        CLogicSemaleg* le_leg = reinterpret_cast<CLogicSemaleg*>( logel );
        le_leg->SetOddness( odn );
        return *this;
    }

    CmdLtu::CmdLtu( const char* snam )
        : Element()
    {
        // обеспечить наличие участка
        logel = ensure( COMMAND, snam, commonEsr() );
    }
    CmdLtu& CmdLtu::addBut(const char* snam)
    {
        CLogicCommand* cmd = static_cast<CLogicCommand*>(logel);
        cmd->addBut( cmd->countBut(), BadgeE( From1251( snam ), commonEsr() ) );
        return *this;
    }
    CmdLtu& CmdLtu::addFollower(const char* snam)
    {
        CLogicCommand* cmd = static_cast<CLogicCommand*>(logel);
        cmd->attachFollower( From1251( snam ) );
        return *this;
    }

    // Связывание двух объектов
    void Element::linkTo( Element& one, unsigned link_type, Element& two )
    {
        CLogicElement* le_one = one.logel;
        CLogicElement* le_two = two.logel;
        BadgeE bdg_one = Etui::instance()->lex().BadgeBy(le_one);
        BadgeE bdg_two = Etui::instance()->lex().BadgeBy(le_two);
        ELEMENT_CLASS cl_one = le_one->GetClass();
        ELEMENT_CLASS cl_two = le_two->GetClass();
        // связь one --> two
        CLink link2( cl_two, bdg_two, link_type );
        vector<CLink> l1 = le_one->GetLinks();
        if ( l1.end() == find( l1.begin(), l1.end(), link2 ) )
        {
            l1.push_back( link2 );
            le_one->SetLinks(l1);
        }
        // связь one <-- two
        CLink link1( cl_one, bdg_one, link_type );
        vector<CLink> l2 = le_two->GetLinks();
        if ( l2.end() == find( l2.begin(), l2.end(), link1 ) )
        {
            l2.push_back( link1 );
            le_two->SetLinks(l2);
        }
    }
}
