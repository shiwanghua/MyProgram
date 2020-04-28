//                                      回合制小游戏：勇士斗恶龙
//                                      2017/8

//   包含头文件
#include<Windows.h>
#include<tchar.h>   //   使用swprintf_s函数所需的头文件
#include<time.h>    //   使用time()函数获取系统时间

//   包含库文件
#pragma comment(lib,"winmm.lib")     //   调用PlaySound函数
#pragma comment(lib,"Msimg32.lib")   //   调用TransparentBlt函数

//   定义辅助宏
#define WINDOW_WIDTH 800
#define WINDOW_HEIGHT 600
#define WINDOW_TITLE L"[致我永不熄灭的游戏开发之梦]GDI游戏程序"   //   窗口标题
#define PARTICLE_NUMBER 50                                     //   雪花粒子数量

//                                      定义全局结构体
struct CHARACTER
{
	int NowHp;          //   目前生命值
	int MaxHp;          //   最大生命值
	int NowMp;          //   目前魔法值
	int MaxMp;          //   最大魔法值
	int Level;          //   角色的等级
	int Intelligence;   //   角色智力值   
	int Strength;       //   角色力量值
	int Agility;        //   角色敏捷值
};

struct SNOW
{
	int x;
	int y;
	BOOL exist;
};

//   定义全局动作枚举体
enum ActionTypes
{
	ACTION_TYPE_NORMAL = 0,     //普通打击
	ACTION_TYPE_CRITICAL = 1,   //致命暴击
	ACTION_TYPE_MAGIC = 2,        //魔法攻击
	ACTION_TYPE_MISS = 3,         //魔法不够攻击落空
	ACTION_TYPE_RECOVER = 4,      //回复魔法
};

//                                      声明全局变量:句柄,数,数组,结构体等;
HDC g_hdc = NULL, g_mdc = NULL, g_bufdc = NULL;
HBITMAP g_hBackGround, g_hGameOver, g_hVictory, g_hSnow;
HBITMAP g_hMonsterBitmap, g_hHeroBitmap, g_hRecoverSkill;
HBITMAP g_hSkillButton1, g_hSkillButton2, g_hSkillButton3, g_hSkillButton4;   //   四个技能按钮位图句柄
HBITMAP g_hHeroSkill1, g_hHeroSkill2, g_hHeroSkill3;
HBITMAP g_hBossSkill1, g_hBossSkill2, g_hBossSkill3;

DWORD g_tPre = 0, g_tNow = 0;
int InitPaintTimes = 0;
int g_SnowNum = 0;                //   窗口里的雪花数
int g_iFrameNum, g_iTxtNum;       //   帧数和文字数量
int g_Mx, g_My;                   //   定义鼠标坐标

wchar_t text[8][50];              //   输出文字数组
SNOW SnowFlowers[PARTICLE_NUMBER];

CHARACTER Hero, Boss;
ActionTypes HeroActionType, BossActionType;
RECT g_rect;                      //   rect结构体，存储内部窗口坐标
BOOL g_bCanAttack, g_bGameOver;   //   角色是否可以攻击/游戏是否可以结束

//                                      声明全局函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam);
BOOL Game_Init(HWND);
VOID Game_InitPaint(HWND);
VOID Game_Main(HWND);       //   绘图函数（原paint）
VOID Message_Insert(wchar_t* str);
VOID Die_Check(int NowHp, bool isHero,HWND hwnd);
VOID HeroAction_Logic();
VOID HeroAction_Paint();
VOID HeroAction_Paint1();
VOID HeroAction_Paint2();
VOID HeroAction_Paint3();
VOID BossAction_Logic();
VOID BossAction_Paint(HWND hwnd);
VOID Snow_Paint();
BOOL Game_ShutDown(HWND);   //   资源清理函数（原clean up）

