#include "draw.h"

//画点函数
//point_size: 点宽度
//sRGB
int lcd_display_point(lcd_dev_t *lcd, int point_size,int sRGB, int x, int y)
{
    unsigned char *lcd_display;

    x = x > lcd->width ? lcd->width : x;
    x = x < 0   ? 0   : x;
    y = y > lcd->height ? lcd->height : y;
    y = y < 0   ? 0   : y;  

    if(x == 0 || y == 0)
        return 0;

    lcd_display = &lcd->display[4 *(y*lcd->width+x)];    //确定屏幕的写入位置 通过y*lcd->width+x确定写入的像素点的位置
    unsigned char sRGB_buf[4] = {sRGB&0xff, (sRGB>>8)&0xff, (sRGB>>16)&0xff,(sRGB>>24)&0xff};  //拆分颜色

    memcpy(lcd_display, sRGB_buf, 4);


    return 1;
}

//连续画线
int DrawLine_series(lcd_dev_t *lcd, touch_dev_t *touch, int x, int y, int color)
{
    static  int x_l, y_l = 0;
    if(touch->BTN_TOUCH_Buf == 0)
    {
        x_l = 0;
        y_l = 0;
        return 0;
    }
    if(x_l == x && y_l == y)
    {
        return 0;
    }
    if(x == 0 || y == 0)
    {
        return 0;
    }else if(x_l == 0 && y_l == 0)
    {
        x_l = x;
        y_l = y;
        return 0;
    }else
    {
        //printf("old:%d,%d ",x_l,y_l);
        //printf("new:%d,%d\r\n",x,y);
        DrawLine_kb(lcd, x_l, y_l, x, y, color);
        x_l = x;
        y_l = y;
    }
    return 1;

}

//y=kx+b x1,y1为起点坐标，x2,y2为终点坐标
void DrawLine_kb(lcd_dev_t *lcd, int x1, int y1, int x2, int y2, int color)
{
    unsigned char *lcd_display;
    int i,j;
    float k = 0;
    float b =0;
    int scan_axis = 0; //0使用x方向扫描，
    //unsigned char sRGB_buf[4] = {(color>>24)&0xff, (color>>16)&0xff, (color>>8)&0xff, color&0xff};  //拆分颜色

    if((x1-x2)*(x1-x2) >= (y1-y2)*(y1-y2) )
    {
        k = (float)(y1 - y2) / (float)(x1 - x2);
        b = (float)((y2*x1) - (y1*x2)) / (float)(x1 - x2);
        scan_axis = 0;
    }else
    {
        k = (float)(x1 - x2) / (float)(y1 - y2);
        b = (float)((x2*y1) - (x1*y2)) / (float)(y1 - y2);
        scan_axis = 1;
    }

    if(scan_axis == 0) //使用x方向扫描
    {
        if(x1 <= x2) //从左到右扫描
        {       
            for(i = 0; i <= (x2 - x1); i++)
            {
                lcd_display_point(lcd, 1, color, x1+i, k*(x1+i)+b);
                //usleep(200);
            }
            //printf("从左到右扫描\r\n");
        }else //从右到左扫描
        {
            for(i = (x1 - x2); i >= 0; i--)
            {
                lcd_display_point(lcd, 1, color, x2+i, k*(x2+i)+b);
                //usleep(200);
            }
            //printf("从右到左扫描\r\n");
        }
    }else
    {
        if(y1 <= y2)  //从上到下扫描
        {
            for(i = 0; i <= (y2 - y1); i++)
            {
                lcd_display_point(lcd, 1, color, k*(y1+i)+b, y1+i);
                //usleep(200);
            }
            //printf("从上到下扫描\r\n");
        }else //从下到上扫描
        {
            for(i = (y1 - y2); i >= 0; i--)
            {
                lcd_display_point(lcd, 1, color, k*(y2+i)+b, y2+i);
                //usleep(200);
            }
            //printf("从下到上扫描\r\n");
        }
    }    
}


//画直线(x1,y1为起点坐标，x2,y2为终点坐标，绘制的直线包括起点，但不包括终点)
void DrawLine(lcd_dev_t *lcd, int x1, int y1, int x2, int y2, int color)
{
 int di;
 int dx;
 int dy;
 int gx;
 int gy;
 di = 0;
 gx = (x2 > x1) ? 1 : -1;
 gy = (y2 > y1) ? 1 : -1;
 dx = (x2 - x1) * gx + 1;
 dy = (y2 - y1) * gy + 1;
 if(dx >= dy)
 {
  for(/*x2 += gx*/; x1 != x2; x1 += gx)
  {
   di += dy;
   if(di <= dx)
   {
//    SetPixel(dc, x1, y1, color);
    lcd_display_point(lcd, 1, color, x1, y1);
    continue;
   }
   while((di > dx) && (y1 != y2))
   {
    di -= dx;
    y1 += gy;
//    SetPixel(dc, x1, y1, color);
    lcd_display_point(lcd, 1, color, x1, y1);
   }
  }
 }
 else
 {
  for(/*y2 += gw*/; y1 != y2; y1 += gy)
  {
   di += dx;
   if(di <= dy)
   {
//    SetPixel(dc, x1, y1, color);
    lcd_display_point(lcd, 1, color, x1, y1);
    continue;
   }
   while((di > dy) && (x1 != x2))
   {
    di -= dy;
    x1 += gx;
//    SetPixel(dc, x1, y1, color);
    lcd_display_point(lcd, 1, color, x1, y1);
   }
  }
 }
}

//-------------------------------------------------------------------------------------------------
//画圆(x,y为圆⼼坐标，r为圆的半径，圆⼼的真实物理位置是x,y这个像素的左上⾓)
void DrawCircle(lcd_dev_t *lcd, int x, int y, int r, int color)
{
 int  xi;
 int  yi;
 int  di;
 di = 0 - (r >> 1);
 xi = 0;
 yi = r;
 while(yi >= xi)
 {
/*   SetPixel(x + xi - 1, y + yi - 1, color);
  SetPixel(x + yi - 1, y + xi - 1, color);
  SetPixel(x - xi, y + yi - 1, color);
  SetPixel(x - yi, y + xi - 1, color);
  SetPixel(x - xi, y - yi, color);
  SetPixel(x - yi, y - xi, color);
  SetPixel(x + xi - 1, y - yi, color);
  SetPixel(x + yi - 1, y - xi, color); */
  xi++;
  if (di < 0)
  {
   di += xi;
  }
  else
  {
   yi--;
   di += xi - yi;
  }
 }
}
