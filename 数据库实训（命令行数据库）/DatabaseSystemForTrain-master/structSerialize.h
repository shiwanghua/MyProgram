#pragma once
#include<iostream>
#include<string>
#include<vector> 
#include<boost/serialization/vector.hpp>
#include<boost/serialization/string.hpp>
#include"DBs.h"
#include"Database.h"
#include"Table.h"
#include"Column.h"
#include"Row.h"

using namespace std;

struct sRow
{
	vector<string> data;
	//序列化函数
	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & data;
	}

	//template <typename Archive> friend void serialize(Archive &ar, sRow & p, const unsigned int version);
};

//enum type { varchar = 0 };

struct sColumn
{
	bool primaryKey;  //主码标识
	string columnName;  //列名
	type columnType; //列的类型
	bool allowNull;  //是否允许为空
	int length;  //列值的长度
	//序列化函数
	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & primaryKey;
		ar & columnName;
		ar & columnType;
		ar & allowNull;
		ar & length;
	}
};

struct sTable
{
	string tableName;  //表名
	int columnCount = 0;  //列的个数
	int rowCount = 0;  //该表当前行的个数
	vector<sColumn> column;  //列数组
	vector<sRow> row; //行数组

	//序列化函数
	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & tableName;
		ar & columnCount;
		ar & rowCount;
		ar & column;
		ar & row;
	}
};

struct sDatabase
{
	string name;
	int tableCount = 0;
	vector<sTable> tables;

	//序列化函数
	friend class boost::serialization::access;
	template <typename Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & name;
		ar & tableCount;
		ar & tables;
	}
};

struct sDBs
{
	vector<sDatabase> databasees;  //储存数据库对象的数组

	//序列化函数
	friend class boost::serialization::access;
	template<typename Archive> void serialize(Archive &ar, const unsigned int version)
	{
		ar & databasees;
	}
};

class structSerialize
{
public:
	DBs* dbs;
	//数据库对象对应的结构
	sDBs sdbs;
	vector<sDatabase> sdatabasees;
	vector<sTable> stables;
	vector<sColumn> scolumn;
	vector<sRow> srow;
	structSerialize();

	//将 DBs 对象中的 Table 对象的 Row 对象数组和 Column 对象数组转换为 sRow 和 sColumn 
	sDBs transformObjectToStruct(DBs dbs);
	DBs* transformStructToObject(sDBs sdbs);
};

template<typename Archive>
inline void serialize(Archive & ar, sRow & p, const unsigned int version)
{
	ar & p.data;
}
