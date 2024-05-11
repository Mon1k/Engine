#pragma once


#include "window.h"
#include "../label.h"
#include "../button.h"
#include <string>

class Confirm : public Window
{
public:
	Confirm();

	void initialize();
	void setText(std::string text);
	void setTextHeader(std::string text);

	virtual bool onKeyboardPress(InputClass::EventKey event);
	virtual void showText(std::string text);

	bool getResult()
	{
		return m_Result;
	}

private:
	Label* m_label;
	Button* m_Ok;
	Button* m_Cancel;
	bool m_Result;
};