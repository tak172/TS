#include "stdafx.h"

#include "TC_ExceptionTracer.h"
#include "../helpful/ExceptionTracer.h"
#include "../helpful/Dump.h"
#include "../helpful/DuePath.h"
#include "../helpful/ErrorHandler.h"
#include "../helpful/Pilgrim.h"
#include "../_version_.h"
#include <conio.h>
#include <signal.h>

CPPUNIT_TEST_SUITE_REGISTRATION( TC_ExceptionTracer );

void TC_ExceptionTracer::setUp()
{
    CProcessDump::SetDetailizedOnly( false );
    auto count_succ = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( count_succ.second );
}

void TC_ExceptionTracer::tearDown()
{
    CProcessDump::SetDetailizedOnly( false );
    auto count_succ = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( count_succ.second );
}

template<class T>
void fire( T t )
{
    try
    {
        throw t;
    }
    catch( const T& /*exc*/ )
    {
        // поймано именно брошенное исключение
    }
    catch( ... )
    {
        // поймано что-то другое
        CPPUNIT_FAIL("Exception class mismatch");
    }
}

void TC_ExceptionTracer::fix()
{
    ExceptionTracer tracer;
    size_t removedFiles;

    fire( std::out_of_range("test one") );
    // проверить, что записан хотя бы один дамп (через его удаление)
    std::tie( removedFiles, std::ignore ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( 0 < removedFiles ); 

    fire( std::invalid_argument("test two") );
    // проверить, что записан хотя бы один дамп (через его удаление)
    std::tie( removedFiles, std::ignore ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( 0 < removedFiles ); 
}

void TC_ExceptionTracer::fix_without_detailed()
{
    ExceptionTracer tracer;
    
    CProcessDump::SetDetailizedOnly( true );
    fire( std::out_of_range("test one") );
    // проверить, что дампов нет вообще
    size_t removedFiles;
    std::tie( removedFiles, std::ignore ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT_EQUAL( size_t(0), removedFiles ); 
}

void TC_ExceptionTracer::skip()
{
    ExceptionTracer tracer;

    size_t removedFiles;
    bool success;
 
    fire( boost::thread_interrupted() );
    
    // проверить, что дампов не появилось
    std::tie( removedFiles, success ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( success && 0 == removedFiles ); 
}

void base()
{
    throw std::out_of_range("nested exception");
}

void wrapp()
{
    try
    {
        base();
    }
    catch (const std::logic_error& /*le*/)
    {
        //OutputDebugString( (L"intermediate catch (std::logic_error) " + FromUtf8( le.what() ) + L"\n").c_str() );
        throw;
    }
    catch (...)
    {
        CPPUNIT_FAIL( "Intermediate type of exception mismatch!" ); 
        throw;
    }
}

void TC_ExceptionTracer::nesting()
{
    ExceptionTracer tracer;

    size_t removedFiles;
    bool success;

    try
    {
        wrapp();
    }
    catch (const std::out_of_range& /*oor*/ )
    {
    }
    catch (...)
    {
        CPPUNIT_FAIL( "Final type of exception mismatch!" ); 
    }

    // проверить, что записан хотя бы один дамп (через его удаление)
    std::tie( removedFiles, success ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( success && 0 < removedFiles ); 
}

void TC_ExceptionTracer::parallel()
{
    volatile bool flag = false;
    auto emitter = [ &flag ](){
        // Трассировку достаточно включить в одной нити (любой),
        // однако для теста специально включается много трассировок,
        // чтобы количество дампов росло квадратично
        ExceptionTracer tracer;

        while( !flag )
        {
            boost::this_thread::sleep( boost::posix_time::milliseconds(1) );
        }
        fire( std::runtime_error( "Some fail" ) );
    };

    const size_t COUNT = 15; // ошибка наблюдается даже в DEBUG уже при значении 7
    boost::thread threads[ COUNT ];

    for( int j = 0; j<COUNT; ++j )
    {
        auto& t = threads[j];
        t = boost::thread( emitter );
        ::SetThreadPriority( t.native_handle(), THREAD_PRIORITY_ABOVE_NORMAL );
        CPPUNIT_ASSERT( t.joinable() );
    }
    // флаг все нити увидят почти одновременно
    flag = true;
    // дать время для срабатывания всех нитей и записи дампа
    boost::this_thread::sleep( boost::posix_time::milliseconds(50) );

    for( auto& t : threads )
    {
        CPPUNIT_ASSERT( t.joinable() );
        t.join();
    }

    // проверить, что записаны все дампы (через его удаление)
    size_t removedFiles;
    bool success;
    std::tie( removedFiles, success ) = CProcessDump::RestrictSize( 0 );
    CPPUNIT_ASSERT( success ); 
    CPPUNIT_ASSERT( COUNT*2 < removedFiles ); 
}

static bool WaitFinalEvent( HANDLE finalEv )
{
    return WAIT_OBJECT_0 == WaitForSingleObject(finalEv, 1000 );
}

void TC_ExceptionTracer::manual_dump()
{
    ExceptionTracer tracer;

    std::wstring modVer = CProcessDump::ModeAndVersion();
    const unsigned pid = GetCurrentProcessId();

    CPPUNIT_NS::stdCOut() << "\n";
    CPPUNIT_NS::stdCOut() << "Started TC_ExceptionTracer::manual_dump\n";
    CPPUNIT_NS::stdCOut() << "PID=" << pid << "\n";
    CPPUNIT_NS::stdCOut() << "modeVersion '" << ToUtf8(modVer) << "'\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (M)ini (F)ullany e(X)ception or (Q)uit ...\n";
    for( bool more = true; more; /*none*/ )
    {
        switch( _getch() )
        {
        case 'm':
        case 'M':
            CPPUNIT_NS::stdCOut()  <<  "Request miniDump ";
            {
                CProcessDump sm( nullptr, false );
            }
            CPPUNIT_NS::stdCOut()  <<  "completed\n";
            break;
        case 'f':
        case 'F':
            CPPUNIT_NS::stdCOut()  <<  "Request fullDump\n";
            {
                CProcessDump sm( nullptr, true );
            }
            CPPUNIT_NS::stdCOut()  <<  "completed\n";
            break;
        case 'x':
        case 'X':
            CPPUNIT_NS::stdCOut()  <<  "Request exception\n";
            {
                try
                {
                    throw std::out_of_range("manual out_of_range");
                }
                catch(...)
                {}
            }
            CPPUNIT_NS::stdCOut()  <<  "completed\n";
            break;
        case 'q':
        case 'Q':
            CPPUNIT_NS::stdCOut()  <<  "Quit\n";
            more = false;
            break;
        default:
            break;
        }
    }
    CPPUNIT_NS::stdCOut()  <<  "Close all handle and exit ...";
}

void TC_ExceptionTracer::terminate_by_thread()
{
    auto funcU = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() << "funcU: started\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() << "funcU: call std::terminate\n";
                std::terminate();
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() << "funcU: finished\n";
    };
    auto funcH = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() <<  "funcH: started\n";
        ExceptionInstall_CurrentThread();
        CPPUNIT_NS::stdCOut() <<  "funcH: exception per thread set\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() <<  "funcH: call std::terminate\n";
                std::terminate();
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() <<  "funcH: finished\n";
    };

    CPPUNIT_NS::stdCOut() << "\n";
    CPPUNIT_NS::stdCOut() << "Started " __FUNCTION__ "\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (U) to unhandled terminate\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (H) to handled terminate\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (Q) to quit\n";

    bool u_term = false;
    bool h_term = false;
    bool q_flag = false;
    auto th_u = boost::thread( funcU, &u_term, &q_flag );
    auto th_h = boost::thread( funcH, &h_term, &q_flag );
    for( bool more = true; more; /*none*/ )
    {
        switch( _getch() )
        {
        case 'u':
        case 'U':
            CPPUNIT_NS::stdCOut()  <<  "Pressed U \n";
            u_term = true;
            break;
        case 'h':
        case 'H':
            CPPUNIT_NS::stdCOut()  <<  "Pressed H \n";
            h_term = true;
            break;
        case 'q':
        case 'Q':
            CPPUNIT_NS::stdCOut()  <<  "Quit\n";
            more = false;
            break;
        default:
            break;
        }
    }
    q_flag = true;
    CPPUNIT_ASSERT( th_u.joinable() );
    CPPUNIT_ASSERT( th_h.joinable() );
    th_u.join();
    th_h.join();

    CPPUNIT_NS::stdCOut()  <<  "Close all handle and exit ...";
}

void TC_ExceptionTracer::terminate_by_exit()
{
    auto funcU = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() << "funcU: started\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() << "funcU: call exit()\n";
                exit(12);
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() << "funcU: finished\n";
    };
    auto funcH = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() <<  "funcH: started\n";
        ExceptionInstall_CurrentThread();
        CPPUNIT_NS::stdCOut() <<  "funcH: exception per thread set\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() <<  "funcH: call exit()\n";
                exit(12);
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() <<  "funcH: finished\n";
    };

    CPPUNIT_NS::stdCOut() << "\n";
    CPPUNIT_NS::stdCOut() << "Started " __FUNCTION__ "\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (U) to unhandled exit\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (H) to handled exit\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (Q) to quit\n";

    bool u_term = false;
    bool h_term = false;
    bool q_flag = false;
    auto th_u = boost::thread( funcU, &u_term, &q_flag );
    auto th_h = boost::thread( funcH, &h_term, &q_flag );
    for( bool more = true; more; /*none*/ )
    {
        switch( _getch() )
        {
        case 'u':
        case 'U':
            CPPUNIT_NS::stdCOut()  <<  "Pressed U \n";
            u_term = true;
            break;
        case 'h':
        case 'H':
            CPPUNIT_NS::stdCOut()  <<  "Pressed H \n";
            h_term = true;
            break;
        case 'q':
        case 'Q':
            CPPUNIT_NS::stdCOut()  <<  "Quit\n";
            more = false;
            break;
        default:
            break;
        }
    }
    q_flag = true;
    CPPUNIT_ASSERT( th_u.joinable() );
    CPPUNIT_ASSERT( th_h.joinable() );
    th_u.join();
    th_h.join();

    CPPUNIT_NS::stdCOut()  <<  "Close all handle and exit ...";
}

void TC_ExceptionTracer::terminate_by_signal()
{
    auto funcU = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() << "funcU: started\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() << "funcU: call raise()\n";
                raise(SIGABRT);
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() << "funcU: finished\n";
    };
    auto funcH = []( bool* flag_term, bool* flag_quit ){
        CPPUNIT_NS::stdCOut() <<  "funcH: started\n";
        ExceptionInstall_CurrentThread();
        CPPUNIT_NS::stdCOut() <<  "funcH: exception per thread set\n";
        while( !*flag_quit ) {
            if ( *flag_term )
            {
                CPPUNIT_NS::stdCOut() <<  "funcH: call exit()\n";
                raise(SIGABRT);
            }
            else
            {
                boost::this_thread::sleep( boost::posix_time::milliseconds(100) );
            }
        }
        CPPUNIT_NS::stdCOut() <<  "funcH: finished\n";
    };

    CPPUNIT_NS::stdCOut() << "\n";
    CPPUNIT_NS::stdCOut() << "Started " __FUNCTION__ "\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (U) to unhandled exit\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (H) to handled exit\n";
    CPPUNIT_NS::stdCOut()  <<  "Press (Q) to quit\n";

    bool u_term = false;
    bool h_term = false;
    bool q_flag = false;
    auto th_u = boost::thread( funcU, &u_term, &q_flag );
    auto th_h = boost::thread( funcH, &h_term, &q_flag );
    for( bool more = true; more; /*none*/ )
    {
        switch( _getch() )
        {
        case 'u':
        case 'U':
            CPPUNIT_NS::stdCOut()  <<  "Pressed U \n";
            u_term = true;
            break;
        case 'h':
        case 'H':
            CPPUNIT_NS::stdCOut()  <<  "Pressed H \n";
            h_term = true;
            break;
        case 'q':
        case 'Q':
            CPPUNIT_NS::stdCOut()  <<  "Quit\n";
            more = false;
            break;
        default:
            break;
        }
    }
    q_flag = true;
    CPPUNIT_ASSERT( th_u.joinable() );
    CPPUNIT_ASSERT( th_h.joinable() );
    th_u.join();
    th_h.join();

    CPPUNIT_NS::stdCOut()  <<  "Close all handle and exit ...";
}
