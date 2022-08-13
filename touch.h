#ifndef __TOUCH_H
#define __TOUCH_H

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

#define TOUCH_PATH "/dev/input/event0"

#pragma pack (1)
typedef struct touch_dev
{
    int touch_fd;
    int x_size;
    int y_size;
    float x_fold;
    float y_fold;
    int ABS_X_Buf;  //x坐标
    int ABS_Y_Buf;  //y坐标
    int BTN_TOUCH_Buf;  //按下弹起标志
//    unsigned char *display;
}touch_dev_t;
#pragma pack ()

/*声明函数*/
int touch_init(touch_dev_t *touch, lcd_dev_t *lcd);
int get_touch_coordinates(touch_dev_t *touch);
int get_touch_coordinates_x(touch_dev_t *touch);



#endif
