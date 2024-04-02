#pragma once

#include "group.h"
#include "label.h"

#include <string>
#include <vector>

class Table : public Group
{
public:
	Table() : Group()
	{

	}

	void initialize();
	virtual void update();

	void addColumn(std::string name, int align = Label::ALIGN_CENTER);
	void clearColumns()
	{
		m_columns.clear();
	}

	void addRow(std::vector<std::string> row);
	void clearRows()
	{
		m_rows.clear();
	}

	int getSelectedRow()
	{
		return m_selectedRow;
	}
	void setSelectedRow(int row)
	{
		m_selectedRow = row;
	}

	std::vector<std::string> getRow(int row)
	{
		return m_rows[row];
	}

	enum EventType : int {
		ROW_CHOOSE = 65737
	};

protected:
	struct Column {
		std::string name;
		int align = Label::ALIGN_CENTER;
	};

	std::vector<Column> m_columns;
	std::vector<std::vector<std::string>> m_rows;
	int m_selectedRow;

	int m_scrollX = 0, m_scrollY = 0;
};