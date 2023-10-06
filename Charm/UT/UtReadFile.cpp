#include "stdafx.h"

// чтение целого файла
std::string UtFileBody( const std::wstring& filePath )
{
    std::string result;
    std::ifstream fs;
    fs.open( filePath, std::ios::in | std::ios::binary | std::ios::ate );
    auto size = fs.tellg();
    result.resize( static_cast<unsigned int>( size ), '\0' ); // construct string to stream size
    fs.seekg( 0 );
    fs.read( &result[ 0 ], size );
    return result;
}

