#ifndef __DRAW_H
#define __DRAW_H
void DrawLine(int x1, int y1, int x2, int y2, int color);
void DrawCircle(int x, int y, int r, int color);

#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/mman.h"
#include "linux/input.h"

#include "lcd.h"
#include "touch.h"

/*声明函数*/
int lcd_display_point(lcd_dev_t *lcd, int point_size,int sRGB, int x, int y);
void DrawLine(int x1, int y1, int x2, int y2, int color);
void DrawCircle(int x, int y, int r, int color);

#endif
