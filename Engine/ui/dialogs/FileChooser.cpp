#include "FileChooser.h"
#include "../../Options.h"
#include "../UIManager.h"
#include "../../tool/Date.h"

FileChooser::FileChooser() : Window()
{
	m_currentRow = 0;
	m_table = 0;
}

bool FileChooser::initialize()
{
	m_width = 640;
	m_height = 480;
	m_x = Options::screen_width / 2 - m_width / 2;
	m_y = Options::screen_height / 2 - m_height / 2;
	bool result = Window::Initialize(m_width, m_height, m_x, m_y);

	setVisible(false);
	setId(m_manager->getNextId());

	addHeader(m_title, HEADER_BUTTON_CLOSE);
	addBody();

	int padding = 5;

	m_table = new Table();
	addChild(m_table);
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

		if (getCurrentRow().is_directory()) {
			std::filesystem::directory_entry entry = getCurrentRow();
			setPath(entry.path().generic_string());
		}
		else {
			proccesedEventHandlers(FileChooser::EventType::FILE_CHOOSE);
		}
	});


	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		m_manager->remove(this->getId());
	});

	setPath(getCurrentPath());

	return result;
}

void FileChooser::show()
{
	AbstractGui::show();
	focus();
}

std::string FileChooser::getCurrentPath()
{
	if (m_path.size() == 0) {
		std::filesystem::path p = std::filesystem::current_path();
		m_path = p.generic_string();
	}

	return m_path;
}

void FileChooser::setPath(std::string path)
{
	m_path = path;
	getRows();
}

std::vector<std::filesystem::directory_entry> FileChooser::getRows()
{
	std::vector<std::string> row;
	std::vector<std::filesystem::directory_entry> rows;
	struct stat result;

	for (const auto& entry : std::filesystem::directory_iterator(m_path)) {
		rows.push_back(entry);
	}

	m_rows.clear();
	m_table->clearRows();

	std::filesystem::path path = m_path;
	std::filesystem::directory_entry entryParent = std::filesystem::directory_entry(path.parent_path());

	// add upper dir
	row.clear();
	row.push_back("..");
	row.push_back("");
	row.push_back("DIR");
	stat(entryParent.path().generic_string().c_str(), &result);
	row.push_back(Date::dateFormat(result));

	m_rows.push_back(entryParent);
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
		row.push_back(entry.path().extension().generic_string());
		row.push_back(std::to_string(entry.file_size()));

		stat(entry.path().generic_string().c_str(), &result);
		row.push_back(Date::dateFormat(result));

		m_rows.push_back(entry);
		m_table->addRow(row);
	}

	m_table->update();

	return m_rows;
}