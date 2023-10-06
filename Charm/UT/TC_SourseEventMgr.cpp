#include "stdafx.h"

#include "../helpful/Attic.h"
#include "TC_SourseEventMgr.h"
#include "../Hem/XmlStrConst.h"
#include "../Actor/HemDraw/HemEventPoint.h"
#include "../Actor/HemDraw/SourceEventMgr.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_SourseEventMgr );

void TC_SourseEventMgr::addChain()
{
SrcEventMgr evMgr;
evMgr.load_chains(vChains);

if ( !vChains.empty() )
    {
    const HemdrawType::BASEPOINT_VECT& vEv = vChains.front();
    EventChainPtr pChain = evMgr.get_chain( vEv.front() );
    CPPUNIT_ASSERT(pChain!=NULL);
    CPPUNIT_ASSERT( pChain->vEv==vEv );
    }

if ( vChains.size()>2 )
{
    const HemdrawType::BASEPOINT_VECT& vEv = vChains[2];
    CPPUNIT_ASSERT(vEv.size());
    EventChainPtr pChain = evMgr.get_chain( vEv.back() );
    CPPUNIT_ASSERT(pChain!=NULL);
    CPPUNIT_ASSERT( pChain->vEv==vEv );
}
}

void TC_SourseEventMgr::acceptChanges()
{
checkAddDelChain();
checkAddDelEvents();
checkChangeEvents();
}


void TC_SourseEventMgr::checkAddDelEvents()
{
    SrcEventMgr evMgr;
    evMgr.load_chains(vChains);

    if ( vChains.size()>1 )
    {
        const HemdrawType::BASEPOINT_VECT& vEv = vChains[1];
        EventChainPtr pChain = evMgr.get_chain( vEv.front() );
        if (vEv.size()>5 && pChain!=NULL && pChain->vEv==vEv )
        {
        //удаление начального события
            TrioGuessSpot trio;
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv.front() ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(vEv[1]) ) );
            trio.coverPre( true );
            std::set<EsrKit> vEsr;
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.front() )==NULL );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[1] )==pChain );
            CPPUNIT_ASSERT( pChain->vEv.front()==vEv[1] );
            CPPUNIT_ASSERT( pChain->vEv.size()+1==vEv.size() );
            
            //Добавление начального события
            trio = TrioGuessSpot();

            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[1] ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(trio.pre()) );
            trio.coverPre( true );
            trio.body( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv.front() )) );
            
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.front() )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv==vEv );

            //удаление конечного события
            trio = TrioGuessSpot();
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[vEv.size()-2] ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(vEv.back() ) ));
            trio.coverPost( true );
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.back() )==NULL );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[vEv.size()-2] )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv.size()+1==vEv.size() );

            //Добавление конечного события
            trio = TrioGuessSpot();
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( pChain->vEv.back() ) ) );
            trio.post( trio.pre() );
            trio.body( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv.back() ) )  );
            trio.coverPost(true);
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.back() )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv==vEv );

            //удаление промежуточных событий
            trio = TrioGuessSpot();
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[1] ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(vEv[5] ) ));
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[2] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[3] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[4] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[1] )==pChain && evMgr.get_chain( vEv[5] )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv.size()+3==vEv.size() );

            //Добавление промежуточных событий
            trio = TrioGuessSpot();
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[1] ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[5] ) ) );
            std::vector<HemdrawType::GuessEventBasePointPtr> vEvPtr;
            vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[2] ) )  );
            vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[3] ) )  );
            vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[4] ) )  );
            trio.body( vEvPtr.begin(), vEvPtr.end() );
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.front() )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv==vEv );
            vEvPtr.clear();
        }
    }
}

