/*!
	created:	2018/07/12
	created: 	18:54  12 »юль 2018
	filename: 	F:\potapTrunk\Charm\UT\TestLogWrn.h
	file path:	F:\potapTrunk\Charm\UT
	file base:	TestLogWrn
	file ext:	h
	author:		 Dremin
	
	purpose:	архиваци€ дл€ тестов
 */
#ifndef TestLogWrn_h__
#define TestLogWrn_h__

#include "../Augur/ALogWrn.h"

class TestLogWrn:public ALogWrn
{
public:
	TestLogWrn(){}
	~TestLogWrn(){}
	using AArchive::pathArch; // директори€ дл€ логировани€
	using AArchive::pathOut;// директори€ дл€ запроса, ответа и лога
	using AModuleArch::init;
	using AModuleArch::archName;// архиватор
	using ALogWrn::sz;// макс. число архивов  в логе исключений
	using AModuleArch::lib;// dll архиватора
private:

};

#endif // TestLogWrn_h__

