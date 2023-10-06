#pragma once

#include "../StatBuilder/GXMLWrapper.h"

class GXMLWrapperTest : public CGXMLWrapper
{
public:
	GXMLWrapperTest( CGraphicDocument * gdoc ) : CGXMLWrapper( gdoc ){}
private:
	bool FileToXmlDoc(const wchar_t * /*file_name*/) override
	{
		initRoot( pdoc->child( "GRAPHIC_SCHEMATA" ) );
		return true;
	}
	virtual bool VerifyEDS() const { return true; }
};