#pragma once

#include <string>

class Date
{
public:
	static std::string dateFormat(struct stat time, std::string format = "%d.%m.%Y %H:%M:%S")
	{
		struct tm* timeinfo = localtime(&time.st_ctime);
		char buffer[80];
		strftime(buffer, sizeof(buffer), format.c_str(), timeinfo);

		return std::string(buffer);
	}
};