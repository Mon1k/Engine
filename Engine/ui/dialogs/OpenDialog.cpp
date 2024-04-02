#include "OpenDialog.h"
#include "../button.h"

OpenDialog::OpenDialog() : FileChooser()
{
	m_title = "Open File";
}

bool OpenDialog::initialize()
{
	FileChooser::initialize();

	m_table->m_height -= 36 + 4;

	Button* open = new Button;
	addChild(open);
	open->initialize(75, 28);
	open->Add("Open", m_x + m_width - 75 * 2 - 10, m_y + m_height - 36);
	open->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
		proccesedEventHandlers(FileChooser::EventType::FILE_CHOOSE_GET);
	});

	Button* cancel = new Button;
	addChild(cancel);
	cancel->initialize(75, 28);
	cancel->Add("Cancel", m_x + m_width - 75 - 5, m_y + m_height - 36);
	cancel->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
		proccesedEventHandlers(Window::EventType::WINDOW_CLOSE);
	});

	return true;
}