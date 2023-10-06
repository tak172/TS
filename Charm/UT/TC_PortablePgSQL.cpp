#include "stdafx.h"
#include "TC_PortablePgSQL.h"
#include <iostream>
#include "boost/format.hpp"
#include "boost/filesystem/path.hpp"
#include "boost/filesystem/operations.hpp"
#include "boost/algorithm/string/predicate.hpp"
#include "../helpful/Pilgrim.h"
#include "../helpful/WorkDirectorySaver.h"


const std::string LOCAL_IP = "127.0.0.1";
const int         LOCAL_PORT = 5020;

const std::string  CONNECT_FORMAT = "host="+ LOCAL_IP +" dbname=postgres port="+ std::to_string(LOCAL_PORT) +" user=postgres";
const std::wstring PORTABLE_START_BAT= L"ppgsql_start.bat";
const std::wstring PORTABLE_STOP_BAT = L"ppgsql_stop.bat";
const std::wstring SCHEME_FILE = L"schema\\CreateSchema.sql";

const std::string LocalPostgresInstance::GetStringConnect()
{
	return CONNECT_FORMAT;
}

bool LocalPostgresInstance::Start()
{
    WorkDirectorySaver ws;

    std::wstring current_folder = GetCurrentFolder();
    SetCurrentDirectory( current_folder.c_str() );
	std::wstring portable_pgsql = current_folder + PORTABLE_START_BAT;
	std::wstring arg = std::to_wstring(LOCAL_PORT) + L" \"" + current_folder + SCHEME_FILE + L"\"";
	return SystemExecute(portable_pgsql, arg, L"server working");
}

bool LocalPostgresInstance::Stop()
{
    WorkDirectorySaver ws;

    std::wstring current_folder = GetCurrentFolder();
    SetCurrentDirectory( current_folder.c_str() );
	std::wstring portable_pgsql = current_folder + PORTABLE_STOP_BAT;
	return SystemExecute(portable_pgsql, L"", L"server stop");
}

const std::wstring LocalPostgresInstance::GetCurrentFolder()
{
	return Pilgrim::instance()->ExePath();
}

const bool LocalPostgresInstance::SystemExecute(const std::wstring& exe, const std::wstring& arg /*= ""*/, const std::wstring& exitLine /*= ""*/)
{
	if (ExistsFile(exe)) 
	{
		std::wstring cmdLine = arg.empty() ? exe : exe + L" " + arg;
		return ExecuteWhile(cmdLine, exitLine);
	}
	else 
	{
		return false;
	}
}

const bool LocalPostgresInstance::ExistsFile(const std::wstring& file)
{
	bool result = false;
	if (!file.empty()) 
	{
		boost::filesystem::path fpath(file.c_str());
		result = boost::filesystem::exists(fpath);
	}
	return result;
}

const bool LocalPostgresInstance::ExecuteWhile(const std::wstring& cmdLine, const std::wstring& exitString)
{
	const int size = 4096;
	char   buffer[size];
	bool result = false;
	std::stringstream log;
	FILE* pipe = _wpopen((cmdLine).c_str(), L"rt");
	if (pipe != NULL) {
		while(fgets(buffer, size, pipe)) {
			if (!exitString.empty() && boost::starts_with(buffer, exitString)) {
				result = true;
				break;
			}
			log << buffer;
		}
        DWORD tick = GetTickCount()+10*1000;
        while( !feof(pipe) && GetTickCount()<tick )
            Sleep(100);
		_pclose(pipe);
	}
	if (!result) 
		std::cout << log.str();
	return result || exitString.empty();
}
