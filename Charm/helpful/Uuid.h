#pragma once
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/nil_generator.hpp>
#include <boost/uuid/string_generator.hpp>
#include <boost/uuid/uuid_io.hpp>

class UUID_t
{
public:
    UUID_t()
        : value( boost::uuids::nil_uuid() )
    {}
    explicit UUID_t( const std::string& s )
    {
        if ( s.empty() )
            value = boost::uuids::nil_uuid();
        else
            value = boost::uuids::string_generator()( s );
    }
    std::string to_string() const
    {
        std::string temp;
        if ( !value.is_nil() )
            temp = boost::uuids::to_string( value );
        return temp;
    }

    friend bool operator== ( const UUID_t& lhs, const UUID_t& rhs )
    {
        return lhs.value == rhs.value;
    }
    friend bool operator< ( const UUID_t& lhs, const UUID_t& rhs )
    {
        return lhs.value < rhs.value;
    }
    friend bool operator!=( const UUID_t& lhs, const UUID_t& rhs )
    {
        return !(lhs == rhs);
    }
    friend bool operator>( const UUID_t& lhs, const UUID_t& rhs )
    {
        return rhs < lhs;
    }
    friend bool operator<=( const UUID_t& lhs, const UUID_t& rhs )
    {
        return !(rhs < lhs);
    }
    friend bool operator>=( const UUID_t& lhs, const UUID_t& rhs )
    {
        return !(lhs < rhs);
    }

private:
    boost::uuids::uuid value;
};

