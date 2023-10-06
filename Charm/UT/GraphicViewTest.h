#pragma once

#include "../StatBuilder/GraphicView.h"
#include "TestContext.h"

class HTMLBrowser;
class CGraphicViewTest : public CGraphicView
{
public:
	HTMLBrowser * htmlBrowser;
	CGraphicDocument * gdoc;
	CGraphicViewTest( CGraphicDocument * gd ) : htmlBrowser(nullptr), gdoc( gd ){}
	using CGraphicView::HitMousePoint;
	TestContext * GetContextTest(){ return new TestContext(); }

private:
	virtual CGraphicDocument * GetDocument() const{ return gdoc; }
	virtual void SetScrollPosition( unsigned int /*xpos*/, unsigned int /*ypos*/ ){}
	virtual void SetScrollSizes(){}
	virtual POINT GetScrollPosition() const{ POINT pt; pt.x = pt.y = 0; return pt; }
	virtual unsigned int GetHorScrollLimit(){ return 0; }
	virtual unsigned int GetVertScrollLimit(){ return 0; }
	virtual void ShowScrollBar( bool /*to_show*/ ){}
	virtual void ScrollToPosition( POINT ){}
	virtual CGridPoint ScreenToGrid( const POINT & ) const{ return CGridPoint( 0, 0 ); }
	virtual void InitialUpdateFrame(){}
	virtual void ShowWindow( bool /*show*/ ) const{}
	virtual void UpdateFrame() const{}
	virtual void ShowWindow() const{}
	virtual void BringWindowToTop() const{}
	virtual void InvalidateRect( const CGridRect * ){}
	virtual void MoveWindow( RECT * ){}
    virtual bool IsWindowVisible() const { return true; }
	virtual void SetCaption( std::wstring ){}
	virtual HTMLBrowser & GetBrowser() const{ ASSERT( htmlBrowser && L"браузер не определен"); return *htmlBrowser; }
	virtual GraphicContext * GetContext(){ return new TestContext(); }
	virtual void ReleaseContext(){}
	virtual void SetCapture(){}
	virtual UserFeedback * GetUserFeedback() const{ ASSERT( false && L"не определено" ); return 0; }
	virtual RECT GetClientRect() const{ RECT r; r.bottom = r.top = r.left = r.right = 0; return r; }
	virtual RECT GetScreenRect() const{ RECT r; r.bottom = r.top = r.left = r.right = 0; return r; }
	virtual void StartAutoscroll(){}
	virtual void StopAutoscroll(){}
	virtual void ModalBoxMessage( const std::wstring & caption, const std::wstring & msgTxt ) const override{}
};