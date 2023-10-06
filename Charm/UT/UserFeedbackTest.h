#pragma once

#include "../StatBuilder/UserFeedback.h"

class UserFeedbackTest : public UserFeedback
{
public:
	virtual void PrintOut( const std::wstring & user_string )
    {
        TRACE( (user_string+L'\n').c_str() );
    }
};