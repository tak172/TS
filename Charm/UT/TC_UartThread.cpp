#include "stdafx.h"
#include <boost/filesystem.hpp>
#include "../helpful/FilesByMask.h"
#include "../helpful/Dictum.h"
#include "../helpful/Log.h"
#include "../helpful/StateCollector.h"
#include "../helpful/Trindex.h"
#include "../Docker/dk_docker.h"
#include "../Haron/HaronAbcLoader.h"
#include "../Haron/HaronBbx.h"
#include "../Haron/HaronChanelCfg.h"
#include "../Haron/HaronCore.h"
#include "../Haron/HaronMgrIO.h"
#include "../Haron/HaronCommandos.h"
#include "../Haron/dc_fmt.h"
#include "../Haron/uart_dc_def.h"
#include "../Haron/uart_dc.h"
#include "../Haron/uart_dc_Dialog.h"
#include "../Haron/HaronWatchdog.h"
#include "../Haron/uart_VecText.h"
#ifndef LINUX
#include "UtDatagram.h"
#endif // !LINUX
#include "TC_UartThread.h"
#include "XmlEqual.h"
#include "UartMsg.h"

namespace bt = boost::posix_time;
namespace bfs = boost::filesystem;

CPPUNIT_TEST_SUITE_REGISTRATION( TC_UartThread );

void TC_UartThread::setUp ()
{
    //m_tempDir = (bfs::temp_directory_path() / L"unittest" / L"x").wstring();
    m_tempDir = (bfs::temp_directory_path() / L"x").wstring();
    m_tempDir.pop_back(); // на конце строки сохранился разделитель
    m_extIn =  L".xin";
    m_extOut = L".xout";
    CPPUNIT_ASSERT( wipeFiles() );
}

void TC_UartThread::tearDown ()
{
    CPPUNIT_ASSERT( wipeFiles() );
    Monitor::KeyValueStateCollector::Shutdowner();
    WriteLog.Shutdowner();
}

bool TC_UartThread::wipeFiles()
{
    bool success = true;
    auto del = [this, &success]( const FilesByMask_Data& blk ){
        bfs::path fp(m_tempDir);
        fp /= blk.fname.to_string();
        boost::system::error_code ec;
        if ( !bfs::remove( fp, ec ) )
            success = false;
        return true;
    };
    FilesByMask( m_tempDir + L"*" + m_extIn, del );
    FilesByMask( m_tempDir + L"*" + m_extOut, del );
    return success;
}

namespace Mock {

    class HaronBbx : public Haron::HaronBbx
    {
    public:
        void Init( const attic::a_node& /*cfg_node*/, int /*BbxType*/ ) override {}
        bool Write( const LOS_AREA_TYPE& /*vSended*/,
            const Bbx::Identifier /*ParcelID*/, const Bbx::Stamp& /*stamp*/, bool /*support*/ ) override { return true; }
        bool WriteSimpleMsg( const char* /*caption*/, size_t /*sz_caption*/,
            const char* /*data*/, size_t /*sz_data*/,
            const Bbx::Identifier /*ParcelID*/, const Bbx::Stamp& /*stamp*/, bool /*direction*/ ) override { return true; }
        bool WriteInc( const Bbx::Buffer& /*caption*/, const Bbx::Buffer& /*before*/,
            const Bbx::Buffer& /*after*/, const Bbx::Identifier /*ParcelID*/, const Bbx::Stamp& /*stamp*/ ) override { return true; }
        bool WriteRef( const Bbx::Buffer& /*caption*/, const Bbx::Buffer& /*data*/,
            const Bbx::Identifier /*ParcelID*/, const Bbx::Stamp& /*stamp*/ ) override { return true; }
        bool IsWriteEnable() const  override { return true; }
    };
};

static const wchar_t* BISK_H1 = 
    L"<?xml version='1.0' encoding='Windows-1251'?>"
    L"<zka>"
    L"  <To>1</To>"
    L"  <Ii>2509202107090345006290</Ii>"
    L"  <Ioz>2509202107071431509560</Ioz>"
    L"  <DoDT>202107090400</DoDT>"
    L"  <Nz>14778</Nz>"
    L"  <Ds>202107071431</Ds>"
    L"</zka>";
static const wchar_t* BISK_S3 =
    L"<?xml version='1.0' encoding='Windows-1251'?>"
    L"<zka>"
    L"<To>2</To>"
    L"<Ip>2509202107090345041760</Ip>"
    L"<Ipz>2509202107082307000136</Ipz>"
    L"<Kp>1</Kp>"
    L"<Ko>101</Ko>"
    L"</zka>";
