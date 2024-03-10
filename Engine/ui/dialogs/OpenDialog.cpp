#include "OpenDialog.h"
#include "../UIManager.h"

#include <filesystem>

OpenDialog::OpenDialog() : Window()
{

}

bool OpenDialog::initialize(int positionX, int positionY)
{
	m_width = 640;
	m_height = 480;
	bool result = Window::Initialize(m_width, m_height, positionX, positionY);

	addHeader("Open Dialog", HEADER_BUTTON_CLOSE);
	addBody();

	setVisible(false);
	setId(m_manager->getNextId());

	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		if (this->m_manager) {
			m_manager->remove(this->getId());
		}
		});

	getCurrentPath();

	return result;
}

std::vector<std::string> OpenDialog::getCurrentPath()
{
	if (m_path.size() == 0) {
		std::filesystem::path p = std::filesystem::current_path();
		m_path = p.generic_string();
	}

	return m_rows;
}