//                                      Windows应用程序的入口函数
int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lPCmdLine, int nShowCmd)
{
	//窗口创建:（1）设计窗口类（初始化参数）
	WNDCLASSEX wndClass = { 0 };
	wndClass.cbSize = sizeof(WNDCLASSEX);                          //   设计结构体字节数大小
	wndClass.style = CS_HREDRAW | CS_VREDRAW;
	wndClass.lpfnWndProc = WndProc;
	wndClass.cbClsExtra = 0;                                       //   窗口类的附加内存
	wndClass.cbWndExtra = 0;                                       //   窗口类的附加内存
	wndClass.hInstance = hInstance;                                //   指定包含窗口过程函数的程序的实例句柄
	wndClass.hIcon = (HICON)::LoadImage(NULL, L"icon.ico", IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_LOADFROMFILE);
	wndClass.hCursor = LoadCursor(NULL, IDC_ARROW);                //   窗口类的光标句柄                                  *
	wndClass.hbrBackground = (HBRUSH)GetStockObject(GRAY_BRUSH);   //   白色画刷句柄
	wndClass.lpszMenuName = NULL;
	wndClass.lpszClassName = L"ForMyDreamOfGameDevelop";           //   窗口类的名字

																   //窗口创建:（2）注册窗口类
	if (!RegisterClassEx(&wndClass))   //   该函数是BOOL类型的
	{
		return -1;
	}

	//窗口创建:（3）正式创建窗口
	HWND hwnd = CreateWindow(L"ForMyDreamOfGameDevelop", WINDOW_TITLE, WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT, WINDOW_WIDTH, WINDOW_HEIGHT, NULL,
		NULL, hInstance, NULL);

	//窗口创建:（4）窗口的移动显示与更新
	MoveWindow(hwnd, 550, 250, WINDOW_WIDTH, WINDOW_HEIGHT, true);   //   窗口在屏幕的位置
	ShowWindow(hwnd, nShowCmd);
	UpdateWindow(hwnd);                                             //   更新窗口

	if (!Game_Init(hwnd))
	{
		MessageBox(hwnd, L"资源初始化失败了。。。问题出在Game_Init()函数或者窗口的创建部分", L"消息窗口", 0);
		return FALSE;
	}
	PlaySound(L"GameMedia\\梦幻西游原声-战斗1-森林.wav", NULL, SND_FILENAME | SND_ASYNC | SND_LOOP);   //   循环播放背景音乐             *

																							//消息循环过程
	MSG msg = { 0 };
	while (msg.message != WM_QUIT)
	{
		if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);      //   将虚拟键消息转换为字符消息, 并依旧存在MSG结构体中
			DispatchMessage(&msg);       //   分发（dispatch）一个消息给窗口程序
		}
		else                             //   没收到消息就进行绘图操作
		{
			g_tNow = GetTickCount();
			if (g_tNow - g_tPre >= 60&&InitPaintTimes==100)   //   此次循环运行与上次绘图时间相差0.06s时再进行重绘操作
				Game_Main(hwnd);
		}
	}

	//窗口类的注销
	UnregisterClass(L"ForTheDreamOfGameDevelop", wndClass.hInstance);

	return 0;
}

//                                      全局函数的定义

