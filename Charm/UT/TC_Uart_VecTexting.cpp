﻿#include "stdafx.h"

#include "TC_Uart_VecTexting.h"
#include "../Haron/uart_VecText.h"
#include "../Haron/uart_hrd.h"
#include "../Haron/uart_dc_Dialog.h"


CPPUNIT_TEST_SUITE_REGISTRATION( TC_UartVecTexting );

void TC_UartVecTexting::Neva()
{
    unsigned char seven[4] = { 0x01,0x00,0x2F,0x31 };

    VecText res = reinterpret_cast<Neva_parcel_t*>(seven) -> VecTexting();
    CPPUNIT_ASSERT( res.get_all().size()==3 );
    CPPUNIT_ASSERT( res.get(0)==L"Кан=3" );
    CPPUNIT_ASSERT( res.get(1)==L"Гр=18" );
    CPPUNIT_ASSERT( res.get(2)==L"10000000000000001111" );
}

void TC_UartVecTexting::Tk_type0()
{
    unsigned char t0[] = { 0x0C, 0x90, 0x0C, 0x0C, 0x15, 0x80, 0x00, 0x48, 0x96, 0x51, 0x5B, 0x2D }; // 0.20

    VecText res = reinterpret_cast<Parcel__t*>(t0) -> VecTexting();
    CPPUNIT_ASSERT( res.get(0)==L"Id=0C" );
    CPPUNIT_ASSERT( res.get(1)==L"Аб=144" );
    CPPUNIT_ASSERT( res.get(2)==L"тип=0" );
    CPPUNIT_ASSERT( res.get(3)==L"Длина=12" );
    CPPUNIT_ASSERT( res.get(4)==L"0C:15 80 00 48 96 51 5B 2D " );
    CPPUNIT_ASSERT( res.get(5)==L"№=91" );
    CPPUNIT_ASSERT( res.get(6)==L"CRC=2D" );
    CPPUNIT_ASSERT( res.get_all().size()==7 );
}

void TC_UartVecTexting::Tk_type1()
{
    unsigned char t1[] = { 0x0C, 0x99, 0x2C, 0x18, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x7F, 0x96 }; // 13.30

    VecText res = reinterpret_cast<Parcel__t*>(t1) -> VecTexting();
    CPPUNIT_ASSERT( res.get(0) == L"Id=0C" );
    CPPUNIT_ASSERT( res.get(1) == L"Аб=153" );
    CPPUNIT_ASSERT( res.get(2) == L"тип=1" );
    CPPUNIT_ASSERT( res.get(3) == L"Длина=12" );
    CPPUNIT_ASSERT( res.get(4) == L"18:00 00 02 00 00 00 7F 96 " );
    CPPUNIT_ASSERT( res.get(5) == L"№=127" );
    CPPUNIT_ASSERT( res.get(6) == L"CRC=96" );
    CPPUNIT_ASSERT( res.get_all().size() == 7 );
}

void TC_UartVecTexting::Tk_type2()
{
    unsigned char t2[] = { 0x0C, 0x4A, 0x47, 0x19, 0x00, 0x33, 0x17             }; // 0.23

    VecText res = reinterpret_cast<Parcel__t*>(t2) -> VecTexting();
    CPPUNIT_ASSERT( res.get_all().size()==7 );
    CPPUNIT_ASSERT( res.get(0)==L"Id=0C" );
    CPPUNIT_ASSERT( res.get(1)==L"Аб=74" );
    CPPUNIT_ASSERT( res.get(2)==L"тип=2" );
    CPPUNIT_ASSERT( res.get(3)==L"Длина=7" );
    CPPUNIT_ASSERT( res.get(4)==L"19:00 33:17 " );
    CPPUNIT_ASSERT( res.get(5)==L"№=51" );
    CPPUNIT_ASSERT( res.get(6)==L"CRC=17" );
}

void TC_UartVecTexting::Tk_type3()
{
    unsigned char t3[] = { 0x0C, 0x13, 0x69, 0x69, 0x20, 0x6B, 0x22, 0x2E, 0x34 }; // 0.24

    VecText res = reinterpret_cast<Parcel__t*>(t3) -> VecTexting();
    CPPUNIT_ASSERT( res.get_all().size()==7 );
    CPPUNIT_ASSERT( res.get(0)==L"Id=0C" );
    CPPUNIT_ASSERT( res.get(1)==L"Аб=19" );
    CPPUNIT_ASSERT( res.get(2)==L"тип=3" );
    CPPUNIT_ASSERT( res.get(3)==L"Длина=9" );
    CPPUNIT_ASSERT( res.get(4)==L"69:20 6B:22 2E:34 " );
    CPPUNIT_ASSERT( res.get(5)==L"№=46" );
    CPPUNIT_ASSERT( res.get(6)==L"CRC=34" );
}

