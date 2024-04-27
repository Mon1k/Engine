#include "FileInput.h"
#include "dialogs\OpenDialog.h"
#include "UIManager.h"

FileInput::FileInput() : Group()
{

}

void FileInput::initialize(int width, int height, int positionX, int positionY)
{
	m_x = positionX;
	m_y = positionY;
	m_width = width;
	m_height = height;

	m_inputPath = new Input();
	Group::addChild(m_inputPath);
	m_inputPath->initialize(width - 50, height, positionX, positionY);
	m_inputPath->addEventHandler(AbstractGui::EventType::OBJECT_BLUR, [this] {
		this->fireEvent(AbstractGui::EventType::OBJECT_BLUR);
	});

	m_buttonChoose = new Button();
	Group::addChild(m_buttonChoose);
	m_buttonChoose->initialize(50, height);
	m_buttonChoose->Add("Choose", positionX + width - 50 - 1, positionY);
	m_buttonChoose->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this] {
		m_dialog->show();
		m_dialog->addEventHandler(FileChooser::EventType::FILE_CHOOSE, [this] {
			m_inputPath->setText(m_dialog->getCurrentFilePath());
			m_dialog->hide();
			this->fireEvent(AbstractGui::EventType::OBJECT_BLUR);
		});
	});

	m_dialog = new OpenDialog;
	Group::addChild(m_dialog);
	m_dialog->initialize();
}