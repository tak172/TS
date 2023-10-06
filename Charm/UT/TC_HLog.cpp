#include "stdafx.h"
#include <io.h>
#include <boost/filesystem.hpp>
#include <boost/date_time.hpp>
#include "../helpful/LogStream.h"
#include "../helpful/FilesByMask.h"
#include "../Hem/HLog.h"
#include "TC_HLog.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_HLog );

using namespace std;

static wstring  moment = L"20150817T112233Z";

void TC_HLog::setUp()
{
    boost::filesystem::path p = boost::filesystem::temp_directory_path();
    wstring mask = (p/(moment+L"*.*")).c_str();
    auto remover = [p]( const FilesByMask_Data& res ){
        DeleteFile( (p/res.fname.to_string()).c_str() );
        return true;
    };
    FilesByMask( mask, remover );
}

void TC_HLog::test()
{
    using namespace boost::posix_time;
    boost::filesystem::path p = boost::filesystem::temp_directory_path();
    wstring param_log_folder = (p/L"x").wstring();
    param_log_folder.pop_back(); // уже с разделителем на конце строки

    time_t t = (from_iso_string(ToUtf8(moment)) - ptime(boost::gregorian::date(1970, 1, 1))).total_seconds();
    wstring crh  = ( p/( moment + L"_h"  + HLogStream::extensions().crush ) ).c_str();
    wstring crh2 = ( p/( moment + L"_h2" + HLogStream::extensions().crush ) ).c_str();
    // проверим создание креша (принудительно)
    {
        HLogStream file_streamer( t, param_log_folder );
        HLogStream::log()       << "normal";
        HLogStream::crush_log() << "exception";
        HLogStream::log()       << "normal";
    }
    CPPUNIT_ASSERT( _waccess( crh.c_str(), 0 ) == 0 );
    // проверим создание креша (выброс исключения)
    {
        HLogStream file_streamer( t, param_log_folder );
        try
        {
            HLogStream::log()       << "exception";
            throw std::wstring( L"выбросить исключение\n" );
        }
        catch (...)
        {
            file_streamer.SetCrush();
            hlog << "\nHEM EXCEPTION!\n";
        }
    }
    CPPUNIT_ASSERT( _waccess( crh2.c_str(), 0 ) == 0 );
}