void TC_UartVecTexting::Trakt()
{
    unsigned char ts[] = { 
        0x17, 0x00, 0x11, 0x05, 0x00, 0x02, 0x10, 0x12,
        0xA9, 0x2F, 0x08, 0x48, 0x8A, 0x00, 0x04, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x17, 0xC0        }; // 7.2 (6)

    VecText res = reinterpret_cast<Trakt_TS_t*>(ts) -> VecTexting();
    CPPUNIT_ASSERT( res.get_all().size() == 12 );
    CPPUNIT_ASSERT(  res.get(0) == L"Длина=17" );
    CPPUNIT_ASSERT(  res.get(1) == L"Получатель=200" );
    CPPUNIT_ASSERT(  res.get(2) == L"Тип=511" );
    CPPUNIT_ASSERT(  res.get(3) == L"Отпр=1210" );
    CPPUNIT_ASSERT(  res.get(4) == L"01:0010.0000.0000.0000" );
    CPPUNIT_ASSERT(  res.get(5) == L"02:0000.0000.0000.0000" );
    CPPUNIT_ASSERT(  res.get(6) == L"03:0000.0000.0000.0000" );
    CPPUNIT_ASSERT(  res.get(7) == L"04:0000.0000." );
    CPPUNIT_ASSERT(  res.get(8) == L"18.04.08 05:20:41" );
    CPPUNIT_ASSERT(  res.get(9) == L"Ид=8A" );
    CPPUNIT_ASSERT( res.get(10) == L"Резерв=0" );
    CPPUNIT_ASSERT( res.get(11) == L"CRC=C017" );
}

