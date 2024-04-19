#include "allclass.h"

///ÿ����һ��Ϊ����ʱ�������һ����ʱ������һ֡
playerclass& playerclass::operator++()
{
	t++;
	shooting_t++;
	//������һ�η����֡������40ʱ��ȡ��������̬
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
///bottom_x���ж�����Ƿ������Ӵ��ĵײ�x������
int  playerclass::bottom_x()
{
	return pos_x + 52;
}
///bottom_y���ж�����Ƿ������Ӵ��ĵײ�y������
int  playerclass::bottom_y()
{
	return pos_y + 116;
}
///contact_x���ж�����Ƿ�����Ʒ�Ӵ���x������
int  playerclass::contact_x()
{
	return pos_x + 65;
}
///contact_y���ж�����Ƿ�����Ʒ�Ӵ���y������
int  playerclass::contact_y()
{
	return pos_y + 60;
}
///���������ײ��
void playerclass::contact_with_monster()
{
	if (status ==dizzy)//���������״̬�����������ײ��Ҳ����Ӧ��
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
///��ʼ����������ҵ���x��y���꣬�Լ�����״̬
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
///ÿ֡����״̬�����߶ȡ�
///�Ǹ�����һ����λ�����޸�pos_y�ģ���������pos_y�޹أ�
void playerclass::adjust_y_by_jumping_status()
{
	int ds;
	//������һ�ε�����������ڸ߶ȱ仯��
	switch (jump_status)
	{
	case 1:
		//�����и��ӣ�����Ҫ��ȥ��Ծ��!�����ϵ���y���ģ�
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
///����������ÿ�ε��ø���t���Ƿ�����true���޸���ҵ�ǰ��yֵ��ע�⣡
void playerclass::adjust_jumping_status(int jump_strength)
{
	if (status == dizzy)//���ѣ���˲�����ı�״̬��
	{
		return;
	}
	if (jump_strength==1)//��ͨ��
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
///���ӹ��ԡ����ƶ��ķ�����ӹ��ԣ�����ƶ�Ϊ0��ô�������ԡ�
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
			inertia_v -= 0.3*inertia_v / fabs(inertia_v);//���0.3�����˹��Ե�˥���ٶȣ������޸ģ�С��ÿ֡���ӵĹ��Լ���
		}
	}
	else
	{
		if (status == dizzy)//���ѣ���˲�������ӹ��ԡ�
		{
			return;
		}
		if (fabs(inertia_v) < V_X)//�����ٶ�
		{
			inertia_v += 2* dx / abs(dx);//�ƶ�ʱÿ֡���ӹ���2���������޸�
		}
	}
	return;
}
///�������x��y����ĺ���
void playerclass::move(int dx, int dy) 
{
	if (status == dizzy)
	{
		dx = 0;//�����ͱ�֤������ѣ���˻�һֱѣ�Σ��޷�������
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
	//�����������Ļ����Ҫ���Դ�����һ�����
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
///��ͼ�л���һ�ν�ɫ��
void playerclass::show(int sum)
{
	//������һ�γ��Է����ʱ����һ����Χ�ڻ�����ұ���̧ͷ״̬��
	if (shooting_t < 40 && status !=dizzy)status = shooting;
	if (t < time_for_a_jump / 4 && status != dizzy)status = (player_status)(status + 1);
	//�����Ŀ���ǣ����t<10˵���ո�����,Ӧ����ʾ����������
	//��������״̬������״̬����һλ�����������һ�ǰ�״̬ת��Ϊ��ǰ״̬��Ӧ������״̬��
	switch (status)
	{
	case right:
		putpng(pos_x, pos_y+sum, 124, 120, &rimage, 372, 0);
		//line(0, bottom_y() + sum, WINDOWW, bottom_y() + sum);//���Զ�λ��
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
		//�ȴ�ӡ��ͣ��ٴ�ӡ���塣
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
	//�ǵ�Ҫ�������
	return;
}
///���þ�����һ�η����֡��Ϊ0��������һ�γ��Է����ʱ����һ����Χ�ڻ�����ұ���̧ͷ״̬��
void playerclass::shot()
{
	shooting_t = 0;
	return;
}

///���жϳ�������ײʱ���á�
void landclass::contact() 
{
	//��������ײɶ��û��
}
///������������Ƿ�����淢����ײ��
bool landclass::is_contact(int last_t_bottom_y,int player_bottom_x,int player_bottom_y)
{
	//-20,+130,Ϊ�����������һ��שҲ����
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
///����ʾ����,����ɶҲ����Ҳ����Ҫд����Ϊ���land�಻�Ǵ��麯����
void landclass::show(int sum)
{
	//ɶ������
}

///�ӵ��ķ��亯��,�ӵ���atlas��939��291������21����22��
void bulletclass::show(int sum)
{
	//һ��ʼϣ������α�������һ������,���ǰ��ӵ���ϵ����Ϊ����������ӣ�����һ��α�����
	//��ô����ͬһ���ӵ�������α���������ͬ�ģ�������ÿ�λ��Ƶ�x��ƫ����������ͬ��
	//�����������ã����������������ֻ����һ�ε�ԭ��
	//srand(i);
	//time��0��Ϊ���ӵ�srandһ���������ֻ����һ�Σ�����ᵼ��ÿ�ε���֮���һ�ε�rand����������������ǳ������
	//���Ҿ�������ͬ������ȥ��������Ҳ�ᵼ��rand�������»ص���һ������ʼ����
	//�������ÿ֡������������������ӣ���ôrand����ÿ�ζ�ֻ����һ��������ÿ�α��ĵ�һ�������Ƿ������
	putpng(pos_x, pos_y+sum, 21, 22, &atlas, 939, 291);
	pos_x -= (random_number & 7) - 3;
	pos_y -= BULLET_DS;
	return;
}

///��ʾһ���̵��档
void greenlandclass::show(int sum)
{
	putpng(pos_x, pos_y+sum, 115, 31, &atlas, 316, 893);
	//line(0, pos_y+2 + sum, WINDOWW, pos_y+2 + sum);//��λ�ߣ�����ʱ�á�
	return;
}

///��ʾһ�������档ÿ����ʾ�Լ��ƶ��Լ���λ�ã�
void bluelandclass::show(int sum)
{
	pos_x += direction * speed;
	//���ש��ײ������Ļ���࣬0��WINDOWW-115���������
	if (pos_x < 0 || pos_x>WINDOWW-115)
	{
		direction = -direction;
	}
	putpng(pos_x, pos_y + sum, 115, 31, &atlas, 316, 924);
	return;
}

///��ʾһ��׵��档
void whitelandclass::show(int sum)
{
	putpng(pos_x, pos_y + sum, 115, 31, &atlas, 446, 826);
	return;
}
///���׿鷢����ײʱ���á�
void whitelandclass::contact()
{
	//live�ص�����˼���ǲ�����Ļ����ʾ��Ҳ�����ж���ײ��
	live = FALSE;
	return;
}

///������ש�鷢����ײʱ��
void fragilelandclass::contact()
{
	broken = TRUE;
	return;
}
///��ʾ����ש�顣
void fragilelandclass::show(int sum)
{
	//���û��
	if (!broken)
	{
		putpng(pos_x, pos_y + sum, 121, 31, &atlas, 324, 826);
	}
	//�������
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

///��ʾ����
void stringlandclass::show(int sum)
{
	//����������ƶ���
	pos_x = base->pos_x + 18 + relative_x;
	pos_y = base->pos_y - 20;
	//������ɱ�������
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
//�����ɷ�����ײʱ��������ʱ��
void stringlandclass::contact()
{
	triggerd = TRUE;
}
//�����뵯�ɽӴ���������
bool stringlandclass::is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y)
{
	//-20,+,Ϊ�����������һ�㵯��Ҳ����
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

///���˻���Ŀ�ʵ�֡�
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

///��ɫС�ֵ����ء�
void red_monster::show(int sum)
{
	if (HP < 1) 
	{
   		if (HP--==0)//��˼�����if����������֮��������ʼ����������ֻ����һ��
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t=0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3*((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)����Ǹ���base_x�������������1��-1.
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

///��ɫ��ֵ����ء�
void green_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//��˼�����if����������֮��������ʼ����������ֻ����һ��
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)����Ǹ���base_x�������������1��-1.
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

///miniС�����ء�
void mini_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//��˼�����if����������֮��������ʼ����������ֻ����һ��
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)����Ǹ���base_x�������������1��-1.
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

//�ڶ�����
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

///��ɫС�ֵ����ء�
void blue_monster::show(int sum)
{
	if (HP < 1)
	{
		if (HP-- == 0)//��˼�����if����������֮��������ʼ����������ֻ����һ��
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)����Ǹ���base_x�������������1��-1.
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
		if (HP-- == 0)//��˼�����if����������֮��������ʼ����������ֻ����һ��
		{
			base_x = pos_x;
			base_y = pos_y;
			last_t = 0;
		}
		pos_y = base_y - ((8 * last_t) - floor(a_half * last_t * last_t));
		pos_x = pos_x + 3 * ((base_x % 2) * 2 - 1);//((base_x % 2) * 2 - 1)����Ǹ���base_x�������������1��-1.
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

//����������
void rocketclass::show(int sum)
{
	//�����Ӧ����������
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
	//����������ƶ���
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