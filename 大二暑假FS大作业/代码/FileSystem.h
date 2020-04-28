#include<string>
#include<fstream>
#include<cstdlib>
#include<iostream>
#include<memory.h>

using namespace std;
typedef struct UserFileDirectory    //用户文件目录
{
	string FileName;
	int StartAddress;                   //文件在磁盘存储空间的起始地址
	int ProtectCode;                    //文件的保护码
	int FileLength;                      //文件当前的长度
	int MaxFilelength;                  //文件的最大长度
	UserFileDirectory *nextFile;       //指向该用户目录下的下一个文件
}UFD, *UFD_ptr;

typedef struct MainFileDirectory         //主文件目录，存储用户信息
{
	string UserName;
	string UserPassword;
	int userNo;                                //用户编号
	int FileNum;

	UserFileDirectory *userFile;            //用户文件
	MainFileDirectory *nextUser;         //下一个用户 

}MFD, *MFD_ptr;

typedef struct RuntimeFileDirectory     //运行时的文件（打开的文件）
{
	string FileName;
	int StartAddress;
	int ProtectCode;
	int FileLength;                            //文件当前的长度
	int MaxFileLength;
	int OpenWays;                            // 文件打开方式，1代表只读，2代表可读可写
	char content[1024 * 1024];         // 文件内容

	RuntimeFileDirectory *nextFile;
}RFD, *RFD_ptr;

typedef struct Disk
{
	int FreeSpace;               //剩余空间
	int NowAddress;            //开始位置
}DiskNode;

void InitialDisk();
bool InitialMFD();
bool InitialUFD(char*);
bool InitialAFD();
void PrintUFD();
void Help();
bool UserLogin();
void RenameFile();
bool CreateMFD();
bool CreateUFD();
bool DeleteUFD();
bool OpenFile();
void getOpenFile();	
bool CloseFile();
bool ReadFile();
bool WriteFile();
bool SaveMFD();
bool SaveUFD(char*);
void SystemInitial();
void FileSystem();
bool QuitSystem();
void DiskFormat();
