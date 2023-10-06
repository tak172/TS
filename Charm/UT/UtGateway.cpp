#include "stdafx.h"
#include "UtGateway.h"
#include "../helpful/Dictum.h"
#include "../helpful/Attic.h"
#include "../Fund/F2A_BbxOption.h"

#include <cppunit/extensions/HelperMacros.h>


UtGateway::UtGateway( void )
: overstock(false)
{
}

UtGateway::~UtGateway( void )
{
}

bool UtGateway::read(const std::vector<char>& , const std::vector<char>& u8data, const F2A_BBX_OPTION& opt)
{
    boost::mutex::scoped_lock lock(mtx);
    std::string s(u8data.begin(), u8data.end());
    dao.push_back(make_pair(s,opt));
    return true;
}

std::wstring UtGateway::description() const
{
    return L"Unit test gateway";
}

void UtGateway::PopReadedMessages( UtGateway::vecDocAndOpt& vec )
{
	boost::mutex::scoped_lock lock(mtx);
	PopWithFlag(false,vec);
}

void UtGateway::PopSeekingResults( UtGateway::vecDocAndOpt& vec )
{   
	boost::mutex::scoped_lock lock(mtx);
	PopWithFlag(true,vec);
}

void UtGateway::PopAll()
{
	UtGateway::vecDocAndOpt tmp;
	PopReadedMessages(tmp);
	PopSeekingResults(tmp);
}

void UtGateway::PopWithFlag( bool fin_state, UtGateway::vecDocAndOpt& vec )
{
	for( vecDocAndOpt::iterator it=dao.begin(); it!=dao.end(); /*none*/ )
		if ( it->second.getFin() == fin_state || it->second.getEof() )
		{
			vec.push_back(*it);
			it = dao.erase(it);
		}
		else
			++it;
}
