#pragma once

#include "../StatBuilder/SXMLWrapper.h"

class SXMLWrapperTest : public CSXMLWrapper
{
public:
	SXMLWrapperTest( CSignalDocument * sdoc ) : CSXMLWrapper( sdoc ){}

private:
	bool FileToXmlDoc(const wchar_t * /*file_name*/) override
	{
		initRoot( pdoc->child( "AbcTable" ) );
		return true;
	}
	virtual bool VerifyEDS() const { return true; }
};