static const wchar_t* BISK_SG =
    L"<?xml version='1.0' encoding='Windows-1251'?>"
    L"<zka>"
    L"<To>2</To>"
    L"<Ip>25092021070903450555555</Ip>"
    L"<Ipz>2509202107082307000136</Ipz>"
    L"<Kp>1</Kp>"
    L"<Ko>101</Ko>"
    L"</zka>";


void TC_UartThread::Bisk_WholeFile()
{
    const unsigned int portH = 30111;
    const unsigned int portF = 30222;
    const bt::time_duration TRANSPORT_DELAY = bt::seconds( 5 );
    
    // создать канал для обмена файлами
    attic::a_document doc;
    {
        std::wstring fnI = m_tempDir + L"*" + m_extIn;
        std::wstring fnO = m_tempDir + L"*" + m_extOut;
        CPPUNIT_ASSERT( doc.load_wide(
            L"<HaronConfiguration>"
            L"  <Channel>"
            L"    <Lk Id='BisK'"
            L"      Hard='WFiles(" + fnI + L")' HardOut='" + fnO + L"' Fmt='Bisk' />"
            L"    <Port number='" + std::to_wstring(portH) + L"'/>"
            L"  </Channel>"
            L"  <BlackBox Path='" + m_tempDir + L"' daysHold='30' limit='50G'/>"
            L"  <OutBlackBox Path='" + m_tempDir + L"' daysHold='30'/>"
            L"  <Log Path='" + m_tempDir + L"'/>"
            L"  <Restrict>"
            L"    <Full IP='127.0.0.1/32'/>"
            L"    <Read IP='0.0.0.0/0'/>"
            L"  </Restrict>"
            L"</HaronConfiguration>"
            ) );
    }
	auto func_lm = []( const std::wstring& /*_text_message*/ ){
	};

    // Запустить ядро Харона
    Mock::HaronBbx inBbx;
    Mock::HaronBbx outBbx;
    Haron::Core hcore( inBbx, outBbx, func_lm );
    hcore.Configure( doc.document_element().to_str() );
    hcore.StartAllProcess();
    boost::this_thread::sleep(bt::milliseconds(100));  // подождём для окончательного запуска ядра

    // запустить докер для связи с ядром Харона
    std::shared_ptr<Network::Docker> dkr = make_link( portH, portF );
    boost::this_thread::sleep(bt::milliseconds(100));  // ожидаем запуск Докеров

    // прием одиночного сообщения из файла и передача его в докер
    {
        MsgHardXml hard1( ToUtf8( BISK_H1 ), m_tempDir, L"11111111_1" + m_extIn );
        MsgSoftXml soft1( *dkr );
        CPPUNIT_ASSERT( hard1.put() );

		CPPUNIT_ASSERT( soft1.get( bt::seconds( 10 )) );
		CPPUNIT_ASSERT( !soft1.empty() );
		if( soft1.kind() == DICTUM_KIND::H2F_LTS )
		{
			// если пришли опорные данные - перезапросить второй пакет
			soft1.pop();
			CPPUNIT_ASSERT( soft1.get( bt::seconds( 10 )) );
			CPPUNIT_ASSERT( !soft1.empty() );
		}
        CPPUNIT_ASSERT( soft1.kind() == DICTUM_KIND::H2F_GuXML );
        CPPUNIT_ASSERT( xmlEqual( soft1.content(), hard1.contentEx() ) );
        //DebugLog( L"Transmit1 file --> docker duration: %s", to_simple_wstring( soft1.time() - hard1.time() ) );
        CPPUNIT_ASSERT( soft1.time() - hard1.time() < TRANSPORT_DELAY );
        soft1.pop();
        CPPUNIT_ASSERT( soft1.empty() );
    }
    // прием одиночного сообщения из докера и запись файла в каталог
    {
        MsgSoftXml soft3( ToUtf8( BISK_S3 ), *dkr, L"333333_3"+m_extOut );
        CPPUNIT_ASSERT( soft3.put() );
        MsgHardXml hard3( m_tempDir, L"*"+m_extOut );
        CPPUNIT_ASSERT( hard3.get( bt::seconds( 10 ) ) );
        CPPUNIT_ASSERT( xmlEqual( soft3.content(), hard3.contentEx() ) );
        //DebugLog( L"Transmit3 file <-- docker duration: %s", to_simple_wstring( hard3.time() - soft3.time() ) );
        CPPUNIT_ASSERT( hard3.time() - soft3.time() < TRANSPORT_DELAY );
    }

    // группа сообщений из докера и запись файлов в каталог
    {
        size_t COUNT = 10;
        std::vector<std::shared_ptr<MsgSoftXml> > vSoft;
        std::vector<std::shared_ptr<MsgHardXml> > vHard;
        for( size_t i=0; i < COUNT; ++i )
        {
            auto ii = ( 7 + i * 13 ) % COUNT; // смешивание номеров
            std::wstring fn = L"555" + std::to_wstring(ii) + L"_" + std::to_wstring(ii) + m_extOut;
            vSoft.push_back( std::make_shared<MsgSoftXml>( ToUtf8( BISK_SG ), *dkr, fn ) );
            vHard.push_back( std::make_shared<MsgHardXml>( m_tempDir, L"*"+m_extOut ) );
        }

        // отправка сразу множества сообщений
        for( size_t i=0; i < COUNT; ++i )
            CPPUNIT_ASSERT( vSoft[i]->put() );
        // прием всех - при одновременной отправке порядок приема не гарантируется
        for( size_t i = 0; i < COUNT; ++i )
            CPPUNIT_ASSERT( vHard[ i ]->get( bt::seconds( 10 ) ) );
        // проверка приема
        auto cmpSoft = []( std::shared_ptr<MsgSoftXml> a, std::shared_ptr<MsgSoftXml> b ) {
            return a->content() < b->content();
        };
        auto cmpHard = []( std::shared_ptr<MsgHardXml> a, std::shared_ptr<MsgHardXml> b ) {
            return a->contentEx() < b->contentEx();
        };
        std::sort( vSoft.begin(), vSoft.end(), cmpSoft );
        std::sort( vHard.begin(), vHard.end(), cmpHard );
        for( size_t i = 0; i < COUNT; ++i ) {
            CPPUNIT_ASSERT( xmlEqual( vSoft[i]->content(), vHard[i]->contentEx() ) );
			CPPUNIT_ASSERT( vHard[i]->time() - vSoft[i]->time() < TRANSPORT_DELAY );
        }
    }
}

