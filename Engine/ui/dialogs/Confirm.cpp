#include "Confirm.h"
#include "../../Options.h"
#include "../UIManager.h"

Confirm::Confirm()
{

}

void Confirm::initialize()
{
	m_width = 300;
	m_height = 150;
	m_x = Options::screen_width / 2 - m_width / 2;
	m_y = Options::screen_height / 2 - m_height / 2;

	Window::Initialize(m_width, m_height, m_x, m_y);
	addHeader("Confirm", HEADER_BUTTON_CLOSE);
	addBody();

	int paddingX = 4;
	int paddingY = 4;

	m_label = new Label();
	m_body->addChild(m_label);
	m_label->setAlign(Label::ALIGN_CENTER);
	m_label->Initialize(m_width - paddingX * 2, m_height - m_header->m_height - paddingY * 2);
	m_label->Add("Confirm", m_x + paddingX, m_y + m_header->m_height + paddingY);

	m_Ok = new Button();
	m_body->addChild(m_Ok);
	m_Ok->initialize();
	m_Ok->Add("OK", m_body->m_x + m_body->m_width - 210, m_body->m_y + m_body->m_height - 30);
	m_Ok->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
		m_Result = true;
		proccesedEventHandlers(AbstractGui::EventType::ACTION_CHOOSE);
		hide();
	});

	m_Cancel = new Button();
	m_body->addChild(m_Cancel);
	m_Cancel->initialize();
	m_Cancel->Add("Cancel", m_body->m_x + m_body->m_width - 105, m_body->m_y + m_body->m_height - 30);
	m_Cancel->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
		m_Result = false;
		proccesedEventHandlers(AbstractGui::EventType::ACTION_CHOOSE);
		hide();
	});

	setVisible(false);
	setId(m_manager->getNextId());

	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		if (this->m_manager) {
			m_manager->remove(this->getId());
		}
	});
}

void Confirm::setText(std::string text)
{
	m_label->Add(&text[0]);
}

void Confirm::setTextHeader(std::string text)
{
	m_title->Add(&text[0]);
}

bool Confirm::onKeyboardPress(InputClass::EventKey event)
{
	if (event.key == DIK_ESCAPE) {
		m_Result = false;
		unfocus();
		hide();
		return true;
	}

	return AbstractGui::onKeyboardPress(event);
}

void Confirm::showText(std::string text)
{
	setId(this->m_manager->getNextId());
	initialize();
	setText(text);
	show();
}