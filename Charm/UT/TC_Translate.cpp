#include "stdafx.h"
#include <string>
#include "TC_Translate.h"
#include "cppunit/TestAssert.h"
#include "../helpful/X_translate.h"
#include "boost/filesystem/operations.hpp"

CPPUNIT_TEST_SUITE_REGISTRATION( TC_Translate );

void TC_Translate::Translate()
{
	std::string lng = trx_language();
	TCHAR buf[MAX_PATH];
	GetCurrentDirectory(MAX_PATH, buf);
	trx_add_path(std::wstring(buf));
	trx_add_domain("Charm", true);
	trx_add_domain("Graphics");

	std::string test_lng = "RU";
	trx_select(test_lng);
	CPPUNIT_ASSERT(trx_language() == test_lng);
	trx_select(lng);
}