static const wchar_t* GUI_S3 =
    L"<GuMulti>"
    L"<Mark action=\"add\">"
    L"(:0001 28002:20 :2 13 02 22 1465\n"
    L"К 1644719880 01 01 031250 031268 040 0222013778 0222013778 00000 0\n"
    L"пометка"
    L"</Mark>"
    L"</GuMulti>"
    ;

static const wchar_t* GUI_SG =
    L"<GuMulti>"
    L"<Mark action=\"add\">"
    L"(:0001 28002:20 :2 13 02 22 1465\n"
    L"К 1644719880 01 01 031250 031268 040 0222013778 0222013778 00000 0\n"
    L"множество пометок"
    L"</Mark>"
    L"</GuMulti>"
    ;

void TC_UartThread::GidUral_WholeFile()
{
    const unsigned int portH = 30101;
    const unsigned int portF = 30202;
#ifndef _DEBUG
    const bt::time_duration TRANSPORT_DELAY = bt::seconds( 5 );
#endif // _DEBUG

    // создать канал для обмена файлами
    attic::a_document doc;
    {
        std::wstring fnI = m_tempDir + L"*" + m_extIn;
        std::wstring fnO = m_tempDir + L"*" + m_extOut;
        CPPUNIT_ASSERT( doc.load_wide(
            L"<HaronConfiguration>"
            L"  <Channel>"
            L"    <Lk Id='GuNote'"
            L"      Hard='WFiles(" + fnI + L")' HardOut='" + fnO + L"' Fmt='GuMulti' />"
            L"    <Port number='" + std::to_wstring(portH) + L"'/>"
            L"  </Channel>"
            L"  <BlackBox Path='" + m_tempDir + L"' daysHold='30' limit='50G'/>"
            L"  <OutBlackBox Path='" + m_tempDir + L"' daysHold='30'/>"
            L"  <Log Path='" + m_tempDir + L"'/>"
            L"  <Restrict>"
            L"    <Full IP='127.0.0.1/32'/>"
            L"    <Read IP='0.0.0.0/0'/>"
            L"  </Restrict>"
            L"</HaronConfiguration>"
            ) );
    }

	auto func_lm = []( const std::wstring& /*_text_message*/ ){
	};

    // Запустить ядро Харона
    Mock::HaronBbx inBbx;
    Mock::HaronBbx outBbx;
    Haron::Core hcore( inBbx, outBbx, func_lm );
    hcore.Configure( doc.document_element().to_str() );
    hcore.StartAllProcess();
    boost::this_thread::sleep(bt::milliseconds(100));  // подождём для окончательного запуска ядра

    // запустить докер для связи с ядром Харона
    std::shared_ptr<Network::Docker> dkr = make_link( portH, portF );
	boost::this_thread::sleep(bt::milliseconds(100));  // ожидаем запуск Докеров

    // прием одиночного сообщения из файла и передача его в докер
    {
        // не реализовано
    }
    // прием одиночного сообщения из докера и запись файла в каталог
    {
        MsgSoftGui soft3( ToUtf8( GUI_S3 ), *dkr, L"Z1122330.000"+m_extOut );
        CPPUNIT_ASSERT( soft3.put() );
        MsgHardFile hard3( m_tempDir, L"*"+m_extOut );
        CPPUNIT_ASSERT( hard3.get( bt::seconds( 10 ) ) );
        CPPUNIT_ASSERT( soft3.contentInside(866) == hard3.content() );
        CPPUNIT_ASSERT( soft3.filenameInside() == hard3.fileNameOnly() );
#ifndef _DEBUG
        CPPUNIT_ASSERT( hard3.time() - soft3.time() < TRANSPORT_DELAY );
#endif // _DEBUG
    }

    // группа сообщений из докера и запись файлов в каталог
    {
        size_t COUNT = 10;
        std::vector<std::shared_ptr<MsgSoftGui> > vSoft;
        std::vector<std::shared_ptr<MsgHardXml> > vHard;
        for( size_t i=0; i < COUNT; ++i )
        {
            std::wstring fn = L"555" + std::to_wstring(i) + L"_" + std::to_wstring(i) + m_extOut;
            vSoft.push_back( std::make_shared<MsgSoftGui>( ToUtf8( GUI_SG ), *dkr, fn ) );
            vHard.push_back( std::make_shared<MsgHardXml>( m_tempDir, L"*"+m_extOut ) );
        }
        // отправка сразу множества сообщений
        for( size_t i=0; i < COUNT; ++i )
            CPPUNIT_ASSERT( vSoft[i]->put() );
        boost::this_thread::sleep(bt::seconds(1));  // ожидаем передачу всех сообщений

        for( size_t i=0; i < COUNT; ++i )
        {
            CPPUNIT_ASSERT( vHard[i]->get( bt::seconds( 10 ) ) );
            CPPUNIT_ASSERT( vSoft[i]->contentInside(866) == vHard[i]->content() );
            CPPUNIT_ASSERT( vSoft[i]->filenameInside() == vHard[i]->fileNameOnly() );
#ifndef _DEBUG
			CPPUNIT_ASSERT(vHard[i]->time() - vSoft[i]->time() < TRANSPORT_DELAY);
#endif // !_DEBUG
        }
    }
}

