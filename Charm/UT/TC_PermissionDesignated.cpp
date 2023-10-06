#include "stdafx.h"

#include <sstream>
#include "TC_PermissionDesignated.h"
#include <boost/asio/ip/tcp.hpp>
#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "../helpful/Log.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_PermissionDesignated );

using namespace std;
using namespace Permission;


void TC_PermissionDesignated::LoadSave()
{
    const wstring text = 
        L"<Test>"
        L"   <Access district='aaa' default='view' >"
        L"       <Rule ip='192.168.1.105' level='edit' />"
        L"   </Access>"
        L"   <Access district='bbb' >"
        L"       <Rule ip='192.168.1.1' level='edit' />"
        L"       <Rule ip='192.168.1.2' level='view' />"
        L"   </Access>"
        L"   <Normative>"
        L"       <Rule ip='192.168.1.77' level='edit' />"
        L"   </Normative>"
        L"   <Supervisor>"
        L"       <Rule ip='192.168.1.2' level='edit' />"
        L"   </Supervisor>"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.77' comment='клиент77' />"
        L"   <Computer ip='192.168.1.105' comment='Kigas primary server' live='Y' />"
        L"</Test>";

    attic::a_document doc;
    attic::a_parse_result res = doc.load_wide( text );
    CPPUNIT_ASSERT( res==true );
    const std::wstring before = doc.pretty_wstr();

    Designated pd;
    attic::a_node rootNode = doc.child("Test");
    auto msg = pd.deserialize( rootNode );
    std::vector< Permission::Computer > vComputers;
    std::set< IpAdr > vLiveComputers;
    pd.deserializeComputers( rootNode, vComputers, vLiveComputers );
    CPPUNIT_ASSERT_MESSAGE( "Ошибка загрузки назначенных прав доступа", msg.empty() );

    // вывод специально дважды в один нод - проверка на появление дублей
    doc.reset();
    doc.set_document_element("Test");
    pd.serialize( doc.child("Test") );
    pd.serializeComputers( doc.child("Test"), vComputers, vLiveComputers );
    const std::wstring after = doc.pretty_wstr();
    CPPUNIT_ASSERT( before == after );
}

void TC_PermissionDesignated::LoadSave_compat()
{
    const wstring text_old = 
        L"<Test>"
        L"   <Access district='aaa' default='view' >"
        L"       <Rule ip='192.168.1.105' level='edit' />"
        L"   </Access>"
        L"   <Access district='bbb' >"
        L"       <Rule ip='192.168.1.1' level='edit' />"
        L"       <Rule ip='192.168.1.2' level='view' />"
        L"   </Access>"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.77' comment='клиент77' />"
        L"   <Computer ip='192.168.1.105' comment='Kigas primary server' live='Y' />"
        L"</Test>";
    const wstring text_curr = 
        L"<Test>"
        L"   <Access district='aaa' default='view' >"
        L"       <Rule ip='192.168.1.105' level='edit' />"
        L"   </Access>"
        L"   <Access district='bbb' >"
        L"       <Rule ip='192.168.1.1' level='edit' />"
        L"       <Rule ip='192.168.1.2' level='view' />"
        L"   </Access>"
        L"   <Normative/>"
        L"   <Supervisor/>"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.77' comment='клиент77' />"
        L"   <Computer ip='192.168.1.105' comment='Kigas primary server' live='Y' />"
        L"</Test>";

    // загрузка старых данных
    attic::a_document doc_old;
    doc_old.load_wide( text_old );
    Designated pd;
    pd.deserialize( doc_old.child("Test") );
    std::vector< Permission::Computer > vComputers;
    std::set< IpAdr > vLiveComputers;
    pd.deserializeComputers( doc_old.child("Test"), vComputers, vLiveComputers );

    // вывод
    attic::a_document doc_out;
    doc_out.set_document_element("Test");
    pd.serialize( doc_out.child("Test") );
    pd.serializeComputers( doc_out.child("Test"), vComputers, vLiveComputers );
    const std::wstring after = doc_out.pretty_wstr();

    // эталон
    attic::a_document doc_et;
    doc_et.load_wide( text_curr );
    const std::wstring etalon = doc_et.pretty_wstr();

    CPPUNIT_ASSERT( etalon == after );
}

