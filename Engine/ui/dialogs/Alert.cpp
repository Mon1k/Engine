#include "Alert.h"
#include "../../Options.h"
#include "../UIManager.h"

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
	m_label->setAlign(Label::ALIGN_CENTER);
	m_label->Initialize(m_width - paddingX * 2, m_height - m_header->m_height - paddingY * 2);
	m_label->Add("Alert", m_x + paddingX, m_y + m_header->m_height + paddingY);

	setVisible(false);
	setId(m_manager->getNextId());

	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		if (this->m_manager) {
			m_manager->remove(this->getId());
		}
	});
}

void Alert::setText(std::string text)
{
	m_label->Add(&text[0]);
}

void Alert::setTextHeader(std::string text)
{
	m_title->Add(&text[0]);
}

bool Alert::onKeyboardPress(InputClass::EventKey event)
{
	if (event.key == DIK_ESCAPE) {
		unfocus();
		hide();
		return true;
	}

	return AbstractGui::onKeyboardPress(event);
}

void Alert::showText(std::string text)
{
	setId(this->m_manager->getNextId());
	initialize();
	setText(text);
	show();
}