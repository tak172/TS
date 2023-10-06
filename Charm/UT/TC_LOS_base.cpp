#include "stdafx.h"

#include "TC_LOS_base.h"
#include "../Actor/LOSBase.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_LOS_base );

const int INVALID_ALL_TYPE = max((int)INVALID_GSTRIP_TYPE,
								 max((int)INVALID_JOINT_TYPE,
								 max((int)INVALID_FIGURE_TYPE,
								 max((int)INVALID_BOARD_TYPE,
								 max((int)INVALID_HEAD_TYPE,
								 max((int)INVALID_LSTRIP_TYPE,
								 (int)INVALID_SEMAFOR_TYPE
								 ))))));

void TC_LOS_base::uniqSigXcode()
{
    // породим все возможные Ћќ—ы
    std::vector<LOS_base*> vLos;
    for ( ELEMENT_CLASS logic_class = STRIP; logic_class <= INVALID_CLASS; logic_class = ELEMENT_CLASS( logic_class << 1 ) )
        for ( ELEMENT_CLASS graphic_class = STRIP; graphic_class <= INVALID_CLASS; graphic_class = ELEMENT_CLASS( graphic_class << 1 ) )
        {
            LOS_base* p = LOS_base::fabric_from_object( logic_class, graphic_class );
            for( auto los : vLos )
            {
                if ( los->isEqual(p) )
                {
                    delete p;
                    p = NULL;
                    break;
                }
            }
            if ( p )
                vLos.push_back(p);
        }

    // проверим совместимость различных Ћќ—ов
    std::string summary;
    for( auto plos : vLos )
    {
        // обработаем все кодировки
        for( std::string curr = plos->used_codes_xml(); !curr.empty(); curr.erase(curr.begin()) )
        {
            const char ch = *curr.begin();
            // код должен быть приемлемым - латинска€ заглавна€ буква
            if ( !isalpha(ch) )
                CPPUNIT_FAIL("alphabetic character"); 
            if ( !isupper(ch) )
                CPPUNIT_FAIL("upper case"); 
            if ( ch==_tolower(ch) )
                CPPUNIT_FAIL("different upper and lower case"); 
            // код не должен повтор€тьс€
            else if ( summary.end()!=find( summary.begin(), summary.end(), ch) )
                CPPUNIT_FAIL("unique character"); 
            else
                summary.push_back(ch);
        }
    }
    while( !vLos.empty() )
    {
        delete vLos.back();
        vLos.pop_back();
    }
}

void TC_LOS_base::addSignalCode()
{
    std::string buff; // минимально необходимое место
    LOS_base::add_one_sig(buff, StateLTS::ACTIVE, 'X');
    LOS_base::add_one_sig(buff, StateLTS::PASSIVE, 'Y');
    LOS_base::add_one_sig(buff, StateLTS::UNDEF, 'Z');
    CPPUNIT_ASSERT( buff=="Xy" );
    LOS_base::add_one_sig(buff, StateLTS::UNDEF, 'X');
    LOS_base::add_one_sig(buff, StateLTS::PASSIVE, 'Y');
    LOS_base::add_one_sig(buff, StateLTS::ACTIVE, 'Z');
    CPPUNIT_ASSERT( buff=="XyyZ" );
}
