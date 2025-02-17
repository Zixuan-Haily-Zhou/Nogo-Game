#define _CRT_SECURE_NO_WARNINGS//高版的VS默认scanf,fopen等函数不安全.在源文件开头加上此语句后可以使用
#include <iostream>
#include<graphics.h>
#include <windows.h>
#include<cstdio>
using namespace std;

#define GRID_NUM 11			//棋盘行数，其中1-9为棋的坐标，0和10为边界
#define GRID_COUNT 81		//可放棋子总数
#define BORDER	3			//边界
#define NOSTONE 	0		//空子无棋
#define STEPCHANGE 12  //AI在12步以后使用贪心算法

typedef struct//（张凯越）
{
	int i;
	int j;
	int di;//棋子路径搜寻时当前所处的状态，0为不可走，1为可以
}Road;

typedef struct//（张凯越）
{
	Road data[GRID_COUNT];
	int top;
}stType;

int winWidth = 700;//窗口宽度
int winHeigth = 520;//窗口长度
int board[GRID_NUM][GRID_NUM];//棋子信息：0表示可以下，1表示黑子，2表示白子，3表示边界
double mark[GRID_NUM][GRID_NUM];
int step = 0;//记录ai下了几步
int chess_color = 1;//棋子颜色,1表示黑色，2表示白色
int mode = 1;;//单人模式为1，双人模式为2
bool ai_turn;//1表示AI下，0表示人下
bool legal = 0;//下的棋合法性（是否下在了已经有棋子的地方）
bool ai_black = 0;//ai是否是黑棋，用于判断胜负
int x;
int y;
double endTime;
double runTime;
double remainTime;//倒计时

mouse_msg msg = { 0 };
MUSIC music;
DWORD status = music.GetPlayStatus();
PIMAGE pimg = newimage();//图像指针
PIMAGE pimg_black = newimage();//黑棋图像
PIMAGE pimg_white = newimage();//白棋图像
PIMAGE pimg_prompt = newimage();//提示棋的图像

void print_firstPage();//(84-97),
void print_secondPage();//(99-115)
void print_thirdPage();//(117-133)
void print_background();
void erase(int left, int top, int right, int bottom);
//这5个函数用于界面的图形的打印（周子瑄）

int play_firstpage();
void play_thirdpage();
//一些模式选择和功能实现（周子瑄）

int ai_play();
int human_play();
int judge_winner(int x);
//下棋交互（周子瑄）

void restart();//重新开始游戏
void save();
int read();
void prompt();//提示
void music_play();
void music_judge();
//这些函数用于菜单功能的实现（周子瑄）

bool Breath(int xi, int yi);		//判断单个棋子周围是否有气
bool Rule_Suicide(int xi, int yi, int color);	//自杀判定
bool Rules(int xi, int yi, int color);			//困子判定
int markpoints(int color);
bool put_OK(int xi, int yi, int color);
void move(int color);
int greedy(int color);//贪心算法
//这些函数用于ai算法（张凯越）



