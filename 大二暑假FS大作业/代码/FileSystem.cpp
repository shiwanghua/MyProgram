#include"FileSystem.h"
#define MaxDiskLength 1024*1024

const int MaxUserNum = 5;
const int MaxOpenFile = 8;
int UserNum = 0;
int FileOpened = 0;

char login = false; // 判断是否已登录
char UserName[20];
char Password[20];
char UserPassword[20];
char DiskContent[MaxDiskLength];

// 指向三种目录的四个指针
MFD* pMFD = NULL; 
UFD* pUFD = NULL;
RFD* pRFD = NULL;
MFD* pCurrentUser = NULL;
Disk DiskHead;  //磁盘头

//初始化磁盘和各种目录
void InitialDisk()
{
	DiskHead.FreeSpace = MaxDiskLength;
	DiskHead.NowAddress = 0;
	memset(DiskContent, '\0', sizeof(char));
}

bool InitialMFD()  // 将文本文件（数据）读入到数据结构中（内存中）
{
	pMFD = new MFD; // 为pMFD指针分配空间
	//第一个MFD结构里不存数据，从第二个开始存
	pMFD->userFile = NULL;
	pMFD->UserName = "";
	pMFD->UserPassword = "";
	pMFD->userNo = 0;
	pMFD->FileNum = 0;

	ifstream mfd("MFD.txt"); //主文件目录输入流对象
	if (!mfd)
	{
		cout << "主目录MFD文件打开失败！\n";
		pMFD->nextUser = NULL;
		return false;
	}

	// 打开了MFD文本文件
	MFD* temp = pMFD;  // 用另一个MFD指针将读取的数据搬运到数据结构中
	while (!mfd.eof())       // MFD文本文件中还有数据未读入内存
	{
		temp->nextUser = new MFD;   // 第一个MFD结构里不存数据，从第二个开始存，这样使得为nextUser分配内存更方便
		temp = temp->nextUser;
		mfd >> temp->UserName >> temp->UserPassword >> temp->userNo>>temp->FileNum;  // 读入用户信息
		temp->userFile = NULL;         // 用户文件的初始化交给InitialUFD函数处理
		temp->nextUser = NULL;       // 假设没有下一个用户了，即跳出wile循环，为最后一个用户的MFD数据结构收尾
	}

	mfd.close();
	return true;
}

bool InitialUFD(char* FileName)
{
	pUFD = new UFD;
	// 第一个UFD结构也不存数据
	pUFD->FileLength = 0;
	pUFD->FileName = "";
	pUFD->MaxFilelength = 0;
	pUFD->ProtectCode = -1;
	pUFD->StartAddress = 0;

	ifstream ufd(FileName);
	if (!ufd)
	{
		cout << "该用户暂未创建文件！\n\n";
		pUFD->nextFile = NULL;
		return false;
	}

	int readFileNum = pCurrentUser->FileNum;
	UFD* tempUFD = pUFD;
	int fileNum = 0;
	char content[MaxDiskLength];   // 存储文件内容
	while (readFileNum--)
	{
		memset(content, '\0', sizeof(content));
		tempUFD->nextFile = new UFD;
		tempUFD = tempUFD->nextFile;
		fileNum++;  

		ufd >> tempUFD->FileName >> tempUFD->StartAddress >> tempUFD->ProtectCode >> tempUFD->FileLength >> tempUFD->MaxFilelength;
		tempUFD->nextFile = NULL;

		ufd.read(content, tempUFD->FileLength+1);
		for (int i = tempUFD->StartAddress, j = 0; j < tempUFD->FileLength+1; i++, j++)  // 把文件内容读到磁盘数组
			DiskContent[i] = content[j];
	
		// 更新磁盘头数据
		DiskHead.FreeSpace -= tempUFD->MaxFilelength;
		DiskHead.NowAddress += tempUFD->MaxFilelength;
	}

	MFD* tempMFD = pMFD->nextUser;
	string userName;
	while (tempMFD)   // 将主文件MFD的用户文件指针指向正确位置
	{
		userName = tempMFD->UserName + ".txt";
		if (userName == FileName)
		{
			tempMFD->userFile = pUFD->nextFile;
			tempMFD->FileNum = fileNum;
			break;
		}
		tempMFD = tempMFD->nextUser;
	}
	
	ufd.close();
	return true;
}