void TC_PermissionDesignated::LoadSave_sortComputers()
{
    const wstring text_in =
        L"<Test>"
        L"   <Access district='aaa' default='view' >"
        L"       <Rule ip='192.168.1.105' level='edit' />"
        L"   </Access>"
        L"   <Normative/>"
        L"   <Supervisor/>"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.3.77' comment='клиент77' />"
        L"   <Computer ip='192.168.1.105' comment='primary server' live='Y' />"
        L"</Test>";
    const wstring text_out =
        L"<Test>"
        L"   <Access district='aaa' default='view' >"
        L"       <Rule ip='192.168.1.105' level='edit' />"
        L"   </Access>"
        L"   <Normative/>"
        L"   <Supervisor/>"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.105' comment='primary server' live='Y' />"
        L"   <Computer ip='192.168.3.77' comment='клиент77' />"
        L"</Test>";

    // загрузка
    attic::a_document doc_in;
    doc_in.load_wide( text_in );
    Designated pd;
    pd.deserialize( doc_in.child( "Test" ) );
    std::vector< Permission::Computer > vComputers;
    std::set< IpAdr > vLiveComputers;
    pd.deserializeComputers( doc_in.child( "Test" ), vComputers, vLiveComputers );

    // вывод
    attic::a_document doc_out;
    doc_out.set_document_element( "Test" );
    pd.serialize( doc_out.child( "Test" ) );
    pd.serializeComputers( doc_out.child( "Test" ), vComputers, vLiveComputers );
    std::string after = doc_out.to_str();

    // эталон
    attic::a_document doc_et;
    doc_et.load_wide( text_out );
    std::string etalon = doc_et.to_str();

    CPPUNIT_ASSERT( etalon == after );
}

void TC_PermissionDesignated::LoadSaveJournal()
{
    const wstring text =
        L"<Test>"
        L"   <Access district='bbb' >"
        L"       <Rule ip='192.168.1.1' level='edit' />"
        L"   </Access>"
        L"   <JournalAccess code='jrn-1' >"
        L"       <Rule ip='192.168.1.1' level='edit' />"
        L"       <Rule ip='192.168.1.2' level='view' />"
        L"   </JournalAccess>"
        L"   <JournalAccess code='jrn-2' >"
        L"       <Rule ip='192.168.2.2' level='view' />"
        L"   </JournalAccess>"
        L"   <Normative>"
        L"       <Rule ip='192.168.1.77' level='edit' />"
        L"   </Normative>"
        L"   <Supervisor>"
        L"       <Rule ip='192.168.1.2' level='edit' />"
        L"   </Supervisor>"
        L"   <Computer ip='192.168.1.1' comment='клиент1' live='Y' />"
        L"   <Computer ip='192.168.1.2' comment='клиент2' />"
        L"   <Computer ip='192.168.1.77' comment='клиент77' />"
        L"   <Computer ip='192.168.1.105' comment='primary server' live='Y' />"
        L"</Test>";

    attic::a_document doc;
    attic::a_parse_result res = doc.load_wide( text );
    CPPUNIT_ASSERT( res == true );
    const std::wstring before = doc.pretty_wstr();

    Designated pd;
    attic::a_node rootNode = doc.child( "Test" );
    auto msg = pd.deserialize( rootNode );
    std::vector< Permission::Computer > vComputers;
    std::set< IpAdr > vLiveComputers;
    pd.deserializeComputers( rootNode, vComputers, vLiveComputers );
    CPPUNIT_ASSERT_MESSAGE( "Ошибка загрузки назначенных прав доступа", msg.empty() );

    // вывод специально дважды в один нод - проверка на появление дублей
    doc.reset();
    doc.set_document_element( "Test" );
    pd.serialize( doc.child( "Test" ) );
    pd.serializeComputers( doc.child( "Test" ), vComputers, vLiveComputers );
    const std::wstring after = doc.pretty_wstr();
    CPPUNIT_ASSERT( before == after );
}

