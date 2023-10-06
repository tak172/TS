#include "stdafx.h"
#include "../helpful/FilesByMask.h"
#include <cppunit/extensions/HelperMacros.h>
#include "UartMsg.h"
#ifndef LINUX
#include "UtDatagram.h"
#endif // !LINUX
#include <boost/filesystem.hpp>

namespace fs = boost::filesystem;
namespace bt = boost::posix_time;

const Bbx::Identifier BBXID = Bbx::Identifier(Bbx::Identifier::FundInput);

MsgHard::MsgHard( boost::string_ref _str )
{
    set( _str );
}

void MsgHard::set(boost::string_ref _str)
{
    m_content = _str.to_string();
    m_time = boost::posix_time::microsec_clock::universal_time();
}

bool MsgHard::empty() const
{
    return m_content.empty();
}

bool MsgHard::put()
{
    if ( putImp() ) {
        m_time = boost::posix_time::microsec_clock::universal_time();
        return true;
    } else {
        m_time = bt::ptime();
        return false;
    }
}

bool MsgHard::get(boost::posix_time::time_duration _duration)
{
    if ( getImp( _duration ) ) {
        m_time = boost::posix_time::microsec_clock::universal_time();
        return true;
    } else {
        m_time = bt::ptime();
        return false;
    }
}

const std::string& MsgHard::content() const
{
    return m_content;
}

boost::posix_time::ptime MsgHard::time() const
{
    ASSERT( !m_time.is_not_a_date_time() );
    return m_time;
}

MsgHardFile::MsgHardFile( boost::string_ref _s, std::wstring _folder, std::wstring _fname  )
    : MsgHard( _s ), m_fname(_fname), m_fmask(), m_folder(_folder)
{}

MsgHardFile::MsgHardFile(std::wstring _folder, std::wstring _fmask)
    : MsgHard( "" ), m_fname(), m_fmask( _fmask ), m_folder( _folder )
{}

std::wstring MsgHardFile::fileNameOnly() const
{
    return m_fname;
}

std::wstring MsgHardFile::fileNameFull() const
{
    return m_folder + L"/" + m_fname;
}

bool MsgHardFile::putImp()
{
#ifndef LINUX
	HANDLE h = ::CreateFile((m_folder + L"/" + m_fname).c_str(),
		GENERIC_WRITE, FILE_SHARE_READ, NULL,
		CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL );
	CPPUNIT_ASSERT(h != INVALID_HANDLE_VALUE);
	DWORD szWrite = (DWORD)content().size();
	DWORD szWrited = 0;
	bool succWr = (WriteFile(h, content().c_str(), szWrite, &szWrited, NULL) && szWrited == szWrite);
	CloseHandle(h);
	CPPUNIT_ASSERT(succWr);
#else
    std::ofstream outs( ToUtf8( m_folder + L"/" + m_fname ), std::ios::out | std::ios::trunc );
    CPPUNIT_ASSERT(outs.is_open());
    outs << content();
	bool succWr = !(outs.fail());
	CPPUNIT_ASSERT(succWr);
    outs.close();
#endif // !LINUX
    return succWr;
}

bool MsgHardFile::getImp( boost::posix_time::time_duration _duration )
{
    //bool succ = false;
    set( boost::string_ref() ); // очистка
    bt::ptime bound = bt::microsec_clock::universal_time() + _duration;
    while( empty() && bt::microsec_clock::universal_time() < bound ) {
        // найти файл по маске
        std::vector<std::wstring> foundnames;
        auto look = [&foundnames]( const FilesByMask_Data& blk ){
            foundnames.push_back(blk.fname.to_string());
            return true; // соберём все файлы чтобы потом отсортировать
        };
        FilesByMask( m_folder + m_fmask,  look );
        
        // использовать или ждать
        if ( foundnames.empty() ) {
            boost::this_thread::sleep( bt::milliseconds( 100 ) );
        } else {
			std::sort(foundnames.begin(), foundnames.end());
            std::wstring foundname = foundnames.front();
            // Чтение
            {
#ifndef LINUX
				std::ifstream ifs(m_folder + foundname);
#else
                std::ifstream ifs(ToUtf8(m_folder + foundname));
#endif // !LINUX
                std::string temp;
                char ch;
                while( ifs.get( ch ) )
                    temp.push_back( ch );
                set( temp );
                m_fname = foundname;
            }
            // удаление файла
            boost::system::error_code ec;
			if (!fs::remove(m_folder + foundname, ec) || ec )
                set( boost::string_ref() ); // очистка
        }
    } 
    return !empty();
}

MsgHardXml::MsgHardXml(boost::string_ref _text, std::wstring _folder, std::wstring _fname)
    : MsgHardFile( _text, _folder, _fname )
{
}

MsgHardXml::MsgHardXml(std::wstring _folder, std::wstring _fmask)
    : MsgHardFile( _folder, _fmask )
{
}

std::string MsgHardXml::contentEx() const 
{
    std::string res;
    attic::a_document doc;
    if ( !fileNameOnly().empty() && doc.load_utf8( MsgHard::content() ) )
    {
        doc.document_element().ensure_attribute("file").set_value( fileNameOnly() );
        res = doc.to_str();
    }
    return res;
}

