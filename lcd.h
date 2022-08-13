#ifndef __LCD_H
#define __LCD_H

#include "stdio.h"
#include "sys/types.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "unistd.h"
#include "stdlib.h"
#include "string.h"
#include "sys/mman.h"
#include "linux/input.h"

#define FB0_PATH "/dev/fb0" //显示屏驱动文件路径

#pragma pack (1)
typedef struct tagBITMAPFILEHEADER
{ 
    unsigned short  bfType;             //2Bytes，必须为"BM"，即0x424D 才是Windows位图文件
    unsigned int    bfSize;             //4Bytes，整个BMP文件的大小
    unsigned short  bfReserved1;        //2Bytes，保留，为0
    unsigned short  bfReserved2;        //2Bytes，保留，为0
    unsigned int    bfOffBits;          //4Bytes，文件起始位置到图像像素数据的字节偏移量
} BMP_BITMAPFILEHEADER;
#pragma pack ()

#pragma pack(1)
typedef struct _tagBMP_INFOHEADER
{
    unsigned int    biSize;                     //4Bytes，INFOHEADER结构体大小，存在其他版本INFOHEADER，用作区分
    unsigned int    biWidth;                    //4Bytes，图像宽度（以像素为单位）
    unsigned int    biHeight;                   //4Bytes，图像高度，+：图像存储顺序为Bottom2Top，-：Top2Bottom
    unsigned short  biPlanes;                   //2Bytes，图像数据平面，BMP存储RGB数据，因此总为1
    unsigned short  biBitCount;                 //2Bytes，图像像素位数, 1个像素占据多少位
    unsigned int    biCompression;              //4Bytes，0：不压缩，1：RLE8，2：RLE4
    unsigned int    biSizeImage;                //4Bytes，4字节对齐的图像数据大小
    unsigned int    biXPelsPerMeter;            //4 Bytes，用象素/米表示的水平分辨率
    unsigned int    biYPelsPerMeter;            //4 Bytes，用象素/米表示的垂直分辨率
    unsigned int    biClrUsed;                  //4 Bytes，实际使用的调色板索引数，0：使用所有的调色板索引
    unsigned int    biClrImportant;             //4 Bytes，重要的调色板索引数，0：所有的调色板索引都重要
}BMP_INFOHEADER;
#pragma pack()

typedef struct bmp_dev
{
    int bmp_fd;     //bmp图片的文件描述符(文件编号)
    int width;      //bmp宽度
    int height;     //bmp高度
    int bitcount;   //bmp位深度( 一个占据多少个字节)
    unsigned char *data;    //存储已经添加了A(亮度)数据，颜色已经翻转的数据，图像垂直翻转
}bmp_dev_t;    //

typedef struct lcd_dev
{
    int lcd_fd;     //lcd液晶屏幕驱动文件的文件描述符
    int width;
    int height;
    int bitcount;
    unsigned char *display; //写入BMP图片颜色数据的内存
}lcd_dev_t;

extern struct input_event input;   //读取触摸屏数据需要用到的结构体

/*声明函数*/
int lcd_init(lcd_dev_t *lcd);
int lcd_display_bmp(lcd_dev_t *lcd, bmp_dev_t *bmp, int x, int y);
void lcd_uninit(lcd_dev_t *lcd);
int bmp_init( bmp_dev_t *bmp, const char *BmpPath);
void bmp_readdata( bmp_dev_t *bmp);



#endif
