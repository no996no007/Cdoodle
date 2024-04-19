#include "allclass.h"

///每调用一次为所有时间变量加一，即时间增加一帧
playerclass& playerclass::operator++()
{
	t++;
	shooting_t++;
	//距离上一次发射的帧数超过40时则取消发射姿态
	if (shooting_t == 40 && status !=dizzy)
	{
		status = right;
	}
	if (dizzy_t >= 0)
	{
		dizzy_t++;
	}
	if (flying_t >= 0)
	{
		flying_t++;
	}
	return *this;
}
///bottom_x是判断玩家是否与地面接触的底部x点坐标
int  playerclass::bottom_x()
{
	return pos_x + 52;
}
///bottom_y是判断玩家是否与地面接触的底部y点坐标
int  playerclass::bottom_y()
{
	return pos_y + 116;
}
///contact_x是判断玩家是否与物品接触的x点坐标
int  playerclass::contact_x()
{
	return pos_x + 65;
}
///contact_y是判断玩家是否与物品接触的y点坐标
int  playerclass::contact_y()
{
	return pos_y + 60;
}
///与怪兽相碰撞。
void playerclass::contact_with_monster()
{
	if (status ==dizzy)//如果是特殊状态计算与怪物碰撞了也不反应。
	{
		return;
	}
	if (status == right || status == right_tojump)
	{
		status_before_falling = right;
	}
	else if (status == left || status == left_tojump)
	{
		status_before_falling = left;
	}
	else
	{
		status_before_falling = right;
	}
	status = dizzy;
	dizzy_t = 0;
	return;
}
///初始设置设置玩家的新x，y坐标，以及人物状态
void playerclass::set(int nx, int ny, player_status ns, IMAGE& nri, IMAGE& nli)
{
	pos_x = nx;
	pos_y = ny;
	status = ns;
	rimage = nri;
	limage = nli;
	jump_from_y = pos_y;
	return;
}
///每帧根据状态调整高度。
///是根据上一其他位置来修改pos_y的！跟所处的pos_y无关！
void playerclass::adjust_y_by_jumping_status()
{
	int ds;
	//根据上一次的起跳情况调节高度变化。
	switch (jump_status)
	{
	case 1:
		//这里有个坑！坐标要减去跳跃量!画布上的是y反的！
		pos_y = jump_from_y - ((V * t) - floor(a_half * t * t));
		break;
	case 2:
		pos_y = jump_from_y - ((STRING_V * t) - floor(a_half * t * t));
		break;
	case 3:
		ds = 25*(1-exp(flying_t - FLYING_T));
		if (ds > V)
		{
			pos_y -= ds;
		}
		else
		{
			this->adjust_jumping_status(1);
			flying_t = -1;
		}
	default:
		break;
	}
	return;
}
///起跳函数，每次调用根据t和是否输入true来修改玩家当前的y值，注意！
void playerclass::adjust_jumping_status(int jump_strength)
{
	if (status == dizzy)//如果眩晕了不允许改变状态。
	{
		return;
	}
	if (jump_strength==1)//普通跳
	{
		t = 0;
		jump_from_y = pos_y;
		jump_status = 1;
	}
	else if (jump_strength == 2)
	{
		t = 0;
		jump_from_y = pos_y;
		jump_status = 2;
	}
	else if (jump_strength == 3)
	{
		flying_t = 0;
		jump_from_y = pos_y;
		jump_status = 3;
	}
	return;
}
///叠加惯性。往移动的方向叠加惯性，如果移动为0那么消减惯性。
void playerclass::accumulate_inertia_v(int dx)
{
	if (dx == 0)
	{
		if (fabs(inertia_v) < 1E-6)
		{
			return;
		}
		else
		{
			inertia_v -= 0.3*inertia_v / fabs(inertia_v);//这个0.3决定了惯性的衰减速度！可以修改，小于每帧增加的惯性即可
		}
	}
	else
	{
		if (status == dizzy)//如果眩晕了不允许叠加惯性。
		{
			return;
		}
		if (fabs(inertia_v) < V_X)//极限速度
		{
			inertia_v += 2* dx / abs(dx);//移动时每帧增加惯性2个，可以修改
		}
	}
	return;
}
///增减玩家x，y坐标的函数
void playerclass::move(int dx, int dy) 
{
	if (status == dizzy)
	{
		dx = 0;//这样就保证玩家如果眩晕了会一直眩晕，无法操作。
	}
	if (dx < 0)
	{
		status = left;
	}
	else if (dx > 0)
	{
		status = right;
	}
	pos_x += dx + round(inertia_v);
	pos_y += dy;
	//如果进入了屏幕两侧要可以从另外一侧出现
	if (pos_x < -80)
	{
		pos_x += WINDOWW + 60;
		return;
	}
	if (pos_x > WINDOWW-20)
	{
		pos_x -= WINDOWW + 60;
		return;
	}
	return;
}
///在图中绘制一次角色。
void playerclass::show(int sum)
{
	//距离上一次尝试发射的时间在一定范围内会让玩家保持抬头状态。
	if (shooting_t < 40 && status !=dizzy)status = shooting;
	if (t < time_for_a_jump / 4 && status != dizzy)status = (player_status)(status + 1);
	//这个的目的是，如果t<10说明刚刚起跳,应该显示起跳动作，
	//正好起跳状态是正常状态的下一位，所以这里加一是把状态转换为当前状态对应的起跳状态。
	switch (status)
	{
	case right:
		putpng(pos_x, pos_y+sum, 124, 120, &rimage, 372, 0);
		//line(0, bottom_y() + sum, WINDOWW, bottom_y() + sum);//测试定位线
		//line(bottom_x(), 0, bottom_x(), WINDOWH);
		break;
	case right_tojump:
		putpng(pos_x, pos_y+sum, 124, 120, &rimage, 248, 0);
		//line(0, bottom_y() + sum, WINDOWW, bottom_y() + sum);
		//line(bottom_x(), 0, bottom_x(), WINDOWH);
		break;
	case left:
		putpng(pos_x, pos_y+sum, 124, 120, &limage, 0, 0);
		//line(0, bottom_y() + sum, WINDOWW, bottom_y() + sum);
		//line(bottom_x(), 0, bottom_x(), WINDOWH);
		break;
	case left_tojump:
		putpng(pos_x, pos_y+sum, 124, 120, &limage, 124, 0);
		//line(0, bottom_y() + sum, WINDOWW, bottom_y() + sum);
		//line(bottom_x(), 0, bottom_x(), WINDOWH);
		break;
	case shooting:
		//先打印嘴巴，再打印身体。
		putpng(pos_x + 52, pos_y+sum, 24, 38, &atlas, 894, 770);
		putpng(pos_x, pos_y+sum, 124, 120, &rimage, 124, 0);
		break;
	case shooting_tojump:
		putpng(pos_x + 52, pos_y+sum, 24, 38, &atlas, 894, 770);
		putpng(pos_x, pos_y+sum, 124, 120, &rimage, 0, 0);
		break;
	case dizzy:
		if (status_before_falling == right)
		{
			putpng(pos_x, pos_y + sum, 124, 120, &rimage, 372, 0);
			if (dizzy_t % 9 < 3)
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 0);
			}
			else if (dizzy_t % 9 < 6)
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 138);
			}
			else
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 69);
			}
		}
		else
		{
			putpng(pos_x, pos_y + sum, 124, 120, &limage, 0, 0);
			if (dizzy_t % 9 < 3)
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 0);
			}
			else if (dizzy_t % 9 < 6)
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 138);
			}
			else
			{
				putpng(pos_x+12, pos_y + sum, 100, 69, &atlas, 870, 69);
			}
		}
		break;
	default:
		break;
	}
	if (t < time_for_a_jump/4 && status != dizzy)status = (player_status)(status - 1);
	//记得要变回来！
	return;
}
///重置距离上一次发射的帧数为0，距离上一次尝试发射的时间在一定范围内会让玩家保持抬头状态。
void playerclass::shot()
{
	shooting_t = 0;
	return;
}

