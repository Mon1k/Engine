#pragma once

#include <string>
#include <vector>
#include <filesystem>

#include "../window.h"
#include "../Table.h"

class FileChooser : public Window
{
public:
	FileChooser();

	virtual bool initialize();
	virtual void show();

	std::string getCurrentPath();
	void setPath(std::string path);
	std::vector<std::filesystem::directory_entry> getRows();
	std::filesystem::directory_entry getCurrentRow()
	{
		return m_rows[m_currentRow];
	}
	std::string getCurrentFilePath()
	{
		return m_rows[m_currentRow].path().generic_string();
	}

	void addDefailtImageFilters();
	void addFilter(std::string extension);
	bool checkFilter(std::filesystem::directory_entry entry);
		
	enum EventType : int {
		FILE_CHOOSE = 65637,
		FILE_CHOOSE_GET = 65638
	};

protected:
	std::string m_title;
	std::string m_path;
	std::vector<std::filesystem::directory_entry> m_rows;
	int m_currentRow;

	Table* m_table;

	std::vector<std::string> m_filter;
};