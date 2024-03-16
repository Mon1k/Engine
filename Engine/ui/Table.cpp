#include "Table.h"

void Table::initialize()
{
}

bool Table::Render()
{
	for (size_t i = 0; i < m_labels.size(); i++) {
		m_labels[i]->Render();
	}

	return true;
}

void Table::Shutdown()
{
	for (size_t i = 0; i < m_labels.size(); i++) {
		removeChild(m_labels[i]);
	}
}

void Table::addColumn(std::string name, int align)
{
	Column column;
	column.name = name;
	column.align = align;
	m_columns.push_back(column);
}

void Table::addRow(std::vector<std::string> row)
{
	m_rows.push_back(row);
}

void Table::update()
{
	for (size_t i = 0; m_labels.size(); i++) {
		removeChild(m_labels[i]);
	}
	m_labels.clear();

	int countColumns = m_columns.size();
	int widthColumn = m_width / countColumns;
	int heightRow = 28;
	int leftRow = m_x;
	int topRow = m_y;
	int lineRow = 0;

	// add row title from column
	for (size_t i = 0; i < m_columns.size(); i++) {
		Column column = m_columns[i];
		Label* label = new Label;
		addChild(label);
		label->Initialize(widthColumn, heightRow);
		label->setAlign(column.align);
		label->Add(&column.name[0], leftRow + i * widthColumn, topRow + heightRow * lineRow);
		m_labels.push_back(label);
	}
	lineRow++;

	// add rows
	for (size_t i = 0; i < m_rows.size(); i++) {
		for (size_t j = 0; j < m_rows[i].size(); j++) {
			Label* label = new Label;
			addChild(label);
			label->Initialize(widthColumn, heightRow);
			label->setAlign(m_columns[j].align);
			label->Add(&m_rows[i][j][0], leftRow + j * widthColumn, topRow + heightRow * lineRow);
			label->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this, i] {
				this->setSelectedRow(i);
				this->proccesedEventHandlers(EventType::ROW_CHOOSE);
				});
			m_labels.push_back(label);
		}

		lineRow++;
	}
}