///当判断出发生碰撞时调用。
void landclass::contact() 
{
	//虚基类的碰撞啥事没有
}
///这个函数返回是否与地面发生碰撞。
bool landclass::is_contact(int last_t_bottom_y,int player_bottom_x,int player_bottom_y)
{
	//-20,+130,为了让玩家摸到一点砖也能跳
	if ((last_t_bottom_y < pos_y+2) && (player_bottom_y >= pos_y+2)
		&& (player_bottom_x >= pos_x - 20) && (player_bottom_x <= pos_x + 130))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
///虚显示函数,就算啥也不干也必须要写，因为这个land类不是纯虚函数！
void landclass::show(int sum)
{
	//啥都不干
}

///子弹的发射函数,子弹在atlas的939，291处，长21，高22。
void bulletclass::show(int sum)
{
	//一开始希望利用伪随机数的一个特性,我们把子弹的系数作为随机数的种子，生成一个伪随机数
	//那么对于同一个子弹，他的伪随机数是相同的，所以他每次绘制的x轴偏移量都是相同的
	//但是这样不好，干扰了随机数种子只生成一次的原则！
	//srand(i);
	//time（0）为种子的srand一个程序最好只调用一次！否则会导致每次调用之后第一次的rand（）产生的随机数非常相近！
	//而且就算你用同样的数去重设他，也会导致rand（）从新回到第一个数开始报！
	//尤其如果每帧都重新设置随机数种子，那么rand（）每次都只报第一个数！，每次报的第一个数是是否相近！
	putpng(pos_x, pos_y+sum, 21, 22, &atlas, 939, 291);
	pos_x -= (random_number & 7) - 3;
	pos_y -= BULLET_DS;
	return;
}

///显示一块绿地面。
void greenlandclass::show(int sum)
{
	putpng(pos_x, pos_y+sum, 115, 31, &atlas, 316, 893);
	//line(0, pos_y+2 + sum, WINDOWW, pos_y+2 + sum);//定位线，测试时用。
	return;
}

///显示一块蓝地面。每次显示自己移动自己的位置！
void bluelandclass::show(int sum)
{
	pos_x += direction * speed;
	//如果砖块撞到了屏幕两侧，0和WINDOWW-115则调换方向。
	if (pos_x < 0 || pos_x>WINDOWW-115)
	{
		direction = -direction;
	}
	putpng(pos_x, pos_y + sum, 115, 31, &atlas, 316, 924);
	return;
}

///显示一块白地面。
void whitelandclass::show(int sum)
{
	putpng(pos_x, pos_y + sum, 115, 31, &atlas, 446, 826);
	return;
}
///当白块发生碰撞时调用。
void whitelandclass::contact()
{
	//live关掉的意思就是不在屏幕上显示，也不再判断碰撞。
	live = FALSE;
	return;
}

///当脆弱砖块发生碰撞时。
void fragilelandclass::contact()
{
	broken = TRUE;
	return;
}
///显示脆弱砖块。
void fragilelandclass::show(int sum)
{
	//如果没坏
	if (!broken)
	{
		putpng(pos_x, pos_y + sum, 121, 31, &atlas, 324, 826);
	}
	//如果坏了
	else
	{
		if (broken_t < 5)
		{
			putpng(pos_x, pos_y + sum, 121, 38, &atlas, 826, 379);
		}
		else if (broken_t < 10)
		{
			putpng(pos_x, pos_y + sum, 121, 58, &atlas, 652, 562);
		}
		else
		{
			putpng(pos_x, pos_y + sum, 121, 65, &atlas, 826, 444);
		}
		pos_y += FRAGILELAND_DS;
		broken_t++;
	}
	return;
}

///显示弹簧
void stringlandclass::show(int sum)
{
	//跟随基地面移动。
	pos_x = base->pos_x + 18 + relative_x;
	pos_y = base->pos_y - 20;
	//如果弹簧被触发了
	if (triggerd)
	{
		putpng(pos_x, pos_y + sum - 33, 34, 54, &atlas, 523, 858);
	}
	else
	{
		putpng(pos_x, pos_y + sum, 34, 23, &atlas, 482, 782);
	}
	return;
}
//当弹簧发生碰撞时（被触发时）
void stringlandclass::contact()
{
	triggerd = TRUE;
}
//重载与弹簧接触的条件。
bool stringlandclass::is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	//-20,+,为了让玩家摸到一点弹簧也能跳
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 43))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///敌人基类的空实现。
void monster::show(int sum)
{
	//nothing.
}
bool monster::is_contact(int x, int y)
{
	return FALSE;
}
bool monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	return FALSE;
}

