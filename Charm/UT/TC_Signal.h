#pragma once

class CAbcSignal;
class CSignalDocument;
class CSignalDocumentTest;
class CSignalManager;
class Abonent;
enum struct SignalType;
class TC_Signal
{
protected:
	TC_Signal();
	virtual ~TC_Signal(){}
	CAbcSignal * BuildSignal( const Abonent &, const SignalType &, bool add_to_doc = true, CSignalDocument * = 0 );
	std::wstring GenerateSignalName( std::wstring base_name );

protected:
	boost::scoped_ptr <CSignalDocumentTest> sigdoc;
	CSignalManager * sim;

private:
	std::set <std::wstring> used_names;
};