#include "stdafx.h"

#include "TC_TowerCmd.h"
#include "../Tower/CarChain.h"
#include "../Tower/Command.h"
#include "../Tower/RollingStock.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_TowerCmd );

void TC_TowerCmd::split()
{
    std::string trash = "x}х";
    std::string sample = ToUtf8(
        L"{action:\"service\", {cycle:\"Train\"},  {get:\"masterTrain\"},{get:\"2П(85310)\"} ,{ get:\"11010-12010\"}}"
        );

    TowerCommand::Splitter cmd;
    // отдельно мусор и команда
    cmd.append( trash );
    CPPUNIT_ASSERT( !cmd.ready() );
    cmd.append( sample );
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res1 = cmd.take();

    // мусор и половинки команды
    size_t pos = sample.size()/2;
    cmd.append( trash+sample.substr(0,pos) );
    CPPUNIT_ASSERT( !cmd.ready() );
    cmd.append( sample.substr(pos)+trash );
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res2 = cmd.take();
    CPPUNIT_ASSERT( res1 == res2 );

    // несколько команд сразу
    cmd.append( sample+sample+sample );
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res3 = cmd.take();
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res4 = cmd.take();
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res5 = cmd.take();
    CPPUNIT_ASSERT( !cmd.ready() );
    CPPUNIT_ASSERT( res3 == res4 );
    CPPUNIT_ASSERT( res4 == res5 );

    TowerCommand::Detail detail( FromUtf8(res1) );
    TowerCommand::Thing th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
}


void TC_TowerCmd::timeout()
{
    std::string big = ToUtf8(
        L"{action:\"service\", {cycle:\"Train\"},  {get:\"masterTrain\"},{get:\"2П(85310)\"} ,{ get:\"11010-12010\"}}"
        );
    std::string lo = big.substr( 0, big.size()-5 );
    std::string hi = big.substr( big.size()-5 );

    TowerCommand::Splitter cmd;
    // две половинки быстро
    cmd.append( lo );
    cmd.append( hi );
    CPPUNIT_ASSERT( cmd.ready() );
    std::string res = cmd.take();

    // две половинки с паузой
    cmd.set_timeout( boost::posix_time::milliseconds(150) );
    cmd.append( lo );
    CPPUNIT_ASSERT( !cmd.ready() );
    boost::this_thread::sleep( boost::posix_time::milliseconds(300) );
    cmd.append( hi );
    CPPUNIT_ASSERT( !cmd.ready() );
}

void TC_TowerCmd::decode_LOS()
{
    TowerCommand::StateDecoder deco;
    auto p = deco.convert( TowerCommand::Refer(SWITCH, SWITCH), L"+" );
    auto m = deco.convert( TowerCommand::Refer(SWITCH, SWITCH), L"-" );
    auto oo= deco.convert( TowerCommand::Refer(SWITCH, SWITCH), L"вне контроля" );
    CPPUNIT_ASSERT( p );
    CPPUNIT_ASSERT( m );
    CPPUNIT_ASSERT( oo );
    CPPUNIT_ASSERT( !attic::identical( p, m ) );
    CPPUNIT_ASSERT( !attic::identical( p, oo ) );
    CPPUNIT_ASSERT( !attic::identical( m, oo ) );


//     CPPUNIT_ASSERT( deco.convert( HEAD, SHUNTING, L"Белый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, SHUNTING, L"Синий") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, SHUNTING, L"Черный") );
// 
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Зеленый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Красный") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Черный") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Желтый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Желтый+Зеленый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Желтый_мигающий") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Зеленый_мигающий") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Желтый+Желтый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, TRAIN, L"Желтый+Желтый_мигающий") );
// 
//     CPPUNIT_ASSERT( deco.convert( HEAD, INVITATION, L"Белый мигающий") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, INVITATION, L"Черный") );
// 
//     CPPUNIT_ASSERT( deco.convert( HEAD, HUMP, L"Зеленый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, HUMP, L"Желтый") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, HUMP, L"Красный") );
//     CPPUNIT_ASSERT( deco.convert( HEAD, HUMP, L"Черный") );
}

