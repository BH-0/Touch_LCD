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
    unsigned char sRGB_buf[4] = {(sRGB>>24)&0xff, (sRGB>>16)&0xff, (sRGB>>8)&0xff, sRGB&0xff};  //拆分颜色

    memcpy(lcd_display, sRGB_buf, 4);


    return 1;
}

//画直线(x1,y1为起点坐标，x2,y2为终点坐标，绘制的直线包括起点，但不包括终点)
void DrawLine(int x1, int y1, int x2, int y2, int color)
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
    continue;
   }
   while((di > dx) && (y1 != y2))
   {
    di -= dx;
    y1 += gy;
//    SetPixel(dc, x1, y1, color);
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
    continue;
   }
   while((di > dy) && (x1 != x2))
   {
    di -= dy;
    x1 += gx;
//    SetPixel(dc, x1, y1, color);
   }
  }
 }
}

//-------------------------------------------------------------------------------------------------
//画圆(x,y为圆⼼坐标，r为圆的半径，圆⼼的真实物理位置是x,y这个像素的左上⾓)
void DrawCircle(int x, int y, int r, int color)
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
