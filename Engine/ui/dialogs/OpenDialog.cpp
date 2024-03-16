#include "OpenDialog.h"
#include "../UIManager.h"
#include "../../tool/Date.h"

OpenDialog::OpenDialog() : Window()
{

}

bool OpenDialog::initialize(int positionX, int positionY)
{
	m_width = 640;
	m_height = 480;
	bool result = Window::Initialize(m_width, m_height, positionX, positionY);

	setVisible(false);
	setId(m_manager->getNextId());

	addHeader("Open Dialog", HEADER_BUTTON_CLOSE);
	addBody();

	int padding = 5;

	m_table = new Table();
	m_body->addChild(m_table);
	m_table->initialize();
	m_table->m_x = m_body->m_x + padding;
	m_table->m_y = m_body->m_y + m_header->m_height + padding;
	m_table->m_width = m_body->m_width - padding * 2;
	m_table->m_height = m_body->m_height - m_header->m_height - padding * 2;

	m_table->addColumn("Name", Label::ALIGN_LEFT);
	m_table->addColumn("Type", Label::ALIGN_LEFT);
	m_table->addColumn("Size", Label::ALIGN_LEFT);
	m_table->addColumn("Date");
	m_table->addEventHandler(Table::EventType::ROW_CHOOSE, [this] {
		m_currentRow = m_table->getSelectedRow();
		proccesedEventHandlers(OpenDialog::EventType::FILE_CHOOSE);
		m_manager->remove(getId());
		});
	

	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		m_manager->remove(this->getId());
		});

	getCurrentPath();
	getRows();

	return result;
}

bool OpenDialog::Render()
{
	Window::Render();
	m_table->Render();

	return true;
}

void OpenDialog::Shutdown()
{
	m_table->Shutdown();
	Window::Shutdown();
}

std::string OpenDialog::getCurrentPath()
{
	if (m_path.size() == 0) {
		std::filesystem::path p = std::filesystem::current_path();
		m_path = p.generic_string();
	}

	return m_path;
}

std::vector<std::filesystem::directory_entry> OpenDialog::getRows()
{
	std::vector<std::string> row;
	std::vector<std::filesystem::directory_entry> rows;
	struct stat result;

	for (const auto& entry : std::filesystem::directory_iterator(m_path)) {
		rows.push_back(entry);
	}

	m_rows.clear();
	m_table->clearRows();

	// add upper dir
	row.clear();
	row.push_back("..");
	row.push_back("");
	row.push_back("DIR");
	row.push_back(""); // @todo - later add time for this dir upper
	m_rows.push_back(rows[0]); // add parent dir
	m_table->addRow(row);

	// first fill dir
	for (size_t i = 0; i < rows.size(); i++) {
		std::filesystem::directory_entry entry = rows[i];
		if (!entry.is_directory()) {
			continue;
		}

		row.clear();
		row.push_back(entry.path().filename().generic_string());
		row.push_back("");
		row.push_back("DIR");

		stat(entry.path().generic_string().c_str(), &result);
		row.push_back(Date::dateFormat(result));

		m_rows.push_back(entry);
		m_table->addRow(row);
	}

	// second other files
	for (size_t i = 0; i < rows.size(); i++) {
		std::filesystem::directory_entry entry = rows[i];
		if (entry.is_directory()) {
			continue;
		}

		row.clear();
		row.push_back(entry.path().filename().generic_string());
		row.push_back(entry.path().extension().generic_string().substr(1));
		row.push_back(std::to_string(entry.file_size()));

		stat(entry.path().generic_string().c_str(), &result);
		row.push_back(Date::dateFormat(result));

		m_rows.push_back(entry);
		m_table->addRow(row);
	}

	m_table->update();

	return m_rows;
}