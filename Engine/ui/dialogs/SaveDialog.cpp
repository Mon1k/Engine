#include "SaveDialog.h"
#include "../button.h"


SaveDialog::SaveDialog() : FileChooser()
{
	m_title = "Save File";

	m_fileName = 0;
}

bool SaveDialog::initialize()
{
	FileChooser::initialize();

	m_table->m_height -= 36 + 4;

	m_fileName = new Input;
	addChild(m_fileName);
	m_fileName->initialize(m_width - 76 * 2 - 10 - 10, 28, m_x + 10, m_y + m_height - 36);
	m_fileName->addEventHandler(FileChooser::EventType::FILE_CHOOSE, [this] {
		m_table->unfocus();
		m_fileName->setText(getCurrentFilePath());
		m_fileName->focus();
	});

	Button* open = new Button;
	addChild(open);
	open->initialize(75, 28);
	open->Add("Save", m_x + m_width - 75 * 2 - 10, m_y + m_height - 36);
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