bool InitialAFD()
{
	pRFD = new RFD;
	pRFD->FileName = "";
	pRFD->StartAddress = -1;
	pRFD->ProtectCode = -1;
	pRFD->FileLength = 0;
	pRFD->MaxFileLength = 0;
	pRFD->OpenWays = -1;
	memset(pRFD->content, '\0', sizeof(pRFD->content));
	pRFD->nextFile = NULL;
	return true;
}

//------------------------------------------------------------------------

void PrintUFD()  //打印用户的文件目录信息
{
	UFD*temp = pCurrentUser->userFile;
	if (!temp)
	{
		cout << "\n该用户没有创建文件! \n" << endl;
		return;
	}

	cout << "\n磁盘剩余空间：" << DiskHead.FreeSpace << endl;
	cout << "\n用户名：" << pCurrentUser->UserName << endl;
	cout << "用户编号：" << pCurrentUser->userNo << endl;
	cout << "用户文件数量：" << pCurrentUser->FileNum << endl << endl;
	cout << "文件名\t\t起始物理地址\t保护码\t文件长度\t最大文件长度" << endl;
	while (temp)
	{
		cout <<"\n"<< temp->FileName<<"\t\t" << static_cast<void*> (&DiskContent[temp->StartAddress] )<< "\t";
		int ptc = temp->ProtectCode;
		if (ptc == 0)
			cout << "---\t";
		else if (ptc == 1)
			cout << "r-x\t";
		else if (ptc == 2)
			cout << "-wx\t";
		else if (ptc == 3)
			cout << "rwx\t";
		cout << temp->FileLength << "\t\t" << temp->MaxFilelength << "\n\n";

		temp = temp->nextFile;
	}

}

void Help()
{
	system("color B1");
	cout << "\n*********************二级文件系统--命令方法**********************" << endl;
	cout << "*\t\t命令			  说明			*" << endl;
	cout << "*                                                               * " << endl;
	cout << "*\t\tlogin             \t登录系统		*" << endl;
	cout << "*\t\tlogout 			退出登录		*" << endl;
	cout << "*\t\tdir	        \t显示目录		*" << endl;
	cout << "*\t\tcreate			创建文件		*" << endl;
	cout << "*\t\tdelete			删除文件		*" << endl;
	cout << "*\t\topen			打开文件		*" << endl;
	cout << "*\t\tgetOpenFile             查看已打开文件          *" << endl;
	cout << "*\t\tclose			关闭文件		*" << endl;
	cout << "*\t\tread			读取文件		*" << endl;
	cout << "*\t\twrite			写入文件		*" << endl;
	cout << "*\t\trename		\t重命名文件		*" << endl;
	cout << "*\t\thelp			帮助菜单		*" << endl;
	cout << "*\t\tcls 			清除屏幕		*" << endl;
	cout << "*\t\tquit			退出系统		*" << endl;
	cout << "*****************************************************************\n" << endl;
	return;
}

bool UserLogin()
{
	while (true)                                      // 直到输入正确为止
	{
		cout << "请输入用户名：";
		cin >> UserName;

		int countInput = 0;                        // 最多尝试输入三次密码
		MFD *tempMFD = pMFD->nextUser;
		while (tempMFD)
		{
			if (UserName == tempMFD->UserName)
			{
				while (true)                                  //  找到用户名对应的MFD结构
				{
					cout << "请输入密码：";
					cin >> Password;

					if (Password == tempMFD->UserPassword)
					{
						cout << "\n登录成功!\n" << endl;
						pCurrentUser = tempMFD;       // 指向当前用户
						char userFileName[20];
						memset(userFileName, '\0', sizeof(userFileName));
						strcpy(userFileName, UserName);
						strcat(userFileName, ".txt");      // 文件全名	
						if (InitialUFD(userFileName))     // 获取用户文件信息
							PrintUFD();                      // 打印用户信息
						return true;
					}
					else {
						if (countInput == 3)
						{
							cout << "三次密码输入错误，请稍候登录\n";
							return false;
						}
						else
						{
							cout << "密码错误，登录失败 !\n";
							countInput++;
						}
					}  // end else
				}      // end inner while
			}          // end if

			tempMFD = tempMFD->nextUser;  // 遍历每一个用户
		}              // End outer while

		cout << "不存在该用户，请重新输入用户名！"<<endl;
	}
}

