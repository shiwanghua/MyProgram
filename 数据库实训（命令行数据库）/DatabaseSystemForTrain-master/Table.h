#pragma once
#include<boost/serialization/vector.hpp>
#include<boost/serialization/string.hpp> 
#include<string>
#include<vector>  //实现 Table 中 Row 对象的动态增加
#include"Column.h"
#include"Row.h"
#include"Where.h"
using namespace std;
class Table
{
private:
	string tableName;  //表名
	int columnCount = 0;  //列的个数
	int rowCount = 0;  //该表当前行的个数
	vector<Column> column;  //列数组
	vector<Row> row; //行数组

	
public:
	Table();  //默认构造函数
	Table(string name);
	Table(string name, int colCount);
	Table(string name,int colCount, vector<Column>* col);
	Table(string name, int columnCount, int rowCount, vector<Column> column, vector<Row> row);

	vector<Column> getColumnArray();
	vector<Row> getRowArray();

	bool setTableName(string);
	string getTableName();

	void setColumn(string columnName);  //通过列名来添加列

	bool setColumnCount(int c);  //调用之前检查rowCount是否为零
	int getColumnCount();

	int getRowCount();

	bool addRow(Row* newRow);
	bool deleteRow(int rowIndex);  //通过行下标
	bool deleteRow(Where whe);  //通过 where 语句 
	bool deleteAllRow();  //清空表

	bool addColumn(Column* newColumn);
	bool deleteColumn(int columnIndex);  //通过列下标
	bool deleteColumn(string columnName);  //通过列名

	void showTable();

	void setRowArray(vector<Row> tempRows);

};


