#pragma once

#include "IPlasmaGenerator.h"

class TestPlasma
	: public IPlasmaGenerator
{
public:
	virtual unsigned char *Generate(unsigned time);
};
