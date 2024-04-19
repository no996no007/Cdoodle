#include<stdio.h>
#include<iostream>
#include<graphics.h>
#include<typeinfo>
#include"tools.h"
#include"mytool.h"
#include"allclass.h"
#include<conio.h>
#include<time.h>
#include<sstream>
#include<fstream>
#include<set>
using std::cout;
using std::cin;
using std::to_string;
using std::set;

//为了生成一个可以在别电脑可用的exe.......需要Debug，X86配置生成，高级里的MFC用标准window就可以了， c++代码生成的多线程调试改成MTD，清单列表输入输出的的嵌入清单也是是就可以了！
//如果成功了，那么生成的exe文件会大一点的！
#define RECORD 1
#define XASP 1//窗口x轴拉伸比例,可以改但是画质会压缩下降，设为1最好。
#define YASP 1//窗口y轴拉伸比例,可以改但是画质会压缩下降，设为1最好。
#define headline WINDOWH*0.4//中线高度。
#define DS 1
#define BULLETNUM 15
#define LANDNUM 25 //不是一个界面中的地面数量，后台绘制游戏窗口和窗口上方一个窗口高度的地面
#define STRINGNUM 6 //后台一共有多少个弹簧
#define MONSTERNUM 2//太多会很烦，建议就一个
#define WINDOW_BOTTOM ( WINDOWH -jump_sum )
#define WINDOW_TOP (-jump_sum)
#define LANDS_SPAN (2 * WINDOWH)//后台所有的地面所在的一个范围内，绝对值，正数。
#define LANDS_SPAN_BOTTOM (WINDOW_BOTTOM + 0.2 * WINDOWH)//后台所有的地面从哪里开始回收
#define INTERVAL_LAND (LANDS_SPAN / LANDNUM)//每一个地面的间隔

//由于使用到了虚函数，虚基类的多态实现方式，要用到typeid比较判别基指针实际指向的类型，虽然可以直接用typeid（classname），并不需要用到type_info变量，这里记录一下使用的细节。
//注意定义的方式！一定要以常引用（&与const）的方式创建type_info！否则编译不通过！
//还有一件事！type_info是和typeid（？）输入的变量绑定的！该变量被释放之后这个type_info就失效了！所以不要像这样传匿名变量greenlandclass（）！
const type_info& landclass_info = typeid(landclass);
int jump_sum;//累计超过中线的高度,是一个正数
IMAGE player_img;
IMAGE player_img2;
IMAGE atlas2;
IMAGE atlas;
IMAGE font;
rocketclass rocket;
playerclass player;
bulletclass *bullets[15];//由于子弹没有多态需求，也不十分耗内存，可以不使用指针了，但是这里统一格式，设置为与和其他的数组一致，更容易管理。
stringlandclass* strings[STRINGNUM];//如果你不对这个指针数组进行初始化，这些数组全部默认设置为空指针。
landclass* lands[LANDNUM];
monster* monsters[MONSTERNUM];
IMAGE bulletclass::atlas;//这个是初始化bulletclass里的静态变量，由于规定,静态变量必须在主函数文件的外部初始化。
IMAGE landclass::atlas;
IMAGE monster::atlas;
IMAGE rocketclass::atlas;
IMAGE printclass::font;
printclass myprint;
int landclass::background_width = WINDOWW;
int landclass::background_height = WINDOWH;
int the_top_land_index;
int the_highest_solid_land_index;//最后一个实体地面，绿色蓝色（不带弹簧的）地面的index，要保证任何两个实体方块间距离小于玩家可以跳的最大距离。
int the_bottom_land_index;
int last_t_bottom_y;//这个静态变量用于储存上一帧玩家的底部高度坐标，如果上一帧到下一帧穿过了地面的平面则视为发生了碰撞。 
int dead_time;
TCHAR instruction[] = _T("press SPACE to restart or press ESC to exit..............");
set<int,std::greater<int>> scores;//储存分数

