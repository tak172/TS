#pragma once

#include "../StatBuilder/SignalEditableDocument.h"

class CSignalDocumentTest : public CSignalEditableDocument
{
private:
	std::unique_ptr<CSignalView> sview;

	virtual BOOL OnNewDocument(){ return FALSE; }
	virtual BOOL OnOpenDocument(LPCTSTR){ return FALSE; }
	virtual BOOL OnSaveDocument( LPCTSTR ){ return FALSE; }
	virtual void OnCloseDocument(){}

	virtual void SetChangedImpl( bool ){}
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE){}
	virtual bool OnNewDocumentImpl(){ return true; }
	virtual bool OnOpenDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual bool OnSaveDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual void WarningMessage( const std::wstring & ) const{}
	virtual void OnNewDocumentSpectific(){}

	virtual std::wstring GetPathName() const{ return std::wstring(); }
	virtual std::wstring GetTitle() const{ return std::wstring(); }
	virtual void SetTitle(std::wstring){}
	virtual CSignalView * GetView() const { return sview.get(); }
	bool IsChanged() const override{ return false; }
	bool RefreshFileName() override{ return false; }
	bool DoSaving() override { return false; }
	bool DoSaving( std::wstring pathName ) override { return true; }
	virtual void UpdateAllViews() const{}
	virtual void Close(){}
	virtual void PrintConflictInfo() const{}
};