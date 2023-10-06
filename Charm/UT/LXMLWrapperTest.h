#pragma once

#include "../StatBuilder/LXMLWrapper.h"

class LXMLWrapperTest : public CLXMLWrapper
{
public:
	LXMLWrapperTest( CLogicDocument * ldoc ) : CLXMLWrapper( ldoc ){}

private:
	bool FileToXmlDoc(const wchar_t * /*file_name*/) override
	{
		initRoot( pdoc->child( "LOGIC_COLLECTION" ) );
		return true;
	}
	virtual bool VerifyEDS() const { return true; }
};