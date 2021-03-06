#pragma once
#include<string>
#include<boost/serialization/vector.hpp>
#include<boost/serialization/string.hpp> 
using namespace std;

enum type { varchar = 0 };

class Column
{
private:
	bool primaryKey;  //主码标识
	string columnName;  //列名
	type columnType; //列的类型
	bool allowNull;  //是否允许为空
	int length;  //列值的长度

	
public:
	Column();
	Column(string colName, type colType, int colLength, bool colPrimaryKey, bool colAllowNull);

	bool setColumnName(string colName);
	string getColumnName();

	bool getPrimaryKey();
	bool setPrimaryKey(bool colPrimaryKey);

	bool getAllowNull();
	bool setAllowNull(bool colAllowNull);

	type getColumnType();
	bool setColumnType(type colType);

	int getLength();
	bool setLength(int colLength);
};
