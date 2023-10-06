#pragma warning(disable: 4458)
// 
// GDI+ helper file v1.0
// 
// Written by Zoltan Csizmadia (zoltan_csizmadia@yahoo.com)
// Adapted A.P.
// 

// GDIPLUS_USE_GDIPLUS_MEM:
// GdipAlloc and GdipFree is used for memory operations
// In this case _Crt functions cannot be used to detect 
// memory leaks
//#define GDIPLUS_USE_GDIPLUS_MEM

#ifdef _GDIPLUS_H
#error Gdiplus.h is already included. You have to include this file instead.
#endif

// Fix for STL iterator problem
#define iterator _iterator

#define _GDIPLUSBASE_H

namespace Gdiplus
{
    namespace DllExports
    {
#include "GdiplusMem.h"
    };

    class GdiplusBase
    {
    public:
#ifdef _DEBUG
        static void* __cdecl GdiplusAlloc( size_t nSize, LPCSTR szFileName, int nLine )
        {
#ifdef GDIPLUS_USE_GDIPLUS_MEM
            UNREFERENCED_PARAMETER( szFileName );
            UNREFERENCED_PARAMETER( nLine );
            return DllExports::GdipAlloc( nSize );
#else
            return ::operator new( nSize, szFileName, nLine );
#endif
        }

        static void GdiplusFree( void* pVoid, LPCSTR szFileName, int nLine )
        {
#ifdef GDIPLUS_USE_GDIPLUS_MEM
            UNREFERENCED_PARAMETER( szFileName );
            UNREFERENCED_PARAMETER( nLine );
            DllExports::GdipFree( pVoid );
#else
            ::operator delete( pVoid, szFileName, nLine );
#endif
        }

        void* ( operator new )( size_t nSize )
        {
            return GdiplusAlloc( nSize, __FILE__, __LINE__ );
        }
        void* ( operator new[] )( size_t nSize )
        {
            return GdiplusAlloc( nSize, __FILE__, __LINE__ );
        }
        void* ( operator new )( size_t nSize, LPCSTR lpszFileName, int nLine )
        {
            return GdiplusAlloc( nSize, lpszFileName, nLine );
        }
        void( operator delete )( void* pVoid )
        {
            GdiplusFree( pVoid, __FILE__, __LINE__ );
        }
        void( operator delete[] )( void* pVoid )
        {
            GdiplusFree( pVoid, __FILE__, __LINE__ );
        }
        void operator delete( void* pVoid, LPCSTR lpszFileName, int nLine )
        {
            GdiplusFree( pVoid, lpszFileName, nLine );
        }
#else // _DEBUG

        static void* __cdecl GdiplusAlloc( size_t nSize )
        {
#ifdef GDIPLUS_USE_GDIPLUS_MEM
            return DllExports::GdipAlloc( nSize );
#else
            return ::operator new( nSize );
#endif
        }

        static void GdiplusFree( void* pVoid )
        {
#ifdef GDIPLUS_USE_GDIPLUS_MEM
            DllExports::GdipFree( pVoid );
#else
            ::operator delete( pVoid );
#endif
        }

        void* ( operator new )( size_t nSize )
        {
            return GdiplusAlloc( nSize );
        }
        void* ( operator new[] )( size_t nSize )
        {
            return GdiplusAlloc( nSize );
        }
        void( operator delete )( void* pVoid )
        {
            GdiplusFree( pVoid );
        }
        void( operator delete[] )( void* pVoid )
        {
            GdiplusFree( pVoid );
        }
#endif
    };
};

#include <Gdiplus.h> 

#ifdef _DEBUG
#undef new
#endif

#pragma comment (lib, "Gdiplus.lib")

// STL problem
#undef iterator
#pragma warning(default: 4458)
