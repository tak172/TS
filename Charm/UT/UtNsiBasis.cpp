#include "stdafx.h"
#include "UtNsiBasis.h"
#include "../helpful/Amqp/RestClient.h"
#include "../helpful/Serialization.h"
#include "../helpful/NsiBasis.h"
#include "../helpful/Attic.h"

UtNsiBasis::UtNsiBasis( const std::string& loadStr ) : NsiBasis( Rest::Response( std::move( std::unordered_map<std::string, std::string>() ) ) )
{
    attic::a_document doc;
    if ( doc.load_utf8(loadStr) )
    {
        std::unordered_map<std::string, std::string> resources;

        for (attic::a_node resourceNode : doc.document_element().children())
        {
            //Пока только Junctions
            if ( auto _junctionsPtr = deserialize<Junctions>( resourceNode.pretty_str() ) )
                junctionsPtr = _junctionsPtr;
            else if ( auto _stationRegistryPtr = deserialize<StationsRegistry>( resourceNode.pretty_str() ) )
                stationsRegPtr = _stationRegistryPtr;
        }
    }
}