///扫描每一个怪物是否有人物，子弹碰撞。
void contact_monster()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		//如果怪物不存在，或者已经被关掉显示了，或者已经被打屎了，则不需要在判读是否相碰撞。
		if (monsters[i] == nullptr || monsters[i]->live == FALSE || monsters[i]->HP < 1)
		{
			continue;
		}
		for (int j = 0; j < BULLETNUM; j++)//如果子弹与怪物相碰。
		{
			if ((bullets[j] != nullptr && bullets[j]->live == TRUE) && typeid(*monsters[i]) != typeid(blackhole))
			{
				if (monsters[i]->is_contact(bullets[j]->pos_x - +10, bullets[j]->pos_y + 10))
				{
					bullets[j]->live = FALSE;
					bullets[j]->CD = BULLET_CD;
					monsters[i]->HP--;
				}
			}
		}
		//如果有玩家相撞。
		if (player.status != dizzy&& monsters[i]->is_contact(player.contact_x(), player.contact_y()))//注意防止玩家已经晕了重复碰撞,
		{
			//如果玩家正在飞行，直接撞飞。
			if (player.flying_t >= 0)
			{
				monsters[i]->HP=0;
			}
			else
			{
				player.contact_with_monster();
			}
		}
	}
	return;
}

void contact_rocket()
{
	if (rocket.live == FALSE)
	{
		return;
	}
	//判断玩家是否摸到了火箭，注意确保玩家没有晕,注意防止反复碰撞！
	if (player.flying_t < 0 &&rocket.triggerd_t<0&& player.status != dizzy && rocket.is_contact(player.contact_x(), player.contact_y()))
	{
		rocket.base_player = &player;
		rocket.live == FALSE;
		player.adjust_jumping_status(3);
	}
	return;
}

///扫描是否踩在了怪物头上。
void on_monster()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		if (monsters[i] == nullptr || monsters[i]->live == FALSE || monsters[i]->HP < 1 || typeid(*monsters[i]) == typeid(blackhole))
		{
			continue;
		}
		if (player.status != dizzy && monsters[i]->be_trample(last_t_bottom_y, player.bottom_x(), player.bottom_y()))//如果玩家与怪物相碰,注意防止重复碰撞。
		{
			player.pos_y += (monsters[i]->pos_y - player.bottom_y());//因为有可能上一帧在地面上，但是下一帧已经在地面下了，不能从地面下其他，要将人物移动到地面上。
			player.adjust_jumping_status(1);
			monsters[i]->HP = 0;
			break;
		}
	}
}

///查找是否与任何一个地面或弹簧相碰撞。
void on_string()
{
	//扫描弹簧。
	for (int i = 0; i < STRINGNUM; i++)
	{
		if (strings[i] == nullptr || strings[i]->live == FALSE)
		{
			continue;
		}
		if (player.status != dizzy && (player.t >= time_for_a_jump / 2) && strings[i]->is_contact(last_t_bottom_y, player.bottom_x(), player.bottom_y()))
		{
			player.pos_y += (strings[i]->pos_y - player.bottom_y());//因为有可能上一帧在地面上，但是下一帧已经在地面下了，不能从地面下其他，要将人物移动到地面上。
			player.adjust_jumping_status(2);
			strings[i]->contact();
			break;
		}
	}
	return;
}

