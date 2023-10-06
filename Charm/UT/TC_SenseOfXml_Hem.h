#ifndef TC_SENSEOFXML_HEM_H
#define TC_SENSEOFXML_HEM_H

#include <cppunit/extensions/HelperMacros.h>
#include "UtTearDown.h"

//Тестирование XML-команд для Hem

class TC_SenseOfXml_Hem : public UtTearDowner
{
    CPPUNIT_TEST_SUITE( TC_SenseOfXml_Hem );
    CPPUNIT_TEST( check_GetAll );
    CPPUNIT_TEST( check_PostInitialInfo );
    CPPUNIT_TEST_SUITE_END();

protected:
    void check_GetAll();
    void check_PostInitialInfo();
private:
    void makeFiles( const std::wstring& WHERE,
        std::wstring& topologyFilePath,
        std::wstring& topologyHelperFilePath,
        std::wstring& topologyBlacklistFilePath,
        std::wstring& regulatoryScheduleFilePath
        );
};

#endif // TC_SENSEOFXML_HEM_H