void TC_UartThread::Asoup_WholeFile()
{
    const unsigned int portH = 30111;
    const unsigned int portF = 30222;
#ifndef _DEBUG
	const bt::time_duration TRANSPORT_DELAY = bt::seconds(5);
#endif // !_DEBUG

    // создать канал для обмена файлами
    attic::a_document doc;
    {
        std::wstring fnI = m_tempDir + L"*" + m_extIn;
        std::wstring fnO = m_tempDir + L"*" + m_extOut;
        CPPUNIT_ASSERT( doc.load_wide(
            L"<HaronConfiguration>"
            L"  <Channel>"
            L"    <Lk Id='ASOUP1'"
            L"      Hard='WFiles(" + fnI + L")' HardOut='" + fnO + L"' Fmt='ASOUP' />"
            L"    <Port number='" + std::to_wstring(portH) + L"'/>"
            L"  </Channel>"
            L"  <BlackBox Path='" + m_tempDir + L"' daysHold='30' limit='50G'/>"
            L"  <OutBlackBox Path='" + m_tempDir + L"' daysHold='30'/>"
            L"  <Log Path='" + m_tempDir + L"'/>"
            L"  <Restrict>"
            L"    <Full IP='127.0.0.1/32'/>"
            L"    <Read IP='0.0.0.0/0'/>"
            L"  </Restrict>"
            L"</HaronConfiguration>"
            ) );
    }

	auto func_lm = []( const std::wstring& /*_text_message*/ ){
	};

    // Запустить ядро Харона
    Mock::HaronBbx inBbx;
    Mock::HaronBbx outBbx;
    Haron::Core hcore( inBbx, outBbx, func_lm );
    hcore.Configure( doc.document_element().to_str() );
    hcore.StartAllProcess();
	boost::this_thread::sleep(bt::milliseconds(100));  // подождём для окончательного запуска ядра

	// запустить докер для связи с ядром Харона
	std::shared_ptr<Network::Docker> dkr = make_link(portH, portF);
	boost::this_thread::sleep(bt::milliseconds(100));  // ожидаем запуск Докеров

    // прием группы сообщений из файлов и передача в докер
    {
        std::string letters = "0123456789";
        std::vector<std::shared_ptr<MsgHardFile> > vHardFiles;
        std::vector<std::shared_ptr<MsgSoftXml> > vSoft;
        for( size_t i=0; i < letters.size(); ++i )
        {
            size_t sz = 20;
             if ( i%2 )
                 sz += i;   // четное: +2 +4 +6 ...
             else
                 sz -= i+1; // нечетное -2 -4 -6 ...

            std::string text = "(:1042 "+ std::string( sz, letters[i] ) + ":)";
            std::wstring fn = L"apovs_" + std::to_wstring(i) + m_extIn;
            vHardFiles.push_back( std::make_shared<MsgHardFile>( text, m_tempDir, fn ) );
        }
        // отправка сразу множества сообщений
        for( auto ph : vHardFiles )
            CPPUNIT_ASSERT( ph->put() );

        MsgSoftXml soft( *dkr );
		// ------------------------

        for( size_t i=0; i < vHardFiles.size(); ++i )
        {
            CPPUNIT_ASSERT( soft.get( bt::seconds( 3 ) ) );
			CPPUNIT_ASSERT( !soft.empty() );
			if( soft.kind() == DICTUM_KIND::H2F_LTS )
			{
				// если пришли опорные данные - перезапросить второй пакет
				soft.pop();
				CPPUNIT_ASSERT( soft.get( bt::seconds( 3 )) );
				CPPUNIT_ASSERT( !soft.empty() );
			}
            CPPUNIT_ASSERT( soft.kind() == DICTUM_KIND::H2F_ASOUP );
            CPPUNIT_ASSERT( soft.content() == vHardFiles[i]->content() );
#ifndef _DEBUG
			CPPUNIT_ASSERT(soft.time() - vHardFiles[i]->time() < TRANSPORT_DELAY);
#endif // !_DEBUG
            soft.pop();
            CPPUNIT_ASSERT( soft.empty() );
        }
    }
}