//                                      定义回调函数
LRESULT CALLBACK WndProc(HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam)
{
	switch (message)
	{
	case WM_TIMER:
		if (InitPaintTimes < 100)
			Game_InitPaint(hwnd);
		break;

	case WM_KEYDOWN:
		switch (wParam)
		{
		case VK_ESCAPE:
			DestroyWindow(hwnd);                                                                 //           *
			PostQuitMessage(0);
			break;

		case VK_NUMPAD1:
			g_bCanAttack = TRUE;
			HeroActionType = ACTION_TYPE_NORMAL;
			break;

		case VK_NUMPAD2:
			g_bCanAttack = TRUE;
			HeroActionType = ACTION_TYPE_MAGIC;
			break;

		case VK_NUMPAD3:
			g_bCanAttack = TRUE;
			HeroActionType = ACTION_TYPE_RECOVER;
			break;
		}
		break;

	case WM_MOUSEMOVE:
		g_Mx = LOWORD(lParam);
		g_My = HIWORD(lParam);
		break;

	case WM_LBUTTONDOWN:
		if (!g_bCanAttack&&!g_bGameOver)
		{
			if (g_Mx >= 555 && g_Mx <= 605 && g_My >= 345 && g_My <= 395)
			{
				g_bCanAttack = TRUE;
				HeroActionType = ACTION_TYPE_NORMAL;
			}
			else if (g_Mx >= 605 && g_Mx <= 655 && g_My >= 395 && g_My <= 445)
			{
				g_bCanAttack = TRUE;
				HeroActionType = ACTION_TYPE_MAGIC;
			}
			else if (g_Mx >= 655 && g_Mx <= 705 && g_My >= 445 && g_My <= 495)
			{
				g_bCanAttack = TRUE;
				HeroActionType = ACTION_TYPE_RECOVER;
			}
			else 
				MessageBox(NULL, L"家伙！你点错地方了！没有这种操作！", L"HINT", 0);
		}
		else if (g_bGameOver)   //   重新开始功能
		{
			if (Boss.NowHp > 0)
			{
				if (g_Mx > 253 && g_Mx < 542 && g_My>379 && g_My < 433)
				{
					if (!Game_Init(hwnd))
					{
						MessageBox(hwnd, L"资源初始化失败了。。。问题出在Game_Init()函数或者窗口的创建部分", L"消息窗口", 0);
						return FALSE;
					}
				}
			}
			else   //   玩家赢了
			{
				MessageBox(hwnd, L"还想再来一次英雄救美吗？那就扔下公主让她再被抓一次吧！", L"消息窗口", MB_YESNO);
				if (g_Mx > 395 && g_Mx < 478 && g_My>324 && g_My < 349)   //   这里有个bug，要在出现Box之前点OK区域才能，下一次点OK或者NO区域才能进入该if
				{                                                         //   千万别露馅！
					if (!Game_Init(hwnd))
					{
						MessageBox(hwnd, L"资源初始化失败了。。。问题出在Game_Init()函数或者窗口的创建部分", L"消息窗口", 0);
						return FALSE;
					}
				}
				else if (g_Mx < 495 && g_Mx>575 && g_My > 325 && g_My < 348)
				{
					message = WM_DESTROY;
					DestroyWindow(hwnd);                                                        
					PostQuitMessage(0);
				}
			}
		}
		break;

	case WM_DESTROY:
		Game_ShutDown(hwnd);
		PostQuitMessage(0);
		break;

	default:
		return DefWindowProc(hwnd, message, wParam, lParam);
	}

	return 0;
}

