#pragma once

#include <string>
#include <cstdarg>
#include <vector>

class String
{
public:
	std::string static ssprintf(const char* format, ...)
	{
        va_list vaArgs;
        va_start(vaArgs, format);

        // reliably acquire the size
        // from a copy of the variable argument array
        // and a functionally reliable call to mock the formatting
        va_list vaArgsCopy;
        va_copy(vaArgsCopy, vaArgs);
        const int iLen = std::vsnprintf(NULL, 0, format, vaArgsCopy);
        va_end(vaArgsCopy);

        // return a formatted string without risking memory mismanagement
        // and without assuming any compiler or platform specific behavior
        std::vector<char> zc(iLen + 1);
        std::vsnprintf(zc.data(), zc.size(), format, vaArgs);
        va_end(vaArgs);

        return std::string(zc.data(), iLen);
	}

    std::vector<std::string> static explode(std::string delimiter, std::string string)
    {
        std::vector<std::string> array;
        array.clear();

        int shift = 0;
        do {
            int nextDelimiter = string.find_first_of(delimiter, shift);
            if (nextDelimiter == -1) {
                nextDelimiter = string.size() - 1;
            }

            std::string scope = string.substr(shift, nextDelimiter - shift - 1);
            array.push_back(scope);
            shift = nextDelimiter + 1;
        } while (shift < string.size());

        return array;
    }
};