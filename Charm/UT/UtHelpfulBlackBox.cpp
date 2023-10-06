#include "stdafx.h"
#include <sstream>
#include <boost/filesystem.hpp>
#include "UtHelpfulBlackBox.h"
#include "../helpful/Log.h"

static std::ostringstream oss;

Bbx::Writer& operator<<(Bbx::Writer& bbx, const Sup& rec)
{
    attic::a_document doc;
    attic::a_node xroot = BBX_prepareRoot(doc, rec.getTime());
    attic::a_node xlos = xroot.ensure_child( BBX_LOS() );
    /*attic::a_node xtu =  */xroot.ensure_child( BBX_TU() );
    xlos.ensure_attribute(CONTENT_ATTR).set_value(CONTENT_SUP);
    std::string fill( size_t(rec.getTime()%20), 's' );
    xlos.ensure_attribute("fill").set_value(fill);
    xlos.brief_attribute("special", rec.getSpecial() );
    if ( ! bbx.pushReference(Bbx::Buffer(), doc.to_str(), rec.getTime(), Bbx::Identifier()) )
        CPPUNIT_FAIL("Write ref to bbx - error!");
    oss << " (" << (rec.getTime()%1000) << ")";
    return bbx;
}

Bbx::Writer& operator<<(Bbx::Writer& bbx, const Inc& rec)
{
    attic::a_document doc;
    attic::a_node xroot = BBX_prepareRoot( doc, rec.getTime() );
    attic::a_node xlos = xroot.ensure_child(BBX_LOS() );
    /*attic::a_node xtu = */xroot.ensure_child(BBX_TU() );
    xlos.ensure_attribute(CONTENT_ATTR).set_value(CONTENT_INC);
    std::string fill( size_t(rec.getTime()%20), 'i' );
    xlos.ensure_attribute("fill").set_value(fill);
    xlos.brief_attribute("special", rec.getSpecial() );
    std::string str = doc.to_str();
    if ( !bbx.pushIncrement(Bbx::Buffer(), str, str, rec.getTime(), Bbx::Identifier()) )
        CPPUNIT_FAIL("Write inc to bbx - error!");
    oss << " ." << (rec.getTime()%1000) << ".";
    return bbx;
}

Bbx::Writer& operator<<(Bbx::Writer& bbx, const PackageIn& rec)
{
    attic::a_document doc;
    attic::a_node xroot = BBX_prepareRoot( doc, rec.getTime() );
    attic::a_node xlos = xroot.ensure_child(BBX_LOS() );
   /* attic::a_node xtu = */xroot.ensure_child(BBX_TU() );
    xlos.ensure_attribute(CONTENT_ATTR).set_value(CONTENT_PACKAGE_IN);
    std::string fill( size_t(rec.getTime()%20), 'i' );
    xlos.ensure_attribute("fill").set_value(fill);
    xlos.brief_attribute("special", rec.getSpecial() );
    std::string str = doc.to_str();
    if ( !bbx.pushIncomingPackage(Bbx::Buffer(), str, rec.getTime(), Bbx::Identifier()) )
        CPPUNIT_FAIL("Write incoming package to bbx - error!");
    oss << " ." << (rec.getTime()%1000) << ".";
    return bbx;
}

Bbx::Writer& operator<<(Bbx::Writer& bbx, const PackageOut& rec)
{
    attic::a_document doc;
    attic::a_node xroot = BBX_prepareRoot( doc, rec.getTime() );
    attic::a_node xlos = xroot.ensure_child(BBX_LOS() );
    /*attic::a_node xtu = */xroot.ensure_child(BBX_TU() );
    xlos.ensure_attribute(CONTENT_ATTR).set_value(CONTENT_PACKAGE_OUT);
    std::string fill( size_t(rec.getTime()%20), 'i' );
    xlos.ensure_attribute("fill").set_value(fill);
    xlos.brief_attribute("special", rec.getSpecial() );
    std::string str = doc.to_str();
    if ( !bbx.pushOutboxPackage(Bbx::Buffer(), str, rec.getTime(), Bbx::Identifier()) )
        CPPUNIT_FAIL("Write outbox package to bbx - error!");
    oss << " ." << (rec.getTime()%1000) << ".";
    return bbx;
}

