#pragma once 
#include<boost/serialization/vector.hpp> 
#include<string>
#include<iostream>
#include<vector>
#include<map>
#include"Table.h"
#include"Database.h"
#include"Where.h"
#include"Row.h"

using namespace std;

class DBs
{
private:
	vector<Database> databasees;  //储存数据库对象的数组
public:
	DBs();
	DBs(vector<Database> databasees);
	bool insert(string databaseName, string tableName, map<string, string> insertData);
	bool insert(string databaseName, string tableName, string* insertData);  //insert 语句给出了每一个列对应的值
	bool Delete(string databaseName, string tableName, Where deleteCondition);  //Where 结构体的作用是确定Row对象及其下标
	bool Delete(string databaseName, string tableName);  //无where语句
	void select(string databaseName, string tableName, vector<string> columnName, Where selectCondition);
	void select(string databaseName, string tableName, vector<string> columnName);  //无 where 语句
	bool update(string databaseName, string tableName, map<string, string> updateData, Where updateCondition);
	bool update(string databaseName, string tableName, map<string, string> updateData);  //无 where 语句
	bool createTable(string databaseName, Table* newTable); //调用 Database.addTable(Table*)
	bool dropTable(string databaseName, string tableName);  //调用 Database.deleteTable(String tableName)
	bool createDatabase(Database* database);
	bool dropDatabase(string databaseName);
	int getTableColumnCount(string databaseName, string tableName);
	vector<Database> getDatabasees();
	void showDBs();
};
