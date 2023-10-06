#include "stdafx.h"
#include <boost/filesystem.hpp>

#include "../helpful/Attic.h"
#include "../helpful/StrToTime.h"
#include "../helpful/Log.h"
#include "../helpful/FilesByMask.h"
#include "../Hem/Filer.h"
#include "TC_Canopy_Filer.h"

namespace bfs = boost::filesystem;

static boost::system::error_code ec;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_CanopyFiler );

static time_t M( const std::string& s )
{
    time_t moment = StrDataTime_To_UTC( s, 0 );
    CPPUNIT_ASSERT( 0!=moment );
    return moment;
}
static interval_time_t M2( const std::string& s1, const std::string& s2 )
{
    return interval_time_t( M(s1), M(s2) );
}


void TC_CanopyFiler::setUp()
{
    fold = (boost::filesystem::temp_directory_path() / L"x").wstring();
    fold.pop_back(); // на конце строки сохранился разделитель
    m_old_FilerTimeout = NsCanopy::FilerBase::lock_timeout();
    NsCanopy::FilerBase::lock_timeout( boost::posix_time::seconds( 2 ) );
    NsCanopy::Filer temp( fold, 100 );
    unlink_all_files( temp.filemask() );
}

void TC_CanopyFiler::tearDown()
{
    unlink_all_files( NsCanopy::Filer( fold, 100 ).filemask() );
    NsCanopy::FilerBase::lock_timeout( m_old_FilerTimeout );
}

void TC_CanopyFiler::test_write_split()
{
    attic::a_document doc;
    doc.load_utf8("<HEM some='111' more='111' />");
    NsCanopy::Filer filer( fold, 30 );
    CPPUNIT_ASSERT( 0 == filer.last_known() );
    filer.initCurrentShift( M("2014-08-16 14:25") );
    // никаких смен нет
    CPPUNIT_ASSERT( filer.count()==0 );
    // несколько раз пишем одну и ту же дневную смену
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 14:25"), doc ) );
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 17:59:59"), doc ) );
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 17:59"), doc ) ); // нарушение порядка внутри смены разрешено
    CPPUNIT_ASSERT( filer.count()==1 );
    // пишем следующую (подряд) смену
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 19:00"), doc ) );
    CPPUNIT_ASSERT( filer.count()==2 );
    CPPUNIT_ASSERT( filer.write( M("2014-08-17 03:00"), doc ) );
    CPPUNIT_ASSERT( filer.count()==2 );
    // перескакиваем несколько смен и пишем
    CPPUNIT_ASSERT( filer.write( M("2014-08-22 19:00"), doc ) );
    CPPUNIT_ASSERT( filer.count()==3 );
}

void TC_CanopyFiler::test_write_overwrite()
{
    attic::a_document doc;
    doc.load_utf8("<HEM some='111' more='111' />");
    {
        NsCanopy::Filer filer( fold, 30 );
        CPPUNIT_ASSERT( 0 == filer.last_known() );
        filer.initCurrentShift( M("2014-08-16 14:25") );
        // пишем две смены подряд
        CPPUNIT_ASSERT( filer.write( M("2014-08-16 14:25"), doc ) );
        CPPUNIT_ASSERT( filer.write( M("2014-08-16 20:00"), doc ) );
        // перезапись НЕпоследней смены запрещена
        CPPUNIT_ASSERT( !filer.write( M("2014-08-16 14:26"), doc ) );
    }
    // проверка, что другой экземпляр перезапишет только последнюю смену
    {
        NsCanopy::Filer filer2( fold, 30 );
        CPPUNIT_ASSERT( 0 != filer2.last_known() );
        filer2.initCurrentShift( filer2.last_known() );
        // перезапись НЕ последней смены запрещена
        CPPUNIT_ASSERT( !filer2.write( M("2014-08-16 15:03"), doc ) );
        // пишем следующую ночную смену
        CPPUNIT_ASSERT( filer2.write( M("2014-08-16 20:04"), doc ) );
    }
}

