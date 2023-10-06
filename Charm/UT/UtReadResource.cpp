#include "stdafx.h"
#include <cppunit/extensions/HelperMacros.h>

std::string UtReadResource( const std::wstring &resName, int resId )
{
#ifndef LINUX
	HMODULE hMod = GetModuleHandle(NULL);
	CPPUNIT_ASSERT(hMod && "Can't get module handle...");
	HRSRC hResource = FindResource(hMod, MAKEINTRESOURCE(resId), resName.c_str());
	CPPUNIT_ASSERT(hResource);
	HGLOBAL hBlock = LoadResource(hMod, hResource);
	CPPUNIT_ASSERT(hBlock && "Can't load resource");
	char* pData = reinterpret_cast<char*>(LockResource(hBlock));
	CPPUNIT_ASSERT(pData && "Can't lock resource");
	DWORD resourceSize = SizeofResource(hMod, hResource);
	std::string res(pData, resourceSize);
	return res;
#else
	return std::string();
#endif // !LINUX
}