#ifndef LINUX
static const wchar_t* DIA_S3 =
    L"<Gid2Dialog>"
    L"    <train Region='11111:09580' SysNumber='123'  FactNumber='123'  TrainIdx='1234-555-6789' UsedSig='333331001' ObjType='1' Deviate='11' />"
    L"    <train Region='11111:09580' SysNumber='456'  FactNumber='456'  TrainIdx='2222-333-4444' UsedSig='777772022' ObjType='2' Deviate='22' />"
    L"    <train Region='77777:88888' SysNumber='6022' FactNumber='6022' TrainIdx='5555-666-7777' UsedSig='666663033' ObjType='3' Deviate='33' />"
    L"    <train Region='77777:88888' SysNumber='3057' FactNumber='3057' TrainIdx='8888-999-7372' UsedSig='222224044' ObjType='4' Deviate='44' />"
    L"</Gid2Dialog>";

void TC_UartThread::Dialog_Udp()
{
    const unsigned int portH = 30111;
    const unsigned int portF = 30222;
    const unsigned int portLoc = 30333;
    const unsigned int portRem = 30444;
    const std::string ipLoc = "127.0.0.1/"+std::to_string(portLoc);
    const std::string ipRem = "127.0.0.1/"+std::to_string(portRem);
    const bt::time_duration TRANSPORT_DELAY = bt::seconds( 5 );

    // сокет UPD для получения с. от ядра Харона
    UtDatagram dg( ipRem );

    // создать канал для обмена файлами
    attic::a_document doc;
    {
        const std::string TempDir = ToUtf8(m_tempDir);
        CPPUNIT_ASSERT( doc.load_utf8(
            "<HaronConfiguration>"
            "  <Channel>"
            "    <Lk Id='DiaN' Hard='UdpIp(LOCAL="+ipLoc+";REMOTE="+ipRem+")' Fmt='dialog' />"
            "    <Port number='" + std::to_string(portH) + "'/>"
            "  </Channel>"
            "  <BlackBox Path='" + TempDir + "' daysHold='30' limit='50G'/>"
            "  <OutBlackBox Path='" + TempDir + "' daysHold='30'/>"
            "  <Log Path='" + TempDir + "'/>"
            "  <Restrict>"
            "    <Full IP='127.0.0.1/32'/>"
            "    <Read IP='0.0.0.0/0'/>"
            "  </Restrict>"
            "</HaronConfiguration>"
            ) );
        // ЛК_UDP_OUT=UdpIp(LOCAL=192.168.1.105/7071;REMOTE=192.168.160.151/7073),dialog,1-99,зазор=20ms
        // <Lk Id="ЛК_RIG_VAL_NEVA_1" Hard="UdpIp(LOCAL=192.168.1.105/7001;REMOTE=127.0.0.1/7009)" Fmt="traktn" watchdog="neva"/>
    }

	auto func_lm = []( const std::wstring& /*_text_message*/ ){
	};

    // Запустить ядро Харона
    Mock::HaronBbx inBbx;
    Mock::HaronBbx outBbx;
    Haron::Core hcore( inBbx, outBbx, func_lm );
    hcore.Configure( doc.document_element().to_str() );
    hcore.StartAllProcess();

    // запустить докер для связи с ядром Харона
    std::shared_ptr<Network::Docker> dkr = make_link( portH, portF );

    // одно с. от фонда приводит к записи двух файлов в каталог (т.к. разные регионы)
    MsgSoftXml soft3( ToUtf8( DIA_S3 ), *dkr, L"nothing" );
    CPPUNIT_ASSERT( soft3.put() );
    bt::ptime momA, momB;
    {
        MsgHardUdp hardA( dg );
        CPPUNIT_ASSERT( hardA.get( Haron::GID2DIALOG_INTERVAL * 2 + bt::seconds( 1 ) ) );
        momA = bt::microsec_clock::universal_time();
        const std::string shippedA = hardA.content();
        const DialogTrainsHeader_t* pHdr=nullptr;
        const DialogTrains_t::Train_t* pTr = nullptr;                      // D96 R82>=56+  40  
        CPPUNIT_ASSERT( shippedA.size()>= sizeof(*pHdr) + 2*sizeof(*pTr) );//D136R123>=56+2*40
        pHdr = reinterpret_cast<const DialogTrainsHeader_t*>( &*shippedA.begin() );
        pTr  = reinterpret_cast<const DialogTrains_t::Train_t*>( &*(pHdr+1) );
        CPPUNIT_ASSERT( pHdr->CheckId() );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(95800), pHdr->Esr1 );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(111114), pHdr->Esr2); // Янамуйжа
        CPPUNIT_ASSERT_EQUAL( short(2), pHdr->Count );
        CPPUNIT_ASSERT_EQUAL( 123, static_cast<int>(pTr->getSysNumber()) );
        CPPUNIT_ASSERT_EQUAL( 123, static_cast<int>(pTr->getFactNumber()) );
        CPPUNIT_ASSERT( pTr->getIndex() == Trindex(L"1234-555-6789") );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(333331001), pTr->getUsedSig() );
        CPPUNIT_ASSERT_EQUAL( 1, static_cast<int>(pTr->getObjType()) );
        CPPUNIT_ASSERT_EQUAL( 11, static_cast<int>(pTr->getDeviate()) );

        CPPUNIT_ASSERT_EQUAL( 456, static_cast<int>((pTr+1)->getSysNumber()) );
        CPPUNIT_ASSERT_EQUAL( 456, static_cast<int>((pTr+1)->getFactNumber()) );
        CPPUNIT_ASSERT( (pTr+1)->getIndex() == Trindex(L"2222-333-4444") );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(777772022), (pTr+1)->getUsedSig() );
        CPPUNIT_ASSERT_EQUAL( 2, static_cast<int>((pTr+1)->getObjType()) );
        CPPUNIT_ASSERT_EQUAL( 22, static_cast<int>((pTr+1)->getDeviate()) );
    }
    {
        MsgHardUdp hardB( dg );
        CPPUNIT_ASSERT( hardB.get( Haron::GID2DIALOG_INTERVAL * 2 + bt::seconds( 1 ) ) );
        momB = bt::microsec_clock::universal_time();
        const std::string shippedB = hardB.content();
        const DialogTrainsHeader_t* pHdr=nullptr;
        const DialogTrains_t::Train_t* pTr = nullptr;
        CPPUNIT_ASSERT( shippedB.size()>= sizeof(*pHdr) + sizeof(*pTr) );
        pHdr = reinterpret_cast<const DialogTrainsHeader_t*>( &*shippedB.begin() );
        pTr  = reinterpret_cast<const DialogTrains_t::Train_t*>( &*(pHdr+1) );
        CPPUNIT_ASSERT( pHdr->CheckId() );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(777776), pHdr->Esr1 );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(888882), pHdr->Esr2 );
        CPPUNIT_ASSERT_EQUAL( short(2), pHdr->Count  );
        CPPUNIT_ASSERT_EQUAL( 6022, static_cast<int>(pTr->getSysNumber()) );
        CPPUNIT_ASSERT_EQUAL( 6022, static_cast<int>(pTr->getFactNumber()) );
        CPPUNIT_ASSERT( pTr->getIndex() == Trindex(L"5555-666-7777") );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(666663033), pTr->getUsedSig() );
        CPPUNIT_ASSERT_EQUAL( 3, static_cast<int>(pTr->getObjType()) );
        CPPUNIT_ASSERT_EQUAL( 33, static_cast<int>(pTr->getDeviate()) );

        CPPUNIT_ASSERT_EQUAL( 3057, static_cast<int>((pTr+1)->getSysNumber()) );
        CPPUNIT_ASSERT_EQUAL( 3057, static_cast<int>((pTr+1)->getFactNumber()) );
        CPPUNIT_ASSERT( (pTr+1)->getIndex() == Trindex(L"8888-999-7372") );
        CPPUNIT_ASSERT_EQUAL( static_cast<unsigned int>(222224044), (pTr+1)->getUsedSig() );
        CPPUNIT_ASSERT_EQUAL( 4, static_cast<int>((pTr+1)->getObjType()) );
        CPPUNIT_ASSERT_EQUAL( 44, static_cast<int>((pTr+1)->getDeviate()) );
    }
    bt::time_duration diffAB = momB - momA;
    CPPUNIT_ASSERT( diffAB >= bt::milliseconds( 20 ) );
}
#endif // !LINUX

