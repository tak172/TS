#pragma once

#include "../StatBuilder/ProjectView.h"

class CProjectViewTest : public CProjectView
{
public:
	CProjectDocument * pdoc;
	CProjectViewTest( CProjectDocument * pd ) : pdoc( pd ){}
private:
	virtual void UpdateFrame() const{}
};