void TC_PermissionDesignated::GetSetPermit()
{
    wstring U1 = L"уч.111-222";
    IpAdr Alfa( "192.168.1.1" );
    IpAdr Beta( "192.168.1.101" );

    Designated pd;
    // добавим участок
    CPPUNIT_ASSERT( pd.getDistricts().size() == pd.numberFakeDistricts() ); // всегда есть права на псевдо-участки
    pd.addDistrict( U1, Level::VIEW() );
    CPPUNIT_ASSERT( pd.getDistricts().size() == 1 + pd.numberFakeDistricts() );
    CPPUNIT_ASSERT( pd.takeChanges() );
    // один редактор и один наблюдатель
    pd.setPermit( U1, Level::EDIT(), Alfa );
    CPPUNIT_ASSERT( pd.takeChanges() );
    pd.setPermit( U1, Level::VIEW(), Beta );
    CPPUNIT_ASSERT( pd.takeChanges() );
    {
        auto e = pd.getPermits( Alfa, Level::EDIT() );
        CPPUNIT_ASSERT( 1 == count( e.begin(), e.end(), U1 ) );
        CPPUNIT_ASSERT( e.size() == 1 );
        auto v = pd.getPermits( Beta, Level::VIEW() );
        CPPUNIT_ASSERT( 1 == count( v.begin(), v.end(), U1 ) );
        CPPUNIT_ASSERT( v.size() == 1 );
    }
    // заменяем наблюдателя на редактора
    pd.setPermit( U1, Level::EDIT(), Beta );
    CPPUNIT_ASSERT( pd.takeChanges() );
    {
        auto e = pd.getPermits( Beta, Level::EDIT() );
        CPPUNIT_ASSERT( 1 == count( e.begin(), e.end(), U1 ) );
        CPPUNIT_ASSERT( e.size() == 1 );
        auto v = pd.getPermits( Beta, Level::VIEW() );
        CPPUNIT_ASSERT( v.empty() );
    }
    // вообще отнимаем права одного редактора - остается только один редактор
    pd.setPermit( U1, Level(), Alfa );
    CPPUNIT_ASSERT( pd.takeChanges() );
    {
        auto e = pd.getPermits( Alfa, Level::EDIT() );
        CPPUNIT_ASSERT( e.empty() );
        e = pd.getPermits( Beta, Level::EDIT() );
        CPPUNIT_ASSERT( 1 == count( e.begin(), e.end(), U1 ) );
        CPPUNIT_ASSERT( e.size() == 1 );
    }
}

void TC_PermissionDesignated::GetPermitByIp()
{
    wstring DV = L"уч.111-222";
    wstring DE = L"уч.222-333";
    IpAdr   ipA("192.168.1.1");
    IpAdr   ipB("192.168.1.11");
    IpAdr   ipC("192.168.1.111");

    Designated pd;
    // исходные данные
    pd.addDistrict( DV, Level::VIEW() );
    pd.addDistrict( DE, Level::EDIT() );
    pd.setPermit( DV, Level::EDIT(), ipA );
    pd.setPermit( DE, Level::VIEW(), ipB );
    pd.setPermit( DE, Level::EDIT(), ipC );
    CPPUNIT_ASSERT( pd.takeChanges() );
    // проверка
    {
        auto e = pd.getPermits( ipA, Level::EDIT() );
        CPPUNIT_ASSERT( 0 < std::count(e.begin(), e.end(), DV) );
        CPPUNIT_ASSERT( 0 < std::count(e.begin(), e.end(), DE) );
        auto v = pd.getPermits( ipA, Level::VIEW() );
        CPPUNIT_ASSERT( v.empty() );
    }
    // проверка
    {
        auto e = pd.getPermits( ipB, Level::EDIT() );
        CPPUNIT_ASSERT( e.empty() );
        auto v = pd.getPermits( ipB, Level::VIEW() );
        CPPUNIT_ASSERT( 0 < std::count(v.begin(), v.end(), DV) );
        CPPUNIT_ASSERT( 0 < std::count(v.begin(), v.end(), DE) );
    }
    // проверка
    {
        auto e = pd.getPermits( ipC, Level::EDIT() );
        CPPUNIT_ASSERT( 0 < std::count(e.begin(), e.end(), DE) );
        auto v = pd.getPermits( ipC, Level::VIEW() );
        CPPUNIT_ASSERT( 0 < std::count(v.begin(), v.end(), DV) );
    }
}

