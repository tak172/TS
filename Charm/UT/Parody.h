#ifndef _PARODY_H_
#define _PARODY_H_

#include "../helpful/RT_Singleton.h"
#include "../Fund/Lexicon.h"
#include "../StatBuilder/LogicSwitch.h"
#include "../StatBuilder/LogicStrip.h"
#include "../StatBuilder/LogicFigure.h"
#include "../StatBuilder/LogicHead.h"
#include "../StatBuilder/LogicSemaleg.h"
#include "../StatBuilder/LogicCommand.h"

class Ailerons;

namespace Parody
{
    class Etui
    {
    public:
        DECLARE_SINGLETON(Etui);

        Lexicon& lex()       { return *pLex;      }
        Ailerons& ailerons() { return *pAilerons; }

        void complete();
        bool completed() const { return wasComplete; };
    private:
        boost::scoped_ptr<Lexicon> pLex;
        boost::scoped_ptr<Ailerons> pAilerons;
        bool wasComplete;
    };

    typedef void ( (LOS_base::*BOOL_SET_MEMBER) (bool) );
    typedef bool ( (LOS_base::*BOOL_GET_MEMBER) () const );
    class Elements
    {
    public:
        explicit Elements( const char* self_nam );
        Elements( ELEMENT_CLASS cl, const char* self_nam, unsigned el_type=0 );
        Elements( const EsrKit& esr, ELEMENT_CLASS cl, const char* self_nam, unsigned el_type=0 );

        void make_ts( const char * ts_list );
        void set_ts( const char * ts_list );
        bool check( const char * ts_list );
        void call_los( BOOL_SET_MEMBER memb, bool val );
        bool chk_los( BOOL_GET_MEMBER memb, bool val );
        static void commonEsr( const EsrKit& esr )
        {
            commonESR = esr;
        }
        static const EsrKit& commonEsr()
        {
            return commonESR;
        }
        std::string get_los_state();
        static std::vector<std::string> split_names( const std::string& self_nam );
    private:
        static EsrKit commonESR;
        std::vector<std::string> names;
    };
    class Element
    {
    public:
        Element()
            : logel( NULL )
        {}
        CLogicElement* ensure( ELEMENT_CLASS cl, const std::string& snam, const EsrKit& esr, unsigned int el_type=0 );
        void update( const char* snam, const EsrKit& esr );
        Element& make_ts( const char * ts_list );
        Element& fill_ts( const char * attr_name, const BadgeU& attr_value );
        void set_ts( const char * ts_list );
        void call_los( BOOL_SET_MEMBER memb, bool val );
        bool call_los( BOOL_GET_MEMBER memb );
        bool check( const char * ts_list );
        std::string getName() const;
        EsrKit getEsr() const;
        static void commonEsr( const EsrKit& esr )
        {
            Elements::commonEsr(esr);
        }
        static const EsrKit& commonEsr()
        {
            return Elements::commonEsr();
        }
        static void linkTo( Element& one, unsigned lnktype, Element& two );
    protected:
        CLogicElement* logel;
        static int last_ts;
    private:
        static const char * ts_name( char letter );
        StateLTS ts_value( char letter );
        static bool get_sig( const char * name );

        typedef bool ((LOS_base::*STATE_GETTER)() const);
        static STATE_GETTER select_getter( char letter );
        void mark_and_expanse();
    };
    class Switch : public Element
    {
    public:
        explicit Switch( const char* self_nam );
        Switch& base ( const char* snam );
        Switch& plus ( const char* snam );
        Switch& minus( const char* snam );
    private:
        void make_and_link( CLink::SWITCH_STRIP_LINK lnktype, const char* snam );
    };
    class Strip : public Element
    {
    public:
        explicit Strip( const std::string& self_nam, const EsrKit esrKit = EsrKit() );
        explicit Strip( BadgeE badge );
    };
    class Head : public Element
    {
    public:
        explicit Head( const char* self_name );
        Head& onLeg( const char* snam );
        Head& setType(unsigned _type);
    };
    class Joint : public Element
    {
    public:
        explicit Joint( Strip& one, Strip& two );
        Joint& linkStrip( const char* snam );
        Joint& linkStrip( Strip& strip );
        Joint& coord( const std::wstring& ruler, const std::string& coord );
    };
    class SemaLeg : public Element
    {
    public:
        explicit SemaLeg( const char* self_name, EsrKit esrKit = EsrKit() );
        SemaLeg& from_to( Strip& from, Strip& to );
        SemaLeg& oddness( Oddness odn );
    };
    class CmdLtu : public Element
    {
    public:
        explicit CmdLtu( const char* self_nam );
        CmdLtu& addBut(const char* snam);
        CmdLtu& addFollower(const char* snam);
    };
};

#endif // _PARODY_H_
