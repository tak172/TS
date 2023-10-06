#include "stdafx.h"
#include <boost/filesystem/path.hpp>
#include <boost/filesystem/operations.hpp>
#include <boost/algorithm/string/predicate.hpp>
#include "TC_Pilgrim.h"
#include "../helpful/Pilgrim.h"
#include "../helpful/Log.h"

namespace bfs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Pilgrim );

static boost::system::error_code ec;

void TC_Pilgrim::setUp()
{
    bfs::path ep( Pilgrim::instance()->ExePath() );
    d1 = ep / "d1";
    d1d2 = ep / "d1" / "d2";
    if( bfs::exists(d1d2) )
        if ( !bfs::remove( d1d2, ec ) || ec )
            WriteLog( L"Can't delete own file '%s'", d1d2.c_str() );
    if( bfs::exists( d1 ) )
        if( !bfs::remove( d1, ec ) || ec )
            WriteLog( L"Can't delete own file '%s'", d1.c_str() );
    CPPUNIT_ASSERT( bfs::create_directories( d1d2, ec ) && !ec );
}

void TC_Pilgrim::tearDown()
{
    bfs::remove( d1d2, ec );
    bfs::remove( d1, ec );
    Pilgrim::Shutdowner();
}

void TC_Pilgrim::TestSoundPath()
{
    // относительный путь удлиняется до абсолютного
    Pilgrim::instance()->Set_SoundPath( L"d1\\bad\\../other\\../\\d2" );
    auto res = Pilgrim::instance()->AddSoundPath( L"$xxx.mp3" );
    std::wstring at_end = L"/d1/d2/$xxx.mp3";
    CPPUNIT_ASSERT( boost::algorithm::ends_with( res, at_end  ) );
    CPPUNIT_ASSERT( res.size() >= at_end.size()+2 );
}
