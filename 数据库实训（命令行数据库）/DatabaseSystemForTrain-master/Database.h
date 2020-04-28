#pragma once
#include<boost/serialization/vector.hpp>
#include<boost/serialization/string.hpp> 
#include<vector>
#include"Table.h"
#include"Where.h"
using namespace std;
class Database
{
private:
	string name;
	int tableCount=0;
	vector<Table> tables;

public:
	
	Database(string databaseName);
	Database(string databaseName, int tableCount, vector<Table> tables);
	bool addTable(Table* newTable);
	bool deleteTableRow(string tableName, Where);
	bool emptyTable(string tableName);
	bool dropTable(string tableName);

	vector<Table> getTables();
	bool setDatabaseName(string databaseName);
	string getDatabaseName();
	int getTableCount();

	void setTables(vector<Table> tempTables);

	void showDatabase();
};