void TC_UartVecTexting::DialogTs()
{
    unsigned char ts[] = { 
        0x02, 0x00, 0x5C, 0x04, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
        0x6C, 0x6C, 0x5C, 0x02, 0xBF, 0x23, 0x00, 0x00, 0x01, 0xFF, 0xFF, 0xFF, 0x02, 0xFF, 0xFF, 0xFF,
        0x03, 0xFF, 0xFF, 0xFF, 0x04, 0xFF, 0xFF, 0xFF, 0x05, 0xFF, 0xFF, 0xFF, 0x06, 0xFF, 0xFF, 0xFF,
        0x07, 0xFF, 0xFF, 0xFF, 0x08, 0xFF, 0xFF, 0xFF, 0x09, 0xFF, 0xFF, 0xFF, 0x0A, 0xFF, 0xFF, 0xFF,
        0x0B, 0xFF , 0xFF , 0xFF , 0x0C , 0xFF , 0xFF , 0xFF , 0x0D , 0xFF , 0xFF , 0xFF , 0x0E , 0xFF , 0xFF , 0xFF, 
        0x0F, 0xFF , 0xFF , 0xFF , 0x10 , 0xFF , 0xFF , 0xFF , 0x11 , 0xFF , 0xFF , 0xFF , 0x12 , 0xFF , 0xFF , 0xFF, 
        0x13, 0xFF , 0xFF , 0xFF , 0x14 , 0xFF , 0xFF , 0xFF , 0x15 , 0xFF , 0xFF , 0xFF , 0x16 , 0xFF , 0xFF , 0xFF, 
        0x17, 0xFF , 0xFF , 0xFF , 0x21 , 0xFF , 0xFF , 0xFF , 0x22 , 0xFF , 0xFF , 0xFF , 0x23 , 0xFF , 0xFF , 0xFF, 
        0x24, 0x07 , 0x00 , 0x80 , 0x25 , 0x07 , 0x02 , 0x80 , 0x26 , 0x07 , 0x00 , 0x80 , 0x27 , 0x87 , 0xE5 , 0x88, 
        0x28, 0x47 , 0x04 , 0xAB , 0x29 , 0x07 , 0x00 , 0xA8 , 0x2A , 0x17 , 0x12 , 0x82 , 0x2B , 0x07 , 0x00 , 0x81, 
        0x2C, 0x07 , 0xF0 , 0x80 , 0x2D , 0x07 , 0x00 , 0x82 , 0x2E , 0x07 , 0x00 , 0x81 , 0x2F , 0x07 , 0x00 , 0x82, 
        0x30, 0x47 , 0x20 , 0x80 , 0x31 , 0x07 , 0x00 , 0x80 , 0x32 , 0x47 , 0x40 , 0xB4 , 0x33 , 0x07 , 0x04 , 0x80, 
        0x34, 0x2F , 0x00 , 0xA2 , 0x35 , 0xAF , 0x52 , 0x95 , 0x36 , 0x4F , 0x01 , 0x85 , 0x37 , 0xFF , 0xFF , 0xFF, 
        0x41, 0xFF , 0xFF , 0xFF , 0x42 , 0xFF , 0xFF , 0xFF , 0x43 , 0xFF , 0xFF , 0xFF , 0x44 , 0xFF , 0xFF , 0xFF, 
        0x45, 0xFF , 0xFF , 0xFF , 0x46 , 0xFF , 0xFF , 0xFF , 0x47 , 0x47 , 0x00 , 0x81 , 0x48 , 0x07 , 0x04 , 0x80, 
        0x49, 0x87 , 0x40 , 0x80 , 0x4A , 0xFF , 0xFF , 0xFF , 0x4B , 0xFF , 0xFF , 0xFF , 0x4C , 0xFF , 0xFF , 0xFF, 
        0x4D, 0xFF , 0xFF , 0xFF , 0x4E , 0xFF , 0xFF , 0xFF , 0x4F , 0xFF , 0xFF , 0xFF , 0x50 , 0xFF , 0xFF , 0xFF, 
        0x51, 0xFF , 0xFF , 0xFF , 0x52 , 0xFF , 0xFF , 0xFF , 0x53 , 0xFF , 0xFF , 0xFF , 0x54 , 0xFF , 0xFF , 0xFF, 
        0x55, 0xFF , 0xFF , 0xFF , 0x56 , 0xFF , 0xFF , 0xFF , 0x57 , 0xFF , 0xFF , 0xFF , 0x61 , 0xFF , 0xFF , 0xFF, 
        0x62, 0xFF , 0xFF , 0xFF , 0x63 , 0xFF , 0xFF , 0xFF , 0x64 , 0xFF , 0xFF , 0xFF , 0x65 , 0xFF , 0xFF , 0xFF, 
        0x66, 0xFF , 0xFF , 0xFF , 0x67 , 0xFF , 0xFF , 0xFF , 0x68 , 0xFF , 0xFF , 0xFF , 0x69 , 0xFF , 0xFF , 0xFF, 
        0x6A, 0xFF , 0xFF , 0xFF , 0x6B , 0xFF , 0xFF , 0xFF , 0x6C , 0xFF , 0xFF , 0xFF , 0x6D , 0xFF , 0xFF , 0xFF, 
        0x6E, 0xFF , 0xFF , 0xFF , 0x6F , 0xFF , 0xFF , 0xFF , 0x70 , 0xFF , 0xFF , 0xFF , 0x71 , 0xFF , 0xFF , 0xFF, 
        0x72, 0xFF , 0xFF , 0xFF , 0x73 , 0xFF , 0xFF , 0xFF , 0x74 , 0xFF , 0xFF , 0xFF , 0x75 , 0xFF , 0xFF , 0xFF, 
        0x76, 0xFF, 0xFF, 0xFF, 0x77, 0xFF, 0xFF, 0xFF
    };
    VecText res = reinterpret_cast<DialogTs_t*>(ts) -> VecTexting();
    CPPUNIT_ASSERT( res.get_all().size() == 143 );
    CPPUNIT_ASSERT( res.get_all()[0] == L"тип=TS" );
    CPPUNIT_ASSERT( res.get_all()[1] == L"Круг=9151" );
    CPPUNIT_ASSERT( res.get_all()[2] == L"GrSz=4" );
    CPPUNIT_ASSERT( res.get_all()[3] == L"Канал0=везде единицы" );
    CPPUNIT_ASSERT( res.get_all()[4] == L"Кан=1" );
    CPPUNIT_ASSERT( res.get_all()[5] == L"Гр=1" );
    CPPUNIT_ASSERT( res.get_all()[6] == L"11111111111111111111" );
    CPPUNIT_ASSERT( res.get_all()[56] == L"Гр=18" );
    CPPUNIT_ASSERT( res.get_all()[57] == L"00010000000100010110" );
    CPPUNIT_ASSERT( res.get_all()[139] == L"Кан=2" );
    CPPUNIT_ASSERT( res.get_all()[140] == L"Гр=23" );
    CPPUNIT_ASSERT( res.get_all()[141] == L"11111111111111111111" );
    CPPUNIT_ASSERT( res.get_all()[142] == L"Канал3=везде единицы" );
}