#include"DBs.h"
#include"Table.h"
#include"Column.h"
#include"Row.h"
#include"Database.h"
#include<iostream>
#include<vector>
#include<string>
using namespace std;
DBs::DBs()
{
}

DBs::DBs(vector<Database> databasees)
{
	this->databasees = databasees;
}

bool DBs::insert(string databaseName, string tableName, map<string, string> insertData)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{
					string* newRow = new string[currentTables[j].getColumnCount()];
					vector<Column> currentColumnArray = currentTables[j].getColumnArray();
					for (int n = 0; n < currentTables[j].getColumnCount(); n++)
					{
						if (insertData.count(currentColumnArray[n].getColumnName()) > 0)  //如果关联数组中存在这个键
							newRow[n] = insertData[currentColumnArray[n].getColumnName()];
						else
						{
							if (currentColumnArray[n].getAllowNull())
								newRow[n] = "null";
							else
							{
								cout << "Column " << currentColumnArray[n].getColumnName() << " do not allow null!\n";
								return false;
							}
						}
					}
					if (currentTables[j].addRow(new Row(newRow)))
					{
						databasees[i].setTables(currentTables);  //用添加数据后的表组替换原表组
						cout << "Insert data successfully.\n";
						return true;
					}
					else
						return false;
					
				}
			}
			cout << "No such table!\n";
			return false;
		}
	}
	cout << "No such database!\n";
	return false;
}

bool DBs::insert(string databaseName, string tableName, string * insertData)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{
					if (currentTables[j].addRow(new Row(insertData)))
					{
						databasees[i].setTables(currentTables);  //用添加数据后的表组替换原表组
						cout << "Insert data successfully.\n";
						return true;
					}
					else
						return false;
				}
			}
			cout << "No such table!\n";
			return false;
		}
	}
	cout << "No such database!\n";
	return false;
}


bool DBs::Delete(string databaseName, string tableName, Where whe)  //删除表中的一行
{
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == databaseName)
		{
			return it->deleteTableRow(tableName, whe);
		}
	}
	return false;
}

bool DBs::Delete(string databaseName, string tableName)
{
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == databaseName)
		{
			return it->emptyTable(tableName);
		}
	}
	return false;
}


void DBs::select(string databaseName, string tableName, vector<string> columnName, Where selectCondition)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;  //记录当前数据库的表对象数组
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{


					vector<Column> currentColumnArray = currentTables[j].getColumnArray();  //记录当前表的列对象数组
					vector<Row> currentRowArray = currentTables[j].getRowArray();  //记录当前表的行对象数组
					Table newTable = Table("selectStatementReturnTable", (int)columnName.size());  //select 函数的返回值
					for (int n = 0; n < columnName.size(); n++)
						newTable.setColumn(columnName[n]);

					int columnIndex;  //记录 where 语句中的 columnName 所属对象在 Table 的 column 对象数组中的下标
					//找到 where 语句中对应的列在当前表的列数组中的下标 
					for (int n = 0; n < currentTables[j].getColumnCount(); n++)
					{
						if (currentColumnArray[n].getColumnName() == selectCondition.getWhereColumnName())
						{
							columnIndex = n;
							break;
						}
					}
					//找到 select 语句中对应的列在当前表的列数组中的下标
					int* rowIndex = new int[columnName.size()];
					for (int n = 0; n < columnName.size(); n++)
					{
						for (int m = 0; m < currentTables[j].getColumnCount(); m++)
						{
							if (currentColumnArray[m].getColumnName() == columnName[n])
							{
								rowIndex[n] = m;
								break;
							}
						}
					}
					//遍历当前表中的每一行，选出符合 Where 条件的行
					for (int m = 0; m < currentTables[j].getRowCount(); m++)
					{
						if (selectCondition.getWhereValue() == currentRowArray[m].getCell(columnIndex))
						{
							string* newRowData = new string[columnName.size()];
							for (int k = 0; k < columnName.size(); k++)
							{
								newRowData[k] = currentRowArray[m].getCell(rowIndex[k]);
							}
							newTable.addRow(new Row(newRowData));  //将创建的新行加到要返回的表中
						}
					}
					newTable.showTable();  //输出选择语句的结果
					return;
				}
			}
			cout << "No such table!\n";
			return;
		}
	}
	cout << "No such database!\n";
	return;
}