//                                      定义初始化函数
BOOL Game_Init(HWND hwnd)
{
	srand((unsigned)time(NULL));

	//创建三缓冲体系
	g_hdc = GetDC(hwnd);
	g_mdc = CreateCompatibleDC(g_hdc);
	g_bufdc = CreateCompatibleDC(g_hdc);
	HBITMAP bmp = CreateCompatibleBitmap(g_hdc, WINDOW_WIDTH, WINDOW_HEIGHT);   //   创建一个和窗口兼容的空的位图对象

	SelectObject(g_mdc, bmp);

	g_hGameOver = (HBITMAP)LoadImage(NULL, L"GameMedia\\gameover.bmp", IMAGE_BITMAP, 1086, 396, LR_LOADFROMFILE);   //   1086是两张图的宽度
	g_hVictory = (HBITMAP)LoadImage(NULL, L"GameMedia\\victory.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);

	g_hBackGround = (HBITMAP)LoadImage(NULL, L"GameMedia\\bg.bmp", IMAGE_BITMAP, 800, 600, LR_LOADFROMFILE);
	g_hSnow = (HBITMAP)LoadImage(NULL, L"GameMedia\\snow.bmp", IMAGE_BITMAP, 30, 30, LR_LOADFROMFILE);
	g_hRecoverSkill = (HBITMAP)LoadImage(NULL, L"GameMedia\\recover.bmp", IMAGE_BITMAP, 150, 150, LR_LOADFROMFILE);

	g_hMonsterBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\monster.bmp", IMAGE_BITMAP, 360, 360, LR_LOADFROMFILE);
	g_hBossSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monsterslash.bmp", IMAGE_BITMAP, 234, 188, LR_LOADFROMFILE);
	g_hBossSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstermagic.bmp", IMAGE_BITMAP, 387, 254, LR_LOADFROMFILE);
	g_hBossSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\monstercritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE);

	g_hHeroBitmap = (HBITMAP)LoadImage(NULL, L"GameMedia\\hero.bmp", IMAGE_BITMAP, 360, 360, LR_LOADFROMFILE);
	g_hHeroSkill1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heroslash.bmp", IMAGE_BITMAP, 364, 140, LR_LOADFROMFILE);
	g_hHeroSkill2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\heromagic.bmp", IMAGE_BITMAP, 374, 288, LR_LOADFROMFILE);
	g_hHeroSkill3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\herocritical.bmp", IMAGE_BITMAP, 574, 306, LR_LOADFROMFILE);
	g_hSkillButton1 = (HBITMAP)LoadImage(NULL, L"GameMedia\\skillbutton1.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
	g_hSkillButton2 = (HBITMAP)LoadImage(NULL, L"GameMedia\\skillbutton2.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
	g_hSkillButton3 = (HBITMAP)LoadImage(NULL, L"GameMedia\\skillbutton3.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);
	g_hSkillButton4 = (HBITMAP)LoadImage(NULL, L"GameMedia\\skillbutton4.bmp", IMAGE_BITMAP, 50, 50, LR_LOADFROMFILE);   //   被动技能:恩赐解脱

	GetClientRect(hwnd, &g_rect);    //   获取内部窗口大小，存到g_rect里

    //设定玩家的各项属性值
	Hero.NowHp = Hero.MaxHp = 1000;
	Hero.NowMp = Hero.MaxMp = 60;
	Hero.Level = 6;
	Hero.Strength = 10;
	Hero.Agility = 20;
	Hero.Intelligence = 18;                                                                            //                         *

	//设定怪物的各项属性值
	Boss.NowHp = Boss.MaxHp = 2000;
	Boss.NowMp = Boss.MaxMp = 0;
	Boss.Level = 12;
	Boss.Strength = 10;
	Boss.Agility = 10;
	Boss.Intelligence = 9;

	g_bGameOver = 0;
	g_bCanAttack = 0;
	g_iTxtNum = 0;                   //   初始化窗口显示消息的行数

	HFONT hFont;
	hFont = CreateFont(20, 0, 0, 0, 700, 0, 0, 0, GB2312_CHARSET, 0, 0, 0, 0, TEXT("微软雅黑"));
	SelectObject(g_mdc, hFont);
	SetBkMode(g_mdc, TRANSPARENT);   //设置文字背景为透明

	InitPaintTimes = 0;
	SetTimer(hwnd, 1, 1, NULL);

	return TRUE;
}

//                                      定义初始Paint函数
VOID Game_InitPaint(HWND hwnd)
{
	SelectObject(g_bufdc, g_hBackGround);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	Snow_Paint();

	SelectObject(g_bufdc, g_hMonsterBitmap);
	TransparentBlt(g_mdc, 0.1*InitPaintTimes, 0.7*InitPaintTimes, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

	SelectObject(g_bufdc, g_hHeroBitmap);
	TransparentBlt(g_mdc, 550-InitPaintTimes, 550-5.5*InitPaintTimes, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);

	InitPaintTimes++;
	if (InitPaintTimes == 100)
	{
		KillTimer(hwnd, 1);
		Game_Main(hwnd);
	}
}

//                                      定义绘制函数                                   
VOID Game_Main(HWND hwnd)
{
	wchar_t str[100];

	SelectObject(g_bufdc, g_hBackGround);
	BitBlt(g_mdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);

	//如果游戏未结束，模拟下雪场景,并显示对战信息
	if (!g_bGameOver)
	{
		Snow_Paint();  

		//上面四句用黑笔
		if (g_iTxtNum <= 4)
		{
			SetTextColor(g_mdc, RGB(0, 0, 0));
			for (int i = 0; i < g_iTxtNum; i++)
				TextOut(g_mdc, 20, 410 + i * 18, text[i], wcslen(text[i]));
		}
		//下面四句用白笔
		if (g_iTxtNum > 4)             
		{
			SetTextColor(g_mdc, RGB(0, 0, 0));
			for (int i = 0; i < 4; i++)
				TextOut(g_mdc, 20, 410 + i * 18, text[i], wcslen(text[i]));

			SetTextColor(g_mdc, RGB(255, 255, 255));
			for (int i = 4; i < g_iTxtNum; i++)
				TextOut(g_mdc, 90, 410 + i * 18, text[i], wcslen(text[i]));
		}
	}

	//贴上怪物图，显示怪物血量
	if (Boss.NowHp > 0)   //   显示的前提生命值是为正数
	{
		SelectObject(g_bufdc, g_hMonsterBitmap);
		TransparentBlt(g_mdc, 10, 70, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));

		swprintf_s(str, L"Hp: %d/%d", Boss.NowHp, Boss.MaxHp);
		SetTextColor(g_mdc, RGB(255, 10, 10));
		TextOut(g_mdc, 70, 395, str, wcslen(str));                                           //                                     *
	
		swprintf_s(str, L"/我魔法值无限大");
		SetTextColor(g_mdc, RGB(10, 10, 255));
		TextOut(g_mdc, 183, 395, str, wcslen(str));
	}

	//贴勇士图，显示生命值和魔法值
	if (Hero.NowHp >= 0)   //   显示的前提是生命值为正数
	{
		swprintf_s(str, L"Hp: %d/%d", Hero.NowHp, Hero.MaxHp);
		SetTextColor(g_mdc, RGB(255, 10, 10));
		TextOut(g_mdc, 635, 350, str, wcslen(str));

		swprintf_s(str, L"Mp: %d/%d", Hero.NowMp, Hero.MaxMp);
		SetTextColor(g_mdc, RGB(10, 10, 255));
		TextOut(g_mdc, 635, 370, str, wcslen(str));

		SelectObject(g_bufdc, g_hHeroBitmap);
		TransparentBlt(g_mdc, 450, 50, 360, 360, g_bufdc, 0, 0, 360, 360, RGB(0, 0, 0));
	}

	if (g_bGameOver)
	{
		if (Hero.NowHp <= 0)
		{
			SelectObject(g_bufdc, g_hGameOver);
			BitBlt(g_mdc, 120, 50, 543, 396, g_bufdc, 543, 0, SRCAND);
			BitBlt(g_mdc, 120, 50, 543, 396, g_bufdc, 0, 0, SRCPAINT);
		}
		else
		{
			SelectObject(g_bufdc, g_hVictory);
			TransparentBlt(g_mdc, 0, 0, 800, 600, g_bufdc, 0, 0, 800, 600, RGB(0, 0, 0));
		}
	}
	//如果游戏没有结束而且勇士可以攻击,则贴出技能按钮
	else if (!g_bCanAttack)
	{
		SelectObject(g_bufdc, g_hSkillButton1);
		BitBlt(g_mdc, 555, 345, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillButton2);
		BitBlt(g_mdc, 605, 395, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillButton3);
		BitBlt(g_mdc, 655, 445, 50, 50, g_bufdc, 0, 0, SRCCOPY);
		SelectObject(g_bufdc, g_hSkillButton4);
		BitBlt(g_mdc, 725, 445, 50, 50, g_bufdc, 0, 0, SRCCOPY);
	}
	//如果游戏没有结束且玩家已选择攻击技能,即处于不可攻击状态，此时需展现攻击效果
	else
	{
		g_iFrameNum++;

		//第五到十四个画面时显示玩家的攻击效果
		if (g_iFrameNum >= 3 && g_iFrameNum <= 5)
		{
			if (g_iFrameNum == 5)
			{
				HeroAction_Logic();
				//勇士攻击完就判断恶龙是否死亡
				Die_Check(Boss.NowHp, false,hwnd);
			}
			HeroAction_Paint();
		}
		else if (g_iFrameNum == 8)
			HeroAction_Paint1();
		else if (g_iFrameNum == 11)
			HeroAction_Paint2();
		else if (g_iFrameNum == 14)
			HeroAction_Paint3();

		//第15和第26到30个画面时判断恶龙的攻击并显示攻击效果
		if (g_iFrameNum == 15)
		{
			BossAction_Logic();
		}

		if (g_iFrameNum>=25&&g_iFrameNum <= 30)
			BossAction_Paint(hwnd);

		if (g_iFrameNum == 30)                 //   回合结束
		{
			g_bCanAttack = false;              //   新回合开始后玩家可以再次攻击了
			g_iFrameNum = 0;                   //   帧数返回为0

											   //每回合结束时勇士获得一定的魔法自然恢复：6+2*（0到智力值之间的一个随机数）
			if (!g_bGameOver)
			{
				int MpRecover = 2 * (rand() % Hero.Intelligence) + 6;
				Hero.NowMp += MpRecover;
				if (Hero.NowMp > Hero.MaxMp)   //   防止魔法值超出范围   
				{
					Hero.NowMp = Hero.MaxMp;
				}
				swprintf_s(str, L"回合结束，勇士恢复了【%d】点魔法", MpRecover);
				Message_Insert(str);
			}
		}   //   退出第30帧的if
	}   //   退出游戏未结束的if

	SetTextColor(g_mdc, RGB(255, 255, 255));
	swprintf_s(str, L"鼠标X坐标：%d", g_Mx);
	TextOut(g_mdc, 0, 0, str, wcslen(str));
	swprintf_s(str, L"鼠标Y坐标：%d", g_My);
	TextOut(g_mdc, 0, 20, str, wcslen(str));

	BitBlt(g_hdc, 0, 0, WINDOW_WIDTH, WINDOW_HEIGHT, g_mdc, 0, 0, SRCCOPY);
	g_tPre = GetTickCount();   //   记录此次绘图时间
}

//                                      定义对战消息函数,用于插入文字消息
VOID Message_Insert(wchar_t* str)
{
	//如果没有满8行消息，直接新开一条
	if (g_iTxtNum < 8)
	{
		swprintf_s(text[g_iTxtNum], str);
		g_iTxtNum++;
	}
	//如果满了8行消息，挤走最上面的一条
	else
	{
		for (int i = 0; i < g_iTxtNum - 1; i++)
		{
			swprintf_s(text[i], text[i + 1]);
		}
		swprintf_s(text[7], str);
	}
}

//                                      定义Die_Check函数,进行死亡鉴定(含文字消息和音乐播放）
VOID Die_Check(int NowHp, bool isHero, HWND hwnd)   //   第一个参数填某角色当前生命值,第二个参数判断该角色是否是勇士
{
	wchar_t str[100];

	if (NowHp < 0)
	{
		g_bGameOver = true;

		if (isHero)
		{
			PlaySound(L"FirstBlood.wav", NULL, SND_FILENAME | SND_ASYNC);
			swprintf_s(str, L"你输了你输了你输了哈哈哈连我都打不赢......");
		}
		else   //   恶龙挂了，玩家赢了
		{
			PlaySound(L"GameMedia\\victory.wav", NULL, SND_FILENAME | SND_ASYNC);
	
			if (Hero.NowHp < 200)
				swprintf_s(str, L"这次让你险胜了，我再来一刀你就挂了，有种再来单挑一次啊！");
			else
				swprintf_s(str, L"技术不错啊,公主殿下是你的咯···");
		}
		Message_Insert(str);
	}
}

//                                      定义英雄动作逻辑函数
VOID HeroAction_Logic()
{
	int damage = 0;
	wchar_t str[100];

	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		if (1 == rand() % 4)   //   打出被动技能
		{
			HeroActionType = ACTION_TYPE_CRITICAL;
			//暴击伤害是普通攻击的4.5倍
			damage = (int)(4.5f*(float)(3 * (rand() % Hero.Agility) + Hero.Level*Hero.Strength + 20));
			swprintf_s(str, L"普攻触发被动技能‘恩赐解脱’,4.5倍暴击...对恶龙造成了【%d】点伤害", damage);
		}
		else
		{
			//普通攻击：3乘以（0到灵敏度之间的一个随机数）加上等级乘以力量加上20点基础伤害
			damage = 3 * (rand() % Hero.Agility) + Hero.Level*Hero.Strength + 20;
			swprintf_s(str, L"勇士用普通攻击‘无敌斩’,只打了怪物【%d】点伤害", damage);
		}
		Boss.NowHp -= damage;
		Message_Insert(str);
		break;

	case ACTION_TYPE_MAGIC:
		if (Hero.NowMp >= 30)
		{
			//魔法攻击：5乘以（2*（0到灵敏度之间的一个随机数）+等级*IQ值）
			damage = 5 * (2 * rand()%Hero.Agility + Hero.Level*Hero.Intelligence);
			Boss.NowHp -= (int)damage;
			Hero.NowMp -= 30;
			swprintf_s(str, L"勇者释放烈火剑法...对怪物造成【%d】的伤害值", damage);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"逗比勇士！魔法值没30点，施法失败！这回合白费了...等死吧你就!");
		}
		Message_Insert(str);
		break;

	case ACTION_TYPE_RECOVER:
		if (Hero.NowMp >= 40)
		{
			//气疗术：减少40魔法值，恢复血量：5乘以（5*（0到IQ值之间的随机数）+40基础恢复）
			int HpRecover = 5 * (3 * (rand() % Hero.Intelligence) + 40);
			Hero.NowMp -= 40;
			Hero.NowHp += HpRecover;
			if (Hero.NowHp > Hero.MaxHp)
			{
				Hero.NowHp = Hero.MaxHp;
			}
			swprintf_s(str, L"玩家开始怕死了，使用气疗术恢复了【%d】点生命值。", HpRecover);
		}
		else
		{
			HeroActionType = ACTION_TYPE_MISS;
			swprintf_s(str, L"你怕是要完了，魔法值不足40点，施法失败！");
		}
		Message_Insert(str);
		break;
	}   //   结束switch语句
}   //   结束逻辑函数

//                                      定义英雄动作贴图函数
VOID HeroAction_Paint()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hHeroSkill1);
		TransparentBlt(g_mdc, 220, 50, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 220, 50, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hHeroSkill3);
		TransparentBlt(g_mdc, 60, 50, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 600, 160, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		break;
	}
}
VOID HeroAction_Paint1()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hHeroSkill1);
		TransparentBlt(g_mdc, 50, 180, 364, 140, g_bufdc, 0, 0, 364, 140, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 175, 100, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hHeroSkill3);
		TransparentBlt(g_mdc, 100, 80, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 620, 200, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		break;
	}
}
VOID HeroAction_Paint2()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hHeroSkill1);
		TransparentBlt(g_mdc, 80, 210, 364, 140, g_bufdc, 0, 0, 364, 140, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 130, 50, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hHeroSkill3);
		TransparentBlt(g_mdc, 140, 100, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 620, 240, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		break;
	}
}
VOID HeroAction_Paint3()
{
	switch (HeroActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hHeroSkill1);
		TransparentBlt(g_mdc, 110, 240, 364, 140, g_bufdc, 0, 0, 364, 140, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hHeroSkill2);
		TransparentBlt(g_mdc, 85, 100, 374, 288, g_bufdc, 0, 0, 374, 288, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hHeroSkill3);
		TransparentBlt(g_mdc, 180, 50, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 630, 280, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));
		break;
	}
}

