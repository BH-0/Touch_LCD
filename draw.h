#ifndef __DRAW_H
#define __DRAW_H

#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/mman.h"
#include "linux/input.h"
#include <unistd.h>

#include "lcd.h"
#include "touch.h"

/*声明函数*/
int lcd_display_point(lcd_dev_t *lcd, int point_size,int sRGB, int x, int y);
void DrawLine_kb(lcd_dev_t *lcd, int x1, int y1, int x2, int y2, int color);
void DrawLine(lcd_dev_t *lcd, int x1, int y1, int x2, int y2, int color);
int DrawLine_series(lcd_dev_t *lcd, touch_dev_t *touch, int x, int y, int color);
void DrawCircle(lcd_dev_t *lcd, int x, int y, int r, int color);


#endif

