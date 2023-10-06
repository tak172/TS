#pragma once

#include "../helpful/Attic.h"
#include "../helpful/Badge.h"
#include "HemEventRegistry.h"

class HemEvent
{
public:
	HemEvent( HCode, const BadgeE&, time_t );
	HemEvent( const attic::a_node & event_node );

	virtual ~HemEvent(){}
	
    // свободная функция
    friend attic::a_node operator<< ( attic::a_node parent_node, const HemEvent& hem_event );

	virtual bool operator != ( const HemEvent & hEvent ) const { return !( *this == hEvent ); }
	virtual bool operator == ( const HemEvent & ) const;

	time_t GetTime() const { return born_time; }
	std::wstring GetString() const;
	const BadgeE& GetBadge() const { return strip_badge; }
	HCode GetCode() const{ return eventCode; }

	virtual attic::a_node MakeNode(attic::a_node &parent_node) const = 0;

protected:
	void SetTime( time_t new_time ) { born_time = new_time; }
	virtual attic::a_node operator >>( attic::a_node & ) const;

private:
	void operator << ( const attic::a_node & );
	BadgeE strip_badge;
	time_t born_time;
    HCode eventCode;

	HemEvent();
    void IntegrityCheck() const;
};

// свободная функция сравнения 
bool operator < ( const HemEvent & lhs, const HemEvent & rhs );