//                                      定义Boss动作逻辑函数（只判断出本回合的攻击类型，不计算伤害值等）
VOID BossAction_Logic()
{
	srand((unsigned)time(NULL));
	if (Boss.NowHp > (Boss.MaxHp / 2))
	{
		switch (rand() % 3)
		{
		case 0:
			BossActionType = ACTION_TYPE_NORMAL;
			break;

		case 1:
			BossActionType = ACTION_TYPE_CRITICAL;
			break;

		case 2:
			BossActionType = ACTION_TYPE_MAGIC;
			break;
		}
	}
	//当生命值小于二分之一时发狂，增加一项回血技能并不在发出普通攻击
	else
	{
		switch (rand() % 3)
		{
		case 0:
			BossActionType = ACTION_TYPE_MAGIC;
			break;

		case 1:
			BossActionType = ACTION_TYPE_CRITICAL;
			break;

		case 2:
			BossActionType = ACTION_TYPE_RECOVER;
			break;
		}
	}   //   退出if语句
}

//                                      定义Boss动作贴图函数（包括计算伤害值等）
VOID BossAction_Paint(HWND hwnd)
{
	int damage = 0, recover = 0;
	wchar_t str[100];

	switch (BossActionType)
	{
	case ACTION_TYPE_NORMAL:
		SelectObject(g_bufdc, g_hBossSkill1);
		TransparentBlt(g_mdc, 520, 150, 234, 188, g_bufdc, 0, 0, 234, 188, RGB(0, 0, 0));

		//第三十个画面时计算玩家受伤害程度并插入显示消息，后面的case同理
		if (g_iFrameNum == 30)
		{
			//普攻“幽冥鬼火”：0到Boss灵敏度之间的一个随机数+Boss力量值*怪物等级
			damage = rand() % Boss.Agility + Boss.Strength*Boss.Level;
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"魔龙释放幽冥鬼火...烧去玩家【%d】点伤害", damage);
			Message_Insert(str);

			//魔龙攻击完就判断英雄是否幸存，后面的case同理
			Die_Check(Hero.NowHp, true, hwnd);
		}
		break;

	case ACTION_TYPE_MAGIC:
		SelectObject(g_bufdc, g_hBossSkill2);
		TransparentBlt(g_mdc, 500, 70, 387, 254, g_bufdc, 0, 0, 387, 254, RGB(0, 0, 0));

		if (g_iFrameNum == 30)
		{
			//嗜血咒魔法攻击：2乘以（2* 0到Boss灵敏度之间的一个随机数+力量值乘以智力值）  
			//魔法攻击回血：  吸血量是伤害值的百分之20！
			damage = 2 * (2 * (rand() % Boss.Agility) + Boss.Strength*Boss.Intelligence);
			Hero.NowHp -= damage;
			recover = (int)((float)damage*0.2f);
			Boss.NowHp += recover;

			swprintf_s(str, L"邪恶势力对正义势力造成【%d】点伤害值，并法术吸血【%d】点生命值.", damage, recover);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true, hwnd);
		}
		break;

	case  ACTION_TYPE_CRITICAL:
		SelectObject(g_bufdc, g_hBossSkill3);
		TransparentBlt(g_mdc, 280, 100, 574, 306, g_bufdc, 0, 0, 574, 306, RGB(0, 0, 0));

		if (g_iFrameNum == 30)
		{
			//释放技能“致命一击”：2* (灵敏度+ 等级*力量值)
			damage = 2 * (rand() % Boss.Agility + Boss.Level*Boss.Strength);
			Hero.NowHp -= (int)damage;

			swprintf_s(str, L"恶龙暴怒，砍伤玩家【%d】点生命值！", damage);
			Message_Insert(str);

			Die_Check(Hero.NowHp, true,hwnd);
		}
		break;

	case ACTION_TYPE_RECOVER:
		SelectObject(g_bufdc, g_hRecoverSkill);
		TransparentBlt(g_mdc, 150, 150, 150, 150, g_bufdc, 0, 0, 150, 150, RGB(0, 0, 0));

		if (g_iFrameNum == 30)
		{
			//梅肯斯姆回血技能：2乘以（智力值)^2
			recover = 2 * Boss.Intelligence*Boss.Intelligence;
			Boss.NowHp += recover;

			swprintf_s(str, L"魔龙君使用梅肯斯姆回血【%d】点生命值...你打不死他了，哈哈哈，没想到吧！", recover);
			Message_Insert(str);
		}
		break;
	}
}