void RenameFile()
{
	string oldFileName;
	string newFileName;
	UFD* tempUFD;
	bool answer = true;
	while (answer)
	{
		cout << "请输入需要修改的文件名：";
		cin >> oldFileName;

		tempUFD = pUFD->nextFile;
		while (tempUFD)
		{
			if (tempUFD->FileName == oldFileName)
			{
				RFD* tempRFD = pRFD->nextFile;
				while (tempRFD)
				{
					if (tempRFD->FileName == oldFileName)
					{
						cout << "当前文件已打开，请关闭后再重命名！\n";
						return;
					}
					tempRFD = tempRFD->nextFile;
				}

				cout << "请输入新的文件名：";
				cin >> newFileName;
				tempUFD->FileName = newFileName;
				cout << "修改成功";
				PrintUFD();
				return;
			}
			tempUFD = tempUFD->nextFile;
		}

		cout << "您输入的文件名不存在，是否继续输入?<y/n>";
		char input;
		cin >> input;
		if (input == 'n')
			answer = false;
	}
}

bool CreateMFD()
{
	if (MaxUserNum == UserNum)
	{
		cout << "已经创建" << MaxUserNum << "个用户，达到上限！\n";
		return false;
	}
	string userName;
	string password;
	MFD* temp = pMFD->nextUser;
	
	cout << "请输入用户名：";
label1:
	cin >> userName;
	if (temp)  // 如果非空就查重
	{
		while (temp)                                        // 用户名查重
		{
			if (userName == temp->UserName)
			{
				cout << "已有该用户名，请重新输入：";
				temp = pMFD->nextUser;
				goto label1;
			}

			if (!temp->nextUser)  // 遍历到最后一个用户, 说明无重复，即时跳出while，以保留此指针的值
				break;
			temp = temp->nextUser;
		}
	}
	else
		temp = pMFD;

	cout << "请输入用户密码：";
	cin >> password;
	temp->nextUser = new MFD;
	temp = temp->nextUser;
	temp->nextUser = NULL;
	temp->userFile = NULL;
	temp->UserName = userName;
	temp->UserPassword = password;
	temp->userNo = ++UserNum;
	temp->FileNum = 0;
	return true;
}

bool CreateUFD()
{
	string fileName;
	UFD* temp = pUFD->nextFile;
	int startAddress;
	int protectCode;
	int fileLength = 0;
	int maxFileLength;

	cout << "请输入新创建的文件名：";
label2:
	cin >> fileName;
	if (temp)
	{
		while (temp)
		{
			if (temp->FileName == fileName)
			{
				cout << "文件名重复，请重新输入：";
				temp = pUFD->nextFile;
				goto label2;
			}

			if (!temp->nextFile)    // 此时temp指向该用户的最后一个文件UFD
				break;
			temp = temp->nextFile;
		}
		DiskHead.NowAddress = temp->StartAddress + temp->MaxFilelength;  // 移动磁头到最后一个文件的末尾
	}
	else
		temp = pUFD;

	startAddress = DiskHead.NowAddress;

	cout << "请输入文件读写保护码(0,1,2,3)：";
	cin >> protectCode;
	while (protectCode < 0 || protectCode>3)
	{
		cout << "请输入0,1,2或3：";
		cin >> protectCode;
	}

	cout << "请输入文件最大容量：";
label3:
	cin >> maxFileLength;
	if (maxFileLength + DiskHead.NowAddress > MaxDiskLength)
	{
		cout << "磁盘空间不足！还剩" << MaxDiskLength - DiskHead.NowAddress << "个字符的空间！" << endl;
		cout << "请重新输入文件最大容量：";
		goto label3;
	}
	DiskHead.FreeSpace -= maxFileLength;

	// 写入初始文件内容
	char input;
	cout << "请输入文件内容（最大容量为" << maxFileLength << "，输入$再按回车即可结束）：\n";
	input = cin.get();
	while (input!='$')  // 输入文件内容，输入 $ 代表结束
	{
		DiskContent[startAddress + fileLength++] = input;
		if (fileLength == maxFileLength)
		{
			cout << "输入内容达到文件最大容量，输入停止！\n";
			break;
		}
		input = cin.get();
	}

	temp->nextFile = new UFD;
	temp = temp->nextFile;
	// 设置文件属性，前面都是按顺序一个一个获取文件属性
	temp->FileName = fileName;
	temp->StartAddress = startAddress;
	temp->ProtectCode = protectCode;
	temp->FileLength = fileLength;
	temp->MaxFilelength = maxFileLength;
	temp->nextFile = NULL;

	pCurrentUser->FileNum++;
	if (pCurrentUser->FileNum == 1)  // 用户的第一个文件
		pCurrentUser->userFile = temp;
	cout << "新建文件成功！" << endl;
	PrintUFD();                      // 打印用户信息
	return true;
}