///查找是否与任何一个地面相碰撞。
void on_land()
{
	//扫描地面。
	for (int i = 0; i < LANDNUM; i++)
	{
		//如果判断这个地面为基类地面，那么不需要进入碰撞判断。
		// 下面的typeid(landclass)用landclass_info也可以。
		if (lands[i]->live == FALSE || typeid(*lands[i]) == typeid(landclass))
		{
			continue;
		}
		//player.t >= time_for_a_jump / 2 的意思是玩家正在下落。
		if (player.status != dizzy && (player.t >= time_for_a_jump / 2) && lands[i]->is_contact(last_t_bottom_y, player.bottom_x(), player.bottom_y()))
		{
			//如果碰撞的是蓝色地面
			if (typeid(*lands[i]) == typeid(greenlandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//因为有可能上一帧在地面上，但是下一帧已经在地面下了，不能从地面下其他，要将人物移动到地面上。
				player.adjust_jumping_status(1);
				break;
			}
			//如果碰撞的是蓝色地面
			if (typeid(*lands[i]) == typeid(bluelandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//因为有可能上一帧在地面上，但是下一帧已经在地面下了，不能从地面下其他，要将人物移动到地面上。
				player.adjust_jumping_status(1);
				break;
			}
			//如果碰撞的是白色地面
			if (typeid(*lands[i]) == typeid(whitelandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//因为有可能上一帧在地面上，但是下一帧已经在地面下了，不能从地面下其他，要将人物移动到地面上。
				player.adjust_jumping_status(1);
				lands[i]->contact();
				break;
			}
			//如果碰撞的是易碎地面
			if (typeid(*lands[i]) == typeid(fragilelandclass))
			{
				//不起跳!
				lands[i]->contact();
				break;
			}
		}
	}
	return;
}

///整合两个函数，查找是否与任何一个地面或者弹簧相碰撞,任何刷新上一帧的玩家底部位置。
void on_string_or_land_or_monster()
{
	on_string();
	on_land();
	on_monster();
	last_t_bottom_y = player.bottom_y();
}

///为一个地面生成一个弹簧。
void create_a_string(landclass* base_land)
{
	//先找出一个可以生成的空位。
	for (int i = 0; i < STRINGNUM; i++)
	{
		if ( nullptr == strings[i]  || strings[i]->live == FALSE)
		{
			//如果是nullptr == strings[i]进入的不可以参与判断strings[i]->live == FALSE
			if (nullptr != strings[i])
			{
				delete strings[i];
			}
			strings[i] = new stringlandclass;
			strings[i]->live = TRUE;
			strings[i]->triggerd = FALSE;
			strings[i]->base = base_land;
			strings[i]->relative_x = rand() % 58;
			strings[i]->pos_x = base_land->pos_x + 18 + strings[i]->relative_x;
			strings[i]->pos_y = base_land->pos_y - 20;
			return;
		}
	}
	return;
}

///尝试生成火箭。
void create_rocket(landclass* base_land)
{
	if (rocket.live == FALSE)
	{
		rocket.base = base_land;
		rocket.pos_x = base_land->pos_x;
		rocket.pos_y = base_land->pos_y;
		rocket.triggerd_t = -1;
		rocket.falling_t = -1;
		rocket.live = TRUE;
	}
	return;
}

///尝试生成一个敌人
void create_a_monster(int y)
{
	static int seed;
	for (int i = 0; i<MONSTERNUM; i++)
	{
		if (nullptr == monsters[i] || monsters[i]->live == FALSE)
		{
			if (monsters[i] != nullptr)
			{
				delete monsters[i];
			}
			seed = rand() % ((WINDOWW - 200) + 50);
			if (seed % 100 < 20)
			{
				monsters[i] = new flying_monster;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 5;
			}
			else if (seed % 100 < 40)
			{
				monsters[i] = new red_monster;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 5;
			}
			else if (seed % 100 < 50)
			{
				monsters[i] = new blackhole;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 1;
			}
			else if (seed % 100 < 60)
			{
				monsters[i] = new blue_monster;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 5;
			}
			else if (seed % 100 < 80)
			{
				monsters[i] = new mini_monster;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 3;
			}
			else
			{
				monsters[i] = new green_monster;
				monsters[i]->base_x = seed;
				monsters[i]->base_y = y;
				monsters[i]->live = TRUE;
				monsters[i]->last_t = 0;
				monsters[i]->HP = 12;
			}
			return;
		}
	}
	return;
}

///尝试生成一个子弹（弹夹还有空就可以生成），生成成功则返回真。
bool create_a_bullet()
{
	//找出一个未被调用（即为live==flase）的子弹，将之位置分配到玩家嘴边。
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullets[i] == nullptr || (bullets[i]->live == FALSE && bullets[i]->CD <= 0))
		{
			if (bullets[i] != nullptr)
			{
				delete bullets[i];
			}
			bullets[i] = new bulletclass;
			bullets[i]->live = TRUE;
			bullets[i]->CD = 0;
			bullets[i]->random_number = rand();
			bullets[i]->pos_x = player.pos_x + 52;
			bullets[i]->pos_y = player.pos_y - 20;
			return TRUE;
		}
	}
	return FALSE;
}

///初始化角色数据。
void initplayer()
{
	player.set(WINDOWW * 0.2, WINDOWH * 0.8 - jump_sum, right, player_img2, player_img);
	player.atlas = atlas;
	player.atlas2 = atlas2;
	player.adjust_jumping_status(3);
	player.t = 0;
	player.inertia_v = 0;
	player.shooting_t = 1000;
	return;
}

///绘制初始界面的所有地面
void initlands()
{
	int seed = 0;
	int land_x, land_y;
	for (int i = 0; i < LANDNUM; i++)
	{
		seed = rand() % 3000;
		///land_x指当前这一块地随机生成的x坐标，505=620-115地图宽减去地砖长。
		land_x = seed % (WINDOWW-115);
		///land_y指当前这一块地随机生成的y坐标，初始化时地图高度分成地面数份逐份向上生成。
		land_y = LANDS_SPAN_BOTTOM - i * INTERVAL_LAND;

		///生成绿色地面。
		if (seed < 1800)
		{
			lands[i] = new greenlandclass;
			lands[i]->live = TRUE;
			lands[i]->pos_x = land_x;
			lands[i]->pos_y = land_y;
			if (seed % 5 == 1&&i>4)
			{
				create_a_string(lands[i]);
			}
			else//如果不创建弹簧的话他就是最高的那一个实体地面。
			{
				//我们来判断是否存在卡关现象，如果当前这个实体方块离现有（也就是上一个）的最高的实体方块超过了玩家可以起跳的最高高度，
				//那我们需要为上一个最高的实体方块创建弹簧，防止出现卡关。
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					//如果没有超过玩家起跳高度什么都不用干。
				}
				//继续将之重置为上一个最高的实体方块。
				the_highest_solid_land_index = i;
			}
			continue;
		}
		///生成脆弱地面。
		else if (seed < 2500)
		{
			lands[i] = new fragilelandclass;
			lands[i]->live = TRUE;
			lands[i]->pos_x = land_x;
			lands[i]->pos_y = land_y;
			continue;
		}
		///生成空地面（地面基类）
		else
		{
			lands[i] = new landclass;
			lands[i]->live = TRUE;
			lands[i]->pos_x = land_x;
			lands[i]->pos_y = land_y;
			continue;
		}
	}
	//将最后生成的那一个定义为目前最高的地面。
	the_top_land_index = LANDNUM - 1;
	return;
}

