#pragma once
#include<stdio.h>
#include<graphics.h>
#pragma comment( lib, "MSIMG32.LIB")

///һ��������Ϊ��ʵ�ֽ�png͸�����
void putpng(int x, int y, IMAGE* srcimg, IMAGE* dstimg = NULL);
///�����������һ���Ĳ�֮ͬ�����ڣ��е��زĺϲ���һ��ͼ�У�������Ҫ����Ķ�λfromx��fromy
void putpng(int x, int y, int xlength, int ylength, IMAGE* srcimg, int fromx, int fromy, IMAGE* dstimg = NULL);

///��ͼ����������͸���� AA
void drawAlpha(int x, int y, int width, int height, IMAGE* image, int pic_x, int pic_y, int WindowWidth, int WindowHeight, double AA);