bool DeleteUFD()
{
	string fileName;
	cout << "请输入要删除的文件名：";
	cin >> fileName;
	RFD* tempRFD = pRFD->nextFile;
	while (tempRFD)
	{
		if (tempRFD->FileName == fileName)
		{
			cout << "该文件处于打开状态，请先关闭，再删除。\n" << endl;
			return false;
		}
		tempRFD = tempRFD->nextFile;
	}

	UFD* tempUFD = pUFD;
	while (tempUFD->nextFile)
	{
		if (tempUFD->nextFile->FileName == fileName)
		{
			/*// 删除文本文件-- 可用于删除用户
			string tempFileName = tempDelete->FileName + ".txt";
		    char* tempFN = new char(1+tempFileName.length());
			strcpy(tempFN, tempFileName.c_str());
			remove(tempFN);*/

			UFD* tempDelete = tempUFD->nextFile;
			tempUFD->nextFile = tempDelete->nextFile;
			pCurrentUser->userFile = pUFD->nextFile;  // 如果删除的是第一个位置的文件，就需要改变pCurrentUser指针的userFile指针
			pCurrentUser->FileNum--;

			// 处理磁盘
			DiskHead.FreeSpace += tempDelete->MaxFilelength;
			for (int i = tempDelete->StartAddress; i < tempDelete->StartAddress + tempDelete->MaxFilelength; i++)
				DiskContent[i] = '\0';

			delete tempDelete;
			cout << "文件成功删除。" << endl;
			PrintUFD();
			return true;
		}
		tempUFD = tempUFD->nextFile;
	}

	cout << "不存在该文件！" << endl;
	return false;
}

bool OpenFile()
{
	if (FileOpened == MaxOpenFile)
	{
		cout << "已经打开8个文件，达到了打开文件的上限。可以先关闭其他文件再尝试打开文件！\n";
		return false;
	}
	string fileName;
	cout << "\n请输入要打开的文件名：";
	cin >> fileName;

	UFD* tempUFD = pUFD->nextFile;
	RFD* tempRFD = pRFD->nextFile;
	while (tempRFD)
	{
		if (tempRFD->FileName == fileName)
		{
			cout << "该文件已经打开！\n";
			return true;
		}
		if (!tempRFD->nextFile)  // 这是tempRFD指向最后一个打开的文件
			break;
		else
			tempRFD = tempRFD->nextFile;
	}
	if (!tempRFD)   // 如果本来就没有打开的文件！
		tempRFD = pRFD;

	while (tempUFD)
	{
		if (tempUFD->FileName == fileName)
		{
			if (tempUFD->ProtectCode == 0)
			{
				cout << "保护码为0，不能打开。\n\n";
				return false;
			}
			else if (tempUFD->ProtectCode == 1)
			{
				tempRFD->nextFile = new RFD;
				tempRFD = tempRFD->nextFile;
				tempRFD->FileName = fileName;
				tempRFD->StartAddress = tempUFD->StartAddress;
				tempRFD->ProtectCode = tempUFD->ProtectCode;
				tempRFD->FileLength = tempUFD->FileLength;
				tempRFD->MaxFileLength = tempUFD->MaxFilelength;
				tempRFD->OpenWays = 1;
				tempRFD->nextFile = NULL;
				for (int i = 0, j = tempRFD->StartAddress; i < tempRFD->FileLength; i++, j++)
					tempRFD->content[i] = DiskContent[j];
				FileOpened++;
				cout << "此文件打开成功，保护码为1，只能读，不能写。\n";
				return true;
			}
			else if (tempUFD->ProtectCode == 3)
			{
				cout << "以只读方式打开文件请输入1，以可读可写方式打开文件输入2：";
				int input;
				cin >> input;
				while (input != 1 && input != 2)
				{
					cout << "您的输入有误，请输入1或2：";
					cin >> input;
				}
				tempRFD->nextFile = new RFD;
				tempRFD = tempRFD->nextFile;
				tempRFD->FileName = fileName;
				tempRFD->StartAddress = tempUFD->StartAddress;
				tempRFD->ProtectCode = tempUFD->ProtectCode;
				tempRFD->FileLength = tempUFD->FileLength;
				tempRFD->MaxFileLength = tempUFD->MaxFilelength;
				tempRFD->OpenWays = input;
				tempRFD->nextFile = NULL;
				for (int i = 0, j = tempRFD->StartAddress; i < tempRFD->FileLength; i++, j++)
				{
					tempRFD->content[i] = DiskContent[j];
				}
				FileOpened++;
				cout << "此文件打开成功，保护码为"<< tempUFD->ProtectCode<<"，打开方式为";
				if (input == 1)
					cout << "只读。\n\n";
				else
					cout << "可读可写。\n\n";
				return true;
			}
		}
		tempUFD = tempUFD->nextFile;
	}

	cout << "不存在该文件！打开失败! \n";
	return false;
}