///初始化所有的全局变量。
void initglobal_variable()
{
	int temp;
	jump_sum = 0;
	the_top_land_index = 0;
	the_highest_solid_land_index = 0;
	the_bottom_land_index=0;
	last_t_bottom_y = 0;
	dead_time = -1;
	if (RECORD == 1)
	{
		std::ifstream file;
		file.open("scores.txt");
		if (file.is_open()) {
			while (!file.eof())
			{
				file >> temp;
				scores.insert(temp);
			}
		}
		file.close();
	}
	return;
}

//初始化所有弹簧
void initstrings()
{
	for (int i = 0; i < STRINGNUM; i++)
	{
		strings[i] = nullptr;
	}
	return;
}

///初始化敌人数组
void initmonsters()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		monsters[i] = nullptr;
	}
	return;
}

///初始化子弹数组
void initbullets()
{
	for (int i = 0; i < BULLETNUM; i++)
	{
		bullets[i] = nullptr;
	}
	return;
}

///初始化火箭。
void initrocket()
{
	rocket.pos_x = 0;
	rocket.pos_y = 0;
	rocket.triggerd_t = -1;
	rocket.falling_t = -1;
	rocket.base_x = 0;
	rocket.base_y = 0;
	rocket.base = nullptr;
	rocket.base_player = nullptr;
	rocket.live = FALSE;
	rocket.base_player = &player;
	return;
}

///一局结束后删除所有的弹簧。
void delete_all_strings()
{
	//注意删除掉指针指向的空间后，指针本身没有修改。
	for (int i = 0; i < STRINGNUM; i++)
	{
		if (strings[i] != nullptr)
		{
			delete strings[i];
		}
		strings[i] = nullptr;
	}
	return;
}

///一局结束后删掉所有的地面。
void delete_all_lands()
{
	for (int i = 0; i < LANDNUM; i++)
	{
		if (lands[i] != nullptr)
		{
			delete lands[i];
		}
		lands[i] = nullptr;
	}
	return;
}

///一局结束后删掉所有的敌人。
void delete_all_monsters()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		if (monsters[i] != nullptr)
		{
			delete monsters[i];
		}
		monsters[i] = nullptr;
	}
	return;
}

///一局结束后删掉所有的子弹。
void delete_all_bullets()
{
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullets[i] != nullptr)
		{
			delete bullets[i];
		}
		bullets[i] = nullptr;
	}
	return;
}

