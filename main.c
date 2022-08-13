#include "main.h"

int main()
{
    lcd_dev_t lcd;
    bmp_dev_t bmp;
    touch_dev_t touch_dev_user;

    lcd_init(&lcd); //初始化屏幕

    bmp_init(&bmp, "b.bmp");    //初始化bmp图片
    bmp_readdata(&bmp);         //读取，修改bmp图片的信息
    touch_init(&touch_dev_user,&lcd); //初始化触摸屏    
    lcd_display_bmp( &lcd, &bmp, 0,0);  //显示bmp图片
   
    bmp_uninit( &bmp);          //显示完图片后，必须调用该函数，销毁申请bmp的堆空间
    
     while(1)
    {
        get_touch_coordinates_x(&touch_dev_user); //获取触摸坐标
        lcd_display_point(&lcd, 1, 0x00ffffff, touch_dev_user.ABS_X_Buf, touch_dev_user.ABS_Y_Buf); //画点
        
    }

    lcd_uninit( &lcd);  //卸载lcd
    return 0;
}




