#include "Row.h"

Row::Row()
{
}

Row::Row(string * rowData)
{
	data = rowData;
}

string Row::getCell(int index)
{
	return data[index];
}

bool Row::setCell(int index,string cellContent)
{
	data[index] = cellContent;
	return true;
}

string * Row::getData()
{
	return data;
}
