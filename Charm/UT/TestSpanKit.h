#pragma once

#include "../StatBuilder/SpanKit.h"
#include "../Guess/TrainField.h"

class TestSpanKit : public SpanKit
{
	static std::set <const CLogicElement *> mockstrips;

public:
	using SpanKit::FillForGuess;
	TestSpanKit( const EsrKit & spanCode, const std::list <LogicSpanWay> & spanWays, std::shared_ptr<Oddness> _userDefinedIncOddness = nullptr ) : 
		SpanKit( nullptr, nullptr, nullptr, mockstrips, spanCode ), userDefinedIncOddnessPtr( _userDefinedIncOddness )
	{
		SetWays( spanWays );
	}
	std::shared_ptr<Oddness> GetUserDefinedIncOddness() const { return userDefinedIncOddnessPtr; }

private:
	bool AlienLinked( const LogicSpanWay & ) const override;
	std::shared_ptr<Oddness> userDefinedIncOddnessPtr;
};

typedef std::shared_ptr<TestSpanKit> TestSpanKitPtr;