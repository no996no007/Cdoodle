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

//Ϊ������һ�������ڱ���Կ��õ�exe.......��ҪDebug��X86�������ɣ��߼����MFC�ñ�׼window�Ϳ����ˣ� c++�������ɵĶ��̵߳��Ըĳ�MTD���嵥�б���������ĵ�Ƕ���嵥Ҳ���ǾͿ����ˣ�
//����ɹ��ˣ���ô���ɵ�exe�ļ����һ��ģ�
#define RECORD 1
#define XASP 1//����x���������,���Ըĵ��ǻ��ʻ�ѹ���½�����Ϊ1��á�
#define YASP 1//����y���������,���Ըĵ��ǻ��ʻ�ѹ���½�����Ϊ1��á�
#define headline WINDOWH*0.4//���߸߶ȡ�
#define DS 1
#define BULLETNUM 15
#define LANDNUM 25 //����һ�������еĵ�����������̨������Ϸ���ںʹ����Ϸ�һ�����ڸ߶ȵĵ���
#define STRINGNUM 6 //��̨һ���ж��ٸ�����
#define MONSTERNUM 2//̫���ܷ��������һ��
#define WINDOW_BOTTOM ( WINDOWH -jump_sum )
#define WINDOW_TOP (-jump_sum)
#define LANDS_SPAN (2 * WINDOWH)//��̨���еĵ������ڵ�һ����Χ�ڣ�����ֵ��������
#define LANDS_SPAN_BOTTOM (WINDOW_BOTTOM + 0.2 * WINDOWH)//��̨���еĵ�������￪ʼ����
#define INTERVAL_LAND (LANDS_SPAN / LANDNUM)//ÿһ������ļ��

//����ʹ�õ����麯���������Ķ�̬ʵ�ַ�ʽ��Ҫ�õ�typeid�Ƚ��б��ָ��ʵ��ָ������ͣ���Ȼ����ֱ����typeid��classname����������Ҫ�õ�type_info�����������¼һ��ʹ�õ�ϸ�ڡ�
//ע�ⶨ��ķ�ʽ��һ��Ҫ�Գ����ã�&��const���ķ�ʽ����type_info��������벻ͨ����
//����һ���£�type_info�Ǻ�typeid����������ı����󶨵ģ��ñ������ͷ�֮�����type_info��ʧЧ�ˣ����Բ�Ҫ����������������greenlandclass������
const type_info& landclass_info = typeid(landclass);
int jump_sum;//�ۼƳ������ߵĸ߶�,��һ������
IMAGE player_img;
IMAGE player_img2;
IMAGE atlas2;
IMAGE atlas;
IMAGE font;
rocketclass rocket;
playerclass player;
bulletclass *bullets[15];//�����ӵ�û�ж�̬����Ҳ��ʮ�ֺ��ڴ棬���Բ�ʹ��ָ���ˣ���������ͳһ��ʽ������Ϊ�������������һ�£������׹���
stringlandclass* strings[STRINGNUM];//����㲻�����ָ��������г�ʼ������Щ����ȫ��Ĭ������Ϊ��ָ�롣
landclass* lands[LANDNUM];
monster* monsters[MONSTERNUM];
IMAGE bulletclass::atlas;//����ǳ�ʼ��bulletclass��ľ�̬���������ڹ涨,��̬�����������������ļ����ⲿ��ʼ����
IMAGE landclass::atlas;
IMAGE monster::atlas;
IMAGE rocketclass::atlas;
IMAGE printclass::font;
printclass myprint;
int landclass::background_width = WINDOWW;
int landclass::background_height = WINDOWH;
int the_top_land_index;
int the_highest_solid_land_index;//���һ��ʵ����棬��ɫ��ɫ���������ɵģ������index��Ҫ��֤�κ�����ʵ�巽������С����ҿ������������롣
int the_bottom_land_index;
int last_t_bottom_y;//�����̬�������ڴ�����һ֡��ҵĵײ��߶����꣬�����һ֡����һ֡�����˵����ƽ������Ϊ��������ײ�� 
int dead_time;
TCHAR instruction[] = _T("press SPACE to restart or press ESC to exit..............");
set<int,std::greater<int>> scores;//�������

