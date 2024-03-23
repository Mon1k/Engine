#pragma once

#include "FileChooser.h"
#include "../input.h"
#include <string>

class SaveDialog : public FileChooser
{
public:
	SaveDialog();

	virtual bool initialize();
	
	std::string getFilename()
	{
		return m_fileName->getValue();
	}

protected:
	Input* m_fileName;
};
