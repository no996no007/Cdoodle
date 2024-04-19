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

//这是一个实现多态的函数，根据传入的地面类型调用其自身对应的显示函数。
//犯了一个错！多态只能是下面这个函数接受一个派生类的指针或者派生类的引用！
//是不可以把一个派生类复制给基类的对象，然后传这个基类进来，这样调用还是基类的函数！
//你只能直接传派生类引用或者基类但指向派生类的指针！
//实际上只用第一种情况（传派生类引用）有效，应对第二种情况（传基类但指向派生类的指针）有点多余，因为
//基类但指向派生类的指针直接调用虚函数就可以直接对应到派生类的虚函数，本程序就是第二种情况，但是这是编写才发现的，故不做修改。
void draw_a_land(landclass* a_land, int sum = 0);
void draw_a_monster(monster* a_monster, int sum = 0);
//lands[1] = &greenland;
//draw_a_land(lands[1]);（函数接受基类的指针）可以多态调用的是派生类的函数！
//draw_a_land(greenland);（函数接受派生类的引用）可以多态！调用的是派生类的函数！
//landclass land = greenland;
//draw_a_land(land);（函数接受基类的引用）不能多态！调用的是基类的函数！
//这说明了派生类向基类赋值时，并不会改变这基类对象的虚函数表！虚函数表里永远只放这个类的虚函数！

///设计一个函数来记录一秒内显示了多少帧，它要每帧都调用一次.每过一秒他就自己输出一次帧数。
int show_FPS();

///这是一个仿函数，用于在屏幕上输入文字。
class printclass
{
public:
	static IMAGE font;
	void operator()(const string& text,int x,int y);
};