void DeleteBlackBox(const Bbx::Location& bbxLocation)
{
    TC_Bbx::DeleteBlackBoxFiles(bbxLocation);
}

static time_t generate_fix_moment()
{
    // начальное стандартное время выбрано произвольно
    struct tm work;
    work.tm_sec = 5;     /* seconds after the minute - [0,59] */
    work.tm_min = 4;     /* minutes after the hour - [0,59] */
    work.tm_hour = 3;    /* hours since midnight - [0,23] */
    work.tm_mday = 2;    /* day of the month - [1,31] */
    work.tm_mon = 1;     /* months since January - [0,11] */
    work.tm_year = 2011 - 1900;   /* years since 1900 */
    work.tm_wday = 0;    /* days since Sunday - [0,6] */
    work.tm_yday = 0;    /* days since January 1 - [0,365] */
    work.tm_isdst = 0;   /* daylight savings time flag */
    time_t fx = mktime( &work );
    fx -= fx % 1000000;
    return fx;
}

time_t PrepareBlackBox(Bbx::Location& bbxLocation, std::tuple<unsigned, unsigned, unsigned, unsigned> *count)
{
    // Заполнить Создать имя временного файла
    bbxLocation = Bbx::Location( boost::filesystem::temp_directory_path().wstring(), L"Arch", L".bbx" );
    DeleteBlackBox(bbxLocation);

    time_t fix_moment = generate_fix_moment();
    // Создадим ящик [ALLBEG..ALLEND] с лакуной [LACUNA_BEG..LACUNA_END_-1]
    oss.str( std::string() );
    {
        auto bbxWriter = Bbx::Writer::create(bbxLocation);
        *bbxWriter << Sup(fix_moment + c_BbxBegin) 
            << Inc(fix_moment + c_BbxBegin + 1)
            << Inc(fix_moment + c_BbxBegin + 1)
            << PackageIn(fix_moment + c_BbxBegin + 2)
            << Inc(fix_moment + c_BbxBegin + 2)
            << Inc(fix_moment + c_BbxBegin + 4)
            << PackageOut(fix_moment + c_BbxBegin + 5)
            << Sup(fix_moment + c_LacunaAft);

        bbxWriter->flush();

        if (nullptr != count)
        {
            *count = bbxWriter->getWrittenMessagesCounts();
        }
    }
    oss << " лакуна ";
    DebugLog( L"PrepareBlackBox %s", FromUtf8(oss.str()) );
    oss.str( std::string() );
   {
        auto bw = Bbx::Writer::create(bbxLocation);
        *bw << Sup(fix_moment+c_LacunaEnd)
            << Sup(fix_moment+411) // время замерло 411
            << Inc(fix_moment+411) // время замерло
            << Sup(fix_moment+422) // время замерло 422
            << Inc(fix_moment+422) // время замерло
            << Sup(fix_moment+422) // время замерло
            << Inc(fix_moment+422) // время замерло
            << Sup(fix_moment+433) // время замерло 433
            << Inc(fix_moment+433) // время замерло
            << Sup(fix_moment+c_BbxEnd);

        bw->flush();

        if (nullptr != count)
        {
            auto add = bw->getWrittenMessagesCounts();
            std::get<0>(*count) += std::get<0>(add);
            std::get<1>(*count) += std::get<1>(add);
            std::get<2>(*count) += std::get<2>(add);
            std::get<3>(*count) += std::get<3>(add);
        }

       //TRACE("\n");
    }
    DebugLog( L"PrepareBlackBox %s", FromUtf8(oss.str()) );
    DebugLog( L"PrepareBlackBox -------- end" );
    return fix_moment;
}