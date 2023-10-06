#include "stdafx.h"

#include <cstdio>
#include <cppunit/CompilerOutputter.h>
#include <cppunit/TestResult.h>
#include <cppunit/TestResultCollector.h>
#include <cppunit/TestRunner.h>
#include <cppunit/XmlOutputter.h>
#include <cppunit/extensions/TestFactoryRegistry.h>
#include <cppunit/extensions/HelperMacros.h>
#include <cppunit/TestFailure.h>
#include <cppunit/extensions/HelperMacros.h>
#include <stdexcept>
#include "RT_Progressor.h"
#include "../helpful/Log.h"
#include "../helpful/Utf8.h"
#include "../helpful/ErrorHandler.h"
#include "../helpful/WatchDog.h"
//#include "TrackerInfrastructure.h"
#include "../_version_.h"
#include "../helpful/RT_ThreadName.h"


static void WaitKey( WatchDog& watchDog )
{
    watchDog.off();
// #ifdef _DEBUG
//     CPPUNIT_NS::stdCOut()  <<  "Press any key ...";
//     getchar();
//     CPPUNIT_NS::stdCOut()  <<  "\n" << "Exiting ...";
// #endif
}

//#pragma runtime_checks( "", off )
class TestLogTune
{
public:
    TestLogTune(const std::wstring& exe_fold)
    {
        InterlockedLog::instance()->initialize_path( exe_fold, "UT", CHARM_VERSION_STR );
        InterlockedLog::instance()->limit_logs( 3 );
    }
    ~TestLogTune()
    {
		WriteLog(L"The end.");
        InterlockedLog::Shutdowner();
    }
};

int main( int argc, char* argv[] )
{
    ExceptionProcessHandler eph;
    auto alerter = []( std::wstring ) {
        CPPUNIT_FAIL( "-= UT wait timeout =-" );
    };
    RT_SetThreadName("UT_MainProccess");
    WatchDog watchDog(boost::posix_time::seconds(60),alerter);
    //_CrtSetBreakAlloc(14711);
//     setlocale( LC_ALL, "" );    // Sets the locale to the default, which is the user-default ANSI code page obtained from the operating system.
//     setlocale( LC_NUMERIC, "C" );
//    const std::wstring exe_folder = Pilgrim::instance()->ExePath();
//    TestLogTune tuneLog( exe_folder );
//    auto cnt_succ = CProcessDump::RestrictSize( 0 );
//    CPPUNIT_ASSERT_MESSAGE( "Can't delete dump file", cnt_succ.second );

    // Retreive test path from command line first argument.
    // Default to "" which resolve to the top level suite.
    std::vector <std::string> testP;
    for( int k = 1; k<argc; ++k )
    {
        std::string t = argv[k];
        if ( t=="*" )
        {
            testP.emplace_back( "" );
        }
        else if ( std::string::npos == t.find('*') )
            testP.push_back( t );
        else
        {
            auto asterisk = t.find('*');
            auto count = std::stoi( t );
            for( t = t.substr( asterisk+1 ); !t.empty() && count>0; --count )
            {
                testP.push_back( t );
            }
        }
    }
    if ( testP.empty() )
        testP.emplace_back("");
    bool all = ( testP.end() != find( testP.begin(), testP.end(), "" ) );

    // Create the event manager and test controller
    CPPUNIT_NS::TestResult controller;

    // Add a listener that colllects test result
    CPPUNIT_NS::TestResultCollector result;
    controller.addListener( &result );        

    // Add a listener that print dots as test run.
    RT_Progressor progress(3000,true,watchDog);
    controller.addListener( &progress );      

    // Add the top suite to the test runner
    CPPUNIT_NS::TestRunner runner;
    runner.addTest( CPPUNIT_NS::TestFactoryRegistry::getRegistry().makeTest() );   
    DWORD beg_tick = GetTickCount();
    try
    {
        for (const auto & test : testP)
        {
            CPPUNIT_NS::stdCOut() << "Running "  <<  std::string( test );
            runner.run( controller, test );
        }

        // Print test in a compiler compatible format.
        std::ostringstream myOut;
        CPPUNIT_NS::CompilerOutputter outputter( &result, myOut );
        outputter.write(); 
        CPPUNIT_NS::stdCOut() << myOut.str();
        //TRACE( "%s", myOut.str().c_str() );

        // Uncomment this for XML output
        std::ofstream file( /*exe_folder+*/"UT.xml" );
        CPPUNIT_NS::XmlOutputter xml( &result, file, "windows-1251" );
        xml.setStyleSheet( "report.xsl" );
        xml.write();
        file.close();
    }
    catch ( std::invalid_argument &e )  // Test path not resolved
    {
        CPPUNIT_NS::stdCOut()  <<  "\n"  
            <<  "ERROR: "  <<  e.what()
            << "\n";
        WaitKey(watchDog);
        return 1;
    }

    DWORD end_tick = GetTickCount();
    CPPUNIT_NS::stdCOut()  <<  "Elapsed time " << (end_tick-beg_tick) << " ms. \n";  

    //TrackerInfrastructure::Shutdowner();

    if ( !all )
    {
        WaitKey( watchDog );
        return result.testFailuresTotal()? 1:0;
    }
    else
    {
        // Требуемое количество ошибок. Желательно - ноль. 
        // Однако с момента добавления нового теста 
        // и     до момента его исправления НЕНУЛЕВОЕ число ошибок
        // позволяет контролировать успешность остальных тестов.  
        const int KNOWN_LINE=__LINE__ +1;
        const int KNOWN_FAILURESTOTAL = 0; // если не 0, то из-за какого теста?


        if ( KNOWN_FAILURESTOTAL==result.testFailuresTotal())
        {
            WaitKey( watchDog );
            return 0;
        }
        else
        {
            CPPUNIT_NS::stdCOut()  
                << "\n"  
                << "FAILURE: "  <<  __FILE__ << ":" << __LINE__ << "\n" 
                << "   Expected " << KNOWN_FAILURESTOTAL        << " error(s); please check file " << __FILE__ << ":" << KNOWN_LINE << "\n"
                << "   Detected " << result.testFailuresTotal() << " error(s)." << "\n"
                << "\n";
            WaitKey( watchDog );
            return 1;
        }
    }
}
//#pragma runtime_checks( "", restore )
