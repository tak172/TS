#pragma once

#include "../StatBuilder/ProjectDocument.h"
#include "../StatBuilder/GraphicEditableFactory.h"
#include "ProjectViewTest.h"

class CProjectViewTest;
class CProjectDocumentTest : public CProjectDocument
{
public:
	CProjectDocumentTest();
	~CProjectDocumentTest(){}
	virtual CProjectView * GetView() const;
	void OpenAllPools() const;
	void SetThisProject() { CProjectDocument::SetProject( this ); }

private:
	boost::scoped_ptr <CGraphicEditableFactory> GEF;
	boost::scoped_ptr <CProjectViewTest> projview;
	virtual void Close(){}
	virtual std::wstring GetPathName() const{ return std::wstring(); }
	virtual void UpdateAllViews() const{}
	virtual bool DoFileSave() const{ return false; }
	virtual bool WindowIsActive() const{ return false; }
	virtual void SetChildName( const CGraphicDocument *, std::wstring ){}
	virtual void SetChildName( const CLogicDocument * , std::wstring ){}
	virtual void SetChildName( const CSignalDocument * , std::wstring ){}
	virtual void PrintMsgBar( std::wstring ) const{}
	virtual const CGraphicFactory * GetFactoryPtr() const override { return GEF.get(); }
	virtual void CreateIfNeed( std::wstring & abspath, FileKind, bool user_new_adding, CGraphicEditableDocument *& ){}
	virtual void CreateIfNeed( std::wstring & abspath, FileKind, bool user_new_adding, CLogicEditableDocument *& ){}
	virtual void CreateIfNeed( std::wstring & abspath, FileKind, bool user_new_adding, CSignalEditableDocument *& ){}
	virtual bool IsWindowOpened( const std::wstring &, FileKind ) const { return false; }
	virtual bool IsWindowOpened( const std::wstring & docpath, FileKind fileKind, CGraphicEditableDocument *& ) const { return false; }
	virtual bool IsWindowOpened( const std::wstring & docpath, FileKind fileKind, CLogicEditableDocument *& ) const { return false; }
	virtual bool IsWindowOpened( const std::wstring & docpath, FileKind fileKind, CSignalEditableDocument *& ) const { return false; }

	virtual void SetChanged( bool ){}
	virtual void SetPathName(LPCTSTR lpszPathName, BOOL bAddToMRU = TRUE){}
	virtual void FinalizeLoading(){}
	virtual bool OnNewDocumentImpl(){ return true; }
	virtual bool OnOpenDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual bool OnSaveDocumentImpl( LPCTSTR lpszPathName ){ return true; }
	virtual void WarningMessage( const std::wstring & ) const{}
	virtual void OnNewDocumentSpectific(){}
	virtual void PrintConflictInfo() const{}
};