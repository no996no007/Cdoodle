#pragma once
#include<iostream>
#include "allclass.h"
#include "tools.h"
#include<stdio.h>
#include<graphics.h>
#include<iostream>
#include<string>
#include<time.h>

using std::cout;
using std::string;

//����һ��ʵ�ֶ�̬�ĺ��������ݴ���ĵ������͵����������Ӧ����ʾ������
//����һ������ֻ̬�������������������һ���������ָ���������������ã�
//�ǲ����԰�һ�������ิ�Ƹ�����Ķ���Ȼ���������������������û��ǻ���ĺ�����
//��ֻ��ֱ�Ӵ����������û��߻��൫ָ���������ָ�룡
//ʵ����ֻ�õ�һ������������������ã���Ч��Ӧ�Եڶ�������������൫ָ���������ָ�룩�е���࣬��Ϊ
//���൫ָ���������ָ��ֱ�ӵ����麯���Ϳ���ֱ�Ӷ�Ӧ����������麯������������ǵڶ���������������Ǳ�д�ŷ��ֵģ��ʲ����޸ġ�
void draw_a_land(landclass* a_land, int sum = 0);
void draw_a_monster(monster* a_monster, int sum = 0);
//lands[1] = &greenland;
//draw_a_land(lands[1]);���������ܻ����ָ�룩���Զ�̬���õ���������ĺ�����
//draw_a_land(greenland);��������������������ã����Զ�̬�����õ���������ĺ�����
//landclass land = greenland;
//draw_a_land(land);���������ܻ�������ã����ܶ�̬�����õ��ǻ���ĺ�����
//��˵��������������ำֵʱ��������ı�����������麯�����麯��������Զֻ���������麯����

///���һ����������¼һ������ʾ�˶���֡����Ҫÿ֡������һ��.ÿ��һ�������Լ����һ��֡����
int show_FPS();

///����һ���º�������������Ļ���������֡�
class printclass
{
public:
	static IMAGE font;
	void operator()(const string& text,int x,int y);
};