void TC_SourseEventMgr::checkChangeEvents()
{
    SrcEventMgr evMgr;
    evMgr.load_chains(vChains);

    if ( vChains.size()>1 )
    {
        const HemdrawType::BASEPOINT_VECT& vEv = vChains[1];
        EventChainPtr pChain = evMgr.get_chain( vEv.front() );
        if (vEv.size()>5 && pChain!=NULL && pChain->vEv==vEv )
        {
            //изменение начального события
            TrioGuessSpot trio;
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv.front() ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(vEv[1]) ) );
            trio.coverPre( true );
            GuessEventBasePoint ev = vEv.front();
            ev.SetCode( ev.GetCode()==HCode::FORM ? HCode::DISFORM : HCode::FORM );
            ev.SetTime( ev.GetTime() + 1 );
            trio.body( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( ev )) );
            const std::set<EsrKit> vEsr ;
            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv.front() )==NULL );
            CPPUNIT_ASSERT( evMgr.get_chain( ev )==pChain );
            CPPUNIT_ASSERT( pChain->vEv.front()==ev );
            CPPUNIT_ASSERT( pChain->vEv.size()==vEv.size() );

            //изменение промежуточных событий
            trio = TrioGuessSpot();
            GuessEventBasePoint ev_chg[2];
            ev_chg[0] = vEv[3];
            ev_chg[0].SetCode(vEv[2].GetCode());
            ev_chg[0].SetTime(vEv[2].GetTime());
            ev_chg[1] = vEv[4];
            ev_chg[1].SetTime(vEv[3].GetTime());
            ev_chg[1].SetCode(vEv[3].GetCode());
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( vEv[1] ) ) );
            trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(vEv[5] ) ));
            std::vector<HemdrawType::GuessEventBasePointPtr> vEvPtr;
            vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( ev_chg[0] ) )  );
            vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( ev_chg[1] ) )  );
            trio.body(vEvPtr.begin(), vEvPtr.end());

            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[2] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[3] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[4] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( vEv[1] )==pChain && evMgr.get_chain( vEv[5] )==pChain );
            CPPUNIT_ASSERT( evMgr.get_chain( ev_chg[0] )==pChain && evMgr.get_chain( ev_chg[1] )==pChain );
            CPPUNIT_ASSERT(  pChain->vEv.size()+1==vEv.size() );
            CPPUNIT_ASSERT(  pChain->vEv[2]==ev_chg[0] );
            CPPUNIT_ASSERT(  pChain->vEv[3]==ev_chg[1] );

            //изменение одного события
            trio = TrioGuessSpot();
            ev = ev_chg[0]; 
            trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( ev ) ) );
            trio.post( trio.pre() );
            trio.coverPre(true);
            trio.coverPost(true);
            ev.SetTime(ev.GetTime()-100);
            trio.body( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(ev)) );

            evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
            CPPUNIT_ASSERT( evMgr.get_chain( ev_chg[0] )==NULL  );
            CPPUNIT_ASSERT( evMgr.get_chain( ev )==pChain  );
            CPPUNIT_ASSERT(  pChain->vEv.size()+1==vEv.size() );
            CPPUNIT_ASSERT(  pChain->vEv[2]==ev );
        }
    }
}

void TC_SourseEventMgr::checkCreateChain()
{
    SrcEventMgr evMgr;

    const std::wstring trio_txt = 
    L"<HappenLayer>"
    L"<Trio>"
        L"<Body name='Form' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' index='' num='' noinfo='Y' />"
        L"<Body name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' />"
        L"</Trio>"
        L"<Trio>"
        L"<Pre name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' />"
        L"<Post name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' cover='Y' />"
        L"<Body name='Span_move' Bdg='N1PC[11412:11420]' create_time='20160524T012542Z' waynum='1'>"
        L"<rwcoord picketing1_val='130~720' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='130~298' picketing1_comm='Рига-Зилупе' />"
        L"</Body>"
        L"</Trio>"
        L"<Trio>"
        L"<Pre name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' cover='Y' />"
        L"<Post name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' cover='Y' />"
        L"<Body name='Departure' Bdg='4C[11420]' create_time='20160524T012401Z' waynum='4' parknum='1' index='0905-097-0600' num='1356' length='57' weight='1165' cont='Y' />"
        L"</Trio>"
    L"</HappenLayer>";

    attic::a_document x_doc;
    CPPUNIT_ASSERT(x_doc.load_wide( trio_txt ));
    
    HemdrawType::GuessEventBasePointPtr ev_check;

    const std::set<EsrKit> vEsr;
    for ( attic::a_node trio_node=x_doc.document_element().child(Trio_xTag); trio_node; trio_node=trio_node.next_sibling(Trio_xTag) )
    {
    TrioGuessSpot trio;
    trio << trio_node;
    evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
    ev_check = trio.body()[0];
    }
CPPUNIT_ASSERT(evMgr.eventMapSize()==3);
EventChainPtr pChain = evMgr.get_chain(*ev_check);
CPPUNIT_ASSERT( pChain && pChain->vEv[1]==*ev_check);

}

