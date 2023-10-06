#pragma once

#include "../Haron/UARTDatagramSocket.h"

class UtDatagram
{
public:
    typedef std::string message;

    explicit UtDatagram( std::string _ownAddr );
    ~UtDatagram();
    bool empty() const;
    message get();
private:
    CUARTDatagramSocket   m_socket;
    CINetSocketAddress    m_ownAddr;
    mutable boost::mutex  m_mtx;
    std::queue< message > m_income;
    boost::thread         m_worker;

    void run();
};