///更新当前所有的地图元素。即将已经在屏幕下方（live=FALSE）的地图元素，重新在上方生成。
void refresh_all_elements()
{
	///这个blank_land_in_series是用来判断当前是否连续生成的多个空地面（就是没生成），如果连续两个没生成才有空间产生怪物。
	static int blank_land_in_series = 0;
	int seed = 0;
	int land_x, land_y;
	//srand(unsigned(time(0)));//这个函数有一个巨坑！time返回的是秒！但是你是每帧都调用一次srand！所以一秒内每一帧接收的种子都一样！一秒内生成的随机数都一样！
	while (lands[the_top_land_index]->pos_y > LANDS_SPAN_BOTTOM - LANDS_SPAN)
	{
		//如果你重复调用srand，一秒内的rand产生的第一个随机数都一样，下一秒rand产生的第一个
		//数会只比上一秒产生的稍微大几而已，余了1000之后，导致seed几乎没有很大变化！
		seed = rand() % 3000;
		///land_x指当前这一块地随机生成的x坐标，505=620-115地图宽减去地砖长。
		land_x = seed % (WINDOWW - 115);
		///land_y指当前这一块地随机生成的y坐标，初始化时地图高度分成地面数份逐份向上生成。
		land_y = lands[the_top_land_index]->pos_y - INTERVAL_LAND;
		the_top_land_index = the_bottom_land_index;
		delete lands[the_bottom_land_index];
		///生成绿色地面。
		if (seed % 100 < 30)
		{
			lands[the_bottom_land_index] = new greenlandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			if (seed % 10 == 0)//设置成几就算几分之一的概率产生弹簧。
			{
				create_a_string(lands[the_bottom_land_index]);
			}
			else//如果不创建弹簧的话他就是最高的那一个实体地面。
			{
				//我们来判断是否存在卡关现象，如果当前这个实体方块离现有（也就是上一个）的最高的实体方块超过了玩家可以起跳的最高高度，
				//那我们需要为上一个最高的实体方块创建弹簧，防止出现卡关。
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					if (seed % 10 < 2)
					{
						//如果没有超过玩家起跳高度那么说明上一个最高的实体方块肯定没有弹簧，可以生成火箭。
						create_rocket(lands[the_highest_solid_land_index]);
					}
				}
				//继续将之重置为上一个最高的实体方块。
				the_highest_solid_land_index = the_bottom_land_index;
			}
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//生成蓝色地面
		if (seed % 100 < 40)
		{
			lands[the_bottom_land_index] = new bluelandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			if (seed % 5 == 1)
			{
				create_a_string(lands[the_bottom_land_index]);
			}
			else//如果不创建弹簧的话他就是最高的那一个实体地面。
			{
				//我们来判断是否存在卡关现象，如果当前这个实体方块离现有（也就是上一个）的最高的实体方块超过了玩家可以起跳的最高高度，
				//那我们需要为上一个最高的实体方块创建弹簧，防止出现卡关。
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					if (seed % 10 < 2)
					{
						//如果没有超过玩家起跳高度那么说明上一个最高的实体方块肯定没有弹簧，可以生成火箭。
						create_rocket(lands[the_highest_solid_land_index]);
					}
				}
				//继续将之重置为上一个最高的实体方块。
				the_highest_solid_land_index = the_bottom_land_index;
			}
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//生成白色地面
		if (seed % 100 < 50)
		{
			lands[the_bottom_land_index] = new whitelandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			//因为白色砖块不可以设置弹簧，所以就算实际是可以踩的也不设置为实体方块。
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//生成易碎地面
		if (seed % 100 < 65)
		{
			lands[the_bottom_land_index] = new fragilelandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		///生成空地面（地面基类）
		else
		{
			blank_land_in_series++;
			///这个blank_land_in_series是用来判断当前是否连续生成的多个空地面（就是没生成），如果连续没生成才有空间产生怪物。
			if (blank_land_in_series >= 3)//这里填即大于几就算空几个就生成。可以修改。
			{
				if (seed % 1 == 0)
				{
					create_a_monster(land_y - LANDS_SPAN / (2 * LANDNUM));
					blank_land_in_series = 0;
				}
			}
			lands[the_bottom_land_index] = new landclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			continue;
		}
	}
	return;
}

///更新当前的跳跃总高度.
void refresh_jump_sum()
{
	if (player.pos_y < headline)
	{
		///jump_sum记为玩家y坐标小于400的最大差值。
		jump_sum = max(jump_sum, headline - player.pos_y);
	}
	return;
}

///绘制所有的弹簧，并且将在地图下的标记为FALSE。
void draw_all_monsters(int sum)
{	
	for (int i = 0; i < MONSTERNUM; i++)
	{
		//别对还没生成的弹簧操作。
		if (monsters[i] == nullptr)
		{
			continue;
		}
		if (monsters[i]->pos_y > WINDOW_BOTTOM )
		{
			monsters[i]->live = FALSE;
		}
		//已经离开窗口的一定不能显示！仍在显示可能导致地面已经被回收弹簧仍在读取地面地面数据！
		if (monsters[i]->live == TRUE)
		{
			draw_a_monster(monsters[i],jump_sum);
		}
	}
	return;
}

///绘制所有的弹簧，并且将在地图下的标记为FALSE。
void draw_all_strings(int sum)
{
	for (int i = 0; i < STRINGNUM; i++)
	{
		//别对还没生成的弹簧操作。
		if (strings[i] == nullptr)
		{
			continue;
		}
		if (strings[i]->pos_y > WINDOW_BOTTOM)
		{
			strings[i]->live = FALSE;
		}
		//已经离开窗口的一定不能显示！仍在显示可能导致地面已经被回收弹簧仍在读取地面地面数据！
		if (strings[i]->live == TRUE)
		{
			draw_a_land(strings[i], jump_sum);
		}
	}
	return;
}

///绘制当前界面所有的地面。并且将在地图下的标记为FALSE。
void draw_all_lands(int sum)
{
	//static landclass* testptr = new stringlandclass;
	//testptr->pos_x = 400;
	//testptr->pos_y = 500;
	//draw_a_land(testptr, 0);
	//显示在窗口范围内的地砖live=TRUE,
	for (int i = 0; i < LANDNUM; i++)
	{
		//意思是如果地砖已经在地图底下的话那就关掉他的live，就不显示了，也不判断碰撞。
		if(lands[i]->pos_y > WINDOW_BOTTOM)
		{
			lands[i]->live = FALSE;
		}
		//只输出所有TRUE，即应该显示的砖块。
		if (lands[i]->live==TRUE)
		{
			draw_a_land(lands[i], jump_sum);
		}
	}
	return;
}

///绘制出所有的子弹,并且自动移动。
void draw_all_bullets(int sum = 0)
{
	for (int i = 0; i < BULLETNUM; i++)
	{
		if (bullets[i] == nullptr)
		{
			continue;
		}
		if (bullets[i]->pos_y < LANDS_SPAN_BOTTOM - LANDS_SPAN)
		{
			bullets[i]->CD = BULLET_CD;
			bullets[i]->live = FALSE;
		}
		if (bullets[i]->live == TRUE)
		{
			bullets[i]->show(sum);
		}
		bullets[i]->CD--;
	}
}

//绘制火箭。
void draw_rocket(int sum)
{
	if (rocket.pos_y > WINDOW_BOTTOM)
	{
		rocket.live = FALSE;
	}
	if (player.flying_t >= 0)//如果玩家正在飞行，那么直接复活火箭，让它飞到玩家身上。
	{
		rocket.live = TRUE;
	}
	if (rocket.live == TRUE)
	{
		rocket.show(sum);
	}
	return;
}

//绘制所有底层UI，如背景。
void draw_background_UI(int sum = 0)
{
	//先绘制背景图。
	//atlas2 的0,0点是背景图片
	setaspectratio(WINDOWW * XASP / 620.0, WINDOWH * YASP / 820.0);
	putimage(0, 0, 620, 820, &atlas2, 0, 0);
	setaspectratio(XASP, YASP);
	return;
}

//绘制所有上层UI，如计分。
void draw_upper_UI(int sum = 0)
{
	//这个函数可以绘制半透明物体
	// 透明状态栏可以打开，但是我不是很喜欢。
	//drawAlpha(0, 0, 640 * XASP, 105 * YASP, &atlas, 0, 132, WINDOWW * XASP, WINDOWH * YASP, 0.5);
	//drawAlpha(640 * XASP, 0, 640 * XASP, 105 * YASP, &atlas, 0, 132, WINDOWW * XASP, WINDOWH * YASP, 0.5);
	myprint(to_string(jump_sum/10), 30, 30);
	myprint(to_string(show_FPS()),500,30);
	return;
}

///用于读取用户的键盘与鼠标输入。还有子弹的生成也在其中。
void player_control()
{	
	if (_kbhit()) //如果有键盘按下则进入语块
	{
		//别用_getch()！这个函数相较于直接调用windowapi的效率低的多的多的多！会导致非常卡！
		//那是因为_getch()是阻塞式的，而windowapi这个是非阻塞式的！
		//注意！下面只写’A‘是可以同时识别大小写的，但是
		if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))// 按下A键或左方向键
		{
			player.move(-DS, 0);
			player.accumulate_inertia_v(-DS);//只检测输入函数的正负，决定惯性增加的方向。
		}
		if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))// 按下D键或右方向键
		{
			player.move(+DS, 0);
			player.accumulate_inertia_v(+DS);
		}
		static int last_shoot = 0, now = 10000;//last_shoot是上一次发射时绘制的时间，now是这一次绘制的时间。
		if ((GetAsyncKeyState(VK_SPACE)|| GetAsyncKeyState(VK_UP)) && now - last_shoot >  40)//按下空格或者上键发射，并且两次发射时间需要大于一定的间隔。
		{
			if (player.status !=dizzy)
			{
				//重置距离上一次发射的帧数为0。（这个帧数是用来维持角色姿态的）
				player.shot();
				//如果生成子弹成功，重置距离上一次发射的时间为0。（这个时间是用来计算两次成功发射直接的间隔的，判断要不要接受键盘数据）
				if (create_a_bullet())
				{
					last_shoot = now;
				}
			}
		}
		now = GetTickCount();
		//如果没有按下左右方向键
		if (!(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) && !(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')))
		{
			player.move(0, 0);
		}
		//每帧都要惯性消减。（输入0就是消减）
		player.accumulate_inertia_v(0);
	}
	//这个kbhit的特点是检测缓冲区有无字符，但是并不会读取它！所以你一旦有过键盘输入他返回的一直是1！要调用getch（）不断吸收键盘区的字符！但是这样还会带来卡顿！
	return;
}