void TC_TowerCmd::build()
{
    std::wstring g1 = L"{action:\"service\",{cycle:\"Obj\"}}";
    TowerCommand::Detail c( TowerCommand::act_service );
    c.push_back_cycle( false, true );
    std::wstring w1 = c.to_str();
    CPPUNIT_ASSERT( w1 == g1 );

    std::wstring g2 = L"{action:\"service\",{cycle:\"Obj\"},{get:\"44444\"},{get:\"33333-77777\"}}";
    c.push_back( L"get", EsrKit(44444) );
    c.push_back( L"get", EsrKit(33333,77777) );
    std::wstring w2 = c.to_str();
    CPPUNIT_ASSERT( w2 == g2 );

    std::wstring g3 = L"{action:\"service\",{cycle:\"Obj\"},{get:\"44444\"},{get:\"33333-77777\"},{get:\"2Пж(66666)\"}}";
    c.push_back( L"get", BadgeE( L"2Пж", EsrKit(66666) ) );
    std::wstring w3 = c.to_str();
    CPPUNIT_ASSERT( w3 == g3 );
}

void TC_TowerCmd::parse_setObject()
{
    std::wstring source = L"{action:\"setObject\", {object: \"11/13(11112)\", state: \"+\", object:\"9*11(22222)\", state:\"белый\"}}";
    TowerCommand::Detail detail( source );
    const TowerCommand::Thing& th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
    CPPUNIT_ASSERT( th.childs() == 2 );
    CPPUNIT_ASSERT( th[0].name() == L"action" );

    const TowerCommand::Thing& sub = th[1];

    CPPUNIT_ASSERT( sub.childs() == 4 );
    CPPUNIT_ASSERT( sub[0].name() == L"object" );
    CPPUNIT_ASSERT( sub[0].as<BadgeE>() == BadgeE( L"11/13", EsrKit(11112) ) );
    CPPUNIT_ASSERT( sub[1].name() == L"state" );
    CPPUNIT_ASSERT( sub[1].as<std::wstring>() == L"+" );
    CPPUNIT_ASSERT( sub[2].as<BadgeE>() == BadgeE( L"9*11", EsrKit(22222) ) );
    CPPUNIT_ASSERT( sub[3].as<std::wstring>() == L"белый" );

    std::wstring revert = th.to_str();
    CPPUNIT_ASSERT( !revert.empty() );
    auto iSrc = source.begin();
    auto iRev = revert.begin();
    while( source.end() != iSrc && revert.end() != iRev )
    {
        if ( *iSrc == *iRev )
            ++iSrc, ++iRev;
        else if ( ' ' == *iSrc )
            ++iSrc;
        else if ( ' ' == *iRev )
            ++iRev;
        else
            break;
    }
    CPPUNIT_ASSERT( source.end() == iSrc );
    CPPUNIT_ASSERT( revert.end() == iRev );
}

