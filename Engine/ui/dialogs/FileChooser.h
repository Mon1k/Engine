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

	std::string getCurrentPath();
	void setPath(std::string path);
	std::vector<std::filesystem::directory_entry> getRows();
	std::filesystem::directory_entry getCurrentRow()
	{
		return m_rows[m_currentRow];
	}

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
};