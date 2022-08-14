#include "touch.h"

//初始化触摸屏函数
int touch_init(touch_dev_t *touch, lcd_dev_t *lcd)
{
    //1.打开触摸屏驱动文件
    touch->touch_fd = open(TOUCH_PATH, O_RDONLY);
    touch->x_size = 1024;
    touch->y_size = 600;
    //touch->x_fold = 0.78125;
    touch->x_fold = (lcd->width < touch->x_size)? ((float)lcd->width / (float)touch->x_size) : ((float)touch->x_size / (float)lcd->width) ;
    //touch->y_fold = 0.8;
    touch->y_fold = (lcd->height < touch->y_size)? ((float)lcd->height / (float)touch->y_size) : ((float)touch->y_size / (float)lcd->height) ;
    //printf("x_fold = %f ,y_fold = %f\r\n",touch->x_fold ,touch->y_fold);
    touch->ABS_X_Buf = 0;
    touch->ABS_Y_Buf = 0;
    touch->BTN_TOUCH_Buf = 0;
    if( touch->touch_fd == -1)
    {
        printf("打开触摸屏驱动(%s)失败\r\n", TOUCH_PATH);
        return -1;
    }
    return 0;
}

//点动获取触摸屏坐标
int get_touch_coordinates(touch_dev_t *touch)
{
        read( touch->touch_fd, &input, sizeof(input));    //阻塞性函数

        //3.处理从触摸屏驱动文件中读取数据
        // printf("事件类型:%d\r\n", input.type);
        // printf("事件代码:%d\r\n", input.code);
        // printf("发生的值:%d\r\n", input.value);
        if( input.type == EV_ABS && input.code == ABS_X)
        {
            touch->ABS_X_Buf = input.value * touch->x_fold;
            printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else if( input.type == EV_ABS && input.code == ABS_Y)
        {
            touch->ABS_Y_Buf = input.value * touch->y_fold;
            printf("y(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else if( input.type == EV_KEY && input.code == BTN_TOUCH)
        {
            touch->BTN_TOUCH_Buf = input.value;  
            printf("B(%d)\r\n",touch->BTN_TOUCH_Buf);  
        }
        return  touch->BTN_TOUCH_Buf;
}

//滑动获取触摸屏坐标
int get_touch_coordinates_x(touch_dev_t *touch)
{
    static int x,y = 0;
    static unsigned char next_bit = 0;
    read( touch->touch_fd, &input, sizeof(input));    //阻塞性函数

    if( input.type == EV_ABS && input.code == ABS_X)
    {
        x = input.value * touch->x_fold;
//        printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
    }else if( input.type == EV_ABS && input.code == ABS_Y)
    {
        y = input.value * touch->y_fold;
//        printf("y(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
    }else if( input.type == EV_KEY && input.code == BTN_TOUCH)
    {
        touch->BTN_TOUCH_Buf = input.value;    
        printf("B(%d)\r\n",touch->BTN_TOUCH_Buf); 

    } 

    if(touch->BTN_TOUCH_Buf != 0)
    {
        if(touch->ABS_X_Buf != x && touch->ABS_Y_Buf != y && x != 0 && y != 0 || next_bit > 4) //要有滑动才画点 ，此处为开始滑动的过滤次数
        {   
            next_bit = 0;
            //if(x != 0)
                touch->ABS_X_Buf = x; 
            //if(y != 0)
                touch->ABS_Y_Buf = y;
            printf("x(%d,%d)\r\n",touch->ABS_X_Buf,touch->ABS_Y_Buf);
        }else
        {
            next_bit ++;
        }
    }else
    {
        x = 0;
        y = 0;
        touch->ABS_X_Buf = 0;
        touch->ABS_Y_Buf = 0;
        next_bit = 0;
    }


    return  touch->BTN_TOUCH_Buf;
}
