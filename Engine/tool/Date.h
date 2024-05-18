#pragma once

#include <string>

class Date
{
public:
	static std::string dateFormat(time_t time, std::string format = "%d.%m.%Y %H:%M:%S")
	{
		struct tm* timeinfo = localtime(&time);
		char buffer[80];
		strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);

		return std::string(buffer);
	}
};