///红色小怪的重载。
void red_monster::show(int sum)
{
	if (HP < 1) 
	{
   		if (HP--==0)//意思是这个if里面是死亡之后的坐标初始化，死亡后只调用一次
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t=0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3*((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)这个是根据base_x产生的随机数，1或-1.
		putpng(pos_x, pos_y + sum, 97, 70, &atlas, 873, 219);
		if (last_t % 9 < 3)
		{
			putpng(pos_x -5, pos_y -35+ sum, 100, 69, &atlas, 870, 0);
		}
		else if (last_t % 9 < 6)
		{
			putpng(pos_x -5, pos_y -35+ sum, 100, 69, &atlas, 870, 138);
		}
		else
		{
			putpng(pos_x -5, pos_y -35+ sum, 100, 69, &atlas, 870, 69);
		}
		last_t++;
		return;
	}
	last_t++;
	pos_x = base_x + round(30 * sin(last_t * 6 * PI / T));
	pos_y = base_y + round(15 * sin(last_t * PI / T));
	putpng(pos_x, pos_y + sum, 97, 70, &atlas, 873, 219);
	//line(pos_x-20, pos_y + sum, pos_x + 97+15, pos_y + sum);
	//line(pos_x-20, pos_y + 70+20 + sum, pos_x + 97+15, pos_y + 70+20 + sum);
	//line(pos_x-20, pos_y + sum, pos_x-20, pos_y + 70+20 + sum);
	//line(pos_x + 97+15, pos_y + sum, pos_x + 97+15, pos_y + 70+20 + sum);
	return;
}
bool red_monster::is_contact(int x, int y)
{
	if (x >= pos_x-20 && x< pos_x + 97+15 && y>pos_y && y < pos_y + 70+20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool red_monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 70 + 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///绿色大怪的重载。
void green_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//意思是这个if里面是死亡之后的坐标初始化，死亡后只调用一次
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)这个是根据base_x产生的随机数，1或-1.
		putpng(pos_x, pos_y + sum, 163, 120, &atlas, 0, 601);
		if (last_t % 9 < 3)
		{
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 0);
		}
		else if (last_t % 9 < 6)
		{
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 138);
		}
		else
		{
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 69);
		}
		last_t++;
		return;
	}
	last_t++;
	pos_x = base_x + round(10 * sin(last_t * 2 * PI / T));
	pos_y = base_y + round(6 * sin(last_t * PI / T));
	if (HP > 8)
	{
		putpng(pos_x, pos_y + sum, 163, 103, &atlas, 0, 721);
	}
	else if (HP > 4)
	{
		putpng(pos_x, pos_y + sum, 163, 105, &atlas, 0, 496);
	}
	else
	{
		putpng(pos_x, pos_y + sum, 163, 120, &atlas, 0, 601);
	}
	//line(pos_x - 20, pos_y + sum, pos_x + 163 + 15, pos_y + sum);
	//line(pos_x - 20, pos_y + 103 + 20 + sum, pos_x + 163 + 15, pos_y + 103 + 20 + sum);
	//line(pos_x - 20, pos_y + sum, pos_x - 20, pos_y + 103 + 20 + sum);
	//line(pos_x + 163 + 15, pos_y + sum, pos_x + 163 + 15, pos_y + 103 + 20 + sum);
	return;
}
bool green_monster::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 163 + 15 && y>pos_y && y < pos_y + 103 + 20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool green_monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 103 + 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///mini小怪重载。
void mini_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//意思是这个if里面是死亡之后的坐标初始化，死亡后只调用一次
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)这个是根据base_x产生的随机数，1或-1.
		putpng(pos_x, pos_y + sum, 110, 98, &atlas, 652, 630);
		if (last_t % 9 < 3)
		{
			putpng(pos_x + 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 0);
		}
		else if (last_t % 9 < 6)
		{
			putpng(pos_x + 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 138);
		}
		else
		{
			putpng(pos_x + 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 69);
		}
		last_t++;
		return;
	}
	last_t++;
	pos_x = base_x + round(20 * sin(last_t * 10 * PI / T));
	pos_y = base_y + round(15 * sin(last_t * 2*PI / T));
	putpng(pos_x, pos_y + sum, 110, 98, &atlas, 652, 630);
	//line(pos_x-20, pos_y + sum, pos_x + 110+15, pos_y + sum);
	//line(pos_x-20, pos_y + 98+20 + sum, pos_x + 110+15, pos_y + 98+20 + sum);
	//line(pos_x-20, pos_y + sum, pos_x-20, pos_y + 98+20 + sum);
	//line(pos_x + 110+15, pos_y + sum, pos_x + 110+15, pos_y + 98+20 + sum);
	return;
}
bool mini_monster::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 110 + 15 && y>pos_y && y < pos_y + 98 + 20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool mini_monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 110 + 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//黑洞重载
void blackhole::show(int sum)
{
	last_t++;
	pos_x = base_x;
	pos_y = base_y;
	putpng(pos_x, pos_y + sum, 136, 131, &atlas, 495 ,650);
	//line(pos_x - 20, pos_y + sum, pos_x + 136 + 15, pos_y + sum);
	//line(pos_x - 20, pos_y + 131 + 15 + sum, pos_x + 136 + 15, pos_y + 131 + 15 + sum);
	//line(pos_x - 20, pos_y + sum, pos_x - 20, pos_y + 131 + 15 + sum);
	//line(pos_x + 136 + 15, pos_y + sum, pos_x + 136 + 15, pos_y + 131 + 15 + sum);
	return;
}
bool blackhole::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 136 + 15 && y>pos_y && y < pos_y + 131 + 15)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

