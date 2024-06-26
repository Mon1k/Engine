#include "FileChooser.h"
#include "../../Options.h"
#include "../UIManager.h"
#include "../../tool/Date.h"
#include "../../tool/String.h"

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

	m_table->addColumn("Name", Label::ALIGN_LEFT, 50);
	m_table->addColumn("Type", Label::ALIGN_LEFT, 15);
	m_table->addColumn("Size", Label::ALIGN_LEFT, 17);
	m_table->addColumn("Date", Label::ALIGN_CENTER, 18);
	m_table->addEventHandler(Table::EventType::ROW_CHOOSE, [this] {
		m_currentRow = m_table->getSelectedRow();

		if (getCurrentRow().is_directory()) {
			std::filesystem::directory_entry entry = getCurrentRow();
			setPath(entry.path().generic_string());
			getRows();
		}
		else {
			proccesedEventHandlers(FileChooser::EventType::FILE_CHOOSE);
		}
	});


	addEventHandler(Window::EventType::WINDOW_CLOSE, [this] {
		m_manager->remove(this->getId());
	});
	addEventHandler(Window::EventType::WINDOW_OPEN, [this] {
		getRows();
	});

	getCurrentPath();

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
		m_path = p.generic_string()+"/";
	}

	return m_path;
}

void FileChooser::setPath(std::string path)
{
	m_path = path;
}

std::vector<std::filesystem::directory_entry> FileChooser::getRows()
{
	std::vector<std::string> row;
	std::vector<std::filesystem::directory_entry> rows;
	struct stat result;

	if (!std::filesystem::is_directory(m_path)) {
		m_path = "";
		getCurrentPath();
	}

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
	row.push_back(Date::dateFormat(result.st_mtime));

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
		row.push_back(Date::dateFormat(result.st_mtime));

		m_rows.push_back(entry);
		m_table->addRow(row);
	}

	// second other files
	for (size_t i = 0; i < rows.size(); i++) {
		std::filesystem::directory_entry entry = rows[i];
		if (entry.is_directory()) {
			continue;
		}
		if (m_filter.size() > 0 && !checkFilter(entry)) {
			continue;
		}

		row.clear();
		row.push_back(entry.path().filename().generic_string());
		row.push_back(entry.path().extension().generic_string());
		row.push_back(std::to_string(entry.file_size()));

		stat(entry.path().generic_string().c_str(), &result);
		row.push_back(Date::dateFormat(result.st_mtime));

		m_rows.push_back(entry);
		m_table->addRow(row);
	}

	m_table->update();

	return m_rows;
}

void FileChooser::addDefaultImageFilters()
{
	m_filter.push_back("jpg");
	m_filter.push_back("jpeg");
	m_filter.push_back("bmp");
	m_filter.push_back("gif");
	m_filter.push_back("png");
	m_filter.push_back("tga");
	m_filter.push_back("dds");
}

void FileChooser::addDefaultModelsFilters()
{
	m_filter.push_back("ds");
	m_filter.push_back("obj");
	m_filter.push_back("fbx");
	m_filter.push_back("md5mesh");
}


void FileChooser::addFilter(std::string extension)
{
	m_filter.push_back(extension);
}

bool FileChooser::checkFilter(std::filesystem::directory_entry entry)
{
	std::string extension = entry.path().extension().generic_string();
	if (extension.substr(0, 1) == ".") {
		extension = extension.substr(1);
	}

	for (size_t i = 0; i < m_filter.size(); i++) {
		if (m_filter[i] == extension) {
			return true;
		}
	}

	return false;
}

std::string FileChooser::getCurrentFilePath(bool pathIsRelative)
{
	std::string path = std::filesystem::current_path().generic_string()+"/";
	std::string currentRow = getCurrentRow().path().generic_string();
	if (pathIsRelative) {
		return String::replace(currentRow, path, "");
	}

	return path;
}