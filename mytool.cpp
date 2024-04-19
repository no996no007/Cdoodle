#include"mytool.h"

///传入一个虚基类的指针，调用他实际指向的派生类的show（）函数
void draw_a_land(landclass* a_land, int sum)
{
	a_land->show(sum);
}
void draw_a_monster(monster* a_monster, int sum)
{
	a_monster->show(sum);
}

//设计一个函数来记录一秒内显示了多少帧，它要每帧都调用一次.每过一秒他就自己输出一次帧数。
int show_FPS()
{
	//设计两个变量来记录一秒内显示了多少帧
	static int t1 = time(0), t2 = time(0), flash_sum = 0;
	static int last_second_FPS = 0;
	flash_sum++;
	t2 = time(0);
	if (t2 - t1 == 1)
	{
		t1 = t2;
		cout << "FPS:" << flash_sum << "\n";
		last_second_FPS = flash_sum;
		flash_sum = 0;
	}
	return last_second_FPS;
}

void printclass::operator()(const string& text,int x, int y)
{
	for (int i = 0; i < text.length(); i++)
	{
		if (text[i] == '1')putpng(x, y, 14, 42, &font, 0, 252);
		else if (text[i] == '2')putpng(x, y, 37, 41, &font, 14, 252);
		else if (text[i] == '6')putpng(x, y, 37, 44, &font, 266, 180);
		else if (text[i] == '7')putpng(x, y, 37, 44, &font, 228, 180);
		else if (text[i] == '0')putpng(x, y, 37, 44, &font, 190, 183);
		else if (text[i] == '9')putpng(x, y, 33, 44, &font, 303, 180);
		else if (text[i] == '4')putpng(x, y, 30, 43, &font, 336, 180);
		else if (text[i] == '3')putpng(x, y, 34, 44, &font, 436, 173);
		else if (text[i] == '8')putpng(x, y, 34, 43, &font, 472, 173);
		else if (text[i] == '5')putpng(x, y, 38, 44, &font, 396, 175);
		else if (text[i] == 's')putpng(x, y, 26, 30, &font, 436, 218);
		else if (text[i] == 'c')putpng(x, y, 28, 30, &font, 0, 295);
		else if (text[i] == 'o')putpng(x, y, 34, 32, &font, 250, 225);
		else if (text[i] == 'r')putpng(x+4, y-2, 28, 37, &font, 115, 244);
		else if (text[i] == 'e')putpng(x, y, 34, 28, &font, 27, 293);
		else if (text[i] == 'h')putpng(x, y - 23, 31, 56, &font, 15, 143);
		else if (text[i] == 'i')putpng(x, y - 15, 10, 46, &font, 498, 121);
		else if (text[i] == 't')putpng(x, y-23, 26, 53, &font, 278, 125);
		else if (text[i] == 'g')putpng(x, y, 30, 49, &font, 57, 196);
		if (text[i] >= '0' && text[i] <= '9')x += 39;
		else if (text[i] == 'i') x += 15;
		else x += 33;
	}
	return;
}