///这个是初始化游戏变量的函数，如初始化角色。
void initgame()
{
	delete_all_bullets();
	delete_all_lands();
	delete_all_monsters();
	delete_all_strings();
	initglobal_variable();
	//初始化角色的位置，加载角色对象内的图片文件，初始化角色对象内的各各时间变量。
	initplayer();
	//初始化一些类当中的静态图片。
	//将素材赋给静态变量中的图片。
	bulletclass::atlas = atlas;
	landclass::atlas = atlas;
	monster::atlas = atlas;
	rocketclass::atlas = atlas;
	printclass::font = font;
	//初始化所有的地面。即绘制第一帧中的地面。
	initrocket();
	initbullets();
	initstrings();
	initlands();
	initmonsters();
}

///这个是自定义的函数，为所有图片全局变量初始化。
void loadimg()
{ 
	//loadimage(&atlas2, _T("./shapes/atlas2.png"));
	//loadimage(&player_img, _T("./shapes/playerSheet.png"));
	//loadimage(&player_img2, _T("./shapes/playerSheet2.png"));
	loadimage(&atlas, _T("IMAGE"), _T("atlas1"));
	loadimage(&atlas2, _T("IMAGE"), _T("atlas2"));
	loadimage(&player_img, _T("IMAGE"), _T("playerSheet1"));
	loadimage(&player_img2, _T("IMAGE"), _T("playerSheet3"));
	//loadimage(&player_img2, _T("IMAGE"), _T("playerSheet2"));playerSheet2转移之后得到的文件，这个文件似乎依然不能用，仅仅只是在IMAGE类下面内显示而已，要将之修改为外部文件
	loadimage(&font, _T("IMAGE"),_T("bitmapFont_1"));
	//loadimage(&atlas, _T("./shapes/atlas.png"));
	//loadimage(&font, _T("./shapes/bitmapFont_0.png"));
};

