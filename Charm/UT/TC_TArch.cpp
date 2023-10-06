/*!
	created:	2018/04/26
	created: 	15:59  26 Апрель 2018
	filename: 	F:\potap\Charm\UT\TC_TArch.cpp
	file path:	F:\potap\Charm\UT
	file base:	TC_TArch
	file ext:	cpp
	author:		 Dremin
	
	purpose:	тест архивации
  
  */
 
 #include "stdafx.h"
 #include "TC_Thread.h"
#include "../Augur/AArchive.h"
#include "../helpful/Attic.h"
#include "../Augur/AClash.h"
#include "TestLogWrn.h"

static const wchar_t* outFl= L"out.7z";

#ifdef _WIN64
#define PATH_DLL  L"..\\AdoptTools\\7z\\win64\\7z.dll"
#define PATH_EXE  L"..\\AdoptTools\\7z\\win64\\7z.exe"
#else
#define PATH_DLL  L"..\\AdoptTools\\7z\\win32\\7z.dll"
#define PATH_EXE  L"..\\AdoptTools\\7z\\win32\\7z.exe"
#endif

void TC_AThread::testArch()
{// тест архивации
	wchar_t  path [ _MAX_PATH ];
	wchar_t  drive [ _MAX_DRIVE ];
	wchar_t  dir [ _MAX_DIR ];
	wchar_t  file [ _MAX_FNAME/*+30*/ ];
	wchar_t  ext [ _MAX_EXT ];
	GetModuleFileNameW(	NULL,  path,sizeof( path )-1);
	_wsplitpath_s( path, drive,_MAX_DRIVE,dir,_MAX_DIR,file,_MAX_FNAME,ext,_MAX_EXT);
	_wmakepath_s( path, drive, dir, NULL, NULL );

	const wchar_t* tbl[]={L"tst1.tmp",L"tst2.tmp",L"tst3.tmp" };
	VWSTR nmF;
	int ind=1;
	for (const auto& t: tbl)
	{
		std::wostringstream s;
		s<<path<<t;
		nmF.push_back(s.str());
	}
	int i=1;
	for (const auto& t: nmF)
	{
		std::wofstream fl(t.c_str(),std::ios_base::trunc);
		if(!fl)
			break;
		fl<<L" Name = "<< t.c_str()<<std::endl<<L" IND ="<<i <<std::endl;
		i++;
	}
	std::wstring p(path);
	std::wstring pathArch = p+L"TestArch\\" ;
	nmF.insert(nmF.begin(),pathArch+outFl );
	{
		VWSTR fls=AArchive::getFiles(pathArch,L"*");
		fls.push_back(p+L"7z.dll");
		fls.push_back(p+L"7z.exe");
		for (const auto& t: fls)
			::DeleteFileW(t.c_str());
		::RemoveDirectory(pathArch.c_str());
		fls=AArchive::getFiles(pathArch,L"*");
		CPPUNIT_ASSERT(fls.empty() && "Очистка поддиректории");
	}
	// 7z.dll
	{

		{
#define BUFSIZE 4096
			wchar_t  buffer[BUFSIZE]=L""; 
			wchar_t   buf[BUFSIZE]=L""; 
			wchar_t ** lppPart={NULL};
			if(::GetFileAttributesW(TestLogWrn::archName.c_str()) != INVALID_FILE_ATTRIBUTES &&
				::GetFullPathNameW(TestLogWrn::archName.c_str(),BUFSIZE,buffer,lppPart))
			{
				::DeleteFileW(TestLogWrn::archName.c_str());
				CPPUNIT_ASSERT(::GetFileAttributesW(TestLogWrn::archName.c_str()) == INVALID_FILE_ATTRIBUTES && " Не удалить  7z.dll");
			}
		}

		attic::a_document configDoc;
		std::wstringstream s;
		s<<L"<Augur> <Archive  path=\""<<pathArch<<L"\" name=\"arch\" depth=\"3\"  arch=\"7z.dll\"/> </Augur>";
		auto r=configDoc.load_wide(s.str());
		CPPUNIT_ASSERT ( configDoc.document_element().name_is("Augur"));
		attic::a_node root=configDoc.child("Augur");
		{
			auto error=AArchive::parseConf(root);
			AArchive arch;
			auto logNm=std::wstring(pathArch)+L"log.txt";
			CPPUNIT_ASSERT(arch.nmFileExtW(AArchive::LOG)==logNm);
			CPPUNIT_ASSERT(!error.empty()  && error.find("Отсутствует файл архиватора")!=std::string::npos);
		}
		auto n=pathArch.rfind(L"_Out_");
		CPPUNIT_ASSERT(n!=std::string::npos);
		auto dllNm=pathArch.substr(0,n)+ PATH_DLL; 
		p+=L"7z.dll";
		::CopyFileW(dllNm.c_str(),p.c_str(),TRUE );
		{
			auto error=AArchive::parseConf(root);
			CPPUNIT_ASSERT(error.empty() );
			AArchive arch;
			auto err= arch.archive(nmF);
			CPPUNIT_ASSERT(err.empty() && "Архивирование через dll прошло.");
			VWSTR fls=AArchive::getFiles(pathArch, L"out.7z");
			CPPUNIT_ASSERT(fls.size()==1 && "Создан архив");
		}
	}
	// 7z.exe
	{
		p=path;
		{
			VWSTR fls=AArchive::getFiles(pathArch,L"*");
			for (const auto& t: fls)
				::DeleteFileW(t.c_str());
		}
		attic::a_document configDoc;
		std::wstringstream s;
		s<<L"<Augur> <Archive  path=\""<<pathArch<<L"\" name=\"arch\" depth=\"3\"  arch=\"7z.exe\"/> </Augur>";
		auto r=configDoc.load_wide(s.str());
		CPPUNIT_ASSERT ( configDoc.document_element().name_is("Augur"));
		attic::a_node root=configDoc.child("Augur");
		{
			auto error=AArchive::parseConf(root);
			AArchive arch;
			auto logNm=std::wstring(pathArch)+L"log.txt";
			CPPUNIT_ASSERT(arch.nmFileExtW(AArchive::LOG)==logNm);
			CPPUNIT_ASSERT(!error.empty()  && error.find("Отсутствует файл архиватора")!=std::string::npos);
		}
		auto n=pathArch.rfind(L"_Out_");
		CPPUNIT_ASSERT(n!=std::string::npos);
		auto dllNm=pathArch.substr(0,n)+PATH_EXE;
		p+=L"7z.exe";
		::CopyFileW(dllNm.c_str(),p.c_str(),TRUE );
		{
			auto error=AArchive::parseConf(root);
			if(!error.empty())
				std::cout<<error;
			CPPUNIT_ASSERT(error.empty() );
			AArchive arch;
			auto err= arch.archive(nmF);
			CPPUNIT_ASSERT(err.empty() && "Архивирование через 7z.exe прошло.");
			VWSTR fls=AArchive::getFiles(pathArch, L"out.7z");
			CPPUNIT_ASSERT(fls.size()==1 && "Создан архив");
		}
	}
}