///ɨ��ÿһ�������Ƿ�������ӵ���ײ��
void contact_monster()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		//������ﲻ���ڣ������Ѿ����ص���ʾ�ˣ������Ѿ�����ʺ�ˣ�����Ҫ���ж��Ƿ�����ײ��
		if (monsters[i] == nullptr || monsters[i]->live == FALSE || monsters[i]->HP < 1)
		{
			continue;
		}
		for (int j = 0; j < BULLETNUM; j++)//����ӵ������������
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
		//����������ײ��
		if (player.status != dizzy&& monsters[i]->is_contact(player.contact_x(), player.contact_y()))//ע���ֹ����Ѿ������ظ���ײ,
		{
			//���������ڷ��У�ֱ��ײ�ɡ�
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
	//�ж�����Ƿ������˻����ע��ȷ�����û����,ע���ֹ������ײ��
	if (player.flying_t < 0 &&rocket.triggerd_t<0&& player.status != dizzy && rocket.is_contact(player.contact_x(), player.contact_y()))
	{
		rocket.base_player = &player;
		rocket.live == FALSE;
		player.adjust_jumping_status(3);
	}
	return;
}

///ɨ���Ƿ�����˹���ͷ�ϡ�
void on_monster()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		if (monsters[i] == nullptr || monsters[i]->live == FALSE || monsters[i]->HP < 1 || typeid(*monsters[i]) == typeid(blackhole))
		{
			continue;
		}
		if (player.status != dizzy && monsters[i]->be_trample(last_t_bottom_y, player.bottom_x(), player.bottom_y()))//���������������,ע���ֹ�ظ���ײ��
		{
			player.pos_y += (monsters[i]->pos_y - player.bottom_y());//��Ϊ�п�����һ֡�ڵ����ϣ�������һ֡�Ѿ��ڵ������ˣ����ܴӵ�����������Ҫ�������ƶ��������ϡ�
			player.adjust_jumping_status(1);
			monsters[i]->HP = 0;
			break;
		}
	}
}

///�����Ƿ����κ�һ������򵯻�����ײ��
void on_string()
{
	//ɨ�赯�ɡ�
	for (int i = 0; i < STRINGNUM; i++)
	{
		if (strings[i] == nullptr || strings[i]->live == FALSE)
		{
			continue;
		}
		if (player.status != dizzy && (player.t >= time_for_a_jump / 2) && strings[i]->is_contact(last_t_bottom_y, player.bottom_x(), player.bottom_y()))
		{
			player.pos_y += (strings[i]->pos_y - player.bottom_y());//��Ϊ�п�����һ֡�ڵ����ϣ�������һ֡�Ѿ��ڵ������ˣ����ܴӵ�����������Ҫ�������ƶ��������ϡ�
			player.adjust_jumping_status(2);
			strings[i]->contact();
			break;
		}
	}
	return;
}

