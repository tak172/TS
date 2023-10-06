#include "stdafx.h"
#include "XmlEqual.h"
#include "../helpful/Attic.h"

using namespace std;

bool xmlEqual( boost::string_ref str1, boost::string_ref str2 )
{
    bool res = false;
	attic::a_document xdoc1, xdoc2;
	if ( xdoc1.load_utf8( str1 ) && xdoc2.load_utf8( str2 ) )
    {
        auto xstr1 = xdoc1.to_str();
        auto xstr2 = xdoc2.to_str();
        res = xstr1 == xstr2;
		//auto mis = mismatch(xstr1.begin(), xstr1.end(), xstr2.begin());
    }
    return res;
}