void TC_TowerCmd::parse_listTrain()
{
    std::wstring source =
        L"{action:\"listTrain\","
        L" {train:\"50331649\",speed:\"5\",dist:\"85\", pos:\"м23/м102п(7000)\",name:\"маневровый №101\",speedup:\"0\",ahead:\"м102п(7000)\",moment:\"08.12.09\"},"
        L" {train:\"50332008\",speed:\"0\",dist:\"1\",pos:\"4п_2(7000)\",name:\"поезд №2914\",speedup:\"0\",ahead:\"м33п(7000)\",moment:\"08.12.09\"},"
        L"}";
    // 06.11.2020 07:54:25 -  >> {action:"listTrain",{train:"50331649",speed:"5",dist:"85",pos:"м23/м102п(7000)",name:"маневровый №101",speedup:"0",ahead:"м102п(7000)",moment:"08.12.09"},{train:"50332006",speed:"0",dist:"0",pos:"1п_2(7000)",name:"поезд №3022",speedup:"0",ahead:"м27п(7000)",moment:"08.12.09"},{train:"50332007",speed:"0",dist:"124",pos:"26п_2(7000)",name:"поезд №3757",speedup:"0",ahead:"м77п(7000)",moment:"08.12.09"},{train:"50332008",speed:"0",dist:"1",pos:"4п_2(7000)",name:"поезд №2914",speedup:"0",ahead:"м33п(7000)",moment:"08.12.09"},{train:"50332009",speed:"0",dist:"1",pos:"25п_2(7000)",name:"поезд №2478",speedup:"0",ahead:"м75п(7000)",moment:"08.12.09"},{train:"50332010",speed:"0",dist:"0",pos:"5п_2(7000)",name:"поезд №1958",speedup:"0",ahead:"м35п(7000)",moment:"08.12.09"},{train:"50332011",speed:"0",dist:"0",pos:"24п_2(7000)",name:"поезд №2149",speedup:"0",ahead:"м73п(7000)",moment:"08.12.09"},{train:"50332012",speed:"0",dist:"1",pos:"6п_2(7000)",name:"поезд №3032",speedup:"0",ahead:"м37п(7000)",moment:"08.12.09"},{train:"50332013",speed:"0",dist:"0",pos:"23п_2(7000)",name:"поезд №1397",speedup:"0",ahead:"м71п(7000)",moment:"08.12.09"},{train:"50332014",speed:"0",dist:"1",pos:"7п_2(7000)",name:"поезд №3310",speedup:"0",ahead:"м39п(7000)",moment:"08.12.09"},{train:"50332015",speed:"0",dist:"1",pos:"22п_2(7000)",name:"поезд №1264",speedup:"0",ahead:"м69п(7000)",moment:"08.12.09"},{train:"50332016",speed:"0",dist:"1",pos:"8п_2(7000)",name:"поезд №2090",speedup:"0",ahead:"м41п(7000)",moment:"08.12.09"},{train:"50332017",speed:"0",dist:"0",pos:"21п_2(7000)",name:"поезд №2964",speedup:"0",ahead:"м67п(7000)",moment:"08.12.09"},{train:"50332018",speed:"18",dist:"253",pos:"9п_2(7000)",name:"поезд №1911",speedup:"-233.28",ahead:"м43п(7000)",moment:"08.12.09"},{train:"50332019",speed:"25",dist:"491",pos:"20п_2(7000)",name:"поезд №3325",speedup:"-155.52",ahead:"м65п(7000)",moment:"08.12.09"},{train:"50332020",speed:"23",dist:"17",pos:"10п7(7000)",name:"поезд №1050",speedup:"-336.96",ahead:"10п8(7000)",moment:"08.12.09"},{train:"50332021",speed:"28",dist:"5",pos:"235-251п2(7000)",name:"поезд №3349",speedup:"570.24",ahead:"235-251п3(7000)",moment:"08.12.09"},{train:"50332022",speed:"5",dist:"30",pos:"м102п(7000)",name:"поезд №3342",speedup:"0",ahead:"201сп(7000)",moment:"08.12.09"},{train:"50332023",speed:"5",dist:"42",pos:"м23/м102п(7000)",name:"поезд №2109",speedup:"0",ahead:"м102п(7000)",moment:"08.12.09"}}

    TowerCommand::Detail detail( source );
    const TowerCommand::Thing& th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
    CPPUNIT_ASSERT( th.childs() == 3 );
    CPPUNIT_ASSERT( th[0].name( L"action" ) );
    CPPUNIT_ASSERT( th[0].value() == L"listTrain" );

    const TowerCommand::Thing& sub1 = th[1];

    CPPUNIT_ASSERT( sub1[ L"train" ].as<long>() == 50331649 );
    CPPUNIT_ASSERT( sub1[ L"speed" ].as<int>() == 5 );
    CPPUNIT_ASSERT( sub1[ L"dist"  ].as<int>() == 85 );
    CPPUNIT_ASSERT( sub1[ L"pos" ].as<BadgeE>() == BadgeE( L"м23/м102п", EsrKit(7000) ) );
    CPPUNIT_ASSERT( sub1[ L"ahead" ].as<BadgeE>() == BadgeE( L"м102п", EsrKit(7000) ) );
    CPPUNIT_ASSERT( sub1[ L"moment" ].as<std::wstring>() == L"08.12.09" );

    CPPUNIT_ASSERT( th[2][ L"dist" ].as<int>() == 1 );
    CPPUNIT_ASSERT( th[2][ L"name" ].as<std::wstring>() == L"поезд №2914" );
}