///蓝色小怪的重载。
void blue_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//意思是这个if里面是死亡之后的坐标初始化，死亡后只调用一次
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)这个是根据base_x产生的随机数，1或-1.
		putpng(pos_x, pos_y + sum, 75, 98, &atlas, 447, 858);
		if (last_t % 9 < 3)
		{
			putpng(pos_x - 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 0);
		}
		else if (last_t % 9 < 6)
		{
			putpng(pos_x - 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 138);
		}
		else
		{
			putpng(pos_x - 5, pos_y - 35 + sum, 100, 69, &atlas, 870, 69);
		}
		last_t++;
		return;
	}
	last_t++;
	if (last_t == 1)
	{
		pos_x = base_x;
	}
	pos_x = pos_x - 3;
	if (pos_x < -80)
	{
		pos_x += WINDOWW + 60;
	}
	pos_y = base_y + round(15 * sin(last_t * PI / T));
	putpng(pos_x, pos_y + sum, 75, 98, &atlas, 447, 858);
	//line(pos_x-20, pos_y + sum, pos_x + 75+15, pos_y + sum);
	//line(pos_x-20, pos_y + 98+20 + sum, pos_x + 75+15, pos_y + 98+20 + sum);
	//line(pos_x-20, pos_y + sum, pos_x-20, pos_y + 98+20 + sum);
	//line(pos_x + 75+15, pos_y + sum, pos_x + 75+15, pos_y + 98+20 + sum);
	return;
}
bool blue_monster::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 75 + 15 && y>pos_y && y < pos_y + 98 + 20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool blue_monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 75 + 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