void TC_PermissionDesignated::GetIpHostName()
{
    CPPUNIT_ASSERT( IpAdr( "127.0.0.2").gethostname().empty() );
    CPPUNIT_ASSERT( !IpAdr("127.0.0.1").gethostname().empty() );
    CPPUNIT_ASSERT( !IpAdr("8.8.8.8").gethostname().empty() );
}

void TC_PermissionDesignated::accessNormative()
{
    IpAdr Alfa( "192.168.1.1" );
    IpAdr Beta( "192.168.1.101" );

    Designated pd;
    // доступ к нормативному есть у всех
    CPPUNIT_ASSERT( Level() != pd.getNormativePermit( Alfa ) );
    CPPUNIT_ASSERT( Level() != pd.getNormativePermit( Beta ) );
    // даем право редактирования одному
    pd.setNormativePermit( Level::EDIT(), Beta );
    CPPUNIT_ASSERT( pd.takeChanges() );
    {
        std::set<Permission::IpAdr> vAllComputers;
        vAllComputers.insert( Alfa );
        vAllComputers.insert( Beta );

        auto e = pd.getNormativePermits( vAllComputers, Level::EDIT() );
        CPPUNIT_ASSERT( 1 == count( e.begin(), e.end(), Beta ) );
        CPPUNIT_ASSERT( e.size() == 1 );
        auto v = pd.getNormativePermits( vAllComputers, Level::VIEW() );
        CPPUNIT_ASSERT( 1 == count( v.begin(), v.end(), Alfa ) );
        CPPUNIT_ASSERT( v.size() == 1 );
    }
    // отнять право просмотра нельзя
    CPPUNIT_ASSERT( !pd.setNormativePermit( Level(), Alfa ) );
    CPPUNIT_ASSERT( !pd.takeChanges() );

    //
    // возможен доступ через псевдо-код участка
    //
    const std::wstring ALIAS = pd.getNormativeCode();
    CPPUNIT_ASSERT( !pd.addDistrict( ALIAS, Level::VIEW() ) );
    CPPUNIT_ASSERT( !pd.delDistrict( ALIAS ) );
    {
        auto e = pd.getPermits( Beta, Level::EDIT() );
        CPPUNIT_ASSERT( e.empty() && pd.getNormativePermit( Beta )==Level::EDIT() );
        auto v = pd.getPermits( Alfa, Level::VIEW() );
        CPPUNIT_ASSERT( v.empty() && pd.getNormativePermit( Alfa )==Level::VIEW() );
    }
    // отнять право просмотра нельзя
    CPPUNIT_ASSERT( Level::EDIT() == pd.getPermit( ALIAS, Beta ) );
    CPPUNIT_ASSERT( Level::VIEW() == pd.getPermit( ALIAS, Alfa ) );
    CPPUNIT_ASSERT( !pd.setPermit( ALIAS, Level(), Alfa ) );
    // права на нормативный график не связаны с обычными участками
    CPPUNIT_ASSERT( pd.getDistricts().size() == pd.numberFakeDistricts() );
    const auto ds = pd.getDistricts();
    CPPUNIT_ASSERT( ds.end() != std::find( ds.begin(), ds.end(), ALIAS ) );
}

