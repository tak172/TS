#include "stdafx.h"

#include "TC_DigitalSigning.h"
#include "../helpful/DigitalSigner.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_DigitalSign );

void TC_DigitalSign::verify_xml()
{
    const wchar_t * ff =
        L"<?xml version='1.0' encoding='UTF-8' standalone='yes'?>"
        L"<!--Файл описания логики-->"
        L"<LOGIC_COLLECTION name='ДНЦ-17' ESR_code='00205' transroutes='' hintlaunched='Y'>"
        L"<STRIPS />"
        L"<SWITCHES />"
        L"<FIGURES>"
        L"<element name='%PFRv2' type='3' />"
        L"</FIGURES>"
        L"<ROUTES />"
        L"<JOINTS />"
        L"<SEMALEGS />"
        L"<BOARDS />"
        L"<AUTOBLOCKS />"
        L"<COMMUTATORS />"
        L"<LANDSCAPES />"
        L"<COMMANDS />"
        L"<DigitalSignature>A1D2EBD8E5F39A56F910AFB13E64E19C0E4F1262CE97CE417D992A639410ECCFFCDD40B3BD6F1AC6D5C25DDEEE1569FF70FD5716294C309A0E337EDFB3F9F1FBBD52C77E4704D3FCE99122DDFC5B7533E5B0C987493BFEAB1512952B3460AC36CB7655D8AF24D9E2730D7EE62681204ACEF6FDA25D6CB80DB8AECE05862536A7736C203636FFFDCD71304B8783D49FF43492D5B6B55D9CD03A308606D39E71E6FFC38A8B70CD62E080673AD20047C1A62045492732F488CEC0E8B6DD4F25971F</DigitalSignature>"
        L"</LOGIC_COLLECTION>"
        ;
    attic::a_document doc;
    doc.load_wide( ff );

    CDigitalSigner sign_manager;

    // проверка корректности подписи исходного документа
    CPPUNIT_ASSERT( sign_manager.VerifyXML( doc.child("LOGIC_COLLECTION") ) );
    // изменить документ
    doc.child("LOGIC_COLLECTION").child("SWITCHES").brief_attribute("some","mistake");
    // проверка НЕкорректности подписи
    CPPUNIT_ASSERT( !sign_manager.VerifyXML( doc.child("LOGIC_COLLECTION") ) );  
}
