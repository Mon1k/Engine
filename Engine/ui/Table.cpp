#include "Table.h"

void Table::initialize()
{
	addScrollbar(this);
}

void Table::addColumn(std::string name, int align, int percent)
{
	Column column;
	column.name = name;
	column.align = align;
	column.percent = percent;
	m_columns.push_back(column);
}

void Table::addRow(std::vector<std::string> row)
{
	m_rows.push_back(row);
}

void Table::update()
{
	Shutdown();

	int widthColumn;
	int shift = 0;
	int heightRow = 28;
	int leftRow = m_x;
	int topRow = m_y;
	int lineRow = 0;

	// add rows title from columns
	for (size_t i = 0; i < m_columns.size(); i++) {
		Label* label = new Label;
		widthColumn = m_width * m_columns[i].percent / 100;
		addChild(label);
		label->Initialize(widthColumn, heightRow);
		label->setAlign(m_columns[i].align);
		label->Add(&m_columns[i].name[0], leftRow + shift, topRow + heightRow * lineRow);
		shift += widthColumn;
	}
	lineRow++;

	// add rows
	for (size_t i = 0; i < m_rows.size(); i++) {
		shift = 0;
		for (size_t j = 0; j < m_rows[i].size(); j++) {
			Label* label = new Label;
			widthColumn = m_width * m_columns[j].percent / 100;
			addChild(label);
			label->Initialize(widthColumn, heightRow);
			label->setAlign(m_columns[j].align);
			label->Add(&m_rows[i][j][0], leftRow + shift, topRow + heightRow * lineRow);
			label->addEventHandler(AbstractGui::EventType::MOUSE_DOWN, [this, i] {
				this->setSelectedRow((int)i);
				this->proccesedEventHandlers(EventType::ROW_CHOOSE);
			});
			shift += widthColumn;
		}

		lineRow++;
	}

	proccesedEventHandlers(AbstractGui::EventType::SCROLL);
}