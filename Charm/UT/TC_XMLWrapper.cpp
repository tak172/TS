#include "stdafx.h"
#include "TC_XMLWrapper.h"
#include "LogicDocumentTest.h"
#include "../StatBuilder/LXMLWrapper.h"
#include "../StatBuilder/LogicView.h"
#include "../StatBuilder/LosesErsatz.h"
#include "../StatBuilder/LLoadResult.h"
#include "../StatBuilder/ObsoleteLoader.h"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_XMLWrapper );

using namespace std;


class MockL_XmlWrapper : public CLXMLWrapper
{
public:
    MockL_XmlWrapper( CLogicDocument * ldoc, bool eds_flag )
        : CLXMLWrapper( ldoc ), eds(eds_flag)
    {}
    attic::a_node actual_document_root() const
    {
        return pdoc->document_element();
    }

private:
    bool FileToXmlDoc(const wchar_t * /*file_name*/) override
    {
        pdoc->load_wide(L"<LOGIC_COLLECTION name='Станция' ESR_code='88888' />");
		initRoot( pdoc->child( "LOGIC_COLLECTION" ) );
        return true; 
    }
    virtual bool VerifyEDS() const override { return eds; }
    const bool eds;
};

void TC_XMLWrapper::setUp()
{
    TC_Logic::setUp_helper();
}

void TC_XMLWrapper::tearDown()
{
    TC_Logic::tearDown_helper();
}

void TC_XMLWrapper::SuccESD()
{
    test_for_EDS_is( true );
}

void TC_XMLWrapper::FailESD()
{
    test_for_EDS_is( false );
}

void TC_XMLWrapper::test_for_EDS_is( bool eds_val )
{
    LLoadResult LR( L"" );
    CLogicDocumentTest doc;
    MockL_XmlWrapper wrapper( &doc, eds_val );
    wrapper.Load( L"", LR );
    attic::a_node n = wrapper.actual_document_root();
    attic::a_node r = wrapper.getRoot();
	if ( eds_val )
		CPPUNIT_ASSERT( n == r );
	else
		CPPUNIT_ASSERT( !n ); //xml-документ не загружаем
	CPPUNIT_ASSERT( eds_val != LR.Unloadable() );
}
