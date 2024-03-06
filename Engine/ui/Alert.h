#pragma once

#include "window.h"
#include "label.h"
#include <string>

class Alert : public Window
{
public:
	Alert();

	void initialize();
	void setText(std::string text);
	void setTextHeader(std::string text);

	virtual void onKeyboardPress(InputClass::EventKey event);
	virtual void show();
	virtual void showText(std::string text);

private:
	Label* m_label;
};