void flying_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//意思是这个if里面是死亡之后的坐标初始化，死亡后只调用一次
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)这个是根据base_x产生的随机数，1或-1.
		if (last_t % 9 < 3)
		{
			putpng(pos_x, pos_y + sum, 150, 88, &atlas, 675, 378);
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 0);
		}
		else if (last_t % 9 < 6)
		{
			putpng(pos_x, pos_y + sum, 156, 87, &atlas, 495, 562);
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 138);
		}
		else
		{
			putpng(pos_x, pos_y + sum, 150, 83, &atlas, 675, 468);
			putpng(pos_x + 25, pos_y - 35 + sum, 100, 69, &atlas, 870, 69);
		}
		last_t++;
		return;
	}
	last_t++;
	pos_x = base_x + round(10 * sin(last_t * 2 * PI / T));
	pos_y = base_y + round(6 * sin(last_t * PI / T));
	if (last_t%9<=3)
	{
		putpng(pos_x, pos_y + sum, 150, 88, &atlas, 675, 378);
	}
	else if (last_t % 9 <= 6)
	{
		putpng(pos_x, pos_y + sum, 156, 88, &atlas, 495, 561);
	}
	else
	{
		//putpng(pos_x, pos_y + sum, 156, 87, &atlas, 495, 562);
		putpng(pos_x+2, pos_y + sum, 150, 85, &atlas, 675, 466);
	}
	//line(pos_x - 20, pos_y + sum, pos_x + 150 + 15, pos_y + sum);
	//line(pos_x - 20, pos_y + 88 + 20 + sum, pos_x + 150 + 15, pos_y + 88 + 20 + sum);
	//line(pos_x - 20, pos_y + sum, pos_x - 20, pos_y + 88 + 20 + sum);
	//line(pos_x + 150 + 15, pos_y + sum, pos_x + 150 + 15, pos_y + 88 + 20 + sum);
	return;
}
bool flying_monster::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 150 + 15 && y>pos_y && y < pos_y + 88 + 20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}
bool flying_monster::be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	if ((last_t_bottom_y < pos_y) && (player_bottom_y >= pos_y)
		&& (player_bottom_x >= pos_x - 28) && (player_bottom_x <= pos_x + 88 + 10))
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}