void print_firstPage()//打印第一个界面（周子瑄）
{
	setfont(40, 0, "华光琥珀_CNKI");// setfont函数 setfont(x,y,"xx")  x:字体高度  y:字体宽度(0为自适应） "xx":字体
	xyprintf(0.422 * winWidth, 0.2 * winHeigth, "不围棋");
	setfont(20, 0, "楷体");
	xyprintf(0.447 * winWidth, 0.4 * winHeigth, "新游戏");
	xyprintf(0.46 * winWidth, 0.6 * winHeigth, "退出");
	setfont(15, 0, "楷体");
	xyprintf(0.74 * winWidth, 0.955 * winHeigth, "制作人：张凯越，周子瑄");
	pimg = newimage();
	getimage(pimg, "board/pku.png");//获取北大徽志图像
	putimage(20, 410, 290, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
}

void print_secondPage()//打印第二个界面，主要用于说明游戏规则（周子瑄）
{
	setfont(30, 0, "华光琥珀_CNKI");
	xyprintf(0.1 * winWidth, 0.1 * winHeigth, "请先阅读游戏须知~");
	setfont(20, 0, "楷体");
	xyprintf(0.067 * winWidth, 0.25 * winHeigth, "nogo game ，顾名思义，就是想方设法不要让自己提走对方的棋子（围住）");
	xyprintf(0.07 * winWidth, 0.3 * winHeigth, "尽可能让自己的棋子被对方提走（围住）");
	xyprintf(0.067 * winWidth, 0.35 * winHeigth, "黑子先手，双方轮流落子，每一方有10秒思考时间，落子无悔。");
	xyprintf(0.07 * winWidth, 0.40 * winHeigth, "如果一方落子后吃掉了对方的棋子，则落子一方判负；自杀判负");
	xyprintf(0.07 * winWidth, 0.45 * winHeigth, "如果你没有思路，可以点击“提示”键，好心的AI会给你提示哦~");
	xyprintf(0.07 * winWidth, 0.6 * winHeigth, "准备好了吗？祝你好运~");
	setcolor(BLUE);
	xyprintf(0.07 * winWidth, 0.7 * winHeigth, "阅读完毕请按空格键继续");
	pimg = newimage();
	getimage(pimg, "board/pku.png");//继续获取徽志图像
	putimage(20, 410, 290, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
}

void print_thirdPage()//打印第三个界面，主要用于选择游戏模式（周子瑄）
{
	setcolor(BLACK);
	setfont(30, 0, "华光琥珀_CNKI");
	xyprintf(0.1 * winWidth, 0.1 * winHeigth, "请选择游戏模式~");
	setfont(20, 0, "楷体");
	xyprintf(0.07 * winWidth, 0.25 * winHeigth, "如果你想与AI一较高下，请选择");
	pimg = newimage();
	getimage(pimg, "board/single.png");
	putimage(410, 110, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	xyprintf(0.07 * winWidth, 0.35 * winHeigth, "如果你有同伴或想自娱自乐，请选择");
	pimg = newimage();
	getimage(pimg, "board/couple.png");
	putimage(410, 170, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
}

void print_background()//绘制背景图像（周子瑄）
{
	pimg = newimage();
	getimage(pimg, "board/board.png");//获取棋盘图像
	putimage(100, 40, pimg);
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/eecs call.png");
	putimage(540, 110, 120, 100, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/music.png");
	putimage(540, 40, 120, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/save.png");
	putimage(540, 260, 70, 35, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/read.png");
	putimage(540, 300, 70, 35, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/prompt.png");
	putimage(540, 220, 100, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/end.png");
	putimage(590, 460, 80, 40, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	delimage(pimg);
	pimg = newimage();
	getimage(pimg, "board/restart.png");
	putimage(590, 405, 100, 50, pimg, 0, 0, getwidth(pimg), getheight(pimg));
	//绘制菜单图像
}

void erase(int left, int top, int right, int bottom) //用白色矩形覆盖,相当于擦除原先打印的部分（周子瑄）
{
	setfillcolor(WHITE);
	bar(left, top, right, bottom);
	setfillcolor(BLACK);
}


int play_firstpage()//返回值表示是否退出游戏（周子瑄）
{
	for (; is_run(); delay_fps(60))
	{     //is_run()函数判断ege窗口是否还存在，delay_fps(60)指每1/60秒监听一次用户是否点击鼠标，间隔时间太长程序运行不灵敏，太短连续多次点击程序易崩溃
		while (mousemsg())//getmouse 获取鼠标消息
		{
			msg = getmouse();
			if (msg.x <= 390 && msg.x > 300 && msg.y > 200 && msg.y < 230 && msg.is_down())
				return 1;
			if (msg.x <= 360 && msg.x > 310 && msg.y > 320 && msg.y < 350 && msg.is_down())
				return 0;//退出游戏
		}
	}
	return 1;
}

void play_thirdpage()//交互，选择模式（周子瑄）
{
	setcolor(BLUE);
	bool b1 = 0;
	bool b2 = 0;//用于判断是否选择完，若选完则跳出for循环
	for (; is_run() && b1 == 0; delay_fps(60))
	{
		while (mousemsg())//getmouse 获取鼠标消息
		{
			msg = getmouse();
			if (msg.x <= 580 && msg.x > 410 && msg.y > 110 && msg.y < 170 && msg.is_down())
			{
				mode = 1;
				b1 = 1;
				setfont(18, 0, "华光琥珀_CNKI");
				xyprintf(0.77 * winWidth, 0.3 * winHeigth, "成功选择单人模式");
			}
			if (msg.x <= 580 && msg.x > 410 && msg.y > 170 && msg.y < 230 && msg.is_down() && b1 == 0)
			{
				mode = 2;
				ai_turn = 0;
				b1 = 1;
				setfont(18, 0, "华光琥珀_CNKI");
				xyprintf(0.77 * winWidth, 0.3 * winHeigth, "成功选择双人模式");
			}
		}
	}
	if (mode == 1)//单人模式,还需进一步选择
	{
		xyprintf(0.07 * winWidth, 0.45 * winHeigth, "你选择先手还是后手？");
		pimg = newimage();
		getimage(pimg, "board/first.png");
		putimage(100, 270, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
		delimage(pimg);
		pimg = newimage();
		getimage(pimg, "board/follow.png");
		putimage(300, 270, 120, 60, pimg, 0, 0, getwidth(pimg), getheight(pimg));
		delimage(pimg);
		for (; is_run() && b2 == 0; delay_fps(60))
		{
			while (mousemsg() && b2 == 0)//getmouse 获取鼠标消息
			{
				msg = getmouse();
				if (msg.x <= 220 && msg.x > 100 && msg.y > 270 && msg.y < 330 && msg.is_down())//人选择了先手
				{
					ai_turn = 0;
					ai_black = 0;
					b2 = 1;
				}
				if (msg.x > 300 && msg.x <= 570 && msg.y > 270 && msg.y < 330 && msg.is_down() && b2 == 0)//人选择了后手
				{
					ai_turn = 1;
					ai_black = 1;
					b2 = 1;
				}
			}
		}
	}
	if (ai_turn == 0)//第一手该人下，需要提醒
	{
		setfont(18, 0, "华光琥珀_CNKI");
		xyprintf(0.17 * winWidth, 0.80 * winHeigth, "注意：第一手请勿下在棋盘正中央");
	}
	endTime = fclock() + 5;
	for (; is_run(); delay_fps(60))
	{
		runTime = fclock();
		remainTime = endTime - runTime;//当前时间就等于两个时间差
		erase(19, 460, 319, 480);
		xyprintf(19, 460, "%d秒后将跳转至游戏界面", (int)ceil(remainTime));
		if (remainTime <= 0)
			break;
	}
}


int ai_play()//调用算法，AI下棋（周子瑄）
{
	getimage(pimg_black, "board/black1.png");//获取黑棋图像
	getimage(pimg_white, "board/white1.png");//获取白棋图像
	step++;//AI下棋的步数
	if (step <= STEPCHANGE)
	{
		move(chess_color);   //12步前使用抢位算法，此时ai一般不会输
		if (chess_color == 1)
			putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
		else if (chess_color == 2)
			putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
		board[x][y] = chess_color;
		chess_color = chess_color % 2 + 1;//颜色转换
		ai_turn = 0;
	}
	else if (step > STEPCHANGE)
		if (greedy(chess_color))//12步后使用贪心算法,此时ai可能存在无棋可走的情况，即ai输
		{
			if (chess_color == 1)
				putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
			else if (chess_color == 2)
				putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
			board[x][y] = chess_color;
			chess_color = chess_color % 2 + 1;
			ai_turn = 0;
		}
		else return judge_winner(2);//若greedy返回值为0，说明ai无棋可走，直接判输
	return 2;
}

int human_play()
{
	getimage(pimg_black, "board/black1.png");//获取黑棋图像
	getimage(pimg_white, "board/white1.png");//获取白棋图像
	x = (msg.x - 118) / 40 + 1;//对应格点x编号
	y = (msg.y - 59) / 40 + 1;//对应格点y编号
	if (board[x][y] == 0)//没下过
	{
		legal = 1;
		int vic = -1;
		if (Rule_Suicide(x, y, chess_color))//自杀，可直接判断胜负
		{
			if (mode == 1)
				vic = judge_winner(1);
			else if (mode == 2)
				vic = judge_winner(5 - chess_color);
		}
		else
			vic = judge_winner(0);//没有自杀，但可能存在困子判负
		if (vic == 0)
			return 0;
		if (vic == 1)//restart
			return 1;
		//vic==2,继续下棋
		if (chess_color == 1)
		{
			putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_black);
			board[x][y] = 1;
		}
		else if (chess_color == 2)
		{
			putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_white);
			board[x][y] = 2;
		}
		chess_color = chess_color % 2 + 1;
		if (mode == 1)
			ai_turn = 1;
	}
	else legal = 0;
	return 2;
}

int judge_winner(int x)//判断赢家（周子瑄）
{
	int winner = x;//若x不是0，已经判出胜负，直接进入胜负输出部分
	if (winner == 0)//不是因自杀而输，需要进行困子判断
	{
		for (int i = 1; i <= 9; i++)
			for (int j = 1; j <= 9; j++)
			{
				if (Rules(i, j, 2))//黑棋被困死，黑棋获胜
				{
					if (mode == 1)
						winner = 2 - ai_black;
					else if (mode == 2)
						winner = 3;
					break;
				}
				if (Rules(i, j, 1))//白棋被困死，白棋获胜
				{
					if (mode == 1)
						winner = 1 + ai_black;
					else if (mode == 2)
						winner = 4;
					break;
				}
			}
	}
	if (winner)//判断出了输赢
	{
		cleardevice();
		erase(0, 0, 700, 520);
		pimg = newimage();
		if (winner == 1)//ai赢
		{
			getimage(pimg, "board/winner1.png");
			putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
			delimage(pimg);
		}
		else if (winner == 2)//玩家赢
		{
			getimage(pimg, "board/winner2.png");//
			putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
			delimage(pimg);
		}
		else if (winner == 3)//黑棋赢
		{
			getimage(pimg, "board/winner3.png");//
			putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
			delimage(pimg);
		}
		else if (winner == 4)//白棋赢
		{
			getimage(pimg, "board/winner4.png");//
			putimage(30, 10, 640, 400, pimg, 0, 0, getwidth(pimg), getheight(pimg));
			delimage(pimg);
		}
		endTime = fclock() + 10;
		for (; is_run(); delay_fps(60))
		{
			runTime = fclock();
			remainTime = endTime - runTime;
			erase(13, 447, 130, 481);
			xyprintf(19, 460, "倒计时：%d", (int)ceil(remainTime));
			while (mousemsg())
			{
				msg = getmouse();
				if (msg.x > 220 && msg.x <= 450 && msg.y > 225 && msg.y < 272 && msg.is_down())
				{
					restart();//重新开始
					return 1;
				}
			}
			if (remainTime <= 0)
				return 0;
		}
	}
	return 2;
}


void restart()//重新开始游戏（周子瑄）
{
	cleardevice();
	print_thirdPage();
	play_thirdpage();
	cleardevice();
	setcolor(BLACK);
	setfont(20, 0, "楷体");
	erase(80, 20, 260, 40);
	erase(165, 475, 570, 520);
	print_background();

	endTime = fclock() + 10;
	for (int i = 1; i <= 9; i++)
		for (int j = 1; j <= 9; j++)
		{
			board[i][j] = 0;
			mark[i][j] = 0;
		}
	chess_color = 1;
	step = 0;
	legal = 0;
	if (ai_turn == 1)
		ai_play();
}

void save()//保存（周子瑄）
{ 
	FILE* fp1;//文件指针
	FILE* fp2;//文件指针
	fp1 = fopen("data1.txt", "w");
	fp2 = fopen("mark.txt", "w");
	//"w":以“写入”方式打开文件。如果文件存在，那么清空文件内容（相当于删除原文件，再创建一个新文件）。
	for (int i = 1; i <= 9; i++)
		for (int j = 1; j <= 9; j++)
		{
			fprintf_s(fp1, "%d\n", board[i][j]);
			fprintf_s(fp2, "%lf\n", mark[i][j]);
		}
	fclose(fp1);//关闭文件
	FILE* fp;//文件指针
	fp = fopen("turn.txt", "w");
	fprintf_s(fp, "%d\n", chess_color);
	fclose(fp);//关闭文件
}

int read()//读盘（周子瑄）
{
	getimage(pimg_black, "board/black1.png");//获取黑棋图像
	getimage(pimg_white, "board/white1.png");//获取白棋图像
	int i;
	int j;
	FILE* fp1;//文件指针
	fp1 = fopen("data1.txt", "r");//读取chess信息,"r"表示只读模式，读入data数据初始化棋盘
	FILE* fp2;
	fp2 = fopen("mark.txt", "r");
	if (fp1 == NULL)
		return 0;
	print_background();//打印空棋盘
	for (i = 1; i <= 9; i++)
		for (j = 1; j <= 9; j++)
		{
			fscanf_s(fp1, "%d", &board[i][j]);
			if (board[i][j] == 1)
				putimage(114 + (i - 1) * 40, 53 + (j - 1) * 40, pimg_black);
			if (board[i][j] == 2)
				putimage(114 + (i - 1) * 40, 53 + (j - 1) * 40, pimg_white);
			fscanf_s(fp2, "%lf", &mark[i][j]);
		}
	fclose(fp1);//关闭文件
	fclose(fp2);
	FILE* fp;
	fp = fopen("turn.txt", "r");//读取turn
	if (fp == NULL)
		return 2;
	fscanf_s(fp, "%d", &chess_color);
	fclose(fp);//关闭文件
	return 1;
}

void prompt()//提示，用ai的算法（周子瑄）
{
	ai_play();
	chess_color = chess_color % 2 + 1;
	step--;
	board[x][y] = 0;
	getimage(pimg_prompt, "board/temp.png");
	putimage(114 + (x - 1) * 40, 53 + (y - 1) * 40, pimg_prompt);
}

void music_play()//播放音乐（周子瑄）
{
	music.OpenFile("Ludovico Einaudi - Nuvole bianche .mp3");
	music.Play();
	status = music.GetPlayStatus();
}

void music_judge()//根据音乐实际播放情况，决定是暂停还是播放（周子瑄）
{
	if (status == MUSIC_MODE_PLAY)
	{
		music.Pause();//暂停
		status = music.GetPlayStatus();
	}
	else if (status == MUSIC_MODE_PAUSE)
	{
		music.Play();
		status = music.GetPlayStatus();
	}
	//如果音乐已经播放完毕,重复播放
	if (music.GetPlayStatus() == MUSIC_MODE_STOP)
		music.Play(0);////从头开始播放
}


bool Breath(int xi, int yi)	// 局部有气判定（张凯越）
{
	if (board[xi][yi - 1] == 0 || board[xi - 1][yi] == 0 ||
		board[xi + 1][yi] == 0 || board[xi][yi + 1] == 0)
		return true;	// 有气
	return false;		// 无气
}

bool Rule_Suicide(int xi, int yi, int color)	// 自杀判定，该处color为落子颜色（张凯越）
{
	int i = 0;
	int j = 0;
	int di = 0;
	int find = 0;
	int k = color;		// 保存当前棋子颜色
	stType st;
	st.top = 0;
	// 当前棋子位置
	st.data[st.top].i = xi;
	st.data[st.top].j = yi;
	// 搜索路径状态
	st.data[st.top].di = -1;
	// 设置当前棋子颜色 -2(处于搜索状态)
	board[xi][yi] = -2;

	while (st.top > -1)
	{
		i = st.data[st.top].i;
		j = st.data[st.top].j;
		di = st.data[st.top].di;
		if (Breath(i, j) == true)	// 简单有气判定
		{
			for (i = 1; i < 10; i++)
				for (j = 1; j < 10; j++)
					if (board[i][j] == -2)
						board[i][j] = k;
			return false;
		}
		find = 0;
		while (di < 4 && find == 0)//查找下一个可走的方块
		{
			di++;
			switch (di)
			{
			case 0:i = st.data[st.top].i - 1; j = st.data[st.top].j; break;//上
			case 1:i = st.data[st.top].i; j = st.data[st.top].j + 1; break;//右
			case 2:i = st.data[st.top].i + 1; j = st.data[st.top].j; break;//下
			case 3:i = st.data[st.top].i; j = st.data[st.top].j - 1; break;//左
			}
			if (board[i][j] == k)
				find = 1;
		}
		if (find == 1)
		{
			st.data[st.top].di = di;
			st.top++;
			st.data[st.top].i = i;
			st.data[st.top].j = j;
			st.data[st.top].di = -1;
			board[i][j] = -2;
		}
		else
		{
			board[st.data[st.top].i][st.data[st.top].j] = k;
			st.top--;
		}
	}
	for (i = 1; i < 10; i++)
		for (j = 1; j < 10; j++)
			if (board[i][j] == -2)
				board[i][j] = k;
	return true;
}

bool Rules(int xi, int yi, int color)  //困子判定,color=board[xi][yi],即所落子（张凯越）
{
	int i = xi;
	int j = yi;
	int di = -1;
	int find = 0;

	while (di < 4)
	{
		find = 0;
		int iTemp;
		while (di < 4 && find == 0)//查找下一个可走的方块
		{
			di++;
			switch (di)
			{
			case 0:i = xi - 1; j = yi; break;//上
			case 1:i = xi + 1; j = yi; break;//下
			case 2:i = xi; j = yi + 1; break;//右
			case 3:i = xi; j = yi - 1; break;//左
			}
			iTemp = color;
			if (iTemp != 0)
				iTemp = iTemp % 2 + 1;  //转换到对手一方的颜色，以对方的自杀视角来审视是否我方有没有困住对方的子
			if (board[i][j] == iTemp)
				find = 1;
		}
		if (find == 1)
			if (Rule_Suicide(i, j, iTemp) == true)
				return true;
	}
	return false;
}

int markpoints(int color)  //贪心局部评价（张凯越）
{
	int point = 0;
	int enemy = color % 2 + 1;
	for (int x = 1; x <= 9; x++)
	{
		for (int y = 1; y <= 9; y++)
		{
			if (put_OK(x, y, color))
				point++;
			if (put_OK(x, y, enemy))
				point--;
		}
	}
	return point;
}

bool put_OK(int xi, int yi, int color)    //该处color为AI所执棋子颜色（张凯越）
{
	if (board[xi][yi])
		return false;
	board[xi][yi] = color;
	if (Rule_Suicide(xi, yi, color))
	{
		board[xi][yi] = 0;
		return false;
	}
	if (Rules(xi, yi, color))
	{
		board[xi][yi] = 0;
		return false;
	}
	board[xi][yi] = 0;
	return true;
}

void move(int color)//AI所执color,在前12步抢占好位置（张凯越）
{ 
	if (board[1][1] == 0 && put_OK(1, 1, color))
	{
		board[1][1] = color;
		x = 1;
		y = 1;
		return;
	}
	if (board[9][9] == 0 && put_OK(9, 9, color))
	{
		board[9][9] = color;
		x = 9; y = 9;
		return;
	}
	if (board[1][9] == 0 && put_OK(1, 9, color))
	{
		board[1][9] = color;
		x = 1; y = 9;
		return;
	}
	if (board[9][1] == 0 && put_OK(9, 1, color))
	{
		board[9][1] = color;
		x = 9; y = 1;
		return;
	}
	for (int i = 2; i <= 8; i++)
		if (board[1][i - 1] == 0 && board[1][i] == 0 && board[1][i + 1] == 0 && put_OK(1, i, color))
		{
			board[1][i] = color;
			x = 1; y = i;
			return;
		}
	for (int i = 2; i <= 8; i++)
		if (board[9][i - 1] == 0 && board[9][i] == 0 && board[9][i + 1] == 0 && put_OK(9, i, color))
		{
			board[9][i] = color;
			x = 9; y = i;
			return;
		}
	for (int i = 2; i <= 8; i++)
		if (board[i - 1][1] == 0 && board[i][1] == 0 && board[i + 1][1] == 0 && put_OK(i, 1, color))
		{
			board[i][1] = color;
			x = i;
			y = 1;
			return;
		}
	for (int i = 2; i <= 8; i++)
		if (board[i - 1][9] == 0 && board[i][9] == 0 && board[i + 1][9] == 0 && put_OK(i, 9, color))
		{
			board[i][9] = color;
			x = i; y = 9;
			return;
		}
	for (int i = 2; i <= 7; i++)
		if (board[7][i] == 0 && put_OK(7, i, color))
		{
			board[7][i] = color;
			x = 7; y = i;
			return;
		}
}

int greedy(int color)//贪心算法（张凯越）
{
	int max_value = -999;
	int best_i[81] = { 0 }, best_j[81] = { 0 }, best_num = 0;
	memset(mark, 0, sizeof(mark));
	for (int i = 1; i <= 9; i++)
	{
		for (int j = 1; j <= 9; j++)
		{
			if (put_OK(i, j, color))
			{
				board[i][j] = color;
				mark[i][j] = markpoints(color);
				if (mark[i][j] > max_value)
					max_value = mark[i][j];
				board[i][j] = 0;
			}
			else
				mark[i][j] = -99999;
		}
	}
	for (int i = 1; i <= 9; i++)
		for (int j = 1; j <= 9; j++)
			if (mark[i][j] == max_value && board[i][j] == 0)
			{
				best_i[best_num] = i;
				best_j[best_num] = j;
				best_num++;
			}
	if (best_num == 0)
	{
		judge_winner(2);
		return 0;
	}
	int random = rand() % best_num; //在所有最大mark里面随机选
	x = best_i[random];
	y = best_j[random];
	board[x][y] = color;
	return 1;
}



int main()//（周子瑄）
{
	initgraph(winWidth, winHeigth, 0);			//初始化窗口,0是为了去掉ege库的开始动画
	setcaption("nogo game");	//设置窗口标题
	setcolor(BLACK);	   //设置前景色（文字）为黑色
	setbkcolor(WHITE);	   //设置当前绘图背景色（窗口）为白色
	setfontbkcolor(WHITE); //设置文字前景底色为白色（白纸黑字）
	setfont(20, 0, "楷体");
	print_firstPage();
	if (play_firstpage() == 0)
		return 0;//点击了“退出程序”
	cleardevice();
	print_secondPage();
	getch();
	cleardevice();
	print_thirdPage();
	play_thirdpage();
	cleardevice();
	setcolor(BLACK);
	setfont(20, 0, "楷体");
	print_background();//进入第四个页面
	for (int i = 1; i <= 9; i++)
		for (int j = 1; j <= 9; j++)
			board[i][j] = 0;
	music_play();
	for (int i = 0; i < 11; i++)
	{
		board[i][0] = 3;
		board[0][i] = 3;
		board[i][10] = 3;
		board[10][i] = 3;
	}
	if (ai_turn == 1)//该ai下棋
		ai_play();
	endTime = fclock() + 10;
	for (; is_run(); delay_fps(60))
	{    //is_run()函数判断ege窗口是否还存在，delay_fps(60)指每1/60秒监听一次用户是否点击鼠标，
		//间隔时间太长程序运行不灵敏，太短连续多次点击程序易崩溃
		runTime = fclock();
		remainTime = endTime - runTime;//当前时间就等于两个时间差
		erase(13, 447, 130, 481);
		xyprintf(19, 460, "倒计时：%d", (int)ceil(remainTime));
		if (remainTime <= 0)
		{
			setcolor(RED);
			setfont(30, 0, "华文行楷");
			xyprintf(165, 475, "思考时间到，请尽快下棋！");
			setcolor(BLACK);
			setfont(20, 0, "楷体");
		}

		while (mousemsg())//getmouse 获取鼠标消息
		{
			msg = getmouse();
			//xyprintf(80, 20, "鼠标位置：  x   = %4d     y = %4d", msg.x, msg.y);
			if (msg.x > 540 && msg.x <= 660 && msg.y > 40 && msg.y < 80 && msg.is_down())
				music_judge();//暂停或播放音乐
			if (msg.x <= 610 && msg.x > 540 && msg.y > 260 && msg.y < 300 && msg.is_down())
				save();//存盘
			if (msg.x <= 610 && msg.x > 540 && msg.y > 300 && msg.y < 340 && msg.is_down())
				read();//读盘
			if (msg.x > 540 && msg.x <= 640 && msg.y > 210 && msg.y < 260 && msg.is_down())
				prompt();//提示
			if (msg.x > 590 && msg.x <= 670 && msg.y > 460 && msg.y < 500 && msg.is_down())
				return 0;//退出游戏
			if (msg.x > 590 && msg.x <= 690 && msg.y > 405 && msg.y < 455 && msg.is_down())
				restart();//重新开始
			if (msg.x > 124 && msg.x <= 459 && msg.y > 63 && msg.y < 402 && msg.is_down())
			{//在棋盘中，下棋
				if (ai_turn == 0)//该人下棋
				{
					int tem = human_play();
					if (tem == 0)
						return 0;//已决出胜负，并退出
					else if (tem == 2)//没有开始新游戏
					{
						if (legal == 1)//棋是合法的
						{
							endTime = fclock() + 10;
							erase(80, 20, 260, 40);
							erase(165, 475, 570, 520);
							legal = 0;
						}
						else//棋不合法
						{
							setcolor(RED);
							xyprintf(80, 20, "该位置已有棋子！");
							setcolor(BLACK);
						}
					}
				}
				if (ai_turn == 1)//该ai下棋
					if (ai_play() == 0)
						return 0;
			}
		}
	}
	//closegraph();
	return 0;
}
