#include "Table.h"

void Table::initialize()
{
	addScrollbar(this);
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
	Shutdown();

	int countColumns = m_columns.size();
	int widthColumn = m_width / countColumns;
	int heightRow = 28;
	int leftRow = m_x;
	int topRow = m_y;
	int lineRow = 0;

	// add rows title from columns
	for (size_t i = 0; i < m_columns.size(); i++) {
		Label* label = new Label;
		addChild(label);
		label->Initialize(widthColumn, heightRow);
		label->setAlign(m_columns[i].align);
		label->Add(&m_columns[i].name[0], leftRow + i * widthColumn, topRow + heightRow * lineRow);
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
				this->setSelectedRow((int)i);
				this->proccesedEventHandlers(EventType::ROW_CHOOSE);
			});
		}

		lineRow++;
	}

	proccesedEventHandlers(AbstractGui::EventType::SCROLL);
}