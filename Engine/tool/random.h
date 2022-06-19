#pragma once

class Random
{
public:
	double static randDouble(double M, double N)
	{
		return M + (rand() / (RAND_MAX / (N - M)));
	}
};
