#pragma once

#include "../StatBuilder/LogicEditableDocument.h"
#include "../StatBuilder/LogicValidator.h"

class LogicValidatorTest;
struct LogicPool;
class CLogicDocumentTest : public CLogicEditableDocument
{
public:
	using CLogicEditableDocument::GetLObjectManager;
	using CLogicEditableDocument::GetStationManager;
	using CLogicEditableDocument::GetLinkManager;
	CLogicDocumentTest();
	virtual void FinalizeLoading();

private:
	std::unique_ptr<CLogicView> lview;

	virtual BOOL OnNewDocument(){ return FALSE; }
	BOOL OnSaveDocument( LPCTSTR ){ return FALSE; }
	void OnCloseDocument(){}
	virtual void Close(){}

	virtual std::wstring GetTitle() const{ return std::wstring(); }
	virtual void SetTitle(std::wstring){}
	virtual bool IsChanged() const{ return false; }

	virtual void SetChangedImpl( bool ){}
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE){}
	virtual bool OnNewDocumentImpl(){ return true; }
	virtual bool OnOpenDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual bool OnSaveDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual void WarningMessage( const std::wstring & ) const{}
	virtual void OnNewDocumentSpectific(){}
	bool RefreshFileName() override { return false; }
	bool DoSaving( std::wstring pathName ) override { return true; }
	bool DoSaving() override { return false; }

	virtual std::wstring GetPathName() const{ return std::wstring(); }
	virtual void UpdateAllViews() const{}
	virtual CLogicView * GetView() const { return lview.get(); }
	virtual bool WindowIsActive() const{ return false; }
	virtual void PrintConflictInfo() const{}
};