//                                      定义Snow_Paint()函数，模拟雪花纷飞
VOID Snow_Paint()   //   每调用一次Game_Main()函数调用一次Snow_Paint()函数
{
	//依次创建粒子,制造出雪花从天空飘下来的效果
	//当雪花数少于50时就在窗口顶部创建一个雪花对象
	if (g_SnowNum < PARTICLE_NUMBER)
	{
		SnowFlowers[g_SnowNum].x = rand() % g_rect.right;
		SnowFlowers[g_SnowNum].y = 0;
		SnowFlowers[g_SnowNum].exist = true;   //   雪花正式存在，表示下文可以进行绘图
		g_SnowNum++;
	}

	for (int i = 0; i < PARTICLE_NUMBER; i++)
	{
		if (SnowFlowers[i].exist)
		{
			SelectObject(g_bufdc, g_hSnow);
			TransparentBlt(g_mdc, SnowFlowers[i].x, SnowFlowers[i].y, 30, 30, g_bufdc, 0, 0, 30, 30, RGB(0, 0, 0));

			//随机决定粒子雪花的横向移动和偏移量
			if (rand() % 2 == 0)
				SnowFlowers[i].x += rand() % 6;
			else
				SnowFlowers[i].x -= rand() % 6;

			SnowFlowers[i].y += 10;
			//将落到地上的雪花送回顶部
			if (SnowFlowers[i].y > g_rect.bottom)
			{
				SnowFlowers[i].x = rand() % g_rect.right;
				SnowFlowers[i].y = 0;
			}
		}
	}   //   结束for贴图循环
}   //   结束雪花Paint函数


BOOL Game_ShutDown(HWND hwnd)   //   释放资源对象
{
	DeleteObject(g_hBackGround);
	DeleteObject(g_hGameOver);
	DeleteObject(g_hVictory);
	DeleteObject(g_hSnow);
	DeleteObject(g_hRecoverSkill);

	DeleteObject(g_hMonsterBitmap);
	DeleteObject(g_hBossSkill1);
	DeleteObject(g_hBossSkill2);
	DeleteObject(g_hBossSkill3);

	DeleteObject(g_hHeroBitmap);
	DeleteObject(g_hHeroSkill1);
	DeleteObject(g_hHeroSkill2);
	DeleteObject(g_hHeroSkill3);
	DeleteObject(g_hSkillButton1);
	DeleteObject(g_hSkillButton2);
	DeleteObject(g_hSkillButton3);
	DeleteObject(g_hSkillButton4);

	DeleteDC(g_bufdc);
	DeleteDC(g_mdc);
	ReleaseDC(hwnd, g_hdc);

	return true;
}