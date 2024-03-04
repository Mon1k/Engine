#include "Alert.h"
#include "../Options.h"

Alert::Alert()
{

}

void Alert::initialize()
{
	m_width = 300;
	m_height = 150;
	m_x = Options::screen_width / 2 - m_width / 2;
	m_y = Options::screen_height / 2 - m_height / 2;

	Window::Initialize(m_width, m_height, m_x, m_y);
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

void Alert::setTextHeader(std::string text)
{
	m_title->Add(&text[0]);
}

void Alert::onKeyboardPress(InputClass::EventKey event)
{
	if (event.key == DIK_ESCAPE) {
		unfocus();
		hide();
	}

	AbstractGui::onKeyboardPress(event);
}

void Alert::show()
{
	AbstractGui::show();
	AbstractGui::focus();
}