void DBs::select(string databaseName, string tableName, vector<string> columnName)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;  //记录当前数据库的表对象数组
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{
					vector<Column> currentColumnArray = currentTables[j].getColumnArray();  //记录当前表的列对象数组
					vector<Row> currentRowArray = currentTables[j].getRowArray();  //记录当前表的行对象数组
					Table newTable = Table("selectStatementReturnTable", (int)columnName.size());  //select 函数的返回值
					for (int n = 0; n < columnName.size(); n++)
					{
						newTable.setColumn(columnName[n]);
					}
						

					//找到 select 语句中对应的列在当前表的列数组中的下标
					int* rowIndex = new int[columnName.size()];
					for (int n = 0; n < columnName.size(); n++)
					{
						for (int m = 0; m < currentTables[j].getColumnCount(); m++)
						{
							if (currentColumnArray[m].getColumnName() == columnName[n])
							{
								rowIndex[n] = m;
								break;
							}
						}
					}

					//遍历当前表中的每一行，选出对应列的内容创建新行并加入 newTable 中
					for (int m = 0; m < currentTables[j].getRowCount(); m++)
					{
						string* newRowData = new string[columnName.size()];
						for (int k = 0; k < columnName.size(); k++)
						{
							newRowData[k] = currentRowArray[m].getCell(rowIndex[k]);
						}
						newTable.addRow(new Row(newRowData));  //将创建的新行加到要返回的表中
					}
					newTable.showTable();  //输出选择语句的结果
					return;
				}
			}
			cout << "No such table!\n";
			return;
		}
	}
	cout << "No such database!\n";
	return;
}

bool DBs::update(string databaseName, string tableName, map<string, string> updateData, Where updateCodition)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;  //储存当前数据库的表对象数组
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{
					Table currentTable = currentTables[j];  //储存需要更新的表
					vector<Column> currentColumnArray = currentTable.getColumnArray();  //储存需要更新的表的列对象数组
					vector<Row> currentRowArray = currentTable.getRowArray();  //储存需要更新的表的行

					//找到 where 语句中的 column 在该表的 column 对象数组中的下标，并储存在 columnIndex 变量中
					int columnIndex = -1;
					for (int n = 0; n < currentTable.getColumnCount(); n++)
					{
						if (currentColumnArray[n].getColumnName() == updateCodition.getWhereColumnName())
						{
							columnIndex = n;
							break;
						}
					}
					if (columnIndex == -1)
					{
						cout << "Where statement error! No such column!\n";
						return false;
					}

					//找到 set 语句中的 column 在该表的 column 对象数组中的下标，并储存在 rowIndex 数组中
					int* rowIndex = new int[updateData.size()];
					int m = 0;  //内层 for 循环的循环变量，确保当前 column 对象数组只被遍历一次
					for (int n = 0; n < updateData.size(); n++)
					{
						for (; m < currentTable.getColumnCount(); m++)
						{
							if (updateData.count(currentColumnArray[m].getColumnName()) > 0)  //关联数组中存在该 key 值
							{
								rowIndex[n] = m;  //将该 column 对象的下标储存到 rowIndex 数组中
								break;
							}
						}
					}

					//遍历当前表的 Row 对象数组，找到需要修改的 Row 对象并修改
					for (int n = 0; n < currentTable.getRowCount(); n++)
					{
						if (currentRowArray[n].getCell(columnIndex) == updateCodition.getWhereValue())
						{
							for (int m = 0; m < updateData.size(); m++)
							{
								currentRowArray[n].setCell(rowIndex[m], updateData[(currentColumnArray[rowIndex[m]]).getColumnName()]);
							}
						}
					}
					currentTable.setRowArray(currentRowArray);  //用更新后的行数组替换原来的行数组
					currentTables[j] = currentTable;  //用更新完后的表替换原表
					databasees[i].setTables(currentTables);  //用更新后的表对象数组替换原表对象数组
					cout << "Update successfully.\n";
					return true;  //更新成功
				}
			}
			cout << "No such table!\n";
			return false;
		}
	}
	cout << "No such database!\n";
	return false;
}