void TC_PermissionDesignated::accessSupervis()
{
    IpAdr Alfa( "192.168.1.1" );
    IpAdr Beta( "192.168.1.101" );

    Designated pd;
    // формально доступ супервизора есть у всех - только нерабочий
    CPPUNIT_ASSERT( Level() != pd.getSupervisPermit( Alfa ) );
    CPPUNIT_ASSERT( Level() != pd.getSupervisPermit( Beta ) );
    // даем право редактирования одному
    pd.setSupervisPermit( Level::EDIT(), Beta );
    {
        std::set<Permission::IpAdr> vAllComputers;
        vAllComputers.insert( Alfa );
        vAllComputers.insert( Beta );
        auto e = pd.getSupervisPermits( vAllComputers, Level::EDIT() );
        CPPUNIT_ASSERT( 1 == count( e.begin(), e.end(), Beta ) );
        CPPUNIT_ASSERT( e.size() == 1 );
        auto v = pd.getSupervisPermits( vAllComputers, Level::VIEW() );
        CPPUNIT_ASSERT( 1 == count( v.begin(), v.end(), Alfa ) );
        CPPUNIT_ASSERT( v.size() == 1 );
    }
    // отнять право просмотра нельзя
    CPPUNIT_ASSERT( !pd.setSupervisPermit( Level(), Alfa ) );
    //
    // возможен доступ через псевдо-код участка
    //
    const std::wstring ALIAS = pd.getSupervisCode();
    CPPUNIT_ASSERT( !pd.addDistrict( ALIAS, Level::VIEW() ) );
    CPPUNIT_ASSERT( !pd.delDistrict( ALIAS ) );
    {
        auto e = pd.getPermits( Beta, Level::EDIT() );
        CPPUNIT_ASSERT( e.empty() && pd.getSupervisPermit( Beta ) == Level::EDIT() );
        auto v = pd.getPermits( Alfa, Level::VIEW() );
        CPPUNIT_ASSERT( v.empty() && pd.getSupervisPermit( Alfa ) == Level::VIEW() );
    }
    // отнять право просмотра нельзя
    CPPUNIT_ASSERT( Level::EDIT() == pd.getPermit( ALIAS, Beta ) );
    CPPUNIT_ASSERT( Level::VIEW() == pd.getPermit( ALIAS, Alfa ) );
    CPPUNIT_ASSERT( !pd.setPermit( ALIAS, Level(), Alfa ) );
    // права супервизора не связаны с обычными участками
    CPPUNIT_ASSERT( pd.getDistricts().size() == pd.numberFakeDistricts() );
    const auto ds = pd.getDistricts(); 
    CPPUNIT_ASSERT( ds.end() != std::find( ds.begin(), ds.end(), ALIAS ) );
}

void TC_PermissionDesignated::GetSetPermit_Journal()
{
    wstring J1 = L"журнал первый";
    IpAdr Alfa("192.168.1.1" );
    IpAdr Beta("192.168.1.101" );

    Designated pd;
    // обращение к неизвестному журналу
    CPPUNIT_ASSERT( !pd.setJrnPermit( J1, Level::EDIT(), Alfa ) );
    CPPUNIT_ASSERT( !pd.takeChanges() );
    // добавим журнал
    CPPUNIT_ASSERT( pd.getJournals().empty() );
    pd.addJournal( J1 );
    CPPUNIT_ASSERT( pd.getJournals().size() == 1 );
    CPPUNIT_ASSERT( pd.takeChanges() );
    // один редактор и один наблюдатель
    pd.setJrnPermit( J1, Level::EDIT(), Alfa );
    pd.setJrnPermit( J1, Level::VIEW(), Beta );
    CPPUNIT_ASSERT( Level::EDIT() == pd.getJrnPermit( J1, Alfa ) );
    CPPUNIT_ASSERT( Level::VIEW() == pd.getJrnPermit( J1, Beta ) );
    CPPUNIT_ASSERT( pd.takeChanges() );
    // заменяем наблюдателя на редактора
    pd.setJrnPermit( J1, Level::EDIT(), Beta );
    CPPUNIT_ASSERT( pd.takeChanges() );
    CPPUNIT_ASSERT( Level::EDIT() == pd.getJrnPermit( J1, Alfa ) );
    CPPUNIT_ASSERT( Level::EDIT() == pd.getJrnPermit( J1, Beta ) );
    // удаляем журнал
    CPPUNIT_ASSERT( !pd.delJournal( L"нет_такого" ) );
    CPPUNIT_ASSERT( pd.delJournal( J1 ) );
    CPPUNIT_ASSERT( pd.takeChanges() );
    CPPUNIT_ASSERT( Level() == pd.getJrnPermit( J1, Alfa ) );
    CPPUNIT_ASSERT( Level() == pd.getJrnPermit( J1, Beta ) );
}