void TC_TowerCmd::parse_listVan()
{
    std::wstring source =
    L"{action:\"listVan\","
    L" {vag:\"50331650\",lenght:\"14\",name:\"56000001\",axis:\"4\",type:\"полувагон\",destination:\"1п_2\", brutto:\"23\"},"
    L" {vag:\"50331651\",lenght:\"15\",name:\"24672126\",axis:\"5\",type:\"крытый\",   destination:\"26п_2\",brutto:\"21\"},"
    L"}";

    TowerCommand::Detail detail( source );
    const TowerCommand::Thing& th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
    CPPUNIT_ASSERT( th.childs() == 3 );
    CPPUNIT_ASSERT( th[0].name( L"action" ) );
    CPPUNIT_ASSERT( th[0].value() == L"listVan" );

    const TowerCommand::Thing& s1 = th[1];
    const TowerCommand::Thing& s2 = th[2];

    {
        const auto car1 = s1.as<RollingStock::Car>();
        CPPUNIT_ASSERT( car1.number() == 56000001 );
        CPPUNIT_ASSERT( car1.length() == 14 );
        CPPUNIT_ASSERT( car1.gross_weight() == 23 );
        CPPUNIT_ASSERT( car1.axis() == 4 );
        CPPUNIT_ASSERT( car1.destination_text() == L"1п_2" );
        CPPUNIT_ASSERT( car1.destination_num() == 1 );
    }
    {
        const auto car2 = s2.as<RollingStock::Car>();
        CPPUNIT_ASSERT( car2.number() == 24672126 );
        CPPUNIT_ASSERT( car2.length() == 15 );
        CPPUNIT_ASSERT( car2.gross_weight() == 21 );
        CPPUNIT_ASSERT( car2.axis() == 5 );
        CPPUNIT_ASSERT( car2.destination_text() == L"26п_2" );
        CPPUNIT_ASSERT( car2.destination_num() == 26 );
    }
}

void TC_TowerCmd::parse_start()
{

}

void TC_TowerCmd::parse_time()
{

}

void TC_TowerCmd::parse_pult_split()
{
    std::wstring source =
        L"{action:\"pult\",type:\"split\",count:\"2\","
        L"{number:\"56247321\",route:\"21п_2\",index:\"0\",chain:\"0\",marks:\"0 0 0\",weigth:\"mediumheavy\"},"
        L"{number:\"56424321\",route:\"27п_2\",index:\"1\",chain:\"3\",marks:\"0 0 0\",weigth:\"mediumheavy\"}"
        L"}";
    TowerCommand::Detail detail( source );
    const TowerCommand::Thing& th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
    CPPUNIT_ASSERT( th.childs() == 5 );
    CarChain cc3 = th[3].as<CarChain>();
    CPPUNIT_ASSERT( cc3.carNumber() == 56247321 );
    CPPUNIT_ASSERT( cc3.chainNumber() == 1 );
    CarChain cc4 = th[4].as<CarChain>();
    CPPUNIT_ASSERT( cc4.carNumber() == 56424321 );
    CPPUNIT_ASSERT( cc4.chainNumber() == 4 );
}

void TC_TowerCmd::parse_pult_set()
{
    std::wstring pult_set1 = 
        L"{action:\"pult\",type:\"set\",count:\"4\","
        L"{number:\"56000011\",route:\"11п_2\",index:\"0\",chain:\"0\",marks:\"0 0 0\",weigth:\"veryheavy\"},"
        L"{number:\"56153243\",route:\"43п_2\",index:\"1\",chain:\"11\",marks:\"0 0 0\",weigth:\"veryheavy\"}}"
        ;
    TowerCommand::Detail detail( pult_set1 );
    const TowerCommand::Thing& th = detail.get();
    CPPUNIT_ASSERT( !th.empty() );
    CPPUNIT_ASSERT( th.childs() == 5 );
    CarChain cc3 = th[3].as<CarChain>();
    CPPUNIT_ASSERT( cc3.carNumber() == 56247321 );
    CPPUNIT_ASSERT( cc3.chainNumber() == 1 );
    CarChain cc4 = th[4].as<CarChain>();
    CPPUNIT_ASSERT( cc4.carNumber() == 56424321 );
    CPPUNIT_ASSERT( cc4.chainNumber() == 4 );
}
