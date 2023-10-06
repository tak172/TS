#pragma once

#include "TC_Graphic.h"
#include "TC_Logic.h"
#include <cppunit/extensions/HelperMacros.h>

class CProjectDocumentTest;
class TC_Project : 
	public TC_Graphic, 
	public TC_Logic,
	public CPPUNIT_NS::TestFixture
{
public:
	void setUp() override;
    void tearDown() override;

protected:
	boost::scoped_ptr <CProjectDocumentTest> projdoc;
};