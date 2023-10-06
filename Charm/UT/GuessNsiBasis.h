#pragma once

#include "../helpful/NsiBasis.h"

struct TestGuessNsiBasis : public NsiBasis
{
	TestGuessNsiBasis();
    ~TestGuessNsiBasis();

private:
	void MakeJunctions();
};

typedef std::shared_ptr<const TestGuessNsiBasis> GuessNsiBasisCPtr;