//火箭类的重载
void rocketclass::show(int sum)
{
	//如果对应的玩家起飞了
	if (base_player != nullptr && base_player->flying_t >= 0)
	{
		triggerd_t = base_player->flying_t;
		pos_x = base_player->pos_x;
		pos_y = base_player->pos_y;
		if (base_player->status == right || base_player->status == right_tojump || base_player->status == shooting || base_player->status == shooting_tojump)
		{
			setaspectratio(-1, 1);
			if (triggerd_t < 15 || (FLYING_T - triggerd_t) < 15)
			{
				putpng(-(pos_x + 48), pos_y + 35 + sum, 50, 125, &atlas, 906, 641);
			}
			else if (triggerd_t < 30 || (FLYING_T - triggerd_t) < 30)
			{
				putpng(-(pos_x + 48), pos_y + 35 + sum, 50, 125, &atlas, 846, 641);
			}
			else if ((triggerd_t % 6) < 3)
			{
				putpng(-(pos_x + 48), pos_y + 35 + sum, 50, 125, &atlas, 846, 512);
			}
			else
			{
				putpng(-(pos_x + 48), pos_y + 35 + sum, 50, 125, &atlas, 906, 512);
			}
			setaspectratio(1, 1);
		}
		else
		{
			if (triggerd_t < 15 || (FLYING_T - triggerd_t) < 15)
			{
				putpng(pos_x+75, pos_y+35 + sum, 50, 125, &atlas, 906, 641);
			}
			else if (triggerd_t < 30 || (FLYING_T - triggerd_t) < 30)
			{
				putpng(pos_x+75, pos_y+35 + sum, 50, 125, &atlas, 846, 641);
			}
			else if ((triggerd_t % 6) < 3)
			{
				putpng(pos_x+75, pos_y+35 + sum, 50, 125, &atlas, 846, 512);
			}
			else
			{
				putpng(pos_x+75, pos_y+35 + sum, 50, 125, &atlas, 906, 512);
			}
		}
		return;
	}
	if (triggerd_t > 0)
	{
		if (falling_t<0)
		{
			base_y = pos_y;
			base_x = pos_x;
			falling_t = 0;
		}
		pos_y = base_y - ((8 * falling_t) - floor(a_half * falling_t * falling_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);
		putpng(pos_x, pos_y + sum, 50, 100, &atlas, 769, 812);
		falling_t++;
		return;
	}
	//跟随基地面移动。
	pos_x = base->pos_x + 21 + 10;
	pos_y = base->pos_y - 70;
	putpng(pos_x, pos_y + sum, 50, 73, &atlas, 825, 835);
	//line(pos_x - 20, pos_y + sum, pos_x + 50 + 20, pos_y + sum);
	//line(pos_x - 20, pos_y + 73 + 20 + sum, pos_x + 50 + 20, pos_y + 73 + 20 + sum);
	//line(pos_x - 20, pos_y + sum, pos_x - 20, pos_y + 73 + 20 + sum);
	//line(pos_x + 50 + 20, pos_y + sum, pos_x + 50 + 20, pos_y + 73 + 20 + sum);
	return;
}
bool rocketclass::is_contact(int x, int y)
{
	if (x >= pos_x - 20 && x< pos_x + 50 + 20 && y>pos_y-40 && y < pos_y + 73 + 20)
	{
		return TRUE;
	}
	else
	{
		return FALSE;
	}
}