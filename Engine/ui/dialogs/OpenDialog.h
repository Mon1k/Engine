#pragma once

#include "FileChooser.h"

class OpenDialog : public FileChooser
{
public:
	OpenDialog();

	virtual bool initialize();
};