bool DBs::update(string databaseName, string tableName, map<string, string> updateData)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			vector<Table> currentTables;  //储存当前数据库的表对象数组
			currentTables = databasees[i].getTables();
			for (int j = 0; j < currentTables.size(); j++)
			{
				if (currentTables[j].getTableName() == tableName)
				{
					Table currentTable = currentTables[j];  //储存需要更新的表
					vector<Column> currentColumnArray = currentTable.getColumnArray();  //储存需要更新的表的列对象数组
					vector<Row> currentRowArray = currentTable.getRowArray();  //储存需要更新的表的行

					//找到 set 语句中的 column 在该表的 column 对象数组中的下标，并储存在 rowIndex 数组中
					int* rowIndex = new int[updateData.size()];
					int m = 0;  //内层 for 循环的循环变量，确保当前 column 对象数组只被遍历一次
					for (int n = 0; n < updateData.size(); n++)
					{
						for (; m < currentTable.getColumnCount(); m++)
						{
							if (updateData.count(currentColumnArray[m].getColumnName()) > 0)  //关联数组中存在该 key 值
							{
								rowIndex[n] = m;  //将该 column 对象的下标储存到 rowIndex 数组中
								break;
							}
						}
					}

					//遍历当前表的 Row 对象数组，修改每一个 Row 对象
					for (int n = 0; n < currentTable.getRowCount(); n++)
					{
						for (int m = 0; m < updateData.size(); m++)
						{
							currentRowArray[n].setCell(rowIndex[m], updateData[(currentColumnArray[rowIndex[m]]).getColumnName()]);
						}
					}
					currentTable.setRowArray(currentRowArray);  //用更新后的行数组替换原来的行数组
					currentTables[j] = currentTable;  //用更新完后的表替换原表
					databasees[i].setTables(currentTables);  //用更新后的表对象数组替换原表对象数组
					cout << "Update successfully.\n"; 
					return true;  //更新成功
				}
			}
			cout << "No such table!\n";
			return false;
		}
	}
	cout << "No such database!\n";
	return false;
}

bool DBs::createTable(string databaseName, Table * newTable)
{
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == databaseName)
		{
			return it->addTable(newTable);
		}
	}
	cout << "Database name false!\n";
	return false;
}

bool DBs::dropTable(string databaseName, string tableName)
{
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == databaseName)
		{
			return it->dropTable(tableName);
		}
	}
	cout << "Database name false!\n";
	return false;
}

bool DBs::createDatabase(Database* database)
{
	bool databaseExisted = false;
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == database->getDatabaseName())
		{
			databaseExisted = true;
			break;
		}
	}
	if (databaseExisted)
	{
		cout << "DataBase existed!\n";
		return false;
	}
	else
	{
		databasees.push_back(*database);
		cout << "Create database successfully.\n";
		return true;
	}
}

bool DBs::dropDatabase(string databaseName)
{
	for (auto it = databasees.begin(); it != databasees.end(); it++)
	{
		if (it->getDatabaseName() == databaseName)
		{
			databasees.erase(it);
			cout << "Drop database " << databaseName << " successfully.\n";
			return true;
		}
	}
	cout << "Do not exist this database!\n";
	return false;
}

int DBs::getTableColumnCount(string databaseName, string tableName)
{
	for (int i = 0; i < databasees.size(); i++)
	{
		if (databasees[i].getDatabaseName() == databaseName)
		{
			for (int j = 0; j < databasees[i].getTables().size(); j++)
			{
				if ((databasees[i].getTables())[j].getTableName() == tableName)
				{
					return (databasees[i].getTables())[j].getColumnCount();
				}
			}
			cout << "No such table!\n";
			return -1;
		}
	}
	cout << "No such database!\n";
	return -1;
}

vector<Database> DBs::getDatabasees()
{
	return databasees;
}

void DBs::showDBs()
{
	for (auto i = databasees.begin(); i < databasees.end(); ++i)
	{
		cout << endl << i->getDatabaseName() << endl;
		i->showDatabase();
	}
		
}
