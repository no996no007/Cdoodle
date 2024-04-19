#pragma once
#include<stdio.h>
#include<graphics.h>
#pragma comment( lib, "MSIMG32.LIB")

///一个函数，为了实现将png透明输出
void putpng(int x, int y, IMAGE* srcimg, IMAGE* dstimg = NULL);
///这个函数跟上一个的不同之处在于，有的素材合并在一张图中，我们需要而外的定位fromx与fromy
void putpng(int x, int y, int xlength, int ylength, IMAGE* srcimg, int fromx, int fromy, IMAGE* dstimg = NULL);

///绘图函数，补充透明度 AA
void drawAlpha(int x, int y, int width, int height, IMAGE* image, int pic_x, int pic_y, int WindowWidth, int WindowHeight, double AA);