void TC_SourseEventMgr::checkAddDelChain()
{
    SrcEventMgr evMgr;
    evMgr.load_chains(vChains);

    if ( vChains.size()>1 )
    {
        const HemdrawType::BASEPOINT_VECT& vEv = vChains.front();
        if (evMgr.get_chain( vEv.front() )!=NULL && evMgr.get_chain( vEv.front() )->vEv==vEv )
            {
                const std::set<EsrKit> vEsr;
                //удаление цепочки
                TrioGuessSpot trio;
                GuessEventBasePoint ev = vEv.front();
                ev.extent(true);
                trio.pre( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint( ev ) ) );
                ev = vEv.back();                
                ev.extent(true);
                trio.post( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(ev) ) );
                trio.coverPre( true );
                trio.coverPost( true );
                
                evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
                for ( const auto& it : vEv )
                    CPPUNIT_ASSERT( evMgr.get_chain(it)==NULL );

                //добавление цепочки
                trio = TrioGuessSpot();
                std::vector<HemdrawType::GuessEventBasePointPtr> vEvPtr;
                for ( const auto& it : vEv )
                    vEvPtr.push_back( HemdrawType::GuessEventBasePointPtr(new GuessEventBasePoint(it)) );
                trio.body(vEvPtr.begin(), vEvPtr.end());               
                
                evMgr.accept_one_change(trio, LRT_HAPPEN, vEsr);
                EventChainPtr pChainCmp;
                for ( const auto& it : vEv )
                    {
                        EventChainPtr pChain = evMgr.get_chain( vEv.front() );
                        CPPUNIT_ASSERT(pChain!=NULL);
                        CPPUNIT_ASSERT( pChain->vEv==vEv );
                        if (!pChainCmp )
                            pChainCmp = pChain;
                        CPPUNIT_ASSERT( pChain==pChainCmp );
                    }
            }
    }
}

