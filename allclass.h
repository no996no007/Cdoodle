#pragma once
#include<stdio.h>
#include<graphics.h>
#include"tools.h"
//#include"mytool.h"
//万万不可包含它！如果两个头文件是相互依赖的，即a需要包含b才能合法，b也需要包含a才能合法，
//那么用a包含b，b也包含a的方式是不可行的！因为include的本质就是把所有的声明复制过来！
//像这里，mytool依赖于allclass生成，这里你再把mytool包含一次，把它复制过来，导致复制出来的这一份没有依赖的allclass文件！
#include<conio.h>
#include<math.h>
#define WINDOWH 820 //渲染分辨率，建议820
#define WINDOWW 620 //620
#define time_for_a_jump 80 //使用多少帧完成一次完整跳跃
#define V 16 //普通起跳初速度，四分之一的总用帧乘以V即一次起跳最大上升高度/像素
#define V_X 6//玩家横向移动的极限速度。
#define STRING_V 30 //弹簧起跳的初速度
#define JUMP_HEIGHT (V*time_for_a_jump/4) //一次起跳最大上升高度/像素
#define BLUELAND_DS 7//蓝色砖块的最大移动速度,别小于2！
#define FRAGILELAND_DS 8//易碎砖块的下降速度
#define PI (3.14159)
#define T 120 //怪物震荡周期(帧)。
#define BULLET_CD 30 //子弹冷却CD。300
#define BULLET_DS 29
#define FLYING_T 180


//下落加速度的一半,注意那个（float）两个整数作除，不加那个就得0了！
const float a_half = (float)V / time_for_a_jump;

enum player_status
{
	right, right_tojump, left, left_tojump, shooting, shooting_tojump
	,dizzy
};

///子弹类
class bulletclass
{
public:
	friend void contact_monster();
	friend bool create_a_bullet();
	friend void draw_all_bullets(int sum);
	void show(int sum = 0);
	static IMAGE atlas;
private:
	int pos_x;
	int pos_y;
	int random_number;
	int CD=0;//冷却时间，进入live==falsh后开始倒数，小于0即可再次发生。
	bool live=FALSE;
};

///敌人基类
class monster
{
public:
	virtual void show(int sum = 0);
	int base_x;
	int base_y;
	int pos_x;
	int pos_y;
	bool live = FALSE;
	virtual bool is_contact(int x,int y);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	int HP;
	int last_t = 0;//存在了多少帧。
	static IMAGE atlas;
};

///黑洞
class blackhole :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool is_contact(int x, int y);
};

///飞行小怪
class flying_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///绿色大怪
class green_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///蓝色小怪
class blue_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///红色小怪
class red_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

//mini怪
class mini_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///玩家类
class playerclass
{
public:
	///设置玩家的新x，y坐标，以及人物状态
	void set(int nx, int ny, player_status ns, IMAGE& nri, IMAGE& nli);
	///增减玩家x，y坐标的函数
	void move(int dx, int dy);
	///检测到空格输入,记录为上一次尝试发射的时间，距离上一次尝试发射的时间在一定范围内会让玩家保持抬头状态。
	void shot();
	///在图中绘制一次角色。
	void show(int sum = 0);
	///每调用一次为所有时间变量加一，即时间增加一帧
	playerclass& operator++();
	///修改玩家的起跳状态，重置起跳位置为当前值，重置起跳时间，不修改y值
	void adjust_jumping_status(int jump_strength = 0);//跳跃的力度，如果0那就不跳跃，随重力下落，为1则普通跳跃，为2为弹簧跳
	///每帧根据状态调整高度。
	void adjust_y_by_jumping_status();
	///叠加惯性。往移动的方向叠加惯性，如果移动为0那么消减惯性。
	void accumulate_inertia_v(int dx);
	///与怪兽相碰撞。
	void contact_with_monster();

	int pos_x;
	int pos_y;
	//上一次的起跳情况，1是普通跳，2是弹簧跳,3是正在飞行！
	int jump_status;
	///jump_from_y记录上次玩家起跳的位置，这个变量用于规划玩家的跳跃运动轨迹。
	int jump_from_y;
	///bottom_x是判断玩家是否与地面接触的底部x点坐标
	int bottom_x();
	///bottom_y是判断玩家是否与地面接触的底部y点坐标
	int bottom_y();
	///contact_x是判断玩家是否与物品接触的x点坐标
	int contact_x();
	///contact_y是判断玩家是否与物品接触的y点坐标
	int contact_y();
	IMAGE rimage;
	IMAGE limage;
	IMAGE atlas;
	IMAGE atlas2;
	player_status status;
	player_status status_before_falling;
	int flying_t = -1;//这个是时间变量，记录开始飞行了多少帧。如果是-1说明没有起飞。
	int t;//这个是时间变量，用于记录当前距离上一次起跳经过了多少帧
	int shooting_t;//这个是时间变量，用于记录当前距离上一次发射经过了多少帧。
	int dizzy_t = -1;//从开始被撞晕的过了多少帧。如果是负一说明没有被撞晕。
	double inertia_v;//惯性速度，移动可以叠加惯性，停下会减少惯性。惯性会带来而外的速度，注意是带方向的！
};

//记录一下图片的坐标和大小
//嘴巴894,770，长24，高38
//绿地板316，893, 长115，高31
//蓝地板316,924,115,31
//黄地板1 324,826,115,31
//黄地板2 826，379，115，38
//黄地板4 826，444，115，65
//黄地板3 652，562，115，58
//白地板  446,826,115,31
//弹簧1 482，782，34，23
//弹簧2 523，858，34，54

///一个地面的虚基类
class landclass
{
public:
	///虚显示函数,就算啥也不干也必须要写，因为这个land类不是纯虚函数！
	virtual void show(int sum = 0);
	///当判断出发生碰撞时调用。
	virtual void contact();
	///这个函数返回是否与地面发生碰撞。
	virtual bool is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	int pos_x = 0;
	int pos_y = 0;
	bool live = FALSE;
	static int background_width;
	static int background_height;
	static IMAGE atlas;
};

///绿色地面类
class greenlandclass: public landclass
{
public:
	//这一层的virtual可写可不写，因为是最上的一层，如果还有其他类继承Greenlandclass，还是得写上的
	///显示一块绿地面。
	void show(int sum = 0) override;
};

///蓝色地面类
class bluelandclass : public landclass
{
public:
	void show(int sum = 0) override;
	//每个蓝砖生成时产生一个方向变量，1或者-1。rand()%2得0,1再乘二得0,2，减一得-1或1。
	int direction = 2 * (rand() % 2) - 1;
	int speed = rand() % (BLUELAND_DS - 1) + 2;
};

///白色地面类
class whitelandclass : public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
};

///脆弱地面类
class fragilelandclass:public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
	bool broken = FALSE;
	int broken_t = 0;//距离破碎的时间
};

///弹簧类
class stringlandclass:public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
	bool triggerd = FALSE;//是否已经被触发
	bool is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	landclass* base=nullptr;
	int relative_x = 0;
};

///火箭道具,由于他是基于玩家类和地面类的，要放在最下面！
class rocketclass
{
public:
	void show(int sum);
	bool is_contact(int x, int y);
	int pos_x;
	int pos_y;
	int triggerd_t = -1;
	int base_y;
	int base_x;
	int falling_t = -1;
	landclass* base = nullptr;
	playerclass* base_player = nullptr;
	bool live = FALSE;
	static IMAGE atlas;
};