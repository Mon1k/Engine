#include "Table.h"

void Table::initialize()
{
	addEventHandler(AbstractGui::EventType::SCROLL, [this] {
		for (size_t i = 0; i < m_childs.size(); i++) {
			if (m_childs[i]->m_y + m_childs[i]->m_height > m_y + m_height || m_childs[i]->m_y < m_y) {
				m_childs[i]->hide();
			}
			else {
				m_childs[i]->show();
			}
		}
	});
}

bool Table::onScroll(int diff)
{
	int sensibilityY = 5;
	float shift = (float)diff / sensibilityY;


	float maxChildY = 0;
	for (size_t i = 0; i < m_childs.size(); i++) {
		maxChildY = max(maxChildY, m_childs[i]->m_y);
	}
	if (shift < 0 && m_y + m_height > maxChildY) {
		return false;
	}

	scrollY += -1 * shift;
	if (scrollY < 0) {
		scrollY = 0;
		return false;
	}
	
	for (size_t i = 0; i < m_childs.size(); i++) {
		m_childs[i]->move(0, shift);
	}

	return Group::onScroll(diff);
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