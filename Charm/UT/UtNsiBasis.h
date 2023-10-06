#pragma once

#include <cppunit/extensions/HelperMacros.h>

#include "../helpful/NsiBasis.h"

struct UtNsiBasis : public NsiBasis
{
    UtNsiBasis( const std::string& loadStr );
};