#ifndef LINUX
void TC_UartThread::CommandTraktTu()
{
	const unsigned int portH = 30111;
	const unsigned int portF = 30222;
	const unsigned int portLoc = 30333;
	const unsigned int portRem = 30444;
	const std::string ipLoc = "127.0.0.1/"+std::to_string(portLoc);
	const std::string ipRem = "127.0.0.1/"+std::to_string(portRem);
	const bt::time_duration TRANSPORT_DELAY = bt::seconds( 20 );

	attic::a_document doc;
	std::string TempDir = ToUtf8(m_tempDir);
	//создать файл abc
	{
		CPPUNIT_ASSERT( doc.load_wide(
			L"<AbcTable>"
			L"<Station Name='xxx'>"
			L"<Steering ModeDC='TRAKT' Gap='60' ESR_code='12345'>"
			L"<LTU Name='S2' Keyboard='K1-K2' Comment='строка 1'>"
			L"<FTU Obsolete='01 02' HexData='01 02' Abonent='45678' />"
			L"</LTU>"
			L"</Steering>"
			L"</Station>"
			L"<DigitalSignature>855F8A3A68D3DFB3348EB9D614D046201FA5DB65EB7BF34260FA4ABA455850144C9B1BF82C4260E88CF4597CE1C09072F568FB3D64A247225B2CB347655129E480A2E3B38105DF48944D0E18C2012CAD3C0A37A0E7E689A640F08726348403A173A38C2810C1E496398110A8CFF3C5F050D8D6FD07F6867622586825A5002857E7734E13D0C1F6FCF8DE3956033991F513D5043E1FD316D9D285FCAE1F00FE5AA3CA869DB1518AD9BC553620F5946348ECCE8974CBDB68646A0390EE048D7C87</DigitalSignature>"
			L"</AbcTable>"));
		if ( TempDir.back()!='\\' )
			TempDir.push_back('\\');

		doc.save_file( FromUtf8(TempDir + "111.abc"));
	}
	// создать канал для обмена файлами
	{
		CPPUNIT_ASSERT( doc.load_utf8(
			"<HaronConfiguration>"
			"  <Channel>"
			"   <Lk Id='Trakt' Hard='UdpIp(LOCAL="+ipLoc+";REMOTE="+ipRem+")' Fmt='trakt'/>"
			"    <Port number='" + std::to_string(portH) + "'/>"
			"  </Channel>"
			"  <BlackBox Path='" + TempDir + "' daysHold='30' limit='50G'/>"
			"  <OutBlackBox Path='" + TempDir + "' daysHold='30'/>"
			"  <Log Path='" + TempDir + "'/>"
			"  <Restrict>"
			"    <Full IP='127.0.0.1/32'/>"
			"    <Read IP='0.0.0.0/0'/>"
			"  </Restrict>"

			"<Tables abc_root='" + TempDir + "'>"
			"<File Name='<abc_root>\\111.abc'/>"
			"</Tables>"
			"</HaronConfiguration>"
			) );
	}

	auto func_lm = []( const std::wstring& /*_text_message*/ ){
	};

	// сокет UPD для получения с. от ядра Харона
	UtDatagram dg( ipRem );

	// Запустить ядро Харона
	Mock::HaronBbx inBbx;
	Mock::HaronBbx outBbx;
	Haron::Core hcore( inBbx, outBbx, func_lm );
	hcore.Configure( doc.document_element().to_str() );
	hcore.StartAllProcess();

	// запустить докер для связи с ядром Харона
	std::shared_ptr<Network::Docker> dkr = make_link( portH, portF );

	std::string TuCmdStr = 
		"<A2F_SELCMD ltu='S2[12345]' id='2147483647' />";

	MsgSoftXml soft3( TuCmdStr, *dkr, L"nothing" );
	CPPUNIT_ASSERT( soft3.put() );


	{
		//Haron-->канал
		MsgHardUdp hardA( dg );
		CPPUNIT_ASSERT( hardA.get( Haron::GID2DIALOG_INTERVAL * 2 + bt::seconds( 1 ) ) );
		bt::ptime momA = bt::microsec_clock::universal_time();
		std::string strA = hardA.content();
		CPPUNIT_ASSERT( strA.size()>Trakt_TU_OTU_t().sizeHeader() + 2);
		VecText res =  reinterpret_cast<Trakt_TU_OTU_t*>(&strA[0])-> VecTexting();
		CPPUNIT_ASSERT( res.get_all().size() == 6 );
		CPPUNIT_ASSERT(  res.get(0) == L"Длина=32" );
		CPPUNIT_ASSERT(  res.get(1) == L"Получатель=45678" );
		CPPUNIT_ASSERT(  res.get(2) == L"Тип=505" );
		CPPUNIT_ASSERT(  res.get(3) == L"Команда='01 02'" );
	}

	{
		//Haron-->Fund
		MsgHardUdp hardA( dg );
		MsgSoftXml softA( *dkr );

		auto raw_dummy = softA.get_raw(bt::seconds( 10 ));
		CPPUNIT_ASSERT( !raw_dummy.empty() );
        auto raw = softA.get_raw( bt::seconds( 10 ) );
        CPPUNIT_ASSERT( !raw.empty() );

		std::string strSrcAnswer = "<LtuAnswer ltu=\"S2[12345]\"><ahc ab=\"45678\" name=\"S2[12345]\" key=\"K1-K2\" cp=\"UDPIP "+ipRem +"\" userHexCode=\"01 02\"/></LtuAnswer>";
		auto strCurAnswer = std::string(raw.begin(), raw.end());
        CPPUNIT_ASSERT( xmlEqual( strSrcAnswer, strCurAnswer ) );
    }
}
#endif // !LINUX

std::shared_ptr<Network::Docker> TC_UartThread::make_link( unsigned short portH, unsigned short portF ) const
{
    Network::addresseeList knownH;
    knownH.push_back( Network::addressee("localhost", portH) );
    std::shared_ptr<Network::Docker> dkr( new Network::Docker(Network::Docker::createId(), portF, knownH));
    CPPUNIT_ASSERT(dkr->start());
    bt::ptime DOCKER_LINK = bt::microsec_clock::universal_time() + bt::seconds(2);
#ifdef _DEBUG
        DOCKER_LINK = bt::microsec_clock::universal_time() + bt::seconds(10);
#endif
    while( dkr->get_channels().size() != 1 && bt::microsec_clock::universal_time() < DOCKER_LINK )
        boost::this_thread::sleep( bt::milliseconds( 100 ) );
    CPPUNIT_ASSERT_MESSAGE( "Сonnect to Haron::Core timeout", dkr->get_channels().size() >= 1 );
    return dkr;
}

