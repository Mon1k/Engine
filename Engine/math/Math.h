#pragma once

#include <cmath>

class Math
{
public:
	float static setPrecision(float var, int precision)
	{
		float n = std::pow(10.0f, precision);
		return std::round(var * n) / n;
	}
};