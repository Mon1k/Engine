#include "Alert.h"
#include "../Options.h"

Alert::Alert()
{

}

void Alert::initialize()
{
	Window::Initialize(200, 100, Options::screen_width / 2 - 100, Options::screen_height / 2 - 50);
	addHeader("Alert", HEADER_BUTTON_CLOSE);
	addBody();

	int paddingX = 4;
	int paddingY = 4;

	m_label = new Label();
	m_body->addChild(m_label);
	m_label->Initialize(m_width - paddingX * 2, m_height - paddingY * 2);
	m_label->Add("Alert", m_x + paddingX, m_y + paddingY);

	setVisible(false);
}

void Alert::setText(std::string text)
{
	m_label->Add(&text[0]);
}