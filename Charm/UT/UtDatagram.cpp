#include "stdafx.h"
#include "UtDatagram.h"
#include "../helpful/Log.h"

static std::wstring view( const sockaddr& sa )
{
    typedef unsigned char UCHAR;
    boost::wformat wf(L"%d.%d.%d.%d/%d");
    wf % unsigned(sa.sa_data[2])
    % unsigned(sa.sa_data[3])
    % unsigned(sa.sa_data[4])
    % unsigned(sa.sa_data[5])
    % ( UCHAR(sa.sa_data[0]) * 256 + UCHAR(sa.sa_data[1]) );
    return wf.str();
}

UtDatagram::UtDatagram( std::string _ownAddr )
    : m_socket()
{
    m_socket.SetSocketStyle( PF_INET, SOCK_DGRAM, m_socket.ConvertProtocol( L"udp" ) );
    m_socket.CreateSocket();
    m_socket.SetReuseAddressOption( true );
    m_socket.SetBroadcastOption( true );
    m_ownAddr = CINetSocketAddress::ParseAddress( FromUtf8( _ownAddr ) );
    auto vv = view( m_ownAddr );
    WriteLog( L"Собственный адрес %s", vv );
    m_socket.BindSocket( m_ownAddr );
    //m_socket.SetPeerSocketAddress( CINetSocketAddress::ParseAddress( _remote ) );

    m_worker = boost::thread( boost::bind( &UtDatagram::run, this) );
}

UtDatagram::~UtDatagram()
{
    if ( m_worker.joinable() )
    {
        m_worker.interrupt();
        m_worker.join();
    }
}

bool UtDatagram::empty() const
{
    boost::mutex::scoped_lock lock(m_mtx);
    return m_income.empty();
}

UtDatagram::message UtDatagram::get()
{
    message res;
    boost::mutex::scoped_lock lock(m_mtx);
    if ( !m_income.empty() )
    {
        res = m_income.front();
        m_income.pop();
    }
    else
    {
        ASSERT(!"Queue empty");
        throw std::logic_error("Queue empty");
    }
    return res;
}

void UtDatagram::run()
{
    WriteLog( L"Собственный адрес %s", view(m_ownAddr) );
    try {
        timeval tout;
        tout.tv_sec = 0;
        tout.tv_usec = 1000; // microsec
        while( !boost::this_thread::interruption_requested() ) {
            fd_set socks;
            FD_ZERO( &socks );
            FD_SET( m_socket, &socks );
            int temp = select( 0, &socks, nullptr, nullptr, &tout ); 
            switch( temp ) {
            case 1:
                {
                    unsigned long sz = m_socket.GetDriverWaitingSize();
                    ASSERT( sz );    
                    std::string buf;
                    buf.resize( sz, 0x77 );
                    //WriteLog( L"Ожидает чтения %d байт", sz );
                    sockaddr src_sa;
                    int src_sz = sizeof(src_sa);
                    auto state = recvfrom( m_socket, &*buf.begin(), sz, 0, &src_sa, &src_sz );
                    CINetSocketAddress remAddr(src_sa);
                    //WriteLog( L"Прочитано      %d байт от адреса %s", state, view(remAddr) );
                    //ASSERT( sz == state );
                    if ( state > 0 )
                    {
                        boost::mutex::scoped_lock lock(m_mtx);
                        m_income.push( buf );
                    }
                }
                break;
            case 0:
                // ничего не принято
                break;
            case SOCKET_ERROR:
                {
                    int le = WSAGetLastError();
                    WriteLog( L"Socket err = %d (%X)", le,le );
                }
                break;
            default:
                ASSERT( temp != 1 );
                break;
            }
        }
    } catch( boost::thread_interrupted& /*e*/ ) {
        // normal termination thread
    } catch( ... ) {
        ASSERT( !"exception" );
    }
}
