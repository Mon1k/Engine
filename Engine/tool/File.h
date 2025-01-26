#pragma once

#include <string>
#include <fstream>
#include <filesystem>

class File
{
public:
	static void filePutContents(std::string filename, std::string data)
	{
		std::ofstream file(filename);
		file << data;
		file.close();
	}

	static bool fileExists(std::string filename)
	{
		return std::filesystem::exists(filename);
	}
};