///�����Ƿ����κ�һ����������ײ��
void on_land()
{
	//ɨ����档
	for (int i = 0; i < LANDNUM; i++)
	{
		//����ж��������Ϊ������棬��ô����Ҫ������ײ�жϡ�
		// �����typeid(landclass)��landclass_infoҲ���ԡ�
		if (lands[i]->live == FALSE || typeid(*lands[i]) == typeid(landclass))
		{
			continue;
		}
		//player.t >= time_for_a_jump / 2 ����˼������������䡣
		if (player.status != dizzy && (player.t >= time_for_a_jump / 2) && lands[i]->is_contact(last_t_bottom_y, player.bottom_x(), player.bottom_y()))
		{
			//�����ײ������ɫ����
			if (typeid(*lands[i]) == typeid(greenlandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//��Ϊ�п�����һ֡�ڵ����ϣ�������һ֡�Ѿ��ڵ������ˣ����ܴӵ�����������Ҫ�������ƶ��������ϡ�
				player.adjust_jumping_status(1);
				break;
			}
			//�����ײ������ɫ����
			if (typeid(*lands[i]) == typeid(bluelandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//��Ϊ�п�����һ֡�ڵ����ϣ�������һ֡�Ѿ��ڵ������ˣ����ܴӵ�����������Ҫ�������ƶ��������ϡ�
				player.adjust_jumping_status(1);
				break;
			}
			//�����ײ���ǰ�ɫ����
			if (typeid(*lands[i]) == typeid(whitelandclass))
			{
				player.pos_y += (lands[i]->pos_y - player.bottom_y());//��Ϊ�п�����һ֡�ڵ����ϣ�������һ֡�Ѿ��ڵ������ˣ����ܴӵ�����������Ҫ�������ƶ��������ϡ�
				player.adjust_jumping_status(1);
				lands[i]->contact();
				break;
			}
			//�����ײ�����������
			if (typeid(*lands[i]) == typeid(fragilelandclass))
			{
				//������!
				lands[i]->contact();
				break;
			}
		}
	}
	return;
}

///�������������������Ƿ����κ�һ��������ߵ�������ײ,�κ�ˢ����һ֡����ҵײ�λ�á�
void on_string_or_land_or_monster()
{
	on_string();
	on_land();
	on_monster();
	last_t_bottom_y = player.bottom_y();
}

///Ϊһ����������һ�����ɡ�
void create_a_string(landclass* base_land)
{
	//���ҳ�һ���������ɵĿ�λ��
	for (int i = 0; i < STRINGNUM; i++)
	{
		if ( nullptr == strings[i]  || strings[i]->live == FALSE)
		{
			//�����nullptr == strings[i]����Ĳ����Բ����ж�strings[i]->live == FALSE
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

///�������ɻ����
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

///��������һ������
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

///��������һ���ӵ������л��пվͿ������ɣ������ɳɹ��򷵻��档
bool create_a_bullet()
{
	//�ҳ�һ��δ�����ã���Ϊlive==flase�����ӵ�����֮λ�÷��䵽�����ߡ�
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

///��ʼ����ɫ���ݡ�
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

///���Ƴ�ʼ��������е���
void initlands()
{
	int seed = 0;
	int land_x, land_y;
	for (int i = 0; i < LANDNUM; i++)
	{
		seed = rand() % 3000;
		///land_xָ��ǰ��һ���������ɵ�x���꣬505=620-115��ͼ���ȥ��ש����
		land_x = seed % (WINDOWW-115);
		///land_yָ��ǰ��һ���������ɵ�y���꣬��ʼ��ʱ��ͼ�߶ȷֳɵ�����������������ɡ�
		land_y = LANDS_SPAN_BOTTOM - i * INTERVAL_LAND;

		///������ɫ���档
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
			else//������������ɵĻ���������ߵ���һ��ʵ����档
			{
				//�������ж��Ƿ���ڿ������������ǰ���ʵ�巽�������У�Ҳ������һ��������ߵ�ʵ�巽�鳬������ҿ�����������߸߶ȣ�
				//��������ҪΪ��һ����ߵ�ʵ�巽�鴴�����ɣ���ֹ���ֿ��ء�
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					//���û�г�����������߶�ʲô�����øɡ�
				}
				//������֮����Ϊ��һ����ߵ�ʵ�巽�顣
				the_highest_solid_land_index = i;
			}
			continue;
		}
		///���ɴ������档
		else if (seed < 2500)
		{
			lands[i] = new fragilelandclass;
			lands[i]->live = TRUE;
			lands[i]->pos_x = land_x;
			lands[i]->pos_y = land_y;
			continue;
		}
		///���ɿյ��棨������ࣩ
		else
		{
			lands[i] = new landclass;
			lands[i]->live = TRUE;
			lands[i]->pos_x = land_x;
			lands[i]->pos_y = land_y;
			continue;
		}
	}
	//��������ɵ���һ������ΪĿǰ��ߵĵ��档
	the_top_land_index = LANDNUM - 1;
	return;
}

///��ʼ�����е�ȫ�ֱ�����
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

//��ʼ�����е���
void initstrings()
{
	for (int i = 0; i < STRINGNUM; i++)
	{
		strings[i] = nullptr;
	}
	return;
}

///��ʼ����������
void initmonsters()
{
	for (int i = 0; i < MONSTERNUM; i++)
	{
		monsters[i] = nullptr;
	}
	return;
}

///��ʼ���ӵ�����
void initbullets()
{
	for (int i = 0; i < BULLETNUM; i++)
	{
		bullets[i] = nullptr;
	}
	return;
}

///��ʼ�������
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

///һ�ֽ�����ɾ�����еĵ��ɡ�
void delete_all_strings()
{
	//ע��ɾ����ָ��ָ��Ŀռ��ָ�뱾��û���޸ġ�
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

///һ�ֽ�����ɾ�����еĵ��档
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

///һ�ֽ�����ɾ�����еĵ��ˡ�
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

///һ�ֽ�����ɾ�����е��ӵ���
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

///���µ�ǰ���еĵ�ͼԪ�ء������Ѿ�����Ļ�·���live=FALSE���ĵ�ͼԪ�أ��������Ϸ����ɡ�
void refresh_all_elements()
{
	///���blank_land_in_series�������жϵ�ǰ�Ƿ��������ɵĶ���յ��棨����û���ɣ��������������û���ɲ��пռ�������
	static int blank_land_in_series = 0;
	int seed = 0;
	int land_x, land_y;
	//srand(unsigned(time(0)));//���������һ���޿ӣ�time���ص����룡��������ÿ֡������һ��srand������һ����ÿһ֡���յ����Ӷ�һ����һ�������ɵ��������һ����
	while (lands[the_top_land_index]->pos_y > LANDS_SPAN_BOTTOM - LANDS_SPAN)
	{
		//������ظ�����srand��һ���ڵ�rand�����ĵ�һ���������һ������һ��rand�����ĵ�һ��
		//����ֻ����һ���������΢�󼸶��ѣ�����1000֮�󣬵���seed����û�кܴ�仯��
		seed = rand() % 3000;
		///land_xָ��ǰ��һ���������ɵ�x���꣬505=620-115��ͼ���ȥ��ש����
		land_x = seed % (WINDOWW - 115);
		///land_yָ��ǰ��һ���������ɵ�y���꣬��ʼ��ʱ��ͼ�߶ȷֳɵ�����������������ɡ�
		land_y = lands[the_top_land_index]->pos_y - INTERVAL_LAND;
		the_top_land_index = the_bottom_land_index;
		delete lands[the_bottom_land_index];
		///������ɫ���档
		if (seed % 100 < 30)
		{
			lands[the_bottom_land_index] = new greenlandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			if (seed % 10 == 0)//���óɼ����㼸��֮һ�ĸ��ʲ������ɡ�
			{
				create_a_string(lands[the_bottom_land_index]);
			}
			else//������������ɵĻ���������ߵ���һ��ʵ����档
			{
				//�������ж��Ƿ���ڿ������������ǰ���ʵ�巽�������У�Ҳ������һ��������ߵ�ʵ�巽�鳬������ҿ�����������߸߶ȣ�
				//��������ҪΪ��һ����ߵ�ʵ�巽�鴴�����ɣ���ֹ���ֿ��ء�
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					if (seed % 10 < 2)
					{
						//���û�г�����������߶���ô˵����һ����ߵ�ʵ�巽��϶�û�е��ɣ��������ɻ����
						create_rocket(lands[the_highest_solid_land_index]);
					}
				}
				//������֮����Ϊ��һ����ߵ�ʵ�巽�顣
				the_highest_solid_land_index = the_bottom_land_index;
			}
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//������ɫ����
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
			else//������������ɵĻ���������ߵ���һ��ʵ����档
			{
				//�������ж��Ƿ���ڿ������������ǰ���ʵ�巽�������У�Ҳ������һ��������ߵ�ʵ�巽�鳬������ҿ�����������߸߶ȣ�
				//��������ҪΪ��һ����ߵ�ʵ�巽�鴴�����ɣ���ֹ���ֿ��ء�
				if (lands[the_highest_solid_land_index]->pos_y - land_y > JUMP_HEIGHT)
				{
					create_a_string(lands[the_highest_solid_land_index]);
				}
				else
				{
					if (seed % 10 < 2)
					{
						//���û�г�����������߶���ô˵����һ����ߵ�ʵ�巽��϶�û�е��ɣ��������ɻ����
						create_rocket(lands[the_highest_solid_land_index]);
					}
				}
				//������֮����Ϊ��һ����ߵ�ʵ�巽�顣
				the_highest_solid_land_index = the_bottom_land_index;
			}
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//���ɰ�ɫ����
		if (seed % 100 < 50)
		{
			lands[the_bottom_land_index] = new whitelandclass;
			lands[the_bottom_land_index]->live = TRUE;
			lands[the_bottom_land_index]->pos_x = land_x;
			lands[the_bottom_land_index]->pos_y = land_y;
			//��Ϊ��ɫש�鲻�������õ��ɣ����Ծ���ʵ���ǿ��Բȵ�Ҳ������Ϊʵ�巽�顣
			the_bottom_land_index = (++the_bottom_land_index) % LANDNUM;
			blank_land_in_series = 0;
			continue;
		}
		//�����������
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
		///���ɿյ��棨������ࣩ
		else
		{
			blank_land_in_series++;
			///���blank_land_in_series�������жϵ�ǰ�Ƿ��������ɵĶ���յ��棨����û���ɣ����������û���ɲ��пռ�������
			if (blank_land_in_series >= 3)//��������ڼ�����ռ��������ɡ������޸ġ�
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

///���µ�ǰ����Ծ�ܸ߶�.
void refresh_jump_sum()
{
	if (player.pos_y < headline)
	{
		///jump_sum��Ϊ���y����С��400������ֵ��
		jump_sum = max(jump_sum, headline - player.pos_y);
	}
	return;
}

///�������еĵ��ɣ����ҽ��ڵ�ͼ�µı��ΪFALSE��
void draw_all_monsters(int sum)
{	
	for (int i = 0; i < MONSTERNUM; i++)
	{
		//��Ի�û���ɵĵ��ɲ�����
		if (monsters[i] == nullptr)
		{
			continue;
		}
		if (monsters[i]->pos_y > WINDOW_BOTTOM )
		{
			monsters[i]->live = FALSE;
		}
		//�Ѿ��뿪���ڵ�һ��������ʾ��������ʾ���ܵ��µ����Ѿ������յ������ڶ�ȡ����������ݣ�
		if (monsters[i]->live == TRUE)
		{
			draw_a_monster(monsters[i],jump_sum);
		}
	}
	return;
}

///�������еĵ��ɣ����ҽ��ڵ�ͼ�µı��ΪFALSE��
void draw_all_strings(int sum)
{
	for (int i = 0; i < STRINGNUM; i++)
	{
		//��Ի�û���ɵĵ��ɲ�����
		if (strings[i] == nullptr)
		{
			continue;
		}
		if (strings[i]->pos_y > WINDOW_BOTTOM)
		{
			strings[i]->live = FALSE;
		}
		//�Ѿ��뿪���ڵ�һ��������ʾ��������ʾ���ܵ��µ����Ѿ������յ������ڶ�ȡ����������ݣ�
		if (strings[i]->live == TRUE)
		{
			draw_a_land(strings[i], jump_sum);
		}
	}
	return;
}

///���Ƶ�ǰ�������еĵ��档���ҽ��ڵ�ͼ�µı��ΪFALSE��
void draw_all_lands(int sum)
{
	//static landclass* testptr = new stringlandclass;
	//testptr->pos_x = 400;
	//testptr->pos_y = 500;
	//draw_a_land(testptr, 0);
	//��ʾ�ڴ��ڷ�Χ�ڵĵ�שlive=TRUE,
	for (int i = 0; i < LANDNUM; i++)
	{
		//��˼�������ש�Ѿ��ڵ�ͼ���µĻ��Ǿ͹ص�����live���Ͳ���ʾ�ˣ�Ҳ���ж���ײ��
		if(lands[i]->pos_y > WINDOW_BOTTOM)
		{
			lands[i]->live = FALSE;
		}
		//ֻ�������TRUE����Ӧ����ʾ��ש�顣
		if (lands[i]->live==TRUE)
		{
			draw_a_land(lands[i], jump_sum);
		}
	}
	return;
}

///���Ƴ����е��ӵ�,�����Զ��ƶ���
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

//���ƻ����
void draw_rocket(int sum)
{
	if (rocket.pos_y > WINDOW_BOTTOM)
	{
		rocket.live = FALSE;
	}
	if (player.flying_t >= 0)//���������ڷ��У���ôֱ�Ӹ������������ɵ�������ϡ�
	{
		rocket.live = TRUE;
	}
	if (rocket.live == TRUE)
	{
		rocket.show(sum);
	}
	return;
}

//�������еײ�UI���米����
void draw_background_UI(int sum = 0)
{
	//�Ȼ��Ʊ���ͼ��
	//atlas2 ��0,0���Ǳ���ͼƬ
	setaspectratio(WINDOWW * XASP / 620.0, WINDOWH * YASP / 820.0);
	putimage(0, 0, 620, 820, &atlas2, 0, 0);
	setaspectratio(XASP, YASP);
	return;
}

//���������ϲ�UI����Ʒ֡�
void draw_upper_UI(int sum = 0)
{
	//����������Ի��ư�͸������
	// ͸��״̬�����Դ򿪣������Ҳ��Ǻ�ϲ����
	//drawAlpha(0, 0, 640 * XASP, 105 * YASP, &atlas, 0, 132, WINDOWW * XASP, WINDOWH * YASP, 0.5);
	//drawAlpha(640 * XASP, 0, 640 * XASP, 105 * YASP, &atlas, 0, 132, WINDOWW * XASP, WINDOWH * YASP, 0.5);
	myprint(to_string(jump_sum/10), 30, 30);
	myprint(to_string(show_FPS()),500,30);
	return;
}

///���ڶ�ȡ�û��ļ�����������롣�����ӵ�������Ҳ�����С�
void player_control()
{	
	if (_kbhit()) //����м��̰�����������
	{
		//����_getch()��������������ֱ�ӵ���windowapi��Ч�ʵ͵Ķ�Ķ�Ķ࣡�ᵼ�·ǳ�����
		//������Ϊ_getch()������ʽ�ģ���windowapi����Ƿ�����ʽ�ģ�
		//ע�⣡����ֻд��A���ǿ���ͬʱʶ���Сд�ģ�����
		if (GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A'))// ����A���������
		{
			player.move(-DS, 0);
			player.accumulate_inertia_v(-DS);//ֻ������뺯���������������������ӵķ���
		}
		if (GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D'))// ����D�����ҷ����
		{
			player.move(+DS, 0);
			player.accumulate_inertia_v(+DS);
		}
		static int last_shoot = 0, now = 10000;//last_shoot����һ�η���ʱ���Ƶ�ʱ�䣬now����һ�λ��Ƶ�ʱ�䡣
		if ((GetAsyncKeyState(VK_SPACE)|| GetAsyncKeyState(VK_UP)) && now - last_shoot >  40)//���¿ո�����ϼ����䣬�������η���ʱ����Ҫ����һ���ļ����
		{
			if (player.status !=dizzy)
			{
				//���þ�����һ�η����֡��Ϊ0�������֡��������ά�ֽ�ɫ��̬�ģ�
				player.shot();
				//��������ӵ��ɹ������þ�����һ�η����ʱ��Ϊ0�������ʱ���������������γɹ�����ֱ�ӵļ���ģ��ж�Ҫ��Ҫ���ܼ������ݣ�
				if (create_a_bullet())
				{
					last_shoot = now;
				}
			}
		}
		now = GetTickCount();
		//���û�а������ҷ����
		if (!(GetAsyncKeyState(VK_LEFT) || GetAsyncKeyState('A')) && !(GetAsyncKeyState(VK_RIGHT) || GetAsyncKeyState('D')))
		{
			player.move(0, 0);
		}
		//ÿ֡��Ҫ����������������0����������
		player.accumulate_inertia_v(0);
	}
	//���kbhit���ص��Ǽ�⻺���������ַ������ǲ������ȡ����������һ���й��������������ص�һֱ��1��Ҫ����getch�����������ռ��������ַ���������������������٣�
	return;
}

///����ǳ�ʼ����Ϸ�����ĺ��������ʼ����ɫ��
void initgame()
{
	delete_all_bullets();
	delete_all_lands();
	delete_all_monsters();
	delete_all_strings();
	initglobal_variable();
	//��ʼ����ɫ��λ�ã����ؽ�ɫ�����ڵ�ͼƬ�ļ�����ʼ����ɫ�����ڵĸ���ʱ�������
	initplayer();
	//��ʼ��һЩ�൱�еľ�̬ͼƬ��
	//���زĸ�����̬�����е�ͼƬ��
	bulletclass::atlas = atlas;
	landclass::atlas = atlas;
	monster::atlas = atlas;
	rocketclass::atlas = atlas;
	printclass::font = font;
	//��ʼ�����еĵ��档�����Ƶ�һ֡�еĵ��档
	initrocket();
	initbullets();
	initstrings();
	initlands();
	initmonsters();
}

///������Զ���ĺ�����Ϊ����ͼƬȫ�ֱ�����ʼ����
void loadimg()
{ 
	//loadimage(&atlas2, _T("./shapes/atlas2.png"));
	//loadimage(&player_img, _T("./shapes/playerSheet.png"));
	//loadimage(&player_img2, _T("./shapes/playerSheet2.png"));
	loadimage(&atlas, _T("IMAGE"), _T("atlas1"));
	loadimage(&atlas2, _T("IMAGE"), _T("atlas2"));
	loadimage(&player_img, _T("IMAGE"), _T("playerSheet1"));
	loadimage(&player_img2, _T("IMAGE"), _T("playerSheet3"));
	//loadimage(&player_img2, _T("IMAGE"), _T("playerSheet2"));playerSheet2ת��֮��õ����ļ�������ļ��ƺ���Ȼ�����ã�����ֻ����IMAGE����������ʾ���ѣ�Ҫ��֮�޸�Ϊ�ⲿ�ļ�
	loadimage(&font, _T("IMAGE"),_T("bitmapFont_1"));
	//loadimage(&atlas, _T("./shapes/atlas.png"));
	//loadimage(&font, _T("./shapes/bitmapFont_0.png"));
};

///���ú��ж�����ǲ��ǹ��ˣ�������Ļ����Ȼ����ָ���Ӧ������
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
		settextstyle(25, 0, _T("����")); // �������ִ�С������,��һ�����������ָ߶ȣ��ڶ�����ƽ����ȣ���Ϊ0����ݸ߶�����Ӧ
		outtextxy(0, WINDOWH-25, instruction); 
	}
	if (player.bottom_y() > WINDOW_BOTTOM)
	{
		//initplayer();//ֱ������λ�ã�����������Աר���޵а档
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
			exit(0);//�ǵ���exitҪ�������ţ���Ȼ�൱��return
		}
	}
	return;
}

///���������Ŀ���ǻ���һ֡��ǰ����Ϸ����
void draw_game_window() 
{
	//����UI��
	draw_background_UI();
	//��Ҷ�����������һ֡������ʱ��������
	++player;
	//����Ƿ��м������룬�޸���ҵ�����λ�ã��Լ��ж��Ƿ�Ҫ�����ӵ���
	player_control();
	//����y
	player.adjust_y_by_jumping_status();
	//�ж��Ƿ��й��﷢����ײ
	contact_monster();
	//�ж��Ƿ�������ײ��
	contact_rocket();
	//�ж��Ƿ��뵯�ɻ��������ײ��
	on_string_or_land_or_monster();
	//player�ĸ߶�y����ֻ��jumping���ú��޸ģ���y�޸ĺ����ǿ�ʼˢ����Ծ�ܸ߶�
	refresh_jump_sum();
	//���µ��棬
	refresh_all_elements();
	//�ֱ���Ʋ�ͬ����
	draw_all_lands(jump_sum);
	draw_all_strings(jump_sum);
	draw_all_bullets(jump_sum);
	draw_all_monsters(jump_sum);
	draw_rocket(jump_sum);
	player.show(jump_sum);
	draw_upper_UI(jump_sum);
	//�ж���ҹ���û��
	if_player_dead();
}

int main() 
{
	int mytime = 1;
	int last_frame = 1;
	srand(unsigned(time(0)));
	//initgraph(WINDOWW* XASP,WINDOWH* YASP,SHOWCONSOLE);//�������ʾ������ͬʱ����ʾ����̨��
	initgraph(WINDOWW, WINDOWH);
	//���������������ʾ�����õģ��ظ����û�ص��Ѿ����ڵģ�������������
	setaspectratio(XASP, YASP);
	//��ʼ��������ɫ��
	setlinecolor(RGB(0, 0, 0));
	loadimg();
	start_again:
	initgame();
	//��һ����ʾ����������������EndBatchDraw()֮ǰ���л��Ʋ���������һ���������С�
	//������������ᱻ��ʾ�����������ڴ��еġ�
	BeginBatchDraw();
	while (1)
	{
		draw_game_window();
		FlushBatchDraw();
		//ʹ���������������ʹ��һ���Խ��������ڵ�ͼ����ʾ����������С�
		//���������������ڻ��洰���лᵼ����ʾ���л��Ƶ�ͼ�������Ⱥ���Ⱦ�ġ�
		mytime = GetTickCount();
		while (mytime-last_frame<12)//������־���֡�����ޡ�
		{
			Sleep(0.5);//��֪��Ϊʲô�������ó�1��ʹ����߿������ó�0.5�������ˡ�
			mytime = GetTickCount();
		}
		last_frame = GetTickCount();
		//ÿ֡���17���룬ʹ��ÿ������60֡�����ǳ�����Ⱦÿһ֡��Ҫ���������룬�����������С��17����
	}
	EndBatchDraw();
	//getchar();Ҳ����������ס���򣬵���������while������
	return 0;
}