void TC_SourseEventMgr::setUp()
{
    const std::wstring evchains_txt = 
        L"<HappenLayer>"
        L"<HemPath>"
        L"<SpotEvent name='Pocket_entry' Bdg='BGP[09162]' create_time='20160217T103032Z' />"
        L"<SpotEvent name='Death' Bdg='BGP[09162]' create_time='20160218T071943Z' />"
        L"</HemPath>"
        L"<HemPath>"
        L"<SpotEvent name='Form' Bdg='N19P_VAB[11442:11446]' create_time='20160217T152618Z' waynum='1'>"
        L"<rwcoord picketing1_val='363~54' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='363~354' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Transition' Bdg='1BC[11442]' create_time='20160217T153331Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Arrival' Bdg='3AC[11443]' create_time='20160217T153830Z' waynum='3' parknum='1' />"
        L"<SpotEvent name='Departure' Bdg='3BC[11443]' create_time='20160217T155123Z' waynum='3' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1SP[11445]' create_time='20160217T155927Z' />"
        L"<SpotEvent name='Transition' Bdg='10SP[11451]' create_time='20160217T160726Z' />"
        L"<SpotEvent name='Span_move' Bdg='P3P[11000:11451]' create_time='20160217T160820Z' waynum='3'>"
        L"<rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Death' Bdg='P3P[11000:11451]' create_time='20160217T161217Z' waynum='3'>"
        L"<rwcoord picketing1_val='389~476' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='388~658' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"</HemPath>"
        L"<HemPath>"
        L"<SpotEvent name='Form' Bdg='N7PC[11391:11401]' create_time='20160217T152618Z' waynum='1'>"
        L"<rwcoord picketing1_val='156~780' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='157~80' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Arrival' Bdg='3C[11401]' create_time='20160217T152808Z' waynum='3' parknum='1' index='' num='V2263' fretran='Y' />"
        L"<SpotEvent name='Departure' Bdg='3C[11401]' create_time='20160217T153101Z' waynum='3' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='5aC[11412]' create_time='20160217T154225Z' waynum='5' parknum='1' />"
        L"<SpotEvent name='Arrival' Bdg='2C[11420]' create_time='20160217T155222Z' waynum='2' parknum='1' />"
        L"<SpotEvent name='Departure' Bdg='2C[11420]' create_time='20160217T161128Z' waynum='2' parknum='1' />"
        L"<SpotEvent name='Death' Bdg='PRPP[11420]' create_time='20160217T161300Z' />"
        L"</HemPath>"
        L"<HemPath>"
        L"<SpotEvent name='Form' Bdg='N13PC[11390:11391]' create_time='20160217T152637Z' waynum='1'>"
        L"<rwcoord picketing1_val='168~25' picketing1_comm='Рига-Зилупе' />"
        L"<rwcoord picketing1_val='168~325' picketing1_comm='Рига-Зилупе' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Transition' Bdg='1C[11390]' create_time='20160217T153926Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Arrival' Bdg='2C[11381]' create_time='20160217T154807Z' waynum='2' parknum='1' />"
        L"<SpotEvent name='Departure' Bdg='2C[11381]' create_time='20160217T164453Z' waynum='2' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11380]' create_time='20160217T170733Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11311]' create_time='20160217T172212Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='701SP:701+[11321]' create_time='20160217T173303Z' />"
        L"<SpotEvent name='Transition' Bdg='2C[11310]' create_time='20160217T173751Z' waynum='2' parknum='1' index='0900-787-1800' num='2272' fretran='Y' />"
        L"<SpotEvent name='Death' Bdg='17-21SP:21+[11310]' create_time='20160217T174046Z' />"
        L"</HemPath>"
        L"<HemPath>"
        L"<SpotEvent name='Form' Bdg='KK5C[11050:11052]' create_time='20160217T152654Z' waynum='1'>"
        L"<rwcoord picketing1_val='436~500' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='437~500' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Transition' Bdg='1C[11050]' create_time='20160217T152932Z' waynum='1' parknum='1' index='' num='D2843' fretran='Y' />"
        L"<SpotEvent name='Transition' Bdg='1C[11051]' create_time='20160217T154050Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='2C[11060]' create_time='20160217T154733Z' waynum='2' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11062]' create_time='20160217T160715Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='3SP[11063]' create_time='20160217T161716Z' />"
        L"<SpotEvent name='Transition' Bdg='2-4SP:2+[11775]' create_time='20160217T162301Z' />"
        L"<SpotEvent name='Transition' Bdg='1SP[11363]' create_time='20160217T162516Z' />"
        L"<SpotEvent name='Span_move' Bdg='PM8P[11010:11363]' create_time='20160217T163030Z' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Death' Bdg='PM8P[11010:11363]' create_time='20160217T163721Z' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"</SpotEvent>"
        L"</HemPath>"
        L"<HemPath>"
        L"<SpotEvent name='Form' Bdg='1N1IP[11042:16169]' create_time='20160217T152709Z' waynum='1'>"
        L"<rwcoord picketing1_val='459~800' picketing1_comm='Вентспилс' />"
        L"<rwcoord picketing1_val='461~800' picketing1_comm='Вентспилс' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Arrival' Bdg='3C[11042]' create_time='20160217T152832Z' waynum='3' parknum='1' index='' num='D2847' fretran='Y' />"
        L"<SpotEvent name='Departure' Bdg='3C[11042]' create_time='20160217T162225Z' waynum='3' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C&apos;2[11041]' create_time='20160217T163304Z' waynum='1' parknum='2' />"
        L"<SpotEvent name='Transition' Bdg='1AC[11041]' create_time='20160217T163545Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11052]' create_time='20160217T164451Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11050]' create_time='20160217T170006Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11051]' create_time='20160217T171040Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11060]' create_time='20160217T171547Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='1C[11062]' create_time='20160217T173146Z' waynum='1' parknum='1' />"
        L"<SpotEvent name='Transition' Bdg='3SP[11063]' create_time='20160217T173959Z' />"
        L"<SpotEvent name='Transition' Bdg='2-4SP:2+[11775]' create_time='20160217T174542Z' />"
        L"<SpotEvent name='Transition' Bdg='1SP[11363]' create_time='20160217T174811Z' />"
        L"<SpotEvent name='Span_move' Bdg='PM8P[11010:11363]' create_time='20160217T175459Z' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"</SpotEvent>"
        L"<SpotEvent name='Death' Bdg='PM8P[11010:11363]' create_time='20160217T175926Z' waynum='2'>"
        L"<rwcoord picketing1_val='5~500' picketing1_comm='524km' />"
        L"<rwcoord picketing1_val='6~500' picketing1_comm='524km' />"
        L"</SpotEvent>"
        L"</HemPath>"
        L"</HappenLayer>";
        
        attic::a_document x_doc;
        CPPUNIT_ASSERT(x_doc.load_wide( evchains_txt ));

        vChains.clear();
        for ( attic::a_node path_node=x_doc.document_element().child(HemPath_xAttr); path_node; path_node=path_node.next_sibling(HemPath_xAttr) )
        {
            HemdrawType::BASEPOINT_VECT vCurPath;
            for( attic::a_node ite = path_node.child(SpotEvent_xAttr); ite; ite = ite.next_sibling(SpotEvent_xAttr) )
            {
                //считываем событие
                GuessEventBasePoint ev(ite);
                vCurPath.push_back(ev);
            }
                
            vChains.push_back( vCurPath );
        }

}

void TC_SourseEventMgr::tearDown()
{
    SrcEventMgr evMgr;
    evMgr.load_chains(vChains);

    if ( vChains.size()>1 )
    {
        const HemdrawType::BASEPOINT_VECT& vEv = vChains.front();
        EventChainPtr pChain = evMgr.get_chain( vEv.front() );
        CPPUNIT_ASSERT(pChain!=NULL);
        CPPUNIT_ASSERT( pChain->vEv==vEv );
    }
}