///调用后判断玩家是不是挂了（掉出屏幕），然后再指向对应操作。
void if_player_dead()
{
	int base_x;
	int base_y;
	base_x = WINDOWW / 2 - 212;
	base_y = WINDOWH * 0.2 + round(WINDOWH * 0.8 * pow(0.95, dead_time));
	if (dead_time >= 0 )
	{
		dead_time++;
		putpng(base_x,base_y, 424, 150, &atlas, 0, 245);
		myprint("score", base_x, base_y+180);
		myprint(to_string(jump_sum / 10), base_x + 300, base_y +265);
		myprint("highest score", base_x, base_y + 380);
		myprint(to_string(*(scores.begin())), base_x + 300, base_y + 465);
		setbkmode(TRANSPARENT);
		settextcolor(RGB(100, 100, 100)); 
		settextstyle(25, 0, _T("黑体")); // 设置文字大小，字体,第一个参数是文字高度，第二个是平均宽度，设为0则根据高度自适应
		outtextxy(0, WINDOWH-25, instruction); 
	}
	if (player.bottom_y() > WINDOW_BOTTOM)
	{
		//initplayer();//直接重置位置，纯纯开发人员专属无敌版。
		if (dead_time == -1)
		{
			dead_time = 0;
			scores.insert(jump_sum/10);
		}
		if (GetAsyncKeyState(VK_SPACE))
		{
			initgame();
		}
		else if (GetAsyncKeyState(VK_ESCAPE))
		{
			delete_all_bullets();
			delete_all_lands();
			delete_all_monsters();
			delete_all_strings();
			if (RECORD == 1)
			{
				std::ofstream file;
				file.open("scores.txt", std::ios::out);
				for (auto it = scores.begin(); it != scores.end(); it++)
				{
					file << *it << "\n";
				}
				file.close();
			}
			exit(0);//记得用exit要带上括号！不然相当于return
		}
	}
	return;
}