void getOpenFile() // 输入已打开的文件的文件名
{
	RFD* tempRFD = pRFD->nextFile;

	if (tempRFD)
	{
		cout << "\n已打开的文件有：\n       \t\t";
		while (tempRFD)
		{
			cout << tempRFD->FileName << "\n       \t\t";
			tempRFD = tempRFD->nextFile;
		}
	}
	else cout << "\n没有打开的文件！\n\n";
}

bool CloseFile()
{
	string fileName;
	UFD* tempUFD = pUFD->nextFile;
	RFD* tempRFD = pRFD;

	cout << "请输入要关闭的文件名：";
	cin >> fileName;
	
	while (tempRFD->nextFile)
	{
		if (tempRFD->nextFile->FileName == fileName)
		{
			RFD*tempRFDDelete = tempRFD->nextFile;
			tempRFD->nextFile = tempRFDDelete->nextFile;

			if (tempRFDDelete->OpenWays == 2)  // 以可写方式打开的，关闭文件时应该保存相关属性。
			{
				while (tempUFD)
				{
					if (tempUFD->FileName == fileName)
					{
						tempUFD->FileLength = tempRFDDelete->FileLength;
						for (int i = 0, j = tempRFDDelete->StartAddress; i < tempRFDDelete->FileLength; i++, j++)
							DiskContent[j] = tempRFDDelete->content[i];
						DiskContent[tempRFDDelete->StartAddress + tempRFDDelete->FileLength] = '\0';
						delete tempRFDDelete;
						cout << "文件关闭成功！" << endl;
						FileOpened--;
						PrintUFD();
						getOpenFile();
						return true;
					}
					tempUFD = tempUFD->nextFile;
				}
				cout << "关闭文件时保存失败！！！";
				return false;
			}
			FileOpened--;
			cout << "成功关闭文件！\n";
			getOpenFile();
			return true;
		}
		tempRFD = tempRFD->nextFile;
	}
	cout << "此文件并没有打开，无需关闭！" << endl;
	return false;
}

bool ReadFile()
{
	getOpenFile();
	string fileName;
	RFD* tempRFD = pRFD->nextFile;

	if (tempRFD)
	{
		cout << "\n请输入要读取的文件名：";
		cin >> fileName;

		while (tempRFD)
		{
			if (tempRFD->FileName == fileName && tempRFD->ProtectCode != 2)
			{                                                                                 // 保护码为2时只能写入不能读取！
				cout << "文件内容如下：" << endl;
				for (int i = 0; i < tempRFD->FileLength; i++)
					cout << tempRFD->content[i];
				cout << "\n 文件读取完毕！\n\n";
				return true;
			}
			tempRFD = tempRFD->nextFile;
		}

		cout << "读取失败，文件不存在或者未打开或者保护码不是1或3！\n";
		return false;
	}
	else return false;
}