void TC_CanopyFiler::test_write_wipeold()
{
    attic::a_document doc;
    doc.load_utf8("<HEM some='111' more='111' />");
    unsigned HOLDED = 5;
    {
        NsCanopy::Filer filer( fold, HOLDED );
        CPPUNIT_ASSERT( 0 == filer.last_known() );
        filer.initCurrentShift( M("2014-08-11 14:01") );
        // пишем несколько смен последовательно
        CPPUNIT_ASSERT( filer.write( M("2014-08-11 14:01"), doc ) );
        CPPUNIT_ASSERT( filer.write( M("2014-08-11 23:02"), doc ) );
        CPPUNIT_ASSERT( filer.write( M("2014-08-12 14:01"), doc ) );
        CPPUNIT_ASSERT( filer.count()<=HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2014-08-12 23:02"), doc ) );
        CPPUNIT_ASSERT( filer.count()<=HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2014-08-13 14:01"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2014-08-13 23:02"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2014-08-14 14:02"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2014-08-14 23:02"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
    }
    {
        NsCanopy::Filer f2( fold, HOLDED );
        CPPUNIT_ASSERT( 0 != f2.last_known() );
        f2.initCurrentShift( f2.last_known() );
        CPPUNIT_ASSERT( f2.count()==HOLDED );
        // запрет перезаписи не удаляет смены
        CPPUNIT_ASSERT( !f2.write( M("2014-05-01 17:59"), doc ) );
        CPPUNIT_ASSERT( f2.count()==HOLDED );
    }
    {
        NsCanopy::Filer filer( fold, HOLDED );
        filer.initCurrentShift( filer.last_known() );
        // пишем несколько смен с пропусками в месяц
        CPPUNIT_ASSERT( filer.write( M("2015-01-01 01:01"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2015-02-02 02:02"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2015-03-03 03:03"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2015-04-04 04:04"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
        CPPUNIT_ASSERT( filer.write( M("2015-05-05 05:05"), doc ) );
        CPPUNIT_ASSERT( filer.count()==HOLDED );
    }
}

void TC_CanopyFiler::test_catalog()
{
    attic::a_document doc;
    doc.load_utf8("<HEM some='catalog' />");
    NsCanopy::Filer filer( fold, 22 );
    CPPUNIT_ASSERT( filer.count()==0 );
    filer.initCurrentShift( M("2014-08-16 11:11") );
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 14:01"), doc ) );
    CPPUNIT_ASSERT( filer.write( M("2014-08-16 22:02"), doc ) );
    CPPUNIT_ASSERT( filer.write( M("2014-11-27 13:03"), doc ) );
    CPPUNIT_ASSERT( filer.count()==3 );
    CPPUNIT_ASSERT( filer.first_known() == M("2014-08-16 06:00") );
    CPPUNIT_ASSERT( filer.last_known()  == M("2014-11-27 06:00") );
}

void TC_CanopyFiler::test_read()
{
    attic::a_document rd;
    attic::a_document wd;
    wd.load_utf8("<HEM some='catalog' />");
    NsCanopy::Filer filer( fold, 30 );
    filer.initCurrentShift( M("2014-01-07 10:00") );
    // чтение из пустого
    CPPUNIT_ASSERT( !filer.read( M2("2014-01-07 10:01", "2014-01-07 10:05"), rd ) );
    CPPUNIT_ASSERT( filer.get_error_message() != std::wstring() );
    // запись двух несмежных смен
    CPPUNIT_ASSERT( filer.write( M("2014-01-07 10:01"), wd ) );
    CPPUNIT_ASSERT( filer.write( M("2014-11-27 13:03"), wd ) );
    // чтение с ошибками
    CPPUNIT_ASSERT( !filer.read( M2("2014-01-05 12:05", "2014-01-05 12:25"), rd ) ); // до всего
    CPPUNIT_ASSERT( !filer.read( M2("2014-05-07 12:05", "2014-05-07 12:25"), rd ) ); // в отсутствующем интервале
    CPPUNIT_ASSERT( !filer.read( M2("2099-05-07 12:05", "2099-05-07 12:25"), rd ) ); // после всего
    CPPUNIT_ASSERT( !filer.read( M2("2014-01-07 11:11", "2014-01-07 09:09"), rd ) ); // ошибочные границы
    CPPUNIT_ASSERT( !filer.read( M2("2014-01-07 11:11", "2014-01-08 22:22"), rd ) ); // широкие границы
    CPPUNIT_ASSERT( !filer.read( M2("2014-01-07 10:01", "2014-01-07 22:22"), rd ) ); // смена по правой границе отсутствует

    // чтение без ошибок
    CPPUNIT_ASSERT( filer.read( M2("2014-01-07 05:05", "2014-01-07 07:25"), rd ) ); // смена по правой границе
    CPPUNIT_ASSERT( filer.read( M2("2014-11-27 15:50", "2014-11-27 17:55"), rd ) );
}

void TC_CanopyFiler::test_lock()
{
    NsCanopy::Filer filerR( fold, 5 );
    NsCanopy::Filer filerW( fold, 5 );
    const time_t mom = M("2014-11-27 13:03");
    interval_time_t m2( mom-1, mom+1 );

    // запишем и прочитаем документ
    attic::a_document wd("abc");
    attic::a_document rd;
    filerR.initCurrentShift( mom );
    filerW.initCurrentShift( mom );
    CPPUNIT_ASSERT( filerW.write( mom, wd ) );
    CPPUNIT_ASSERT( filerR.read( m2, rd ) );
    // поставим блокировку и прочитать не сможем
    {
        NsCanopy::FilerBase base( fold );
        NsCanopy::FilerLock lock( base.lockfile(), boost::posix_time::milliseconds( 1000 ) );

        CPPUNIT_ASSERT( !filerW.write( mom, wd ) );
        CPPUNIT_ASSERT( filerW.get_error_message() != std::wstring() );
        CPPUNIT_ASSERT( !filerR.read( m2, rd ) );
        CPPUNIT_ASSERT( filerR.get_error_message() != std::wstring() );
    }
    CPPUNIT_ASSERT( filerW.write( mom, wd ) );
    CPPUNIT_ASSERT( filerR.read( m2, rd ) );
}

void TC_CanopyFiler::test_error_write()
{
    time_t moment = M("2014-08-16 14:00");
    std::wstring barrier = fold + L"14081618.hef";
    attic::a_document doc;
    doc.load_utf8("<HEM some='111' more='222' />");

    // записать смену
    {
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( moment );
        CPPUNIT_ASSERT( filer.write( moment, doc ) );
        CPPUNIT_ASSERT( filer.write( moment, doc ) );
    }
    // создать помеху для следующей смены
    CPPUNIT_ASSERT( bfs::create_directory( barrier, ec ) && !ec );
    // записать следующую смену и обнаружить ошибку
    const time_t h12 = 12*3600;
    {
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( filer.last_known() );
        CPPUNIT_ASSERT( !filer.write( moment+h12, doc ) );
        CPPUNIT_ASSERT( filer.get_error_message() != std::wstring() );
    }
    // убрать помеху для следующей смены
    CPPUNIT_ASSERT( bfs::remove( barrier, ec ) && !ec );
    // повторно записать следующую смену успешно
    {
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( filer.last_known() );
        CPPUNIT_ASSERT( filer.write( moment+h12, doc ) );
    }
}

void TC_CanopyFiler::test_write_trash()
{
    time_t moment = M("2014-08-16 20:00");
    attic::a_document doc_good;
    doc_good.load_utf8("<HEM some='111' more='111' />");
    // пишем хороший файл
    {
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( moment );
        CPPUNIT_ASSERT( filer.write( moment, doc_good ) );
    }
    // пишем некорректный файл
    {
        attic::a_document doc_bad;
        doc_bad .load_utf8("<HEM some='111' more='111' />");
        doc_bad.document_element().append_child( "" ).set_value("unnamed child");
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( filer.last_known() );
        CPPUNIT_ASSERT( !filer.write( moment, doc_bad ) );
        CPPUNIT_ASSERT( filer.get_error_message() != std::wstring() );
    }
    // перечитываем и проверяем, что файл хороший сохранился
    {
        NsCanopy::Filer filer( fold, 30 );
        filer.initCurrentShift( filer.last_known() );
        attic::a_document rd;
        interval_time_t m2( moment-1, moment+1 );
        CPPUNIT_ASSERT( filer.read( m2, rd ) );
        CPPUNIT_ASSERT( identical( rd.document_element(), doc_good.document_element() ) );
    }
}

void TC_CanopyFiler::unlink_all_files( const std::wstring& mask )
{
    std::vector<std::wstring> files;
    auto pack = [&files]( const FilesByMask_Data& fd ){
        files.emplace_back( fd.fname.to_string() );
        return true;
    };
    FilesByMask( mask, pack );

    for( auto& f : files )
       CPPUNIT_ASSERT( bfs::remove_all( bfs::path(fold)/f, ec ) );
}