///这个函数的目的是绘制一帧当前的游戏界面
void draw_game_window() 
{
	//绘制UI。
	draw_background_UI();
	//玩家对象又运行了一帧，让其时间自增。
	++player;
	//检测是否有键盘输入，修改玩家的坐标位置，以及判断是否要发生子弹。
	player_control();
	//调整y
	player.adjust_y_by_jumping_status();
	//判断是否有怪物发生碰撞
	contact_monster();
	//判断是否与火箭碰撞。
	contact_rocket();
	//判断是否与弹簧或地面相碰撞。
	on_string_or_land_or_monster();
	//player的高度y坐标只在jumping调用后修改，故y修改后我们开始刷新跳跃总高度
	refresh_jump_sum();
	//更新地面，
	refresh_all_elements();
	//分别绘制不同对象。
	draw_all_lands(jump_sum);
	draw_all_strings(jump_sum);
	draw_all_bullets(jump_sum);
	draw_all_monsters(jump_sum);
	draw_rocket(jump_sum);
	player.show(jump_sum);
	draw_upper_UI(jump_sum);
	//判断玩家挂了没。
	if_player_dead();
}

int main() 
{
	int mytime = 1;
	int last_frame = 1;
	srand(unsigned(time(0)));
	//initgraph(WINDOWW* XASP,WINDOWH* YASP,SHOWCONSOLE);//这个会显示画布的同时还显示控制台。
	initgraph(WINDOWW, WINDOWH);
	//这个函数是启动显示窗口用的，重复调用会关掉已经存在的，再重新启动。
	setaspectratio(XASP, YASP);
	//初始化线条颜色。
	setlinecolor(RGB(0, 0, 0));
	loadimg();
	start_again:
	initgame();
	//打开一个显示缓冲区，即在遇到EndBatchDraw()之前所有绘制操作绘制在一个缓冲区中。
	//这个缓冲区不会被显示，是隐藏在内存中的。
	BeginBatchDraw();
	while (1)
	{
		draw_game_window();
		FlushBatchDraw();
		//使用上面这个函数会使得一次性将缓冲区内的图像显示在输出窗口中。
		//如果不是事先输出在缓存窗口中会导致显示器中绘制的图像看着是先后渲染的。
		mytime = GetTickCount();
		while (mytime-last_frame<12)//这个数字决定帧率上限。
		{
			Sleep(0.5);//不知道为什么这里设置成1会使画面边卡，设置成0.5就无事了。
			mytime = GetTickCount();
		}
		last_frame = GetTickCount();
		//每帧间隔17毫秒，使得每秒最多插60帧，但是程序渲染每一帧都要画个几毫秒，所以这里设个小于17的数
	}
	EndBatchDraw();
	//getchar();也可以用来定住画框，但是这里有while多余了
	return 0;
}