#include "Table.h"
#include<vector>
#include<iostream>
using namespace std;
Table::Table()
{
}
Table::Table(string name)
{
	tableName = name;
	rowCount = 0;
}
Table::Table(string name, int colCount)
{
	tableName = name;
	columnCount = colCount;
	rowCount = 0;
}
Table::Table(string name, int colCount, vector<Column> * col)
{
	tableName = name;
	columnCount = colCount;
	rowCount = 0;
	column = *col;

}

Table::Table(string name, int columnCount, int rowCount, vector<Column> column, vector<Row> row)
{
	tableName = name;
	this->columnCount = columnCount;
	this->rowCount = rowCount;
	this->column = column;
	this->row = row;
}

vector<Column> Table::getColumnArray()
{
	return column;
}

vector<Row> Table::getRowArray()
{
	return row;
}

bool Table::setTableName(string name)
{
	tableName = name;
	return true;
}

string Table::getTableName()
{
	return tableName;
}

void Table::setColumn(string columnName)
{
	Column tempColumn;
	tempColumn.setColumnName(columnName);
	column.push_back(tempColumn);
}

bool Table::setColumnCount(int c)
{
	columnCount = c;
	return true;
}

int Table::getColumnCount()
{
	return columnCount;
}

int Table::getRowCount()
{
	return rowCount;
}

bool Table::addRow(Row * newRow)
{
	//确保 newRow 中的主码是唯一的
	vector<int> primaryColumnIndex;  //保存主码列在当前表的列对象数组中的下标

	for (int i = 0; i < columnCount; i++)
	{
		if (column[i].getPrimaryKey())
			primaryColumnIndex.push_back(i);
	}

	for (int i = 0; i < rowCount; i++)  //遍历当前表中的每一行，查看是否存在主码与 newRow 完全相同的行
	{
		int sameValue = 0;
		for (unsigned int j = 0; j < primaryColumnIndex.size(); j++)
		{
			if (row[i].getCell(primaryColumnIndex[j]) == newRow->getCell(primaryColumnIndex[j]))
				sameValue++;
		}
		if (sameValue == primaryColumnIndex.size())
		{
			cout << "Row existed!\n";
			return false;
		}
	}

	this->row.push_back(*(newRow));
	rowCount++;
	return true;
}

bool Table::deleteRow(Where whe)
{
	int col_num = -1;
	for (int i = 0; i < columnCount; i++)
	{
		if (column[i].getColumnName() == whe.getWhereColumnName())
		{
			col_num = i;
			break;
		}
	}
	if (col_num == -1)  //where 语句中的列名错误
	{
		cout << "Column name errror!\n";
		return false;
	}
	else
	{
		int rowCountOld = rowCount;
		for (int i=0;i<row.size();i++)
		{
			if (row[i].getCell(col_num) == whe.getWhereValue())
			{
				row.erase(row.begin()+i);
				i = 0;  //每删除一次 i 就应该初始化一次
				rowCount--;
			}
		}

		if (rowCount == rowCountOld)
		{
			cout << "Do not exist such row.\n";
			return false;
		}
		else
		{
			cout << rowCountOld - rowCount << " rows deleted successfully.\n";
			return true;
		}
			
	}
}

bool Table::deleteAllRow()
{
	row.clear();
	return true;
}

bool Table::addColumn(Column * newColumn)
{
	for (unsigned int i = 0; i < column.size(); i++)
	{
		if (column[i].getColumnName() == newColumn->getColumnName())
		{
			cout << "Column Existed!\n";
			return false;
		}
	}
	column.push_back(*newColumn);
	columnCount++;
	return true;
}

bool Table::deleteColumn(int columnIndex)
{
	column.erase(column.begin() + columnIndex - 1);
	columnCount--;
	return true;
}

bool Table::deleteColumn(string columnName)
{
	for (vector<Column>::iterator it = column.begin(); it != column.end(); it++)
	{
		if (it->getColumnName() == columnName)
		{
			column.erase(it);
			columnCount--;
			return true;
		}
	}
	cout << "Column name error!\n";
	return false;
}

bool Table::deleteRow(int rowIndex)
{
	row.erase(row.begin() + rowIndex - 1);
	rowCount--;
	return true;
}

void Table::showTable()
{
	cout << "*************************************" << endl;

	cout << "表" << tableName << "如下：\n";
	cout << "     ";
	for (int i = 0; i < columnCount; i++)
	{
		string c_name = column[i].getColumnName();
		cout << "\t" << c_name << "\t";
	}
	cout << endl << endl;
	int rowNum = 1;
	for (auto it = row.begin(); it != row.end(); it++)
	{
		cout << rowNum++ << "    ";
		for (int j = 0; j < columnCount; j++)
		{
			cout << "\t" << it->getCell(j) << "\t";
		}
		cout << endl;
	}
	cout << "*************************************" << endl;
}

void Table::setRowArray(vector<Row> tempRows)
{
	row = tempRows;
}
