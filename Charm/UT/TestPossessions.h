#pragma once

#include "../Augur/Possessions.h"
#include "../Augur/DefType.h"

struct Stech;
class TC_Atopology;
struct NsiBasis;
class TestPossessions : public Possessions
{
public:
	using Possessions::setLimitation;
	TestPossessions( std::shared_ptr<const NsiBasis> );
	void setLimitation( Stech& t,TC_Atopology& tpl);
};
