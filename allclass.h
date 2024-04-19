#pragma once
#include<stdio.h>
#include<graphics.h>
#include"tools.h"
//#include"mytool.h"
//���򲻿ɰ��������������ͷ�ļ����໥�����ģ���a��Ҫ����b���ܺϷ���bҲ��Ҫ����a���ܺϷ���
//��ô��a����b��bҲ����a�ķ�ʽ�ǲ����еģ���Ϊinclude�ı��ʾ��ǰ����е��������ƹ�����
//�����mytool������allclass���ɣ��������ٰ�mytool����һ�Σ��������ƹ��������¸��Ƴ�������һ��û��������allclass�ļ���
#include<conio.h>
#include<math.h>
#define WINDOWH 820 //��Ⱦ�ֱ��ʣ�����820
#define WINDOWW 620 //620
#define time_for_a_jump 80 //ʹ�ö���֡���һ��������Ծ
#define V 16 //��ͨ�������ٶȣ��ķ�֮һ������֡����V��һ��������������߶�/����
#define V_X 6//��Һ����ƶ��ļ����ٶȡ�
#define STRING_V 30 //���������ĳ��ٶ�
#define JUMP_HEIGHT (V*time_for_a_jump/4) //һ��������������߶�/����
#define BLUELAND_DS 7//��ɫש�������ƶ��ٶ�,��С��2��
#define FRAGILELAND_DS 8//����ש����½��ٶ�
#define PI (3.14159)
#define T 120 //����������(֡)��
#define BULLET_CD 30 //�ӵ���ȴCD��300
#define BULLET_DS 29
#define FLYING_T 180


//������ٶȵ�һ��,ע���Ǹ���float���������������������Ǹ��͵�0�ˣ�
const float a_half = (float)V / time_for_a_jump;

enum player_status
{
	right, right_tojump, left, left_tojump, shooting, shooting_tojump
	,dizzy
};

///�ӵ���
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
	int CD=0;//��ȴʱ�䣬����live==falsh��ʼ������С��0�����ٴη�����
	bool live=FALSE;
};

///���˻���
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
	int last_t = 0;//�����˶���֡��
	static IMAGE atlas;
};

///�ڶ�
class blackhole :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool is_contact(int x, int y);
};

///����С��
class flying_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///��ɫ���
class green_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///��ɫС��
class blue_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///��ɫС��
class red_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

//mini��
class mini_monster :public monster
{
public:
	virtual void show(int sum = 0);
	virtual bool be_trample(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	virtual bool is_contact(int x, int y);
};

///�����
class playerclass
{
public:
	///������ҵ���x��y���꣬�Լ�����״̬
	void set(int nx, int ny, player_status ns, IMAGE& nri, IMAGE& nli);
	///�������x��y����ĺ���
	void move(int dx, int dy);
	///��⵽�ո�����,��¼Ϊ��һ�γ��Է����ʱ�䣬������һ�γ��Է����ʱ����һ����Χ�ڻ�����ұ���̧ͷ״̬��
	void shot();
	///��ͼ�л���һ�ν�ɫ��
	void show(int sum = 0);
	///ÿ����һ��Ϊ����ʱ�������һ����ʱ������һ֡
	playerclass& operator++();
	///�޸���ҵ�����״̬����������λ��Ϊ��ǰֵ����������ʱ�䣬���޸�yֵ
	void adjust_jumping_status(int jump_strength = 0);//��Ծ�����ȣ����0�ǾͲ���Ծ�����������䣬Ϊ1����ͨ��Ծ��Ϊ2Ϊ������
	///ÿ֡����״̬�����߶ȡ�
	void adjust_y_by_jumping_status();
	///���ӹ��ԡ����ƶ��ķ�����ӹ��ԣ�����ƶ�Ϊ0��ô�������ԡ�
	void accumulate_inertia_v(int dx);
	///���������ײ��
	void contact_with_monster();

	int pos_x;
	int pos_y;
	//��һ�ε����������1����ͨ����2�ǵ�����,3�����ڷ��У�
	int jump_status;
	///jump_from_y��¼�ϴ����������λ�ã�����������ڹ滮��ҵ���Ծ�˶��켣��
	int jump_from_y;
	///bottom_x���ж�����Ƿ������Ӵ��ĵײ�x������
	int bottom_x();
	///bottom_y���ж�����Ƿ������Ӵ��ĵײ�y������
	int bottom_y();
	///contact_x���ж�����Ƿ�����Ʒ�Ӵ���x������
	int contact_x();
	///contact_y���ж�����Ƿ�����Ʒ�Ӵ���y������
	int contact_y();
	IMAGE rimage;
	IMAGE limage;
	IMAGE atlas;
	IMAGE atlas2;
	player_status status;
	player_status status_before_falling;
	int flying_t = -1;//�����ʱ���������¼��ʼ�����˶���֡�������-1˵��û����ɡ�
	int t;//�����ʱ����������ڼ�¼��ǰ������һ�����������˶���֡
	int shooting_t;//�����ʱ����������ڼ�¼��ǰ������һ�η��侭���˶���֡��
	int dizzy_t = -1;//�ӿ�ʼ��ײ�εĹ��˶���֡������Ǹ�һ˵��û�б�ײ�Ρ�
	double inertia_v;//�����ٶȣ��ƶ����Ե��ӹ��ԣ�ͣ�»���ٹ��ԡ����Ի����������ٶȣ�ע���Ǵ�����ģ�
};

//��¼һ��ͼƬ������ʹ�С
//���894,770����24����38
//�̵ذ�316��893, ��115����31
//���ذ�316,924,115,31
//�Ƶذ�1 324,826,115,31
//�Ƶذ�2 826��379��115��38
//�Ƶذ�4 826��444��115��65
//�Ƶذ�3 652��562��115��58
//�׵ذ�  446,826,115,31
//����1 482��782��34��23
//����2 523��858��34��54

///һ������������
class landclass
{
public:
	///����ʾ����,����ɶҲ����Ҳ����Ҫд����Ϊ���land�಻�Ǵ��麯����
	virtual void show(int sum = 0);
	///���жϳ�������ײʱ���á�
	virtual void contact();
	///������������Ƿ�����淢����ײ��
	virtual bool is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	int pos_x = 0;
	int pos_y = 0;
	bool live = FALSE;
	static int background_width;
	static int background_height;
	static IMAGE atlas;
};

///��ɫ������
class greenlandclass: public landclass
{
public:
	//��һ���virtual��д�ɲ�д����Ϊ�����ϵ�һ�㣬�������������̳�Greenlandclass�����ǵ�д�ϵ�
	///��ʾһ���̵��档
	void show(int sum = 0) override;
};

///��ɫ������
class bluelandclass : public landclass
{
public:
	void show(int sum = 0) override;
	//ÿ����ש����ʱ����һ�����������1����-1��rand()%2��0,1�ٳ˶���0,2����һ��-1��1��
	int direction = 2 * (rand() % 2) - 1;
	int speed = rand() % (BLUELAND_DS - 1) + 2;
};

///��ɫ������
class whitelandclass : public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
};

///����������
class fragilelandclass:public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
	bool broken = FALSE;
	int broken_t = 0;//���������ʱ��
};

///������
class stringlandclass:public landclass
{
public:
	void show(int sum = 0) override;
	void contact() override;
	bool triggerd = FALSE;//�Ƿ��Ѿ�������
	bool is_contact(int last_t_bottom_y, int player_bottom_x, int player_bottom_y);
	landclass* base=nullptr;
	int relative_x = 0;
};

///�������,�������ǻ��������͵�����ģ�Ҫ���������棡
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