bool WriteFile()
{
	string fileName;
	cout << "请输入要写入的文件名：";
	cin >> fileName;

	RFD* tempRFD = pRFD->nextFile;
	while (tempRFD)
	{
		if (tempRFD->FileName == fileName)
		{
			if (tempRFD->ProtectCode < 2)
			{
				cout << "此文件不可写！保护码小于2！\n" << endl;
				return false;
			}
			else if (tempRFD->OpenWays == 1)
			{
				cout << "文件是以只读方式打开的！不能修改！\n\n";
				return false;
			}
			else
			{
				string newContent;
				cout << "旧文件内容：\n    ";
				for (int i = tempRFD->StartAddress; i < tempRFD->StartAddress + tempRFD->FileLength; i++)
					cout << DiskContent[i];
				cout << "\n文件最大长度为" << tempRFD->MaxFileLength << "个字符！";

				cout << "\n请输入新的文件内容以覆盖原文件内容(输入$再按回车即可结束）：\n    ";
				char input;
				int length = 0;
				input = cin.get();
				while (input != '$')  // 输入文件内容，输入 $ 代表结束
				{
					tempRFD->content[length++] = input;
					if (length == tempRFD->MaxFileLength)
					{
						cout << "输入内容达到文件最大容量，输入停止！\n\n";
						break;
					}
					input = cin.get();
				}		

				tempRFD->FileLength = length;						
				cout << "写入文件成功(未保存）！\n\n";
				return true;
			}
		}
		tempRFD = tempRFD->nextFile;
	}  // end while
	cout << "不存在此文件或者此文件未打开，请先尝试open此文件！\n\n" << endl;
	return false;
}

bool SaveMFD()
{
	ofstream ofsMFD("MFD.txt");
	if (!ofsMFD.is_open())
	{
		cout << "MFD.txt文件打开失败！\n";
		ofsMFD.close();
		return false;
	}

	MFD* tempMFD = pMFD->nextUser;
	while (tempMFD)
	{
		ofsMFD << tempMFD->UserName <<" "<< tempMFD->UserPassword << " " << tempMFD->userNo << " " <<tempMFD->FileNum;  // 读入用户信息
		if (tempMFD->nextUser)
			ofsMFD << "\n";
		tempMFD = tempMFD->nextUser;
	}
	ofsMFD.close();
	cout << "成功保存主目录文件...\n" << endl;
	return true;
}

bool SaveUFD(char* name)
{
	ofstream ofsUFD(name);        //文件的输出流对象
	if (!ofsUFD.is_open())
	{
		cout << name<<"文件打不开！保存失败！\n";
		ofsUFD.close();
		return false;
	}
	if (pUFD->nextFile)
	{
		UFD* tempUFD = pUFD->nextFile;
		while (tempUFD)
		{
			char* tempContent = new char(tempUFD->FileLength + 1); // 最后一个字符是空字符！避免出现乱码
			for (int i = 0, j = tempUFD->StartAddress; i < tempUFD->FileLength; i++, j++)
				tempContent[i] = DiskContent[j];

			ofsUFD << tempUFD->FileName << " " << tempUFD->StartAddress << " " << tempUFD->ProtectCode << " " << tempUFD->FileLength << " " << tempUFD->MaxFilelength << " ";
			ofsUFD.write(tempContent, tempUFD->FileLength);
			ofsUFD << " ";
			cout << "文件" << tempUFD->FileName << "保存成功！\n";
			tempUFD = tempUFD->nextFile;
		}
	}
	else
	{
		ofsUFD.close();
		remove(name);
		cout << "无用户文件需要保存！\n";
		return true;
	}
	

	ofsUFD << '\0';
	ofsUFD.close();
	cout << "用户文件保存完毕！" << endl;
	return true;
}

void SystemInitial()
{
	Help();
	InitialDisk();
	InitialAFD();
	if (!InitialMFD())  // 第一次启动系统时MFD文件未创建，请求创建初始用户！
	{
		cout << "第一次启动系统，进入初始化程序......\n开始创建用户:\n" << endl;
		bool goOn = true;
		char input;
		while (goOn)
		{
			CreateMFD();
			cout << "\n是否还需创建新的用户（输入y表示是，n表示否）：";
			cin >> input;
			if (input == 'n')
				goOn = false;
			cout << endl;
		}
		cout << "系统初始化完毕！";
		SaveMFD();
	}
}

