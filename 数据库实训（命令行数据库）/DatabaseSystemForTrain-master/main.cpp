#include<boost/archive/text_oarchive.hpp> 
#include<boost/archive/text_iarchive.hpp> 
#include<boost/serialization/vector.hpp>
#include<boost/serialization/string.hpp>
#include<iostream>
#include<sstream>
#include<string>
#include"DBs.h"
#include"Lexer.h"
#include"Parser.h"
#include"structSerialize.h"
using namespace std;
int main()
{
	DBs* dbs;
	structSerialize tempSS;  //将对象转为结构与将结构转为对象

	sDBs sdbs;  //储存从文件中读取的数据库信息

	//从文件中读取已创建的数据库
	string dbsPath = "E:\\database.txt";
	const char* tempPath = dbsPath.c_str();

	//获取文件大小
	struct _stat info;
	_stat(tempPath, &info);
	int size = info.st_size;

	//读取序列化的数据库，先将磁盘上的数据库读入结构中
	if (size > 0)
	{
		ifstream readDbs(dbsPath);
		boost::archive::text_iarchive ia(readDbs);
		if (readDbs.is_open())
		{
			ia >> sdbs;
		}
		readDbs.close();
		dbs = tempSS.transformStructToObject(sdbs);  //再将结构转换为对象
	}
	else
		dbs = new DBs();


	
	Lexer* lexer = new Lexer("");
	Parser parser(lexer, dbs);
	cout << "开始运行数据库：\n\n";

	bool isEnd = false;
	while (!isEnd)
	{
		char filepath[2048];
		char temp;
		int i = 0;  // 输入字符的个数
		cout << "请输入文件路径或sql语句( 输入#表示结束 输入!退出程序 )：";   // 读取文件路径或sql语句
		temp = cin.get();
		if (temp == '!')
		{
			isEnd = true;
			break;
		}
		while (temp != '#')
		{
			filepath[i++] = temp;
			temp = cin.get();
			if (temp == '!')
			{
				isEnd = true;
				break;
			}
		}
		filepath[i] = '\0';
		lexer = new Lexer(filepath);
		parser.setLexer(lexer);
		parser.program();
	}

	//程序退出时，将序列化的数据库存到磁盘,先将对象转化为结构，再写入磁盘
	sdbs = tempSS.transformObjectToStruct(*dbs);
	ofstream writeDbs(dbsPath);
	boost::archive::text_oarchive oa(writeDbs);
	if (writeDbs.is_open())
		oa << sdbs;
	writeDbs.close();
	return 0;
}