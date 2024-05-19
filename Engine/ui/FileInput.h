#pragma once

#include "group.h"
#include "input.h"
#include "button.h"
#include "dialogs/OpenDialog.h"
#include "../tool/Ref.h"

#include <string>

class FileInput : public Group, public ValueRef
{
public:
	FileInput();

	void initialize(int width, int height, int positionX, int positionY);
	virtual void setValueRefLink(const type_info* typeInfo, void* value)
	{
		m_inputPath->setValueRefLink(typeInfo, value);
	}

	void setFloatFormat(std::string format)
	{
		m_inputPath->setFloatFormat(format);
	}
	
	std::string getValue()
	{
		return m_inputPath->getValue();
	}

	OpenDialog* getDialog()
	{
		return m_dialog;
	}

	Input* getInput()
	{
		return m_inputPath;
	}


protected:
	Input* m_inputPath;
	Button* m_buttonChoose;
	OpenDialog* m_dialog;
};