static const wchar_t* inf=L"<?xml version=\"1.0\"?>"
	L"<root>"
	L"<Current number=\"29\" />"
	L"<Exception>"
	L"<hash3749614101 exception=\" нет ПВЧ no PVX 16170:16180\">"
	L"<Archive_00002_2.7z />"
	L"<Archive_00004_4.7z />"
	L"</hash3749614101>"
	L"<hash2203438887 exception=\"no PVX 07050:07790\">"
	L"<Archive_00001_1.7z />"
	L"</hash2203438887>"
	L"<hash3779036550 exception=\"no PVX 16000:16130\">"
	L"<Archive_00003_3.7z />"
	L"<Archive_00012_12.7z />"
	L"</hash3779036550>"
	L"<hash1456629208 exception=\" no route 11301 - 10 .\">"
	L"<Archive_00004_4.7z />"
	L"</hash1456629208>"
	L"</Exception>"
	L"<Revision time=\"2010:01:10 02:25:55\" />"
	L"</root>";

void TC_AThread::testAuditArch()
{// тест ревизии архивов
 	std::wstring pTMP, pRoot;
 	{// получаем рабочую директорию
 		wchar_t  path [ _MAX_PATH ];
 		wchar_t  drive [ _MAX_DRIVE ];
 		wchar_t  dir [ _MAX_DIR ];
 		wchar_t  file [ _MAX_FNAME/*+30*/ ];
 		wchar_t  ext [ _MAX_EXT ];
 		GetModuleFileNameW(	NULL,  path,sizeof( path )-1);
 		_wsplitpath_s( path, drive,_MAX_DRIVE,dir,_MAX_DIR,file,_MAX_FNAME,ext,_MAX_EXT);
 		_wmakepath_s( path, drive, dir, NULL, NULL );
 		pRoot=path;
		pTMP=pRoot+L"ArcTMP\\";
 	}
 	std::wstring pathArch=pTMP+ARC_USER_DIR,flIn(L"response.txt"),flOut(L"request.txt");
	auto dllR=pRoot+L"7z.dll";
	{	// Очистка поддиректории
		auto v=AArchive::getFiles(pathArch,L"*");
		v.push_back(dllR);
		auto v1=AArchive::getFiles(pTMP,L"*");
		v.insert(v.end(),v1.begin(),v1.end());
		for (const auto& t: v)
			auto r=::DeleteFileW(t.c_str());
		::RemoveDirectory(pathArch.c_str());
		::RemoveDirectory(pTMP.c_str());
	}

	::CreateDirectory(pTMP.c_str(),NULL);
	::CreateDirectory(pathArch.c_str(),NULL);

 	// строим файлы
 	VWSTR nmF;
 	{
		const wchar_t* tbl1[]={L"arch.inf",L"log.txt"};
 		for (const auto& t: tbl1)
			nmF.push_back(pTMP+t);
		flIn=pTMP+flIn;
		flOut=pTMP+flOut;
		nmF.push_back(flOut);
		nmF.push_back(flIn);
 		const wchar_t* tbl[]={L"Archive_00001_1.7z",L"Archive_00002_2.7z",L"Archive_00003_3.7z",L"Archive_00004_4.7z",L"Archive_00012_12.7z" };
 		for (const auto& t: tbl)
 			nmF.push_back(pathArch+t);
 	}
 	int i=0;
 	for (const auto& t: nmF)
 	{
 		std::wofstream fl(t.c_str(),std::ios_base::trunc);
 		if(!fl)
 			break;
		fl.imbue(std::locale("Russian"));
		if(i)
 			fl<<L" Name = "<< t.c_str()<<std::endl<<L" IND ="<<i <<std::endl;
		else
 			fl<<inf<<std::endl;//arch.inf
		i++;
 	}
 	// перенос 7z.dll
 	{
 		auto n=pathArch.rfind(L"_Out_");
 		CPPUNIT_ASSERT(n!=std::string::npos);
 		auto dllNm=pathArch.substr(0,n)+ PATH_DLL; 
 		::CopyFileW(dllNm.c_str(),dllR.c_str(),TRUE );
	}
 	
	TestLogWrn::sz=3;// макс. число архивов  в логе исключений
	TestLogWrn::pathArch=pTMP; // директория для логирования
	TestLogWrn::pathOut= ALogWrn::convWstring(pTMP);
	TestLogWrn::archName=dllR;// архиватор
	{
		TestLogWrn log;
		auto err=log.init();
		CPPUNIT_ASSERT(err.empty());
		AVectClash clash;
		clash.add(P_CLSH(new AClash(L"Проверка исключений.",PrognosisErrorCode::UNIQUE)));
		clash.add(P_CLSH(new AClash(L"Вторая проверка исключений.",PrognosisErrorCode::UNIQUE)));
		err=log.archivate(flIn, flOut, clash);
		CPPUNIT_ASSERT(err.empty());
	
	}
	attic::a_document docInf;
	auto nmInfo= pTMP+L"arch.inf";
	docInf.load_file(nmInfo);
	attic::a_node ndRoot=docInf.document_element();
	attic::a_node ndExc=ndRoot.child(EXCEPT_NODE);


	std::set<std::string> dV;
	for(auto nEx= ndExc.first_child();nEx;nEx= nEx.next_sibling())
		for(auto n= nEx.first_child();n;n= n.next_sibling())
			dV.insert(n.name());
	CPPUNIT_ASSERT(dV.size()==3);
	for (const auto& t: dV)
	{
		auto nmFl=ALogWrn::convWstring(pathArch)+t;
		CPPUNIT_ASSERT(::GetFileAttributesA(nmFl.c_str())!=INVALID_FILE_ATTRIBUTES);
	}
// Очистка поддиректории
 	{
		TestLogWrn::lib.Free();// dll архиватора освобождаем 7z.dll
 		std::set<std::wstring> fls;
		auto v=AArchive::getFiles(pathArch,L"*");
		auto v1=AArchive::getFiles(pTMP,L"*");
		v.insert(v.end(),v1.begin(),v1.end());
		fls.insert(v.begin(),v.end());
 		fls.insert(dllR);

 		for (const auto& t: fls)
		{
			auto r=::DeleteFileW(t.c_str());
			CPPUNIT_ASSERT(r && t.length());
		}
 		::RemoveDirectory(pathArch.c_str());
		::RemoveDirectory(pTMP.c_str());
 		v=AArchive::getFiles(pTMP,L"*");
 		CPPUNIT_ASSERT(v.empty() && "Очистка поддиректории");
 	}
	TestLogWrn::pathArch.clear(); // директория для логирования
	TestLogWrn::pathOut.clear();
	TestLogWrn::archName=L"7z.dll";// архиватор
}