bool MsgHardXml::putImp()
{
    bool succ = false;
    attic::a_document doc;
    if ( doc.load_utf8( content() ) )
    {
        doc.document_element().ensure_attribute("file").set_value( fileNameOnly() );
        succ = doc.save_file( fileNameFull() );
    }
    return succ;
}

MsgSoft::MsgSoft( Network::Docker& _docker )
    : m_rd(), m_manual(false), m_docker( _docker )
{}

MsgSoft::MsgSoft( boost::string_ref _data, Network::Docker& _docker )
    : m_rd(new RichDictum(BBXID, LOS_AREA_TYPE(_data.begin(), _data.end()), time_t(0))), m_manual(true),
    m_docker( _docker )
{
}

bool MsgSoft::empty() const
{
    return !m_rd || m_rd->empty();
}

bool MsgSoft::put()
{
    if ( putImp() ) {
        m_time = boost::posix_time::microsec_clock::universal_time();
        return true;
    } else {
        m_time = bt::ptime();
        return false;
    }
}

bool MsgSoft::get(boost::posix_time::time_duration _duration)
{
    if ( getImp( _duration ) ) {
        m_time = boost::posix_time::microsec_clock::universal_time();
        return true;
    } else {
        m_time = bt::ptime();
        return false;
    }
}

DICTUM_KIND::VALUE_TYPE MsgSoft::kind() const
{
    if ( m_manual || !m_rd )
        return DICTUM_KIND::ANY_NONE;
    else
        return m_rd->kind();
}

boost::string_ref MsgSoft::content() const
{
    size_t skip = m_manual? 0 : DICTUM_GuXML::sizing();
    return m_rd->dataView( skip );
}

boost::posix_time::ptime MsgSoft::time() const
{
    ASSERT( !m_time.is_not_a_date_time() );
    return m_time;
}

void MsgSoft::pop()
{
    m_rd->pop();
}

std::vector<char> MsgSoft::get_raw(boost::posix_time::time_duration _duration)
{
    std::vector<char> res;
    bt::ptime bound = bt::microsec_clock::universal_time() + _duration;
    while( m_docker.empty() && bt::microsec_clock::universal_time() < bound )
        boost::this_thread::sleep( bt::milliseconds( 100 ) );
    if ( !m_docker.empty() )
        if( auto temp = m_docker.receive() )
            if ( !temp->empty() )
                res = *temp;
    return res;
}

bool MsgSoft::put_raw( boost::string_ref _data )
{
    return m_docker.send( _data );
}

MsgSoftXml::MsgSoftXml( Network::Docker& _docker )
    : MsgSoft( _docker )
{
}

static std::string addFname( boost::string_ref _data, std::wstring _fname )
{
    std::string result;
    attic::a_document doc;
    if ( doc.load_utf8( _data ) )
    {
        doc.document_element().ensure_attribute("file").set_value( _fname );
        result = doc.to_str();
    }
    else
        result = _data.to_string();
    return result;
}

MsgSoftXml::MsgSoftXml( boost::string_ref _data, Network::Docker& _docker, std::wstring _fname )
    : MsgSoft( addFname(_data,_fname), _docker )
{
}

bool MsgSoftXml::putImp()
{
    LOS_AREA_TYPE wrappedPacket = LOS_AREA_TYPE( m_rd->dataBegin(), m_rd->dataEnd() );
    return put_raw( boost::string_ref( &wrappedPacket.front(), wrappedPacket.size() ) );
}

bool MsgSoftXml::getImp( boost::posix_time::time_duration _duration )
{
    m_manual = false;
    auto raw = get_raw( _duration );
    if ( !raw.empty() )
        m_rd.reset( new RichDictum( BBXID, LOS_AREA_TYPE( raw.begin(), raw.end() ), time_t(0) ) );
    else
        m_rd.reset();
    return nullptr != m_rd;

}

#ifndef LINUX
MsgHardUdp::MsgHardUdp(UtDatagram& _dg)
    : MsgHard( "" ), m_dg(_dg)
{
}

bool MsgHardUdp::getImp(boost::posix_time::time_duration _duration)
{
    bt::ptime fence = bt::microsec_clock::universal_time() + _duration;
    while( m_dg.empty() && bt::microsec_clock::universal_time() < fence )
        boost::this_thread::sleep( bt::milliseconds( 10 ) );
    CPPUNIT_ASSERT_MESSAGE( "read_udp timeout", !m_dg.empty() );
    set( m_dg.get() );
    return !empty();
}

bool MsgHardUdp::putImp()
{
    ASSERT(!"missing");
    return false;
}
#endif // !LINUX

MsgSoftGui::MsgSoftGui(Network::Docker& _docker)
    : MsgSoftXml( _docker )
{
}

MsgSoftGui::MsgSoftGui(boost::string_ref _data, Network::Docker& _docker, std::wstring _fname)
    : MsgSoftXml( _data, _docker, _fname )
{
}

std::string MsgSoftGui::contentInside( int coding ) const
{
    std::string msg;
    attic::a_document doc;
    if ( doc.load_utf8( content() ) )
    {
        msg = doc.document_element().child("Mark").child_value();
        msg = ToOtherCP( FromUtf8( msg ), coding );
    }
    return msg;
}

std::wstring MsgSoftGui::filenameInside() const
{
    std::wstring fn;
    attic::a_document doc;
    if ( doc.load_utf8( content() ) )
        fn = doc.document_element().attribute("file").as_wstring();
    return fn;
}
