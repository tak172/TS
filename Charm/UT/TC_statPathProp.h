/*!
	created:	2019/05/21
	created: 	11:58  21 Май 2019
	filename: 	F:\potapTrunk\Charm\UT\TC_statPathProp.h
	file path:	F:\potapTrunk\Charm\UT
	file base:	TC_statPathProp
	file ext:	h
	author:		 Dremin
	
	purpose:	настройка свойств станц.путей
 */
#ifndef TC_statPathProp_h__
#define TC_statPathProp_h__
#include "../helpful/ADProperties.h"

extern ADProperties passStopInOutPr;//пасс+для стоянки +не гл + не безост

extern  ADProperties noStopPr;// главный для проследования (безостан)// гл + безост
extern  ADProperties passMovePr;// для просл. не гл , не пасс и  безост

extern ADProperties pMaj;//гл 
extern ADProperties passMaj;//пасс+ гл 
extern ADProperties passStopPr;//пасс

extern ADProperties evenPr;
extern ADProperties oddPr;
extern ADProperties evenMajor;
extern ADProperties oddMajor;
extern ADProperties allPr;//в любом направлении
#endif // TC_statPathProp_h__