void FileSystem()
{
	string command;
	while (true)
	{
		cout << "等待输入命令：";
		cin >> command;
		if (login == false && command != "help"&&command != "cls"&&command != "login"&&command != "quit")
			cout << "请先登录用户再进行操作！\n\n";
		else if (login == false && command == "login")
		{
			if (UserLogin())  // 登录成功
			{
				login = true;
				InitialAFD();
			}
		}
		else if (login == true && command == "login")
			cout << "用户" << UserName << "已登录!一次只能有一个用户登录！ \n\n";
		else if (command == "create")
			CreateUFD();
		else if (command == "delete")
			DeleteUFD();
		else if (command == "open")
			OpenFile();
		else if (command == "getOpenFile")
			getOpenFile();
		else if (command == "close")
			CloseFile();
		else if (command == "read")
			ReadFile();
		else if (command == "write")
			WriteFile();
		else if (command == "dir")
			PrintUFD();
		else if (command == "rename")
			RenameFile();
		else if (command == "help")
			Help();
		else if (command == "cls")
			system("cls");
		else if (command == "logout")  // 退出登录还可以继续进行用户登录
		{
			if (QuitSystem())
			{
				login = false;
				SystemInitial();
			}
			else cout << "退出登录失败！\n";
		}
		else if (command == "quit")
			if (QuitSystem())
			{
				login = false;
				break;
			}
			else
				cout << "退出系统失败！" << endl;
		else
		{
			cout << "您的输入有误，请重新输入。\n";
			Help();
		}
	}
}

bool QuitSystem()
{
	RFD* tempRFD = pRFD->nextFile;
	if (tempRFD)
	{
		cout << "有文件未关闭，是否全部保存？输入1表示全部保存，输入2表示全部不保存，输入3表示撤销命令（即不退出系统）:";
		char input;
label4:
		cin >> input;
		if (input == '1')  // 保存RFD数据到UFD
		{
			while (tempRFD)
			{
				if (tempRFD->OpenWays == 2)  // 以可写方式打开文件才需要保存
				{
					UFD* tempUFD = pUFD->nextFile;
					while (tempUFD)
					{
						if (tempUFD->FileName == tempRFD->FileName)
						{
							tempUFD->FileLength = tempRFD->FileLength;
							for (int i = 0, j = tempRFD->StartAddress; i < tempRFD->FileLength; i++, j++)
								DiskContent[j] = tempRFD->content[i];						
							cout << tempUFD->FileName << "文件保存成功！" << endl;
							break;  // 跳出内层while循环
						}
						tempUFD = tempUFD->nextFile;
					}
				}
				tempRFD = tempRFD->nextFile;
			}
		}
		else if (input == '2')		{		}
		else if (input == '3')  return false;
		else
		{
			cout << "您的输入有误，请重新输入，输入1表示全部保存，输入2表示全部不保存，输入3表示撤销命令（即不退出系统）：";
			goto label4;
		}
	}
	// 保存数据到磁盘文本文件
	SaveMFD();
	if (login)
	{
		char TempUserName[20];
		strcpy(TempUserName, UserName);
		strcat(TempUserName, ".txt");
		SaveUFD(TempUserName);
	}
	Help();
	cout << "已成功退出系统！\n";
	return true;
}

void DiskFormat()  // 磁盘格式化，清空所有用户信息和用户的文件信息
{
	MFD* tempMFD = pMFD->nextUser;
	UFD* tempUFD = pUFD->nextFile;
	RFD* tempRFD = pRFD->nextFile;

	while (tempMFD)
	{
		pMFD->nextUser = tempMFD->nextUser;
		delete tempMFD;
		tempMFD = pMFD->nextUser;
	}

	while (tempUFD)
	{
		pUFD->nextFile = tempUFD->nextFile;
		delete tempUFD;
		tempUFD = pUFD->nextFile;
	}

	while (tempRFD)
	{
		pRFD->nextFile = tempRFD->nextFile;
		delete tempRFD;
		tempRFD->nextFile = pRFD->nextFile;
	}
	memset(DiskContent, '\0', sizeof(DiskContent));
	cout << "磁盘已格式化！";
}

int main()
{
	SystemInitial();
	FileSystem();
	return 0;
}
