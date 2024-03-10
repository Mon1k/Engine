#pragma once

#include "../window.h"

#include <string>
#include <vector>

class OpenDialog : public Window
{
public:
	OpenDialog();

	bool initialize(int positionX, int positionY);
	std::vector<std::string> getCurrentPath();

protected:
	std::string m_path;
	std::vector<std::string> m_rows;
};