#pragma once

#include "../StatBuilder/GraphicEditableDocument.h"
#include "../StatBuilder/GraphicEditableFactory.h"
#include "../StatBuilder/IEditor.h"

struct GraphicPool;
class CGraphicViewTest;
class CGraphicDocumentTest : public CGraphicEditableDocument
{
public:
	using CGraphicEditableDocument::GetGObjectManager;
	CGraphicDocumentTest();
	~CGraphicDocumentTest();
	virtual CGraphicView * GetView() const;
	CGraphicViewTest * GetViewTest() const;

private:
	std::unique_ptr<CGraphicViewTest> gview;
	std::unique_ptr<CGraphicEditableFactory> GEF;

	virtual BOOL OnNewDocument(){ return FALSE; }
	BOOL OnOpenDocument( LPCTSTR ){ return FALSE; }
	BOOL OnSaveDocument( LPCTSTR ){ return FALSE; }
	virtual void OnCloseDocument(){}
	virtual void Close(){}

	virtual void SetTitle( std::wstring ) {}
	virtual std::wstring GetTitle() const{ return std::wstring(); }
	bool IsChanged() const override { return false; }
	bool RefreshFileName() override { return true; }
	bool DoSaving( std::wstring pathName ) override { return true; }
	bool DoSaving() override { return false; }
	
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE){}
	virtual bool OnNewDocumentImpl(){ return true; }
	virtual bool OnOpenDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual bool OnSaveDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual void WarningMessage( const std::wstring & ) const{}
	virtual void SetChangedImpl( bool ) {}

	virtual std::wstring GetPathName() const{ return std::wstring(); }
	virtual void UpdateAllViews() const{}
	virtual void AssertValidness() const{}
	virtual bool WindowIsActive() const{ return false; }
	virtual const CGraphicFactory * GetFactoryPtr() const { return GEF.get(); }
	virtual void PrintConflictInfo() const{}
};