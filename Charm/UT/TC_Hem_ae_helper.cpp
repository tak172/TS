#include "stdafx.h"

#include <cppunit/extensions/HelperMacros.h>
#include "TC_Hem_ae_helper.h"
#include "../Hem/Bill.h"

Hem::Bill createBill( const std::wstring& text )
{
    attic::a_document doc;
    bool succ = doc.load_wide( text );
    CPPUNIT_ASSERT( succ );
    Hem::Bill bill;
    doc.document_element() >> bill;
    return bill;
}

Hem::Unbill createUnbill(const std::wstring& text)
{
    attic::a_document doc;
    bool succ = doc.load_wide( text );
    CPPUNIT_ASSERT( succ );
    Hem::Unbill unbill;
    doc.document_element().first_child() >> unbill;
    return unbill;
}
