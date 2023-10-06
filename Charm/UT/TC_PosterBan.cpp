#include "stdafx.h"

#include "TC_PosterBan.h"
#include "../Fund/PosterBan.h"
#include "../helpful/EsrKit.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_PosterBan );

void TC_PosterBan::direct_and_group()
{
    PosterBan pb;
    pb.append(EsrKit(55111));
    pb.append(BadgeE(L"aaa",EsrKit(33033)));
    pb.append(BadgeE(L"bb", EsrKit(33033)));
    pb.append(BadgeE(L"kk", EsrKit(55111)));

    CPPUNIT_ASSERT( pb.isBanned(  BadgeE(L"aaa",EsrKit(33033)) ));
    CPPUNIT_ASSERT( !pb.isBanned( BadgeE(L"aa", EsrKit(33033)) ));
    CPPUNIT_ASSERT( !pb.isBanned( BadgeE(L"aaa",EsrKit(43033)) ));
    CPPUNIT_ASSERT( !pb.isBanned( BadgeE(L"bb", EsrKit(43033)) ));
    CPPUNIT_ASSERT( pb.isBanned(  BadgeE(L"bb", EsrKit(33033)) ));

    CPPUNIT_ASSERT( pb.isBanned(  BadgeE(L"-z", EsrKit(55111)) ));
    CPPUNIT_ASSERT( pb.isBanned(  BadgeE(L"kk", EsrKit(55111)) ));
}
