#pragma once

#include "../StatBuilder/LogicView.h"
#include "UserFeedbackTest.h"

class HTMLBrowser;
class CLogicViewTest : public CLogicView
{
private:
	virtual void Initialize() override{}
	virtual void ViewStationChange() override{}
	virtual void UpdateTreeNames( const std::set <const CLogicElement *> & ) override{}

	virtual UserFeedback * GetUserFeedback() const override{ return &uback; }
	virtual void ShowWindow( bool /*show*/ ) const override{}
	virtual bool IsWindowVisible() const override{ return false; }
	virtual void InitialUpdateFrame() override{}
	virtual void UpdateFrame() const override{}
	virtual void SetCaption( std::wstring ){}
	virtual void SortItems() override{}
	LogicLinkView * GetLinkView() const { return 0; }

	mutable UserFeedbackTest uback;
};