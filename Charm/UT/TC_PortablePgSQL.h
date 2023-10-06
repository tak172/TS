#pragma once
#include <string>

class LocalPostgresInstance
{
public:
	static bool Start();
	static bool Stop();
	static const std::string GetStringConnect();
private:
	static const std::string connFormat();
	static const int port = 5020;
	static const std::wstring GetCurrentFolder();
	static const bool SystemExecute(const std::wstring& exe, const std::wstring& arg = L"", const std::wstring& lastLine = L"");
	static const bool ExistsFile(const std::wstring& exe);
	static const bool ExecuteWhile(const std::wstring& cmdLine, const std::wstring& exitString);
};
